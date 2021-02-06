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
#include "skPath.h"
#include "OpenGL/skVertexBuffer.h"
#include "skContext.h"

#define vTOL 0.01f

skPath::skPath()
{
    m_texCoBuilt = false;
    m_reserve    = 24;
    m_contour    = new skContour();
    m_scale.x    = 1.f;
    m_scale.y    = 1.f;
    m_bias.x     = 0.f;
    m_bias.y     = 0.f;
    m_buffer     = nullptr;
}

skPath::~skPath()
{
    delete m_buffer;

    m_buffer = nullptr;
    delete m_contour;
    m_contour = nullptr;
}

void skPath::setScale(skScalar x, skScalar y)
{
    m_scale.x = x;
    m_scale.y = y;
}

void skPath::setBias(skScalar x, skScalar y)
{
    m_bias.x = x;
    m_bias.y = y;
}

void skPath::arcTo(skScalar  x,
                   skScalar  y,
                   skScalar  w,
                   skScalar  h,
                   skScalar  angle1,
                   skScalar  angle2,
                   SKwinding winding)
{
    if (!m_ctx)
        return;

    SKint32 vertexCount = m_ctx->getContextI(SK_VERTICES_PER_SEGMENT);

    if (vertexCount <= 0)
        vertexCount = 8;

    const skScalar step = skRadians(skDegrees(angle2) / (skScalar)vertexCount);

    skScalar cx, cy, hw, hh, s, c;
    hw = w * 0.5f;
    hh = h * 0.5f;
    cx = x + hw;
    cy = y + hw;

    m_reserve = vertexCount;

    skScalar a = angle1;

    for (int i = 0; i < vertexCount; i++)
    {
        a += step;
        skMath::sinCos(a, s, c);

        if (winding == SK_CCW)
            s = -s;
        c = -c;
        lineTo(cx + hw * s, cy + hh * c);
    }
}

void skPath::cubicTo(skScalar fx, skScalar fy, skScalar tx, skScalar ty)
{
    SKint32 vertexCount = m_ctx->getContextI(SK_VERTICES_PER_SEGMENT);

    if (vertexCount <= 0)
        vertexCount = 8;

    const skScalar step = skScalar(1.0) / skScalar(vertexCount);

    skScalar       s = 1.f, t = 0.f;
    skVector2      c0, c1, c2, c3, cr;
    const skScalar hd = skVector2(fx, fy).distance(skVector2(tx, ty)) * .5f;

    c0 = skVector2(fx, fy);
    c1 = skVector2(c0.x + hd, c0.y);
    c3 = skVector2(tx, ty);
    c2 = skVector2(c3.x - hd, c3.y);

    moveTo(c0.x, c0.y);

    while (t <= skScalar(1.0))
    {
        const skScalar sss  = s * s * s;
        const skScalar ttt  = t * t * t;
        const skScalar sst3 = skScalar(3.0) * s * s * t;
        const skScalar tts3 = skScalar(3.0) * t * t * s;

        cr = skVector2(
            sss * c0.x + sst3 * c1.x + tts3 * c2.x + ttt * c3.x,
            sss * c0.y + sst3 * c1.y + tts3 * c2.y + ttt * c3.y);
        lineTo(cr.x, cr.y);
        t += step;
        s = skScalar(1.0) - t;
    }
    lineTo(c3.x, c3.y);
}

void skPath::rectTo(skScalar fx, skScalar fy, skScalar tx, skScalar ty)
{
    const skScalar fac = skVector2(tx, ty).distance(skVector2(fx, fy)) * .5f;
    skScalar       cv  = fx + fac;

    if (tx - fac < cv)
        cv = tx - fac;

    moveTo(fx, fy);
    lineTo(cv, fy);
    lineTo(cv, ty);
    lineTo(tx, ty);
}

void skPath::moveTo(skScalar x, skScalar y)
{
    m_cur.x = x;
    m_cur.y = y;
    m_mov.x = x;
    m_mov.y = y;

    m_contour->reserve(m_reserve);
    pushVert(m_cur);
}

void skPath::lineTo(skScalar x, skScalar y)
{
    pushLine(x, y);
}

void skPath::close(void)
{
    pushVert(m_mov);
}

void skPath::clear(void)
{
    m_bounds.clear();
    m_cur.x = m_cur.y = m_mov.x = m_mov.y = 0.f;
    m_contour->clear();
}

void skPath::makeRect(skScalar x, skScalar y, skScalar w, skScalar h)
{
    if (!m_ctx)
        return;

    m_reserve = 5;
    clear();
    moveTo(x, y);
    lineTo(x, y + h);
    lineTo(x + w, y + h);
    lineTo(x + w, y);
    close();
}

void skPath::makeEllipse(skScalar x, skScalar y, skScalar w, skScalar h)
{
    if (!m_ctx)
        return;

    clear();
    SKint32 vertexCount = m_ctx->getContextI(SK_VERTICES_PER_SEGMENT), i;

    if (vertexCount <= 0)
        vertexCount = 8;

    const skScalar iStep = skPi2 / skScalar(vertexCount);

    skScalar cx, cy, hw, hh;
    hw = w * 0.5f;
    hh = h * 0.5f;
    cx = x + hw;
    cy = y + hh;

    m_reserve = vertexCount;

    skScalar s, c;
    for (i = 0; i < vertexCount; i++)
    {
        skMath::sinCos(skScalar(i) * iStep, s, c);
        if (i == 0)
            moveTo(cx + hw * c, cy + hh * s);
        else
            lineTo(cx + hw * c, cy + hh * s);
    }
    close();
}

void skPath::rectCurveTo(skScalar x,
                         skScalar y,
                         skScalar w,
                         skScalar h,
                         skScalar angle1,
                         skScalar angle2)
{
    SKint32 vertexCount = m_ctx->getContextI(SK_VERTICES_PER_SEGMENT);

    if (vertexCount <= 0)
        vertexCount = 8;

    const skScalar step = skRadians(skDegrees(angle2) / (skScalar)vertexCount);

    const skVector2 v0(m_cur.x, m_cur.y);
    const skVector2 v1(x, y);
    const skVector2 cv = (v1 + v0) / 2.f;

    const skScalar hw = w * 0.5f;
    const skScalar hh = h * 0.5f;

    skScalar s, c;
    skScalar a = angle1;

    for (int i = 0; i < vertexCount; i++)
    {
        a += step;
        skMath::sinCos(a, s, c);

        skScalar nx, ny;
        nx = cv.x + hw * s;
        ny = cv.y + hh * -c;
        pushLine(nx, ny);
    }
}

void skPath::makeRoundRect(skScalar x,
                           skScalar y,
                           skScalar w,
                           skScalar h,
                           skScalar aw,
                           skScalar ah,
                           int      corners)
{
    if (!m_ctx)
        return;

    SKint32 vertexCount = m_ctx->getContextI(SK_VERTICES_PER_SEGMENT);

    if (vertexCount <= 0)
        vertexCount = 8;

    aw = skClamp<skScalar>(aw, 0.f, w);
    ah = skClamp<skScalar>(ah, 0.f, h);

    const skScalar xMin = x, xMax = x + w, yMin = y, yMax = y + h;

    skScalar rx, ry;
    rx = aw * 0.5f;
    ry = ah * 0.5f;

    clear();

    m_reserve = 4 * vertexCount * 2;
    if (corners & SK_CNR_RT)
    {
        moveTo(xMax - rx, yMin);
        rectCurveTo(xMax - rx, yMin + ah, aw, ah, 0, skPiH);
        lineTo(xMax, yMax - ry);
    }
    else
        moveTo(xMax, yMin);

    if (corners & SK_CNR_RB)
    {
        lineTo(xMax, yMax - ry);
        rectCurveTo(xMax - aw, yMax - ry, aw, ah, skPiH, skPiH);
        lineTo(xMin + rx, yMax);
    }
    else
        lineTo(xMax, yMax);

    if (corners & SK_CNR_LB)
    {
        lineTo(xMin + rx, yMax);
        rectCurveTo(xMin + rx, yMax - ah, aw, ah, skPi, skPiH);
        lineTo(xMin, yMin + ry);
    }
    else
        lineTo(xMin, yMax);

    if (corners & SK_CNR_LT)
    {
        lineTo(xMin, yMin + ry);
        rectCurveTo(xMin + aw, yMin + ry, aw, ah, skPi + skPiH, skPiH);
        lineTo(xMax - rx, yMin);
    }
    else
        lineTo(xMin, yMin);

    close();
}

void skPath::makeStar(SKscalar x, SKscalar y, SKscalar w, SKscalar h, SKint32 Q, SKint32 P)
{
    clear();
    // there is no tessellation so this will not work for fills
    // https://www.desmos.com/calculator/3jyqtcbssp

    Q = skMax(2, Q);
    if (P > Q)
        P = Q - 1;
    if (P < 1)
        P = 1;

    const skScalar R = skMin(w, h) * skScalar(0.5);
    const skScalar I = skPi2 / skScalar(Q);
    skScalar       s, c;

    x += R;
    y += R;

    SKint32 i;
    for (i = 0; i < Q * P; i++)
    {
        if (i % P == 0)
        {
            // rotate it 90 degrees initially
            // then (2pi/Q)*skip

            c = x + R * skCos(skPiH + I * skScalar(i));
            s = y - R * skSin(skPiH + I * skScalar(i));
            if (i == 0)
                moveTo(c, s);
            else
                lineTo(c, s);
        }
    }

    close();
}

void skPath::makePolygon(const skScalar* vertices,
                         int             count,
                         bool            close,
                         skScalar        scaleX,
                         skScalar        scaleY,
                         skScalar        biasX,
                         skScalar        biasY)
{
    if (!m_ctx)
        return;

    const skScalar* vp = vertices;
    skScalar        x, y;
    clear();

    m_reserve = count;

    for (int i = 0; i < count; i += 2)
    {
        x = *vp++ * scaleX + biasX;
        y = *vp++ * scaleY + biasY;

        if (i == 0)
            moveTo(x, y);
        else
            lineTo(x, y);
    }

    if (close)
        this->close();
}

void skPath::pushVert(const skVertex& v)
{
    if (!m_ctx)
        return;

    if (m_contour != nullptr)
    {
        skVertex pv;
        pv.u = v.u;
        pv.v = v.v;

        pv.x = v.x * m_scale.x + m_bias.x;
        pv.y = v.y * m_scale.y + m_bias.y;

        if (m_ctx->getContextI(SK_METRICS_MODE) == SK_RELATIVE)
        {
            const skVector2 size = m_ctx->getContextV(SK_CONTEXT_SIZE);

            pv.x *= size.x;
            pv.y *= size.y;
        }

        if (!m_buffer)
        {
            m_buffer = new skVertexBuffer();
            m_buffer->addElement(SK_ATTR_POSITION, SK_FLOAT2_32);
            m_buffer->addElement(SK_ATTR_TEXTURE0, SK_FLOAT2_32);
        }

        m_bounds.compare(pv.x, pv.y);
        m_contour->push_back(pv);
        m_texCoBuilt = false;
    }
}

void skPath::addVert(const skVertex& v)
{
    pushVert(v);
    m_texCoBuilt = true;
}

void skPath::pushLine(skScalar x, skScalar y)
{
    if (skEqT(x, m_cur.x, vTOL) && skEqT(y, m_cur.y, vTOL))
        return;

    if (m_contour->empty())
        moveTo(x, y);
    else
    {
        const skVertex v(x, y);
        pushVert(v);
        m_cur = v;
    }
}

void skPath::makeUV(skScalar x, skScalar y, skScalar w, skScalar h)
{
    if (m_texCoBuilt)
        return;

    m_texCoBuilt = true;

    skScalar dx, dy;
    skScalar oneOverMaxX, oneOverMaxY;

    oneOverMaxX = 1.f / (x + w - x);
    oneOverMaxY = 1.f / (y + h - y);

    skContour& cc = *m_contour;

    const skPoly::PointerType p = cc.vertices.ptr();
    const SKsize              s = cc.vertices.size();
    SKsize                    i = 0;

    while (i < s)
    {
        skVertex& v = p[i++];

        dx  = v.x - x;
        dy  = v.y - y;
        v.u = dx * oneOverMaxX;
        v.v = dy * oneOverMaxY;
        v.v = 1.f - v.v;
    }
}

void skPath::makeUV(void)
{
    if (m_texCoBuilt)
        return;

    const skRectangle rct = m_bounds.getRect();
    makeUV(rct.x, rct.y, rct.width, rct.height);
}

void skPath::update(void) const
{
    m_buffer->write(
        m_contour->vertices.ptr(),
        m_contour->vertices.size() * sizeof(skVertex),
        SK_STREAM_DRAW);
}
