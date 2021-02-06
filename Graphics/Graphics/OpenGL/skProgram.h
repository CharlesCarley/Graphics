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
#ifndef _skProgram_h_
#define _skProgram_h_

#include "Math/skScalar.h"
#include "Utils/skArray.h"
#include "skGraphicsConfig.h"

class skProgram
{
protected:
    SKuint32 m_program;

public:
    skProgram();

    ~skProgram();

    bool compile(const char* vert,
                 const char* frag,
                 const char* vertName = "",
                 const char* fragName = "");

    bool compile(const skShaderSource& vertex,
                 const skShaderSource& fragment);

    void enable(bool val) const;

    void enable()
    {
        enable(true);
    }
    void disable()
    {
        enable(false);
    }

    void bindAttribute(const char* loc, SKuint32 attr) const;

    void setUniformMatrix(const char* name, skScalar* matrix) const;

    void setUniform4F(const char* name, skScalar* p) const;

    void setUniform1I(const char* name, SKuint32) const;

    void setUniform1F(const char* name, skScalar) const;

    void setUniformMatrix(SKuint32 loc, const skScalar* matrix) const;

    void setUniform4F(SKuint32 loc, const skScalar* p) const;

    void setUniform1I(SKuint32 loc, SKuint32) const;

    void setUniform1F(SKuint32 loc, skScalar) const;

    void getUniformLoc(const char* name, SKuint32* d) const;
};

#endif  //_skProgram_h_
