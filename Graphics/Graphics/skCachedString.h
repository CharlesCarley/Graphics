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
#ifndef _skCachedString_h_
#define _skCachedString_h_

#include "Utils/skString.h"
#include "skPath.h"


class skCachedString : public skContextObj
{
private:
    skVector2 m_size;
    skPath*   m_path;
    skString  m_cache;

public:
    skCachedString();
    virtual ~skCachedString();

    void rebuild();
    void buildString(const char* str);

    skPath* getPath() const
    {
        return m_path;
    }

    bool isBuilt() const
    {
        return m_size.length2() > 0 || !m_path->isEmpty();
    }

    skVector2 getV(SKstringOptionEnum opt) const;
};


#endif  //_skCachedString_h_
