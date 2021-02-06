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
#ifndef _skCachedProgram_h_
#define _skCachedProgram_h_

#include "Math/skMatrix4.h"
#include "skProgram.h"

class skCachedProgram : public skProgram
{
protected:
    SKuint32 m_zOrder;
    SKuint32 m_viewProj;
    SKuint32 m_mode;
    SKuint32 m_surface;
    SKuint32 m_brush;
    SKuint32 m_ima;

public:
    skCachedProgram();
    
    ~skCachedProgram();

    void setZOrder(skScalar z);

    void setViewProj(const skMatrix4& vProj);

    void setImage(SKuint32 ima);

    void setMode(SKuint32 m);

    void setSurface(const skScalar* p);

    void setBrush(const skScalar* p);
};

#endif  //_skCachedProgram_h_
