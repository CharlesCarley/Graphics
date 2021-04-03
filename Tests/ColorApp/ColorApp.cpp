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
#include "Graphics/skGraphics.h"
#include "Math/skMath.h"
#include "Math/skVector2.h"
#include "Utils/skDisableWarnings.h"
#include "Utils/skLogger.h"
#include "Window/skMouse.h"
#include "Window/skWindow.h"
#include "Window/skWindowEnums.h"
#include "Window/skWindowHandler.h"
#include "Window/skWindowManager.h"
#include "ColorTable.inl"

#if SK_PLATFORM == SK_PLATFORM_EMSCRIPTEN
constexpr int WindowX     = 320;
constexpr int WindowY     = 240;
constexpr int WindowFlags = WM_WF_SHOW_CENT_DIA | WM_WF_MAXIMIZE;
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
    SKfont           m_font;
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
        skClearColor1i(CS_Grey02);
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

        const skScalar maxY  = m_size.y - (75 + y);
        const skScalar maxX = m_size.x - 50;

        skRect(25, y + 50, maxX, maxY);
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
        m_lastFill(0),
        m_grad(nullptr),
        m_font(nullptr),
        m_done(false)
    {
    }

    ~Application() override
    {
        delete m_manager;

        skDeleteImage(m_grad);
        skDeleteFont(m_font);
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
        m_manager->dispatchInitialEvents();

        m_lastFill = 0x00000000;
        m_font     = skNewFont(SK_FONT_DEFAULT, 48, 96);
        m_grad     = skCreateImage(512, 512, SK_RGBA);

        SKcolorStop colorStops[] = {
            {0, 0x000000FF},
            {1, 0xFFFFFFFF},
        };

        skImageLinearGradient(m_grad,
                              colorStops,
                              2,
                              SK_NORTH | SK_EAST);
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
