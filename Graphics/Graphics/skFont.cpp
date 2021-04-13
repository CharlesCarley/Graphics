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
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_ERRORS_H

#include "skFont.h"
#include "Utils/skFileStream.h"
#include "Utils/skLogger.h"
#include "Utils/skMemoryUtils.h"
#include "Utils/skPlatformHeaders.h"
#include "skContext.h"
#include "skGlyph.h"
#include "skPath.h"
#include "skTexture.h"
#ifndef Graphics_NO_BUILTIN
#include "skBuiltinFonts.h"
#endif

// TODO: clean and check the extent functions.
//       naming / spacing / alignment could be more precise too.
const SKint32      CharStart = 32;
const SKint32      CharEnd   = 127;
const SKint32      CharTotal = CharEnd - CharStart;
const SKint32      Spacer    = 2;
const skFont::Char NullChar  = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};

#define FTINT(x) ((x) >> 6)
#define FTI64(x) ((x) << 6)

skFont::skFont() :
    skContextObj(),
    m_chars(nullptr),
    m_image(nullptr),
    m_opts()
{
    m_opts.dpi       = 72;
    m_opts.tabSize   = 4;
    m_opts.scale     = skScalar(1.0);
    m_opts.filter    = SK_FILTER_BI_LINEAR;
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
    if (!str || !m_chars || (!w && !h))
        return;

    if (w)
        *w = 0;
    if (h)
        *h = 0;

    const skScalar scale = getRelativeScale();

    skScalar xOffs = 0.f, yOffs = 0.f;

    const skScalar baseHeight = m_opts.yMax * scale;
    const skScalar baseWidth  = m_opts.xMax * scale;

    if (len <= 0)
        len = (SKint32)strlen(str);

    for (SKint32 i = 0; i < len; i++)
    {
        const char  ch = str[i];
        const Char& fc = getChar(ch);

        if (ch == ' ')
            xOffs += baseWidth;
        else if (ch == '\t')
            xOffs += baseWidth * (skScalar(m_opts.tabSize) * scale);
        else if (ch == '\n')
        {
            yOffs += baseHeight * scale;
            xOffs = 0;
        }
        else
            xOffs += fc.w * scale + fc.xOffs * scale;

        if (w)
            *w = skMax<SKint32>(*w, (SKint32)xOffs);

        if (h)
            *h = skMax<SKint32>(*h, (SKint32)yOffs);
    }

    if (h && yOffs == 0.f)
        *h = (SKint32)baseHeight;
}

void skFont::getTextExtentExt(const char*   str,
                              const SKint32 idx,
                              SKint32       len,
                              SKint32*      w,
                              SKint32*      h) const
{
    if (!str || !m_chars || (!w && !h))
        return;

    if (w)
        *w = 0;

    if (h)
        *h = 0;

    const skScalar scale = getRelativeScale();

    skScalar xOffs = 0.f, yOffs = 0.f;

    const skScalar bh = getChar('0').h;
    const skScalar bw = getChar('0').w;

    if (len <= 0)
        len = (SKint32)strlen(str);

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
#ifndef Graphics_NO_BUILTIN
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
#endif
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

void skFont::buildPath(skPath* path, const char* str, SKuint32 len, skScalar x, skScalar y) const
{
    SK_CHECK_PARAM(path, SK_RETURN_VOID);
    SK_CHECK_PARAM(str, SK_RETURN_VOID);

    const skScalar fntScale = getRelativeScale();
    if (skIsZero(fntScale))
        return;

    const bool yIsUp = m_ctx->yIsUp();

    path->setContext(this->getContext());
    path->clear();

    const skScalar tx = skScalar(1.0) / skScalar(m_image->getWidth());
    const skScalar ty = skScalar(1.0) / skScalar(m_image->getHeight());

    skScalar xOffs = x;
    skScalar yOffs = y;

    const skScalar baseHeight = getChar('0').h * fntScale;
    const skScalar baseWidth  = getChar('0').w * fntScale;

    for (SKuint32 i = 0; i < len; i++)
    {
        const char  cCh = str[i];
        const Char& ch  = getChar(cCh);

        if (cCh == ' ')
        {
            xOffs += baseWidth;
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

                path->addVertex(lt);
                path->addVertex(rt);
                path->addVertex(rb);
                path->addVertex(rb);
                path->addVertex(lb);
                path->addVertex(lt);
            }
        }
    }
}

skScalar skFont::getPixelSize(void) const
{
    return skScalar(FTINT(m_opts.pixelSize * m_opts.dpi));
}

void skFont::getI(SKfontOptionEnum opt, SKint32* v) const
{
    switch (opt)
    {
    case SK_FONT_FILTER:
        *v = (SKint32)m_opts.filter;
        break;
    case SK_FONT_MIPMAP:
        *v = (SKint32)m_opts.mipmap;
        break;
    case SK_FONT_SIZE:
        *v = (SKint32)m_opts.scale;
        break;
    case SK_FONT_PIXEL_SIZE:
        *v = (SKint32)m_opts.pixelSize;
        break;
    case SK_FONT_RELATIVE_SCALE:
        *v = (SKint32)getRelativeScale();
        break;
    case SK_FONT_DPI:
        *v = (SKint32)m_opts.dpi;
        break;
    case SK_FONT_TAB_SIZE:
        *v = (SKint32)m_opts.tabSize;
        break;
    default:
        break;
    }
}

void skFont::setI(SKfontOptionEnum opt, SKint32 v)
{
    switch (opt)
    {
    case SK_FONT_FILTER:
        m_opts.filter = skClamp<SKint32>(v, SK_FILTER_MIN + 1, SK_FILTER_MAX - 1);
        if (m_image)
            m_image->setI(SK_IMAGE_FILTER, m_opts.filter);
        break;
    case SK_FONT_MIPMAP:
        m_opts.mipmap = v != 0 ? 1 : 0;
        if (m_image)
            m_image->setI(SK_IMAGE_MIPMAP, m_opts.mipmap);
        break;
    case SK_FONT_SIZE:
        m_opts.scale = skMax<skScalar>((skScalar)v, skScalar(1));
        break;
    case SK_FONT_TAB_SIZE:
        m_opts.tabSize = skMax(v, 0);
        break;
    default:
        break;
    }
}

void skFont::getF(SKfontOptionEnum opt, skScalar* v) const
{
    switch (opt)
    {
    case SK_FONT_DPI:
    case SK_FONT_FILTER:
    case SK_FONT_MIPMAP:
    case SK_FONT_PIXEL_SIZE:
    case SK_FONT_TAB_SIZE:
    {
        SKint32 r = SK_NO_STATUS;
        getI(opt, &r);
        *v = (skScalar)r;
        break;
    }
    case SK_FONT_SIZE:
        *v = m_opts.scale;
        break;
    case SK_FONT_RELATIVE_SCALE:
        *v = getRelativeScale();
        break;
    default:
        break;
    }
}

void skFont::setF(SKfontOptionEnum opt, skScalar v)
{
    switch (opt)
    {
    case SK_FONT_DPI:
    case SK_FONT_FILTER:
    case SK_FONT_MIPMAP:
    case SK_FONT_PIXEL_SIZE:
    case SK_FONT_TAB_SIZE:
    {
        setI(opt, (SKint32)v);
        break;
    }
    case SK_FONT_SIZE:
        m_opts.scale = skMax<skScalar>(v, 1);
        break;
    default:
        break;
    }
}

void skFont::loadGlyphs(struct FT_FaceRec_* face)
{
    m_glyphs.reserve(CharTotal);
    SKglyphMetrics metrics = {};

    m_opts.xMax    = 0;
    m_opts.yMax    = 0;
    m_opts.average = 0;

    SKint32 j = 0;
    for (SKint32 i = 0; i < CharTotal; ++i)
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

        skGlyph* glyph = new skGlyph(imgBuf, slot->bitmap.width, slot->bitmap.rows);
        m_glyphs.push_back(glyph);

        j++;
        metrics.i        = i;
        metrics.advance  = FTINT(slot->metrics.horiAdvance);
        metrics.yBearing = FTINT(slot->metrics.horiBearingY);
        metrics.width    = FTINT(slot->metrics.width);

        glyph->setMetrics(metrics);

        const SKint32 cx = glyph->getWidth();
        const SKint32 cy = glyph->getHeight();

        if (m_opts.xMax < cx)
            m_opts.xMax = cx;
        if (m_opts.yMax < cy)
            m_opts.yMax = cy;

        m_opts.average += cx;
    }

    if (j > 0)
        m_opts.average /= j;

    const skScalar sr = skSqrt(skScalar(j));

    skScalar sx = skScalar(m_opts.xMax * j);
    sx /= sr;
    sx += skScalar(m_opts.xMax);

    skScalar sy = skScalar(m_opts.yMax * j);
    sy /= sr;
    sy += skScalar(m_opts.yMax);

    delete m_image;

#if SK_PLATFORM == SK_PLATFORM_EMSCRIPTEN
    m_image = m_ctx->createInternalImage(
        (SKint32)skMath::pow2((SKint32)sx),
        (SKint32)skMath::pow2((SKint32)sy),
        SK_RGBA);
#else
    m_image = m_ctx->createInternalImage(
        (SKint32)sx,
        (SKint32)sy,
        SK_ALPHA);

#endif
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

    if ((status = FT_New_Memory_Face(lib, (const FT_Byte*)mem, (FT_Long)len, 0, &face)) != 0)
    {
        skLogd(LD_ERROR, "Failed to load font face.\n");
        skLogd(LD_WARN, FT_Error_String(status));
        return false;
    }

    m_opts.average   = 0;
    m_opts.dpi       = dpi;
    m_opts.pixelSize = size;
    m_opts.scale     = (skScalar)size;

    if ((status = FT_Set_Char_Size(face, FTI64(size), FTI64(size), dpi, dpi)) != 0)
    {
        skLogd(LD_ERROR, "Failed to set the character size.\n");
        skLogd(LD_WARN, FT_Error_String(status));
        return false;
    }

    // loads the glyphs
    // computes the metrics
    // creates the image
    loadGlyphs(face);

    skImage* ima = m_image->getInternalImage();
    if (!ima)
    {
        skLogd(LD_ERROR, "Failed to create the font image.\n");
        return false;
    }

    m_chars = new Char[CharTotal];
    skMemset(m_chars, 0, sizeof(Char) * CharTotal);

    SKint32 x = 0,
            y = 0;

    for (SKuint32 i = 0; i < m_glyphs.size(); ++i)
    {
        skGlyph* glyph = m_glyphs[i];

        glyph->merge(ima, x, y);

        const SKglyphMetrics& metrics = glyph->getMetrics();
        if (i >= CharTotal)
        {
            delete glyph;
            continue;
        }

        Char& ch = m_chars[metrics.i];
        ch.x     = (SKscalar)x;
        ch.y     = (SKscalar)y;
        ch.w     = (SKscalar)glyph->getWidth();
        ch.h     = (SKscalar)glyph->getHeight();
        ch.xOffs = (SKscalar)(metrics.advance - metrics.width);
        ch.yOffs = (SKscalar)(m_opts.yMax - metrics.yBearing);

        const SKuint32 nx = glyph->getWidth() + Spacer;
        if (x + nx + m_opts.xMax >= ima->getWidth())
        {
            x = Spacer;
            y += m_opts.yMax + Spacer;
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
