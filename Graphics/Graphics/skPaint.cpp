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
#include "skPaint.h"
#include "OpenGL/skProgram.h"

skPaint::skPaint()
{
    m_brushMode    = SK_BM_REPLACE;
    m_brushStyle   = SK_BS_SOLID;
    m_penStyle     = SK_PS_SOLID;
    m_penWidth     = 1;
    m_surfaceColor = skColor(1, 1, 1, 1);
    m_brushColor   = skColor(1, 1, 1, 1);
    m_penColor     = skColor(0, 0, 0, 1);
    m_brushPattern = nullptr;
    m_program      = nullptr;
    m_lineType     = SK_LINE_LOOP;
    m_autoClear    = 0;
}

skPaint::~skPaint()
{
}

void skPaint::getI(SKpaintStyle opt, SKint32* v) const
{
    SK_CHECK_PARAM(v, SK_RETURN_VOID);

    switch (opt)
    {
    case SK_PEN_STYLE:
        *v = (SKint32)m_penStyle;
        break;
    case SK_BRUSH_STYLE:
        *v = (SKint32)m_brushStyle;
        break;
    case SK_BRUSH_MODE:
        *v = (SKint32)m_brushMode;
        break;
    case SK_PEN_WIDTH:
        *v = (SKint32)m_penWidth;
        break;
    case SK_LINE_TYPE:
        *v = (SKint32)m_lineType;
        break;
    case SK_AUTO_CLEAR:
        *v = (SKint32)m_autoClear;
        break;
    default:
        break;
    }
}

void skPaint::setI(SKpaintStyle opt, SKint32 v)
{
    switch (opt)
    {
    case SK_PEN_STYLE:
        m_penStyle = (SKpenStyle)skClamp<SKint32>(v, SK_PS_MIN + 1, SK_PS_MAX - 1);
        break;
    case SK_BRUSH_STYLE:
        m_brushStyle = (SKbrushStyle)skClamp<SKint32>(v, SK_BS_MIN + 1, SK_BS_MAX - 1);
        break;
    case SK_BRUSH_MODE:
        m_brushMode = skClamp<SKint32>(v, SK_BM_MIN + 1, SK_BM_MAX - 1);
        break;
    case SK_PEN_WIDTH:
        m_penWidth = skMax<SKscalar>((SKscalar)v, 0.f);
        break;
    case SK_LINE_TYPE:
        m_lineType = skClamp<SKint32>(v, SK_LT_MIN + 1, SK_LT_MAX - 1);
        break;
    case SK_AUTO_CLEAR:
        m_autoClear = (SKint8)v != 0;
        break;
    default:
        break;
    }
}

void skPaint::getF(SKpaintStyle opt, SKscalar* v) const
{
    SK_CHECK_PARAM(v, SK_RETURN_VOID);

    switch (opt)
    {
    case SK_PEN_STYLE:
        *v = (SKscalar)m_penStyle;
        break;
    case SK_BRUSH_STYLE:
        *v = (SKscalar)m_brushStyle;
        break;
    case SK_BRUSH_MODE:
        *v = (SKscalar)m_brushMode;
        break;
    case SK_PEN_WIDTH:
        *v = m_penWidth;
        break;
    case SK_AUTO_CLEAR:
        *v = (SKscalar)m_autoClear;
        break;
    case SK_LINE_TYPE:
        *v = (SKscalar)m_lineType;
        break;
    default:
        break;
    }
}

void skPaint::setF(SKpaintStyle opt, SKscalar v)
{
    switch (opt)
    {
    case SK_PEN_STYLE:
        m_penStyle = (SKpenStyle)skClamp<SKint32>((SKint32)v, SK_PS_MIN + 1, SK_PS_MAX - 1);
        break;
    case SK_BRUSH_STYLE:
        m_brushStyle = (SKbrushStyle)skClamp<SKint32>((SKint32)v, SK_BS_MIN + 1, SK_BS_MAX - 1);
        break;
    case SK_BRUSH_MODE:
        m_brushMode = skClamp<SKint32>((SKint32)v, SK_BM_MIN + 1, SK_BM_MAX - 1);
        break;
    case SK_PEN_WIDTH:
        m_penWidth = skMax(v, 0.f);
        break;
    case SK_LINE_TYPE:
        m_lineType = skClamp<SKint32>((SKint32)v, SK_LT_MIN + 1, SK_LT_MAX - 1);
        break;
    case SK_AUTO_CLEAR:
        m_autoClear = (SKuint8)((int)v ? 1 : 0);
        break;
    default:
        break;
    }
}

void skPaint::setC(SKpaintStyle opt, const skColor& v)
{
    switch (opt)
    {
    case SK_BRUSH_COLOR:
        m_brushColor = v;
        break;
    case SK_PEN_COLOR:
        m_penColor = v;
        break;
    case SK_SURFACE_COLOR:
        m_surfaceColor = v;
        break;
    default:
        break;
    }
}

void skPaint::getC(SKpaintStyle opt, SKuint32* v) const
{
    SK_CHECK_PARAM(v, SK_RETURN_VOID);
    switch (opt)
    {
    case SK_BRUSH_COLOR:
        *v = m_brushColor.asInt();
        break;
    case SK_PEN_COLOR:
        *v = m_penColor.asInt();
        break;
    case SK_SURFACE_COLOR:
        *v = m_surfaceColor.asInt();
        break;
    default:
        break;
    }
}

void skPaint::getT(SKpaintStyle opt, skTexture** v) const
{
    SK_CHECK_PARAM(v, SK_RETURN_VOID);
    if (opt == SK_BRUSH_PATTERN)
        *v = m_brushPattern;
}

void skPaint::setT(SKpaintStyle opt, skTexture* v)
{
    // v is allowed to be null
    if (opt == SK_BRUSH_PATTERN)
        m_brushPattern = v;
}
