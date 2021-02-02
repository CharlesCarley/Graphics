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
#include "Window/skKeyboard.h"
#include "Window/skMouse.h"
#include "Window/skWindow.h"
#include "Window/skWindowHandler.h"
#include "Window/skWindowManager.h"
#include "skContext.h"
#include "skGraphicsConfig.h"
#include "skGraphicsWindow.h"

#ifdef Graphics_BUILD_WINDOW

SK_API SKwindowManager skNewWindowManager(SKuint32 type)
{
    skWindowManager* mgr = new skWindowManager((skContextType)type);
    return (SKwindowManager)mgr;
}

SK_API void skDelWindowManager(SKwindowManager mgr)
{
    SK_CHECK_PARAM(mgr, SK_RETURN_VOID);
    delete SK_GET_WINDOW_MGR(mgr);
}

SK_API void skDispatch(SKwindowManager mgr)
{
    SK_CHECK_PARAM(mgr, SK_RETURN_VOID);
    SK_GET_WINDOW_MGR(mgr)->processInteractive(true);
}

SK_API void skBroadcast(SKwindowManager mgr, SKuint32 evtType)
{
    SK_CHECK_PARAM(mgr, SK_RETURN_VOID);
    SK_GET_WINDOW_MGR(mgr)->broadcastEvent((skEventType)evtType);
}

class SKWindowHandler : public skWindowHandler
{
    SKwindowCallbacks* m_call;

public:
    SKWindowHandler(SKwindowCallbacks* callbacks) :
        m_call(callbacks)
    {
    }

    void handle(const skEventType& evt, skWindow* caller) override
    {
        if (m_call == nullptr)
            return;

        switch (evt)
        {
        case SK_WIN_DESTROY:
            if (m_call->closed)
                m_call->closed((SKwindow)caller, m_call->user);
            break;
        case SK_MOUSE_MOVED:
            if (m_call->mouseMoved)
            {
                skMouse* mouse = caller->getMouse();
                m_call->mouseMoved((SKwindow)caller, m_call->user, mouse->x.rel, mouse->y.rel);
            }
            break;
        case SK_MOUSE_WHEEL:
        case SK_MOUSE_PRESSED:
            if (m_call->mousePressed)
            {
                skMouse* mouse = caller->getMouse();
                m_call->mousePressed(
                    (SKwindow)caller,
                    m_call->user,
                    (skMouseCode)mouse->button,
                    mouse->x.abs,
                    mouse->y.abs);
            }
            break;
        case SK_MOUSE_RELEASED:
            if (m_call->mousePressed)
            {
                skMouse* mouse = caller->getMouse();
                m_call->mouseReleased(
                    (SKwindow)caller,
                    m_call->user,
                    (skMouseCode)mouse->button,
                    mouse->x.abs,
                    mouse->y.abs);
            }
            break;
        case SK_KEY_PRESSED:
            if (m_call->keyPressed)
            {
                skKeyboard* key = caller->getKeyboard();
                m_call->keyPressed(
                    (SKwindow)caller,
                    m_call->user,
                    (skScanCode)key->key);
            }
            break;
        case SK_KEY_RELEASED:
            if (m_call->keyReleased)
            {
                skKeyboard* key = caller->getKeyboard();
                m_call->keyReleased(
                    (SKwindow)caller,
                    m_call->user,
                    (skScanCode)key->key);
            }
            break;
        case SK_WIN_SIZE:
            if (m_call->sizeChange)
                m_call->sizeChange((SKwindow)caller, m_call->user, caller->getWidth(), caller->getHeight());
            break;
        case SK_WIN_PAINT:
            if (m_call->paint)
            {
                m_call->paint((SKwindow)caller, m_call->user);
                caller->flush();
            }
            break;
        case SK_WIN_EVT_UNKNOWN:
        case SK_WIN_SHOWN:
        case SK_WIN_HIDDEN:
            break;
        }
    }
};

SK_API SKwindow skNewWindow(SKwindowManager    mgr,
                            const char*        title,
                            SKuint32           width,
                            SKuint32           height,
                            SKuint32           flags,
                            SKwindowCallbacks* callbacks)
{
    SK_CHECK_PARAM(mgr, nullptr);

    skWindowManager* manager = SK_GET_WINDOW_MGR(mgr);
    if (!manager->hasHandlers())
        manager->addHandler(new SKWindowHandler(callbacks));

    return (SKwindow)manager->create(title, width, height, flags);
}

SK_API void skShowWindow(SKwindow window, SKint8 state)
{
    SK_CHECK_PARAM(window, SK_RETURN_VOID);
    SK_GET_WINDOW(window)->show(state != 0);
}

SK_API void skRedrawWindow(SKwindow window)
{
    SK_CHECK_PARAM(window, SK_RETURN_VOID);
    SK_GET_WINDOW(window)->refresh();
}

SK_API SKuint8 skGetKeyCodeState(SKwindow win, skScanCode sc)
{
    SK_CHECK_PARAM(win, -1);
    return SK_GET_WINDOW(win)->getKeyboard()->isKeyDown(sc) ? 1 : 0;
}

SK_API SKuint8 skGetMouseState(SKwindow win, skMouseCode sc)
{
    SK_CHECK_PARAM(win, -1);
    return SK_GET_WINDOW(win)->getMouse()->isButtonDown(sc) ? 1 : 0;
}

SK_API void skGetMouseCo(SKwindow win, SKint32* x, SKint32* y)
{
    SK_CHECK_PARAM(win, SK_RETURN_VOID);
    skMouse* mse = SK_GET_WINDOW(win)->getMouse();
    if (x)
        *x = mse->x.abs;
    if (y)
        *y = mse->y.abs;
}

SK_API void skGetMouseZ(SKwindow win, SKint32* z)
{
    SK_CHECK_PARAM(win, SK_RETURN_VOID);
    skMouse* mse = SK_GET_WINDOW(win)->getMouse();
    if (z)
        *z = mse->z.abs;
}

SK_API void skGetRelMouseCo(SKwindow win, SKint32* x, SKint32* y)
{
    SK_CHECK_PARAM(win, SK_RETURN_VOID);
    skMouse* mse = SK_GET_WINDOW(win)->getMouse();
    if (x)
        *x = mse->x.rel;
    if (y)
        *y = mse->y.rel;
}

SK_API void skDelWindow(SKwindowManager mgr, SKwindow window)
{
    SK_CHECK_PARAM(mgr, SK_RETURN_VOID);
    SK_CHECK_PARAM(window, SK_RETURN_VOID);
    SK_GET_WINDOW_MGR(mgr)->destroy(SK_GET_WINDOW(window));
}

#endif
