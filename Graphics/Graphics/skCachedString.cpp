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
#include "skCachedString.h"
#include "OpenGL/skImageOpenGL.h"
#include "skContext.h"
#include "skFont.h"

skCachedString::skCachedString()
{
    m_font = new skFont();
    m_path = new skPath();
    m_size = skVector2::Zero;
}

skCachedString::~skCachedString()
{
    if (m_font)
    {
        delete m_font;
        m_font = nullptr;
    }

    if (m_path)
    {
        delete m_path;
        m_path = nullptr;
    }
}
void skCachedString::displayString(const char* str, SKuint32 len, skScalar x, skScalar y)
{
    if (m_path->isEmpty())
        buildString(str, len, x, y);
}

void skCachedString::buildString(const char* str, SKuint32 len, skScalar x, skScalar y)
{
    const skContext& ctx   = ref();
    const skVector2  scale = ctx.getContextV(SK_CONTEXT_SCALE);

    // scale to specific point size
    const skScalar fntScale = m_font->getRelativeScale();
    if (skIsZero(fntScale))
        return;

    auto* img = (skImageOpenGL*)m_font->getImage();
    if (!img)
        return;

    m_path->setContext(this->getContext());
    m_path->clear();

    const skScalar t_xspace = 1.f / skScalar(img->getWidth());
    const skScalar t_yspace = 1.f / skScalar(img->getHeight());

    skScalar xoffs = x;
    skScalar yoffs = y;

    const skScalar bh = m_font->getChar('0').h * fntScale;
    const skScalar bw = m_font->getChar('0').w * fntScale;

    SKint32 tab = 4;
    m_font->getI(SK_FONT_TAB_SIZE, &tab);

    const char* poin = str;
    for (size_t i = 0; i < len; i++)
    {
        const char          cCh = poin[i];
        const skFont::Char& ch  = m_font->getChar(cCh);

        if (cCh == ' ')
        {
            xoffs += bw * fntScale;
            continue;
        }
        if (cCh == '\t')
        {
            xoffs += bw * tab * fntScale;
            continue;
        }
        if (cCh == '\n')
        {
            yoffs += bh;
            xoffs = x;
            //const skFont::Char& cnx = m_font->getChar( poin[i+1 < len-1 ? i+1 : i] );
            continue;
        }

        // unknown char
        if (ch.w == 0.0 && ch.h == 0.0)
            continue;

        skScalar v_xmin, v_xmax, v_ymin, v_ymax;

        // vert coords
        v_xmin = xoffs;
        v_ymax = yoffs;
        v_xmax = xoffs + ch.w * fntScale;
        v_ymin = yoffs + ch.h * fntScale;

        v_xmin *= scale.x;
        v_xmax *= scale.x;

        v_ymin *= scale.x;
        v_ymax *= scale.x;

        skScalar cx, cy;
        cx = xoffs;
        cy = yoffs;

        xoffs += ch.w * fntScale;

        skScalar t_xmin, t_xmax, t_ymin, t_ymax;
        t_xmin = ch.x;
        t_ymax = ch.y;
        t_xmax = ch.x + ch.w;
        t_ymin = ch.y + ch.h;
        t_xmin *= t_xspace;
        t_xmax *= t_xspace;
        t_ymin *= t_yspace;
        t_ymax *= t_yspace;

        // inverted y
        t_ymin = skScalar(1.f) - t_ymin;
        t_ymax = skScalar(1.f) - t_ymax;

        skVertex lt(v_xmin, v_ymin, t_xmin, t_ymin);
        skVertex rb(v_xmax, v_ymax, t_xmax, t_ymax);
        skVertex rt(v_xmax, v_ymin, t_xmax, t_ymin);
        skVertex lb(v_xmin, v_ymax, t_xmin, t_ymax);

        m_path->addVert(lt);
        m_path->addVert(rt);
        m_path->addVert(rb);
        m_path->addVert(rb);
        m_path->addVert(lb);
        m_path->addVert(lt);
    }

    SKint32 w, h;
    m_font->getTextExtents(str, len, &w, &h);
    m_size.x = (skScalar)w;
    m_size.y = (skScalar)h;
}

skVector2 skCachedString::getV(SKstringOptionEnum op) const
{
    switch (op)
    {
    case SK_STRING_SIZE:
    case SK_STRING_EXTENTS:
        return m_size;
    }
    return skVector2(SK_NO_STATUS, SK_NO_STATUS);
}
