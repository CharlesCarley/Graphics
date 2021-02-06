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
#ifndef _skContour_h_
#define _skContour_h_

#include "Utils/Config/skConfig.h"
#include "Utils/skArray.h"
#include "skDefs.h"

template <typename T>
class skVertexT
{
public:
    skVertexT() :
        x(0),
        y(0),
        u(0),
        v(0)
    {
    }

    skVertexT(const skVertexT& v) :
        x(v.x),
        y(v.y),
        u(v.u),
        v(v.v)
    {
    }

    skVertexT(const T& vx, const T& vy) :
        x(vx),
        y(vy),
        u(0),
        v(0)
    {
    }

    explicit skVertexT(const skVector2& v) :
        x(v.x),
        y(v.y),
        u(0),
        v(0)
    {
    }

    skVertexT(const T& vx, const T& vy, const T& ru, const T& rv) :
        x(vx),
        y(vy),
        u(ru),
        v(rv)
    {
    }

    skVertexT& operator=(const skVertexT& rhs)
    {
        if (this != &rhs)
        {
            x = rhs.x;
            y = rhs.y;
            u = rhs.u;
            v = rhs.v;
        }
        return *this;
    }

    T x, y;
    T u, v;
};

typedef skVertexT<SKint32> skVertexI;
typedef skArray<skVertexI> skPolyI;

typedef skVertexT<skScalar> skVertex;
typedef skArray<skVertex>   skPoly;

class skContour
{
public:
    skContour() = default;
    skContour(const skContour& o) = default;

    ~skContour()
    {
        vertices.clear();
    }

    skContour& operator=(const skContour& rhs)
    {
        if (this != &rhs)
            vertices = rhs.vertices;
        return *this;
    }

    void push_back(const skVertex& v)
    {
        vertices.push_back(v);
    }

    void clear()
    {
        vertices.resizeFast(0);
    }

    bool empty() const
    {
        return vertices.empty();
    }

    void reserve(SKuint32 nr)
    {
        vertices.reserve(nr);
    }

    skPoly vertices;
};

typedef skArray<skContour> skContourArray;

#endif  //_skContour_h_
