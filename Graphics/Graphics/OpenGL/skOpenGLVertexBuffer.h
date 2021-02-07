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
#ifndef _skOpenGLVertexBuffer_h_
#define _skOpenGLVertexBuffer_h_

#include "Utils/skArray.h"
#include "skVertexBuffer.h"

typedef struct skVertexElement
{
    SKuint32 name, type;
} skVertexElement;

class skOpenGLVertexBuffer : public skVertexBuffer
{
private:
    SKuint32 m_bufId;
    SKuint32 m_stride;
    SKuint32 m_totalFill;
    SKuint32 m_size;
    SKuint32 m_mode;

    skArray<skVertexElement> m_elements;

public:
    skOpenGLVertexBuffer();
    ~skOpenGLVertexBuffer() override;

    const SKuint32& getId(void) const
    {
        return m_bufId;
    }

    const SKuint32& getMode(void) const
    {
        return m_mode;
    }

    const SKuint32& getSize(void) const
    {
        return m_size;
    }

    SKsize addElement(SKuint32 name, SKuint32 type) override;

    void write(const void* ptr, const SKuint32& sizeInBytes, const SKint32& mode) override;

    void fill(SKuint32 op) const override;
};

#endif  //_skOpenGLVertexBuffer_h_
