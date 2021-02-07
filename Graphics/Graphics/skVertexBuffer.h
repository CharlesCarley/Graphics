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
#ifndef _skVertexBuffer_h_
#define _skVertexBuffer_h_

#include "Utils/skArray.h"

typedef enum skAttribute
{
    SK_ATTR_POSITION,
    SK_ATTR_TEXTURE0,
    SK_ATTR_NORMAL,
    SK_ATTR_COLORS,
} skAttribute;

typedef enum skAttributeType
{
    SK_FLOAT1_32 = 0,  //!< sizeof(float)*1
    SK_FLOAT2_32,      //!< sizeof(float)*2
    SK_FLOAT3_32,      //!< sizeof(float)*3
    SK_FLOAT4_32,      //!< sizeof(float)*4
    SK_UINT1_32,       //!< sizeof(int)  *1
} skAttributeType;

typedef enum skBufferMode
{
    SK_STREAM_DRAW,
    SK_STREAM_READ,
    SK_STREAM_COPY,
    SK_STATIC_DRAW,
    SK_STATIC_READ,
    SK_STATIC_COPY,
    SK_DYNAMIC_DRAW,
    SK_DYNAMIC_READ,
    SK_DYNAMIC_COPY,
} skBufferMode;

class skVertexBuffer
{
public:
    skVertexBuffer()  = default;
    virtual ~skVertexBuffer() = default;

    virtual SKsize addElement(SKuint32 name, SKuint32 type) = 0;

    virtual void write(const void* ptr, const SKuint32& sizeInBytes, const SKint32& mode) = 0;

    virtual void fill(SKuint32 op) const = 0;
};

#endif  //_skVertexBuffer_h_
