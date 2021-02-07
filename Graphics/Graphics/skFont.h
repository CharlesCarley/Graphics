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
#ifndef _skFont_h_
#define _skFont_h_

#include "Utils/skArray.h"
#include "skContextObject.h"
#include "skGlyph.h"

class skImage;

typedef struct SKfontOptions
{
    SKint32  filter;
    SKint32  mipmap;
    SKint32  pixelSize;
    skScalar scale;
    SKint32  dpi;
    SKint32  tabSize;
    SKint32  average;
    SKint32  xMax;
    SKint32  yMax;
} SKfontOptions;

typedef struct SKChar
{
    skScalar x, y;
    skScalar w, h;
    skScalar xOffs;
    skScalar yOffs;
} SKchar;

class skFont : public skContextObj
{
public:
    typedef SKChar Char;

private:
    Char*         m_chars;
    skTexture*    m_image;
    SKfontOptions m_opts;

    skArray<skGlyph*> m_glyphs;

    void loadGlyphs(struct FT_FaceRec_* face);

    bool loadTrueTypeFont(const void* mem,
                          SKsize      len,
                          SKuint32    size,
                          SKuint32    dpi);

public:
    skFont();
    ~skFont();

    void getCharExtent(char c, SKtextExtent* te) const;

    void getCharExtent(char c, SKint32* w, SKint32* h) const;

    void getTextExtent(const char* str,
                       SKint32     len,
                       SKint32*    w,
                       SKint32*    h) const;

    void getTextExtentExt(const char* str,
                          SKint32     idx,
                          SKint32     len,
                          SKint32*    w,
                          SKint32*    h) const;

    SKint32 getAverageWidth(void);

    bool fromEnum(SKbuiltinFont font,
                  SKuint32      size,
                  SKuint32      dpi);

    bool fromFile(const char* path,
                  SKuint32    size,
                  SKuint32    dpi);

    bool fromMemory(const void* mem,
                    SKuint32    len,
                    SKuint32    size,
                    SKuint32    dpi);

    void buildPath(skPath* path, const char* str, SKuint32 len, skScalar x, skScalar y);

    const Char& getChar(char ch) const;

    skScalar getScale(void) const
    {
        return m_opts.scale > 0 ? skScalar(m_opts.scale) : skScalar(1);
    }

    skScalar getPixelSize(void) const
    {
        return skScalar(m_opts.pixelSize * m_opts.dpi) / 64.f;
    }

    skScalar getRelativeScale(void) const
    {
        return getScale() / getPixelSize();
    }

    skTexture* getImage(void) const
    {
        return m_image;
    }

    const SKfontOptions& getOptions(void) const
    {
        return m_opts;
    }

    void getI(SKfontOptionEnum opt, SKint32* v) const;

    void setI(SKfontOptionEnum opt, SKint32 v);

    void getF(SKfontOptionEnum opt, skScalar* v) const;

    void setF(SKfontOptionEnum opt, skScalar v);
};

#endif  //_skFont_h_
