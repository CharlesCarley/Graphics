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
#include "Utils/skFileStream.h"
#include "Utils/skLogger.h"
#include "Utils/skMemoryUtils.h"
#include "Utils/skPlatformHeaders.h"
#include "skBuiltinFonts.h"
#include "skContext.h"
#include "skTexture.h"

// TODO clean and check all of this...


#define FTINT(x) ((x) >> 6)
#define FTI64(x) ((x) << 6)

const SKint32      CharStart = 32;
const SKint32      CharEnd   = 127;
const SKint32      CharTotal = CharEnd - CharStart;
const SKint32      Spacer    = 3;
const skFont::Char NullChar  = {0.f, 0.f, 0.f, 0.f, 0.f};


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

void skFont::getCharExtents(const char c, SKtextExtent* te) const
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

void skFont::getCharExtents(const char c, SKint32* w, SKint32* h) const
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

void skFont::getTextExtents(const char* str, SKint32 len, SKint32* w, SKint32* h) const
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

void skFont::getTextExtentsExt(const char*   str,
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
    case SK_DEFAULT_FIXED:
        return loadTTF(DEJAVUMONO, sizeof DEJAVUMONO, size, dpi);
    case SK_DEFAULT:
    case SK_CMN_FONT_SPC1:
    case SK_CMN_FONT_SPC2:
    case SK_FONT_UI:
    case SK_FONT_UI_LIGHT:
    case SK_FONT_MAX:
        return loadTTF(DEJAVU, sizeof DEJAVU, size, dpi);
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

    const bool res = loadTTF(data, len, size, dpi);
    delete[] data;
    return res;
}

bool skFont::fromMemory(const void* mem, SKuint32 len, SKuint32 size, SKuint32 dpi)
{
    return loadTTF(mem, len, size, dpi);
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
    {
        *v = m_opts.scale;
    }
}

void skFont::setF(SKfontOptionEnum opt, skScalar v)
{
    if (opt != SK_FONT_SIZE)
        setI(opt, (SKint32)v);
    else
        m_opts.scale = v;
}

void Font_calculateLimits(FT_Face        face,
                          const SKint32& space,
                          SKint32&       w,
                          SKint32&       h,
                          SKint32&       xMax,
                          SKint32&       yMax,
                          SKint32&       xAvr)
{
    xMax = 0;
    yMax = 0;
    xAvr = 0;

    SKint32 i;
    for (i = CharStart; i < CharEnd; ++i)
    {
        if (FT_Load_Char(face, i, FT_LOAD_DEFAULT))
            continue;

        FT_GlyphSlot slot = face->glyph;
        if (!slot)
            continue;

        const SKint32 cx = slot->metrics.horiBearingX + slot->metrics.width;
        const SKint32 cy = slot->metrics.height;

        if (yMax < cy)
            yMax = cy;
        if (xMax < cx)
            xMax = cx;

        xAvr += cx;
    }

    xMax = FTINT(xMax);
    yMax = FTINT(yMax);
    xAvr = FTINT(xAvr);
    xAvr /= CharTotal;

    skScalar sx, sy, sr;
    sr = skSqrt(skScalar(CharTotal));

    sx = skScalar((xMax + space) * CharTotal);
    sx /= sr;
    sx += skScalar(xMax);

    sy = skScalar((yMax + space) * CharTotal);
    sy /= sr;
    sy += skScalar(yMax);

    w = (SKint32)sx;
    h = (SKint32)sy;

    w = skMath::pow2(w);
    h = skMath::pow2(h);
}

bool skFont::loadTTF(const void* mem, SKuint32 len, SKuint32 size, SKuint32 dpi)
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
        skLogd(LD_INFO, "Failed to load .\n");
        return false;
    }

    m_opts.average   = 0;
    m_opts.dpi       = dpi;
    m_opts.pixelSize = size;
    m_opts.scale     = (skScalar)size;

    if (FT_Set_Char_Size(face, FTI64(size), FTI64(size), dpi, dpi))
        return false;

    SKuint32 ix, iy;
    SKint32  x, y, w, h;
    SKint32  i;

    Font_calculateLimits(face,
                         Spacer,
                         w,
                         h,
                         m_opts.xMax,
                         m_opts.yMax,
                         m_opts.average);

    delete m_image;

    m_image      = m_ctx->createInternalImage(w, h, SK_ALPHA);
    skImage* ima = m_image->getInternalImage();

    m_chars = new Char[CharTotal];

    x = Spacer;
    y = Spacer;
    for (i = CharStart; i < CharEnd; ++i)
    {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER))
            continue;

        if (!face->glyph)
            continue;

        SKuint8* imgBuf = face->glyph->bitmap.buffer;
        if (!imgBuf)
            continue;

        FT_GlyphSlot slot = face->glyph;
        if (!slot)
            continue;

        FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

        const SKint32  yBearing = FTINT(slot->metrics.horiBearingY);
        const SKint32  xBearing = FTINT(slot->metrics.horiBearingX);
        const SKint32  advance  = FTINT(slot->advance.x);
        const SKint32  yOffs    = m_opts.yMax - yBearing;
        const SKuint32 iHeight  = slot->bitmap.rows;

        for (iy = 0; iy < iHeight && imgBuf; ++iy)
        {
            const SKint32 ny = y + iy;

            for (ix = 0; ix < slot->bitmap.width; ix++)
            {
                const SKint32 nx = x + ix + xBearing;
                const SKuint8 ch = *imgBuf++;
                ima->setPixel(nx, ny, skPixel(ch, ch, ch, ch));
            }
        }

        Char& ch = m_chars[i - CharStart];
        ch.x     = (SKscalar)x;
        ch.y     = (SKscalar)y;
        ch.w     = (SKscalar)advance;
        ch.h     = (SKscalar)m_opts.yMax;
        ch.o     = (SKscalar)yOffs;

        const SKint32 nx = m_opts.xMax + Spacer;
        x += nx;
        if (x + nx > w)
        {
            x = Spacer;
            y += m_opts.yMax + Spacer;
        }
    }

    FT_Done_Face(face);
    FT_Done_FreeType(lib);
    getAverageWidth();

    //m_image->save("FontTest.png");
    return true;
}
