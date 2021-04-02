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
#include "skOpenGLVertexBuffer.h"
#include "Window/OpenGL/skOpenGL.h"


static SKuint32 skOpenGLGetAttributeSize(SKuint32 type)
{
    switch (type)
    {
    case SK_FLOAT1_32:
    case SK_FLOAT2_32:
    case SK_FLOAT3_32:
    case SK_FLOAT4_32:
        return (SKuint32)sizeof(float) * (type + 1);
    default:
        return 0;
    }
}

static SKuint32 skOpenGLGetAttributeTypeSize(SKuint32 type)
{
    switch (type)
    {
    case SK_FLOAT1_32:
    case SK_FLOAT2_32:
    case SK_FLOAT3_32:
    case SK_FLOAT4_32:
        return type + 1;
    default:
        return 0;
    }
}

static SKsize skOpenGLGetAttributeType(SKuint32 type)
{
    switch (type)
    {
    case SK_FLOAT1_32:
    case SK_FLOAT2_32:
    case SK_FLOAT3_32:
    case SK_FLOAT4_32:
        return GL_FLOAT;
    default:
        return 0;
    }
}

static SKsize skOpenGLGetBufferMode(SKsize type)
{
    switch (type)
    {
    case SK_STREAM_DRAW:
        return GL_STREAM_DRAW;
    case SK_STREAM_READ:
        return GL_STREAM_READ;
    case SK_STREAM_COPY:
        return GL_STREAM_COPY;
    case SK_STATIC_DRAW:
        return GL_STATIC_DRAW;
    case SK_STATIC_READ:
        return GL_STATIC_READ;
    case SK_STATIC_COPY:
        return GL_STATIC_COPY;
    case SK_DYNAMIC_DRAW:
        return GL_DYNAMIC_DRAW;
    case SK_DYNAMIC_READ:
        return GL_DYNAMIC_READ;
    case SK_DYNAMIC_COPY:
        return GL_DYNAMIC_COPY;
    default:
        return GL_DYNAMIC_DRAW;
    }
}

skOpenGLVertexBuffer::skOpenGLVertexBuffer()
{
    m_bufId     = 0;
    m_size      = 0;
    m_mode      = 0;
    m_stride    = 0;
    m_totalFill = 0;
}

skOpenGLVertexBuffer::~skOpenGLVertexBuffer()
{
    if (m_bufId)
        glDeleteBuffers(1, &m_bufId);
}

SKsize skOpenGLVertexBuffer::addElement(SKuint32 name, SKuint32 type)
{
    if (skOpenGLGetAttributeSize(type) != 0)
    {
        skVertexElement t;
        t.name = name;
        t.type = type;

        m_elements.push_back(t);
        m_stride += skOpenGLGetAttributeSize(type);

        return m_stride;
    }
    return 0;
}

void skOpenGLVertexBuffer::write(const void* ptr, const SKuint32& sizeInBytes, const SKint32& mode)
{
    if (!ptr || m_elements.empty())
        return;

    if (!m_bufId)
        glGenBuffers(1, &m_bufId);
    glBindBuffer(GL_ARRAY_BUFFER, m_bufId);

    m_totalFill = 0;
    m_size      = sizeInBytes;
    m_mode      = mode;

    glBufferData(GL_ARRAY_BUFFER, m_size, ptr, (GLenum)skOpenGLGetBufferMode(m_mode));

    const skArray<skVertexElement>::PointerType elements = m_elements.ptr();

    SKuint32       i    = 0;
    const SKuint32 size = m_elements.size();

    while (i < size)
    {
        skVertexElement& ele = elements[i++];
        glVertexAttribPointer(ele.name,
                              skOpenGLGetAttributeTypeSize(ele.type),
                              (GLenum)skOpenGLGetAttributeType(ele.type),
                              GL_FALSE,
                              m_stride,
                              (GLvoid*)(SKsize)m_totalFill);

        glEnableVertexAttribArray(ele.name);
        m_totalFill += skOpenGLGetAttributeSize(ele.type);
    }

    if (m_totalFill != 0)
        m_totalFill = m_size / m_totalFill;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void skOpenGLVertexBuffer::fill(SKuint32 op) const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_bufId);
    glDrawArrays(op, 0, m_totalFill);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
