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
#ifndef _skGraphicsWindow_h_
#define _skGraphicsWindow_h_

#include "Utils/Config/skConfig.h"
#include "Window/skWindowEnums.h"

#include "skGraphicsConfig.h"
#include "skGraphics.h"
#ifdef Graphics_BUILD_WINDOW

SK_SIZE_HANDLE(SKwindowManager);
SK_SIZE_HANDLE(SKwindow);

SK_ST_C;

typedef void (*SKwindowCB)(SKwindow, void*);
typedef void (*SKsizeChangeCB)(SKwindow, void*, SKuint32, SKuint32);
typedef void (*SKmouseMovedCB)(SKwindow, void*, SKint32, SKint32);
typedef void (*SKmouseButtonCB)(SKwindow, void*, skMouseCode, SKint32, SKint32);
typedef void (*SKkeyCB)(SKwindow, void*, skScanCode);

typedef struct SKWindowCallbacks
{
    SKwindowCB      closed;
    SKsizeChangeCB  sizeChange;
    SKwindowCB      paint;
    SKmouseMovedCB  mouseMoved;
    SKmouseButtonCB mousePressed;
    SKmouseButtonCB mouseReleased;
    SKkeyCB         keyPressed;
    SKkeyCB         keyReleased;
    void*           user;
} SKwindowCallbacks;

SK_API SKwindowManager skNewWindowManager(SKuint32 type);
SK_API void            skDelWindowManager(SKwindowManager mgr);
SK_API void            skDispatch(SKwindowManager mgr);
SK_API void            skBroadcast(SKwindowManager mgr,  SKuint32 evtType);

SK_API SKwindow skNewWindow(
    SKwindowManager    mgr,
    const char*        title,
    SKuint32           width,
    SKuint32           height,
    SKuint32           flags,
    SKwindowCallbacks* callbacks);

SK_API void    skShowWindow(SKwindow win, SKint8 state);
SK_API void    skRedrawWindow(SKwindow win);
SK_API SKuint8 skGetKeyCodeState(SKwindow win, skScanCode sc);
SK_API SKuint8 skGetMouseState(SKwindow win, skMouseCode sc);
SK_API void    skGetMouseCo(SKwindow win, SKint32* x, SKint32* y);
SK_API void    skGetMouseZ(SKwindow win, SKint32* z);
SK_API void    skGetRelMouseCo(SKwindow win, SKint32* x, SKint32* y);



SK_EN_C;
#endif

#endif /*_skGraphicsWindow_h_*/
