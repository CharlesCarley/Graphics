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
#ifndef _skPath_h_
#define _skPath_h_

#include "Math/skBoundingBox2D.h"
#include "skContextObject.h"
#include "skContour.h"
class skVertexBuffer;

class skPath : public skContextObj
{
private:
    skVertex        m_cur;
    skVertex        m_mov;
    skBoundingBox2D m_bounds;
    skContour*      m_contour;
    skVector2       m_scale, m_bias;
    SKuint32        m_reserve;
    bool            m_texCoBuilt;
    skVertexBuffer* m_buffer;

public:
    skPath();
    ~skPath();

    void setScale(skScalar x, skScalar y);

    void setBias(skScalar x, skScalar y);

    void arcTo(skScalar  x,
               skScalar  y,
               skScalar  w,
               skScalar  h,
               skScalar  angle1,
               skScalar  angle2,
               SKwinding winding);

    void cubicTo(skScalar fx,
                 skScalar fy,
                 skScalar tx,
                 skScalar ty);

    void rectTo(skScalar fx,
                skScalar fy,
                skScalar tx,
                skScalar ty);

    void moveTo(skScalar x, skScalar y);

    void lineTo(skScalar x, skScalar y);


    void makeRect(skScalar x, skScalar y, skScalar w, skScalar h);

    void makeEllipse(skScalar x, skScalar y, skScalar w, skScalar h);

    void makeRoundRect(skScalar x,
                       skScalar y,
                       skScalar w,
                       skScalar h,
                       skScalar aw,  // in degrees
                       skScalar ah,  // in degrees
                       int      corners);

    void makeStar(SKscalar x, SKscalar y, SKscalar w, SKscalar h, SKint32 Q, SKint32 P);

    void makePolygon(const skScalar* vertices,
                     int             count,
                     bool            close,
                     skScalar        scaleX = 1.f,
                     skScalar        scaleY = 1.f,
                     skScalar        biasX  = 0.0,
                     skScalar        biasY  = 0.0);
    void close(void);

    void clear(void);

    void makeUV(void);

    void makeUV(skScalar x, skScalar y, skScalar w, skScalar h);

    skVertexBuffer* getBuffer(void) const
    {
        update();
        return m_buffer;
    }

    void addVert(const skVertex& v);

    skContour* getContour(void) const
    {
        return m_contour;
    }

    const skBoundingBox2D& getAabb(void) const
    {
        return m_bounds;
    }

    bool isEmpty(void) const
    {
        return m_contour != nullptr && m_contour->empty();
    }


protected:
    void update() const;

    void rectCurveTo(skScalar x, skScalar y, skScalar w, skScalar h, skScalar angle1, skScalar angle2);

    void pushVert(const skVertex& v);

    void pushLine(skScalar x, skScalar y);
};

#endif  //_skPath_h_
