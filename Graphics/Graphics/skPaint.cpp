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
    m_brushColor   = skColor(1, 1, 1, 1);
    m_penColor     =  skColor(0, 0, 0, 1);
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
}

void skPaint::setI(SKpaintStyle opt, SKint32 v)
{
}


