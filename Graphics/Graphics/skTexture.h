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
#ifndef _skTexture_h_
#define _skTexture_h_

#include "Image/skImage.h"
#include "Image/skPixel.h"
#include "skContextObject.h"

class skImage;

typedef struct SKimageOptions
{
    SKint32 filter;
    SKint32 mipmap;
} SKimageOptions;

class skTexture : public skContextObj
{
protected:
    skImage*       m_image;
    SKimageOptions m_opts{};

public:
    explicit skTexture();
    skTexture(SKint32 w, SKint32 h, SKpixelFormat fmt);

    virtual ~skTexture();


    skImage* getInternalImage() const
    {
        return m_image;
    }

    void makeLinearGradient(SKcolorStop* stops,
                            SKint32      stopCount,
                            SKuint16     dir);

    void makeRadialGradient(SKcolorStop* stops,
                            SKint32      stopCount);

    void makeLinearGradient(SKint32      fx,
                            SKint32      fy,
                            SKint32      tx,
                            SKint32      ty,
                            SKcolorStop* stops,
                            SKint32      stopCount);

    void makeRadialGradient(SKint32      fx,
                            SKint32      fy,
                            SKint32      tx,
                            SKint32      ty,
                            SKint32      rx,
                            SKint32      ry,
                            SKcolorStop* stops,
                            SKint32      stopCount);

    SKint32 getWidth(void) const
    {
        return m_image ? m_image->getWidth() : 0;
    }

    SKint32 getHeight(void) const
    {
        return m_image ? m_image->getHeight() : 0;
    }

    SKint32 getBPP(void) const
    {
        return m_image ? m_image->getBPP() : 0;
    }

    SKint32 getPitch(void) const
    {
        return m_image ? m_image->getPitch() : 0;
    }

    SKubyte* getBits(void) const
    {
        return m_image ? m_image->getBytes() : nullptr;
    }


    skPixelFormat getFormat(void) const
    {
        return m_image ? m_image->getFormat() : skPixelFormat::SK_ALPHA;
    }

    void save(const char* file) const;
    bool load(const char* file);

    void getI(SKimageOptionEnum opt, SKint32* v) const;
    void setI(SKimageOptionEnum opt, SKint32 v);

protected:

    void makeGradient(SKint32      fx,
                      SKint32      fy,
                      SKint32      tx,
                      SKint32      ty,
                      SKint32      rx,
                      SKint32      ry,
                      SKcolorStop* stops,
                      SKint32      stopCount,
                      bool         isLinear);

    void writePixel(SKint32 x, SKint32 y, const skColor& color) const;

    virtual void notifyImage(void)
    {
    }
};

#endif  //_skTexture_h_
