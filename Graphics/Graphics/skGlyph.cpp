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
#include "skGlyph.h"
#include "Image/skImage.h"
#include "Utils/skMemoryUtils.h"
#include "Utils/skPlatformHeaders.h"

skGlyph::skGlyph(SKuint8* ptr, SKuint32 w, SKuint32 h) :
    m_width(w),
    m_height(h)
{
    SKsize lim = (SKsize)w * (SKsize)h;
    m_data     = new SKuint8[lim];

    skMemcpy(m_data, ptr, lim);
    skMemset(&m_metrics, 0, sizeof(SKglyphMetrics));
}

skGlyph::~skGlyph()
{
    delete[] m_data;
}

void skGlyph::setMetrics(const SKglyphMetrics& metrics)
{
    skMemcpy(&m_metrics, &metrics, sizeof(SKglyphMetrics));
}

void skGlyph::merge(skImage* dest, SKuint32 x, SKuint32 y)
{
    if (!m_data)
        return;

    SKuint8* ptr = m_data;
    SKuint32 ix, iy;

    for (iy = 0; iy < m_height; ++iy)
    {
        const SKint32 ny = y + iy;
        for (ix = 0; ix < m_width; ix++)
        {
            const SKint32 nx = x + ix;
            const SKuint8 ch = *ptr++;
            dest->setPixel(nx, ny, skPixel(ch, ch, ch, ch));
        }
    }
}
