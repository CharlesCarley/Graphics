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
#include "skTexture.h"
#include <memory.h>
#include <cstdio>
#include "Image/skImage.h"
#include "Utils/skString.h"

skTexture::skTexture() :
    m_image(nullptr)
{
    m_opts.filter = SK_FILTER_NONE;
    m_opts.mipmap = 0;
}

skTexture::skTexture(SKint32 w, SKint32 h, SKpixelFormat fmt)
{
    m_opts.filter = SK_FILTER_NONE;
    m_opts.mipmap = 0;
    m_image       = new skImage(w, h, (skPixelFormat)fmt);
}

skTexture::~skTexture()
{
    delete m_image;
}

void skTexture::writePixel(SKint32 x, SKint32 y, const skColor& color) const
{
    if (m_image)
    {
        SKuint8 r, g, b, a;
        color.asInt8(r, g, b, a);
        m_image->setPixel(x, y, skPixel(r, g, b, a));
    }
}

void skTexture::makeLinearGradient(SKcolorStop* stops,
                                   SKint32      stopCount,
                                   SKuint16     dir)
{
    if (!m_image)
        return;

    SKuint32 w, h;
    w = m_image->getWidth();
    h = m_image->getHeight();

    SKuint32 fx = 0, fy = 0, tx = w, ty = h;
    if (dir & SK_WEST)
        skSwap(tx, fx);

    if (dir & SK_NORTH)
        skSwap(ty, fy);

    makeLinearGradient(fx, fy, tx, ty, stops, stopCount);
}

void skTexture::makeRadialGradient(SKcolorStop* stops,
                                   SKint32      stopCount)
{
    const SKuint32 hx = m_image->getWidth() >> 1;
    const SKuint32 hy = m_image->getHeight() >> 1;
    makeRadialGradient(hx, hy, hx, hy, 0, skMax(hx, hy), stops, stopCount);
}

void skTexture::makeLinearGradient(
    SKint32      fx,
    SKint32      fy,
    SKint32      tx,
    SKint32      ty,
    SKcolorStop* stops,
    SKint32      stopCount)
{
    makeGradient(fx, fy, tx, ty, 0, 0, stops, stopCount, true);
}

void skTexture::makeRadialGradient(
    SKint32      fx,
    SKint32      fy,
    SKint32      tx,
    SKint32      ty,
    SKint32      rx,
    SKint32      ry,
    SKcolorStop* stops,
    SKint32      stopCount)
{
    makeGradient(fx, fy, tx, ty, rx, ry, stops, stopCount, false);
}

void skTexture::makeGradient(
    SKint32      fx,
    SKint32      fy,
    SKint32      tx,
    SKint32      ty,
    SKint32      rx,
    SKint32      ry,
    SKcolorStop* stops,
    SKint32      stopCount,
    bool         isLinear)
{
    if (!stops || stopCount <= 0)
        return;

    if (!m_image)
        return;

    SKint32  s;
    SKuint32 x, y, s2;

    skVector2 p0, p1, D, H, C;
    p0 = skVector2((skScalar)fx, (skScalar)fy);
    p1 = skVector2((skScalar)tx, (skScalar)ty);
    D  = p1 - p0;

    skScalar dL = D.length2(), G;
    if (dL <= 0.f)
        dL = 1.f;

    dL = 1.f / dL;

    if (!isLinear)
    {
        H.x = (SKscalar)(m_image->getWidth() >> 1);
        H.y = (SKscalar)(m_image->getHeight() >> 1);
    }

    for (x = 0; x < m_image->getWidth(); ++x)
    {
        for (y = 0; y < m_image->getHeight(); ++y)
        {
            skVector2 pn(skScalar(x + .5), skScalar(y + .5));

            if (isLinear)
                G = D.dot(pn - p0) * dL;
            else
            {
                C = pn - H;
                G = 1.f - C.dot(C) * 1.f / (rx * rx + ry * ry);
            }

            skColor a, b, c(0, 0, 0, 1);
            for (s = 0; s < stopCount; ++s)
            {
                if (stops[s].offset > G)
                    break;
            }

            s2 = skClamp<SKint32>(s - 1, 0, stopCount - 1);

            const skScalar s0 = stops[s2].offset;
            const skScalar s1 = stops[s2 + 1].offset;

            if (G <= s0)
                c = skColor(stops[s2].color);
            else if (G >= s1)
                c = skColor(stops[s2 + 1].color);
            else
            {
                a = skColor(stops[s2].color);
                b = skColor(stops[s2 + 1].color);
                c = (a * (s1 - G) + b * (G - s0)) / (s1 - s0);
            }
            c.limit();
            writePixel(x, y, skColor(c.r, c.g, c.b, c.a));
        }
    }

    notifyImage();
}

void skTexture::getI(const SKimageOptionEnum opt, SKint32* v) const
{
    switch (opt)
    {
    case SK_IMAGE_BPP:
        if (m_image)
            *v = m_image->getBPP();
        break;
    case SK_IMAGE_FILTER:
        *v = m_opts.filter;
        break;
    case SK_IMAGE_MIPMAP:
        *v = m_opts.mipmap;
        break;
    case SK_IMAGE_WIDTH:
        if (m_image)
            *v = m_image->getWidth();
        break;
    case SK_IMAGE_HEIGHT:
        if (m_image)
            *v = m_image->getHeight();
        break;
    case SK_IMAGE_PITCH:
        if (m_image)
            *v = m_image->getPitch();
        break;
    case SK_IMAGE_SIZE_IN_BYTES:
        if (m_image)  // needs unsigned
            *v = (SKint32)m_image->getSizeInBytes();
        else
            *v = -1;
        break;
    case SK_IMAGE_PIXEL_FORMAT:
        if (m_image)  // needs unsigned
            *v = (SKint32)m_image->getFormat();
        break;
    //case SK_IMAGE_BYTES:
    default:
        break;
    }
}

void skTexture::setI(SKimageOptionEnum opt, SKint32 v)
{
    if (opt == SK_IMAGE_FILTER)
    {
        if (v > SK_FILTER_MIN && v < SK_FILTER_MAX)
            m_opts.filter = v;
    }
    else if (opt == SK_IMAGE_MIPMAP)
        m_opts.mipmap = v;
}

void skTexture::save(const char* file) const
{
    if (m_image)
        m_image->save(file);
}

bool skTexture::load(const char* file)
{
    delete m_image;
    m_image = nullptr;

    skImage* ima = new skImage();
    if (ima->load(file))
    {
#if SK_PLATFORM == SK_PLATFORM_EMSCRIPTEN
        switch (ima->getFormat())
        {
        case SK_RGB:
        case SK_BGR:
            m_image = ima->convertToFormat(SK_BGR);
            delete ima;
            break;
        case SK_ARGB:
        case SK_ABGR:
        case SK_RGBA:
        case SK_BGRA:
            m_image = ima->convertToFormat(SK_BGRA);
            delete ima;
            break;
        default:
            m_image = ima;
        }
#else
        m_image = ima;
#endif
    }
    else
        delete ima;
    return m_image != nullptr;
}
