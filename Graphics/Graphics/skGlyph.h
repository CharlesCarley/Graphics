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
#ifndef _skGlyph_h_
#define _skGlyph_h_

#include "Utils/Config/skConfig.h"
class skImage;


typedef struct SKGlyphMetrics
{
    SKint32 yBearing, height;
    SKint32 width, advance;
    SKint32 i;
} SKglyphMetrics;

class skGlyph
{
private:
    SKuint8*       m_data;
    SKuint32       m_width, m_height;
    SKglyphMetrics m_metrics;

public:
    skGlyph(SKuint8* ptr, SKuint32 w, SKuint32 h);
    ~skGlyph();

    void merge(skImage* dest, SKuint32 x, SKuint32 y);
    void setMetrics(const SKglyphMetrics& metrics);

    SKuint32 getWidth() const
    {
        return m_width;
    }

    SKuint32 getHeight() const
    {
        return m_height;
    }

    const SKglyphMetrics& getMertics() const
    {
        return m_metrics;
    }
};
#endif // _skGlyph_h_