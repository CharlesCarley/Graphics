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
#ifndef _skPaint_h_
#define _skPaint_h_

#include "skContextObject.h"

class skCachedProgram;




class skPaint : public skContextObj
{
private:
    friend class skOpenGLRenderer;

    SKbrushStyle     m_brushStyle;
    SKbrushMode      m_brushMode;
    SKpenStyle       m_penStyle;
    SKscalar         m_penWidth;
    skColor          m_brushColor;
    skColor          m_penColor;
    skColor          m_surfaceColor;
    skTexture*       m_brushPattern;
    SKint32          m_lineType;
    SKint8           m_autoClear;
    skCachedProgram* m_program;

public:
    skPaint();
    ~skPaint();

    bool autoClear()
    {
        return m_autoClear != 0;
    }

    void getI(SKpaintStyle opt, SKint32* v) const;

    void setI(SKpaintStyle opt, SKint32 v);

    void getF(SKpaintStyle opt, SKscalar* v) const;

    void setF(SKpaintStyle opt, SKscalar v);

    void getC(SKpaintStyle opt, SKuint32* v) const;

    void setC(SKpaintStyle opt, const skColor& v);

    void getT(SKpaintStyle opt, skTexture** v) const;

    void setT(SKpaintStyle opt, skTexture* v);
};

#endif  //_skPaint_h_
