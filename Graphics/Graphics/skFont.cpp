/*
-------------------------------------------------------------------------------
    Copyright (c) Charles Carley.

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/

#include "skFont.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_ERRORS_H
#include "Image/skPalette.h"
#include "Utils/skFileStream.h"
#include "Utils/skLogger.h"
#include "Utils/skMemoryUtils.h"
#include "Utils/skPlatformHeaders.h"
#include "skBuiltinFonts.h"
#include "skContext.h"
#include "skPath.h"
#include "skTexture.h"

// TODO clean and check all of this...

#define FTINT(x) ((x) >> 6)
#define FTI64(x) ((x) << 6)

const SKint32      CharStart = 32;
const SKint32      CharEnd   = 127;
const SKint32      CharTotal = CharEnd - CharStart;
const SKint32      Spacer    = 2;
const skFont::Char NullChar  = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

skGlyph::skGlyph(SKuint8* ptr, SKuint32 w, SKuint32 h) :
    m_width(w),
    m_height(h)
{
    SKsize lim = (SKsize)w * (SKsize)h;
    m_data     = new SKuint8[lim];

    skMemcpy(m_data, ptr, lim);
    skMemset(&m_metrics, 0, sizeof(SKglyphMetrics));
}

skGlyph::~skGlyph()
{
    delete[] m_data;
}

void skGlyph::setMetrics(const SKglyphMetrics& metrics)
{
    skMemcpy(&m_metrics, &metrics, sizeof(SKglyphMetrics));
}

void skGlyph::merge(skFont* font, skImage* dest, SKuint32 x, SKuint32 y)
{
    if (!m_data)
        return;

    SKuint8* ptr = m_data;
    SKuint32 ix, iy;

    for (iy = 0; iy < m_height; ++iy)
    {
        const SKint32 ny = y + iy;
        for (ix = 0; ix < m_width; ix++)
        {
            const SKint32 nx = x + ix;
            const SKuint8 ch = *ptr++;
            dest->setPixel(nx, ny, skPixel(ch, ch, ch, ch));
        }
    }
}

skFont::skFont() :
    skContextObj(),
    m_chars(nullptr),
    m_pointScale(0),
    m_image(nullptr),
    m_opts()
{
    m_opts.dpi       = 72;
    m_opts.tabSize   = 4;
    m_opts.scale     = skScalar(1.0);
    m_opts.filter    = SK_FILTER_NONE_LINEAR;
    m_opts.mipmap    = 0;
    m_opts.pixelSize = 0;
}

skFont::~skFont()
{
    delete[] m_chars;
    m_chars = nullptr;
    delete m_image;
    m_image = nullptr;
}

void skFont::getCharExtent(const char c, SKtextExtent* te) const
{
    if (!te || !m_chars)
        return;

    const skScalar scale = getRelativeScale();
    const Char     ch    = getChar(c);

    te->width           = ch.w * scale;
    te->height          = ch.h * scale;
    te->ascent          = ch.w * scale;
    te->descent         = ch.h * scale;
    te->externalLeading = ch.w * scale;
}

void skFont::getCharExtent(const char c, SKint32* w, SKint32* h) const
{
    if (!m_chars)
        return;
    if (!w && !h)
        return;

    const skScalar scale = getRelativeScale();

    if (w)
        *w = 0;

    if (h)
        *h = 0;

    if (w)
        *w = (SKint32)(getChar(c).w * scale);

    if (h)
        *h = (SKint32)(getChar(c).h * scale);
}

void skFont::getTextExtent(const char* str, SKint32 len, SKint32* w, SKint32* h) const
{
    if (!str || !m_chars || !w && !h)
        return;

    if (w)
        *w = 0;
    if (h)
        *h = 0;

    const skScalar scale = getRelativeScale();
    skScalar       xOffs = 0.f, yOffs = 0.f;
    const skScalar bh = getChar('0').h;
    const skScalar bw = getChar('0').w;

    for (SKint32 i = 0; i < len; i++)
    {
        const char  ch = str[i];
        const Char& fc = getChar(ch);

        if (ch == ' ')
            xOffs += bw * scale;
        else if (ch == '\t')
            xOffs += bw * skScalar(m_opts.tabSize) * scale;
        else if (ch == '\n')
        {
            yOffs += bh * scale;
            xOffs = 0;
        }
        else
            xOffs += fc.w * scale;

        if (w)
            *w = skMax<SKint32>(*w, (SKint32)xOffs);

        if (h)
            *h = skMax<SKint32>(*h, (SKint32)yOffs);
    }

    if (h && yOffs == 0.f)
        *h = (SKint32)(bh * scale);
}

void skFont::getTextExtentExt(const char*   str,
                              const SKint32 idx,
                              SKint32       len,
                              SKint32*      w,
                              SKint32*      h) const
{
    if (!str || !m_chars || !w && !h)
        return;

    if (w)
        *w = 0;

    if (h)
        *h = 0;

    const skScalar scale = getRelativeScale();
    skScalar       xOffs = 0.f, yOffs = 0.f;
    const skScalar bh = getChar('0').h;
    const skScalar bw = getChar('0').w;

    for (SKint32 i = 0; i < len && i < idx; i++)
    {
        const char  ch = str[i];
        const Char& fc = getChar(ch);

        if (ch == ' ')
            xOffs += bw * scale;
        else if (ch == '\t')
            xOffs += bw * skScalar(m_opts.tabSize) * scale;
        else if (ch == '\n')
        {
            yOffs += bh * scale;
            xOffs = 0;
        }
        else
            xOffs += fc.w * scale;

        if (w)
            *w = skMax<SKint32>(*w, (SKint32)xOffs);

        if (h)
            *h = skMax<SKint32>(*h, (SKint32)yOffs);
    }

    if (h && yOffs == 0.f)
        *h = (SKint32)(bh * scale);
}

SKint32 skFont::getAverageWidth(void)
{
    if (!m_chars || m_opts.average != 0)
        return m_opts.average;

    skScalar av = 0.f;

    const skScalar scale = getRelativeScale();

    for (SKint32 ch = CharStart; ch < CharEnd; ch++)
        av += getChar((char)ch).w * scale;

    av /= skScalar(CharTotal);
    m_opts.average = SKint32(av + SKscalar(0.5));
    return m_opts.average;
}

const skFont::Char& skFont::getChar(char ch) const
{
    if (ch >= CharStart && ch < CharEnd && m_chars != nullptr)
        return m_chars[ch - CharStart];
    return NullChar;
}

bool skFont::fromEnum(SKbuiltinFont font, SKuint32 size, SKuint32 dpi)
{
    switch (font)
    {
#ifdef Graphics_EXTRA_BUILTIN_FONTS
    case SK_FONT_SPC1:
        return loadTrueTypeFont(ROCKSALT_REGULAR, sizeof ROCKSALT_REGULAR, size, dpi);
    case SK_FONT_SPC2:
        return loadTrueTypeFont(CAVEAT_VARIABLEFONT_WGHT, sizeof CAVEAT_VARIABLEFONT_WGHT, size, dpi);
    case SK_FONT_UI:
        return loadTrueTypeFont(ROBOTO_REGULAR, sizeof ROBOTO_REGULAR, size, dpi);
    case SK_FONT_UI_LIGHT:
        return loadTrueTypeFont(ROBOTO_LIGHT, sizeof ROBOTO_LIGHT, size, dpi);
#endif
    case SK_FONT_DEFAULT_FIXED:
        return loadTrueTypeFont(DEJAVUMONO, sizeof DEJAVUMONO, size, dpi);
    case SK_FONT_MAX:
    case SK_FONT_DEFAULT:
        return loadTrueTypeFont(DEJAVU, sizeof DEJAVU, size, dpi);
    default:
        break;
    }
    return false;
}

bool skFont::fromFile(const char* path, SKuint32 size, SKuint32 dpi)
{
    const skFileStream fs(path, skStream::READ);
    if (!fs.isOpen())
        return false;

    SKsize   len  = fs.size();
    SKuint8* data = new SKuint8[len + 1];
    len           = fs.read(data, len);
    data[len]     = 0;

    const bool res = loadTrueTypeFont(data, len, size, dpi);
    delete[] data;
    return res;
}

bool skFont::fromMemory(const void* mem, SKuint32 len, SKuint32 size, SKuint32 dpi)
{
    return loadTrueTypeFont(mem, len, size, dpi);
}

void skFont::buildPath(skPath* path, const char* str, SKuint32 len, skScalar x, skScalar y)
{
    SK_CHECK_PARAM(path, SK_RETURN_VOID);
    SK_CHECK_PARAM(str, SK_RETURN_VOID);
    const skScalar fntScale = getRelativeScale();
    if (skIsZero(fntScale))
        return;

    const skContext& ctx   = ref();
    const skVector2& scale = ctx.getContextV(SK_CONTEXT_SCALE);
    const skVector2& bias  = ctx.getContextV(SK_CONTEXT_BIAS);

    const bool yIsUp   = ctx.getContextI(SK_Y_UP) == 1;
    const bool doScale = scale != skVector2::Unit;
    const bool doBias  = bias != skVector2::Zero;

    path->setContext(this->getContext());
    path->clear();

    const skScalar tx = skScalar(1.0) / skScalar(m_image->getWidth());
    const skScalar ty = skScalar(1.0) / skScalar(m_image->getHeight());

    skScalar xOffs = x;
    skScalar yOffs = y;

    const skScalar baseHeight = m_opts.yMax * fntScale;
    const skScalar baseWidth  = m_opts.xMax * fntScale;

    for (SKuint32 i = 0; i < len; i++)
    {
        const char  cCh = str[i];
        const Char& ch  = getChar(cCh);

        if (cCh == ' ')
        {
            xOffs += baseWidth * fntScale;
        }
        else if (cCh == '\t')
        {
            xOffs += baseWidth * m_opts.tabSize * fntScale;
        }
        else if (cCh == '\n')
        {
            if (yIsUp)
                yOffs -= baseHeight;
            else
                yOffs += baseHeight;

            xOffs = x;
        }
        else
        {
            if (!skIsZero(ch.w) && !skIsZero(ch.h))
            {
                skScalar txMin, txMax, tyMin, tyMax;
                skScalar vxMin, vxMax, vyMin, vyMax;

                const skScalar charWidth  = ch.w * fntScale;
                const skScalar charHeight = ch.h * fntScale;
                const skScalar charOffs   = ch.yOffs * fntScale;

                vxMin = xOffs;
                vxMax = xOffs + charWidth;

                if (yIsUp)
                {
                    vyMax = yOffs - charOffs;
                    vyMin = yOffs - (charHeight + charOffs);
                }
                else
                {
                    vyMax = yOffs + charOffs;
                    vyMin = yOffs + charHeight + charOffs;
                }

                if (doScale)
                {
                    vxMin *= scale.x;
                    vxMax *= scale.x;
                    vyMin *= scale.y;
                    vyMax *= scale.y;
                }

                if (doBias)
                {
                    vxMin += bias.x;
                    vxMax += bias.x;
                    vyMin += bias.y;
                    vyMax += bias.y;
                }

                xOffs += charWidth + ch.xOffs * fntScale;

                txMin = ch.x;
                tyMax = ch.y;
                txMax = ch.x + ch.w;
                tyMin = ch.y + ch.h;

                txMin *= tx;
                txMax *= tx;
                tyMin *= ty;
                tyMax *= ty;

                tyMin = skScalar(1.0) - tyMin;
                tyMax = skScalar(1.0) - tyMax;

                skVertex lt(vxMin, vyMin, txMin, tyMin);
                skVertex rb(vxMax, vyMax, txMax, tyMax);
                skVertex rt(vxMax, vyMin, txMax, tyMin);
                skVertex lb(vxMin, vyMax, txMin, tyMax);

                path->addVert(lt);
                path->addVert(rt);
                path->addVert(rb);
                path->addVert(rb);
                path->addVert(lb);
                path->addVert(lt);
            }
        }
    }
}

void skFont::getI(SKfontOptionEnum opt, SKint32* v) const
{
    if (opt == SK_FONT_FILTER)
        *v = (SKint32)m_opts.filter;
    else if (opt == SK_FONT_MIPMAP)
        *v = (SKint32)m_opts.mipmap;
    else if (opt == SK_FONT_SIZE)
        *v = (SKint32)m_opts.scale;
    else if (opt == SK_FONT_DPI)
        *v = (SKint32)m_opts.dpi;
    else if (opt == SK_FONT_TAB_SIZE)
        *v = (SKint32)m_opts.tabSize;
}

void skFont::setI(SKfontOptionEnum opt, SKint32 v)
{
    if (opt == SK_FONT_FILTER)
    {
        if (v > SK_FILTER_MIN && v < SK_FILTER_MAX)
        {
            if (m_image)
                m_image->setI(SK_IMAGE_FILTER, v);
            m_opts.filter = v;
        }
    }
    else if (opt == SK_FONT_MIPMAP)
    {
        if (m_image)
            m_image->setI(SK_IMAGE_MIPMAP, v != 0);
        m_opts.mipmap = v != 0 ? 1 : 0;
    }
    else if (opt == SK_FONT_SIZE)
        m_opts.scale = (skScalar)v;
    else if (opt == SK_FONT_TAB_SIZE)
        m_opts.tabSize = skMax(v, 0);
}

void skFont::getF(SKfontOptionEnum opt, skScalar* v) const
{
    if (opt != SK_FONT_SIZE)
    {
        SKint32 t;
        getI(opt, &t);
        *v = (skScalar)t;
    }
    else
        *v = m_opts.scale;
}

void skFont::setF(SKfontOptionEnum opt, skScalar v)
{
    if (opt != SK_FONT_SIZE)
        setI(opt, (SKint32)v);
    else
        m_opts.scale = v;
}

void skFont::loadGlyphs(struct FT_FaceRec_* face)
{
    m_glyphs.reserve(CharTotal);
    SKglyphMetrics metrics = {};

    m_opts.xMax    = 0;
    m_opts.yMax    = 0;
    m_opts.average = 0;

    SKint32 i, j = 0;
    for (i = 0; i < CharTotal; ++i)
    {
        if (FT_Load_Char(face, i + CharStart, FT_LOAD_COMPUTE_METRICS | FT_LOAD_RENDER))
            continue;

        FT_GlyphSlot slot = face->glyph;
        if (!slot)
            continue;
        FT_Render_Glyph(slot, FT_RENDER_MODE_LCD);

        SKuint8* imgBuf = face->glyph->bitmap.buffer;
        if (!imgBuf)
            continue;

        skGlyph* gylph = new skGlyph(imgBuf, slot->bitmap.width, slot->bitmap.rows);
        m_glyphs.push_back(gylph);

        j++;
        metrics.i        = i;
        metrics.advance  = FTINT(slot->metrics.horiAdvance);
        metrics.yBearing = FTINT(slot->metrics.horiBearingY);
        metrics.width    = FTINT(slot->metrics.width);

        gylph->setMetrics(metrics);

        const SKint32 cx = gylph->getWidth();
        const SKint32 cy = gylph->getHeight();

        if (m_opts.xMax < cx)
            m_opts.xMax = cx;
        if (m_opts.yMax < cy)
            m_opts.yMax = cy;

        m_opts.average += cx;
    }

    if (j > 0)
        m_opts.average /= j;

    skScalar sx, sy, sr;
    sr = skSqrt(skScalar(j));

    sx = skScalar(m_opts.xMax * j);
    sx /= sr;
    sx += skScalar(m_opts.xMax);

    sy = skScalar(m_opts.yMax * j);
    sy /= sr;
    sy += skScalar(m_opts.yMax);

    delete m_image;

    m_image = m_ctx->createInternalImage(
        (SKint32)sx,
        (SKint32)sy,
        SK_ALPHA);
}

bool skFont::loadTrueTypeFont(const void* mem, SKsize len, SKuint32 size, SKuint32 dpi)
{
    FT_Library lib;
    FT_Face    face;
    FT_Error   status;

    if ((status = FT_Init_FreeType(&lib)) != 0)
    {
        skLogd(LD_ERROR, "FreeType initialization failed.\n");
        skLogd(LD_WARN, FT_Error_String(status));
        return false;
    }

    if (FT_New_Memory_Face(lib, (const FT_Byte*)mem, (FT_Long)len, 0, &face))
    {
        skLogd(LD_ERROR, "Failed to load font face.\n");
        return false;
    }

    m_opts.average   = 0;
    m_opts.dpi       = dpi;
    m_opts.pixelSize = size;
    m_opts.scale     = (skScalar)size;

    if (FT_Set_Char_Size(face, FTI64(size), FTI64(size), dpi, dpi))
        return false;

    // loads the glyphs
    // computes the metrics
    // creates the image
    loadGlyphs(face);

    skImage* ima = m_image->getInternalImage();
    if (!ima)
    {
        skLogd(LD_ERROR, "Failed to get the image for the font.\n");
        return false;
    }

    m_chars = new Char[CharTotal];
    skMemset(m_chars, 0, sizeof(Char) * CharTotal);


    SKint32 x      = 0,
            y      = 0,
            rowMax = 0;

    SKuint32 i;
    for (i = 0; i < m_glyphs.size(); ++i)
    {
        skGlyph* glyph = m_glyphs[i];

        glyph->merge(this, ima, x, y);

        const SKglyphMetrics& metrics = glyph->getMertics();
        if (i >= CharTotal)
            continue;

        Char& ch = m_chars[metrics.i];
        ch.x     = (SKscalar)x;
        ch.y     = (SKscalar)y;
        ch.w     = (SKscalar)glyph->getWidth();
        ch.h     = (SKscalar)glyph->getHeight();
        ch.xOffs = (SKscalar)(metrics.advance - metrics.width);
        ch.yOffs = (SKscalar)(m_opts.yMax - metrics.yBearing);

        SKuint32 nx = glyph->getWidth() + Spacer;
        if (x + nx + m_opts.xMax >= ima->getWidth())
        {
            x = Spacer;
            y += m_opts.yMax + Spacer;
            rowMax = 0;
        }
        else
            x += nx;

        // clean up the temporary object
        delete glyph;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(lib);
    getAverageWidth();

    m_glyphs.clear();
    //m_image->save("FontTest.png");
    return true;
}
