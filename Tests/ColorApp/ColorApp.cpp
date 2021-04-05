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
#include <cmath>
#include <cstdio>
#include "ColorTable.inl"
#include "Graphics/skGraphics.h"
#include "Math/skMath.h"
#include "Math/skVector2.h"
#include "Utils/skDisableWarnings.h"
#include "Utils/skLogger.h"
#include "Window/skMouse.h"
#include "Window/skWindow.h"
#include "Window/skWindowEnums.h"
#include "Window/skWindowHandler.h"

#if SK_PLATFORM == SK_PLATFORM_EMSCRIPTEN
constexpr int WindowX     = 320;
constexpr int WindowY     = 240;
constexpr int WindowFlags = WM_WF_CENTER | WM_WF_SHOWN;
#else
constexpr int WindowX     = 800;
constexpr int WindowY     = 600;
constexpr int WindowFlags = WM_WF_CENTER | WM_WF_MAXIMIZE | WM_WF_SHOWN;
#endif

class Application : public skWindowHandler
{
private:
    skWindowManager* m_manager;
    skWindow*        m_window;
    skVector2        m_size, m_mouseCo;
    SKuint32         m_lastFill;
    SKimage          m_grad;
    bool             m_done;

private:
    void handle(const skEventType& evt, skWindow* caller) override
    {
        switch (evt)
        {
        case SK_WIN_DESTROY:
            m_done = true;
            break;
        case SK_WIN_PAINT:
            draw();
            break;
        case SK_MOUSE_MOVED:
            m_mouseCo.x = (skScalar)caller->getMouse()->x.abs;
            m_mouseCo.y = (skScalar)caller->getMouse()->y.abs;
            caller->refresh();
            break;
        case SK_WIN_SIZE:
            m_size.x = (skScalar)caller->getWidth();
            m_size.y = (skScalar)caller->getHeight();

            skSetContext2f(SK_CONTEXT_SIZE, m_size.x, m_size.y);
            caller->refresh();
            break;
        case SK_KEY_PRESSED:
        case SK_KEY_RELEASED:
        case SK_MOUSE_PRESSED:
        case SK_MOUSE_RELEASED:
        case SK_MOUSE_WHEEL:
        case SK_WIN_SHOWN:
        case SK_WIN_EVT_UNKNOWN:
        case SK_WIN_HIDDEN:
            break;
        }
    }

    void draw(void)
    {
        skClearColor1i(CS_Grey03);
        skClearContext();
        skProjectRect(0, 0, m_size.x, m_size.y);

        skScalar x = 20;
        skScalar y = 20;

        skSelectImage(nullptr);
        skSetPaint1i(SK_BRUSH_MODE, SK_BM_MODULATE);

        for (SKuint32 i = 0; i < COLOR_SIZE; ++i, x += 25)
        {
            skColor1ui(COLOR_TABLE[i]);
            skRect(x, y, 20, 20);
            skFill();

            if (m_mouseCo.contains(x, y, x + 20, y + 20))
                m_lastFill = COLOR_TABLE[i];

            if (x + 25 > m_size.x - 25)
            {
                x = -5;
                y += 25;
            }

            skColor1ui(CS_Grey00);
            skStroke();
            skClearPath();
        }

        y += 25;

        const skScalar ms = skMin<skScalar>(m_size.x, m_size.y - y) / skScalar(1.125);
        const skScalar x1 = (m_size.x - ms) / 2;
        const skScalar y1 = y + (m_size.y - y - ms) / 2;
        const skScalar w  = ms;
        const skScalar h  = ms;

        skRect(x1, y1, w, h);

        skSelectImage(m_grad);
        skColor1ui(m_lastFill);
        skFill();
        skColor1ui(CS_Grey00);
        skStroke();
        skClearPath();
        m_window->flush();
    }

public:
    Application() :
        m_manager(nullptr),
        m_window(nullptr),
        m_lastFill(CS_Transparent),
        m_grad(nullptr),
        m_done(false)
    {
    }

    ~Application() override
    {
        delete m_manager;

        skDeleteImage(m_grad);
        skDeleteContext(skGetCurrentContext());
    }

    void run(void)
    {
        m_manager = new skWindowManager(WM_CTX_SDL, this);
        m_window  = m_manager->create("Color Test", WindowX, WindowY, WindowFlags);

        if (!m_window)
        {
            delete m_manager;
            m_manager = nullptr;
            return;
        }
        skNewContext();


        m_grad = skCreateImage(8, 8, SK_RGBA);
        skSetImage1i(m_grad, SK_IMAGE_FILTER, SK_FILTER_BI_LINEAR);
        SKcolorStop colorStops[] = {
            {0, 0x000000FF},
            {1, 0xFFFFFFFF},
        };

        skImageLinearGradient(m_grad,
                              colorStops,
                              2,
                              SK_NORTH | SK_EAST);
        m_manager->dispatchInitialEvents();
        m_manager->process();
    }
};

int main(int, char**)
{
    try
    {
        skLogger log;
        log.setFlags(LF_STDOUT);

        Application app;
        app.run();
    }
    catch (...)
    {
        puts("An exception occurred\n");
    }
}
