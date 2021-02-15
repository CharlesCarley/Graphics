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
#include "Math/skMath.h"
#include "Math/skVector2.h"
#include "Utils/skDisableWarnings.h"
#include "Utils/skLogger.h"
#include "Window/skKeyboard.h"
#include "Window/skMouse.h"
#include "Window/skWindow.h"
#include "Window/skWindowEnums.h"
#include "Window/skWindowHandler.h"
#include "Window/skWindowManager.h"
#include "skGraphics.h"

const SKuint32 WindowFlags = WM_WF_CENTER | WM_WF_MAXIMIZE | WM_WF_SHOWN;
const int      MaxTest     = 5;


class Application : public skWindowHandler
{
private:
    skWindowManager* m_manager;
    skWindow*        m_window;
    skKeyboard*      m_keyboard;
    skMouse*         m_mouse;
    skVector2        m_size, m_mouseCo;
    int              m_test;
    int              m_projectionMode;
    int              m_accumulator, m_q, m_p;
    SKfont           m_font;
    SKcachedString   m_cstring;

private:
    void handle(const skEventType& evt, skWindow* caller) override
    {
        switch (evt)
        {
        case SK_WIN_DESTROY:
            break;
        case SK_WIN_PAINT:
            draw();
            break;
        case SK_MOUSE_MOVED:
            m_mouseCo.x = (skScalar)m_mouse->x.abs;
            m_mouseCo.y = (skScalar)m_mouse->y.abs;
            break;
        case SK_WIN_SIZE:
            m_size.x = (skScalar)caller->getWidth();
            m_size.y = (skScalar)caller->getHeight();

            skSetContext2f(SK_CONTEXT_SIZE, m_size.x, m_size.y);
            caller->refresh();
            break;
        case SK_KEY_PRESSED:
            handleKey();
            break;
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



    void handleKey()
    {
        if (m_keyboard->key == KC_ESC)
        {
            m_window->close();
        }
        else if (m_keyboard->key == KC_1)
        {
            if (m_projectionMode != 0)
            {
                skSetFont1i(m_font, SK_FONT_SIZE, m_accumulator);
                skSetContext1i(SK_Y_UP, 0);
                skRebuildCachedString(m_cstring);
                m_projectionMode = 0;
                m_window->refresh();
            }
        }
        else if (m_keyboard->key == KC_2)
        {
            if (m_projectionMode != 1)
            {
                skSetFont1i(m_font, SK_FONT_SIZE, m_accumulator);
                skSetContext1i(SK_Y_UP, 1);
                skRebuildCachedString(m_cstring);
                m_projectionMode = 1;
                m_window->refresh();
            }
        }
        else if (m_keyboard->key == KC_UP)
        {
            m_test++;
            if (m_test > MaxTest - 1)
                m_test = MaxTest - 1;
            else
            {
                initTest();
                m_window->refresh();
            }
        }
        else if (m_keyboard->key == KC_DOWN)
        {
            m_test--;
            if (m_test < 0)
                m_test = 0;
            else
            {
                initTest();
                m_window->refresh();
            }
        }
        else
        {
            if (m_keyboard->isKeyDown(KC_LEFT))
            {
                if (m_keyboard->isKeyDown(KC_P))
                    m_p--;
                else if (m_keyboard->isKeyDown(KC_Q))
                    m_q--;
                else
                    m_accumulator--;
                m_window->refresh();
            }
            else if (m_keyboard->isKeyDown(KC_RIGHT))
            {
                if (m_keyboard->isKeyDown(KC_P))
                    m_p++;
                else if (m_keyboard->isKeyDown(KC_Q))
                    m_q++;
                else
                    m_accumulator++;
                m_window->refresh();
            }
        }
    }

    void initTest()
    {
        skLoadIdentity();
        skSetContext1f(SK_OPACITY, 1.f);
        skSetContext2f(SK_CONTEXT_SCALE, 1.f, 1.f);
        skSetContext2f(SK_CONTEXT_BIAS,  0.f, 0.f);

        skSetPaint1ui(SK_BRUSH_COLOR, CS_Grey10);
        skSetPaint1f(SK_BRUSH_MODE, SK_BM_REPLACE);
        skSetPaint1f(SK_PEN_WIDTH, 1.01f);

        if (m_test == 0)
        {
            m_accumulator = 0;
            skSetContext1i(SK_VERTICES_PER_SEGMENT, 32);
            skSetContext1f(SK_OPACITY, 0.6f);
            skSetPaint1ui(SK_BRUSH_COLOR, CS_Grey10);
            skSetPaint1f(SK_BRUSH_MODE, SK_BM_REPLACE);
            skSetPaint1f(SK_PEN_WIDTH, 1);
        }
        else if (m_test == 1)
        {
            skSetContext1i(SK_VERTICES_PER_SEGMENT, 32);
            skSetPaint1f(SK_PEN_WIDTH, 2);
            m_accumulator = 32;
        }
        else if (m_test == 2)
        {
            skSetContext1i(SK_VERTICES_PER_SEGMENT, 16);
        }
        else if (m_test == 3)
        {
            m_q = 5;
            m_p = 2;
            skSetContext1i(SK_VERTICES_PER_SEGMENT, 16);
            skSetContext1f(SK_OPACITY, 1.f);
            skSetPaint1ui(SK_BRUSH_COLOR, CS_Grey10);
            skSetPaint1f(SK_BRUSH_MODE, SK_BM_REPLACE);
            skSetPaint1f(SK_PEN_WIDTH, 1.01f);
        }
        else if (m_test == 4)
        {
            m_accumulator = 48;
        }
    }

public:
    Application() :
        m_manager(nullptr),
        m_window(nullptr),
        m_keyboard(nullptr),
        m_mouse(nullptr),
        m_test(0),
        m_projectionMode(0),
        m_accumulator(0),
        m_q(0),
        m_p(0),
        m_font(nullptr),
        m_cstring(nullptr)
    {
    }

    ~Application() override
    {
        skDeleteCachedString(m_cstring);
        skDeleteFont(m_font);
        skDeleteContext(skGetCurrentContext());
        delete m_manager;
    }

    void drawTest5() const
    {
        const char* text = "The quick brown fox jumped over the lazy dog";


        SKuint32 colors[5] = {
            CS_LabelEmphasis1,
            CS_LabelEmphasis2,
            CS_LabelEmphasis3,
            CS_LabelEmphasis1,
            CS_LabelEmphasis2,
        };

        SKint32 size = m_accumulator, yPos = 20;
        for (unsigned int color : colors)
        {
            skSetFont1i(m_font, SK_FONT_SIZE, size);
            skColor1ui(color);
            skDisplayString(m_font,
                            text,
                            -1,
                            20,
                            skScalar(yPos));

            size -= 8;
            yPos += size + 10;
        }

        skLoadIdentity();
        skTranslate(20, skScalar(yPos));
        skDisplayCachedString(m_cstring);
        skLoadIdentity();
    }


    void drawTest4() const
    {
        skScalar sz[5] = {};
        skGetContext2f(SK_CONTEXT_SIZE, sz);

        sz[4] = skMin(sz[0], sz[1]);


        if (m_projectionMode == 1)
        {
            sz[2] = -sz[4] / 2;
            sz[3] = -sz[4] / 2;
        }
        else
        {
            sz[2] = (sz[0] - sz[4]) / 2;
            sz[3] = (sz[1] - sz[4]) / 2;
        }
        skStar(sz[2], sz[3], sz[4], sz[4], m_q, m_p);
        skColor1ui(CS_Color04);
        skStroke();
    }

    void drawTest3() const
    {
        const SKscalar size = 200;
        const SKscalar ac   = skScalar(m_accumulator) * 5;

        skRoundRect(20, 20, size, size, ac, ac, SK_CNR_RT | SK_CNR_RB | SK_CNR_LB);
        skColor1ui(CS_Color02);
        skFill();
        skColor1ui(CS_Color02HL);
        skStroke();
    }

    void drawTest2() const
    {
        const SKscalar size = 200;
        skSetContext1i(SK_VERTICES_PER_SEGMENT, m_accumulator);

        skEllipse(20, 20, size, size / 2);
        skColor4f(1, 0, 0, .7f);
        skFill();
        skColor1ui(CS_Grey00);
        skStroke();
    }


    void drawTest1() const
    {
        const SKscalar size = 75;
        const SKscalar offs = 37.5;

        skColor1ui(0x00FF00FF);

        skSetPaint1ui(SK_BRUSH_COLOR, CS_Grey10);
        skSetPaint1f(SK_BRUSH_MODE, SK_BM_REPLACE);
        skRect(0, 0, size, size);
        skFill();

        skSetPaint1ui(SK_BRUSH_COLOR, 0xFF0000FF);
        skSetPaint1f(SK_BRUSH_MODE, SK_BM_ADD);
        skRect(offs, offs, size, size);
        skFill();


        skSetPaint1ui(SK_BRUSH_COLOR, 0xFFFFFFFF);
        skSetPaint1f(SK_BRUSH_MODE, SK_BM_SUBTRACT);
        skRect(offs * 2, offs * 2, size, size);
        skFill();

        skSetPaint1ui(SK_BRUSH_COLOR, 0x008000FF);
        skSetPaint1f(SK_BRUSH_MODE, SK_BM_MODULATE);
        skRect(offs * 3, offs * 3, size, size);
        skFill();

        skSetPaint1ui(SK_BRUSH_COLOR, 0x00FFFFFF);
        skSetPaint1f(SK_BRUSH_MODE, SK_BM_DIVIDE);
        skRect(offs * 4, offs * 4, size, size);
        skFill();
    }

    void draw(void) const
    {
        skClearColor1i(CS_Grey02);
        skClearContext();
        skProjectContext(m_projectionMode == 0 ? SK_STANDARD : SK_CARTESIAN);

        switch (m_test)
        {
        case 4:
            drawTest5();
            break;
        case 3:
            drawTest4();
            break;
        case 2:
            drawTest3();
            break;
        case 1:
            drawTest2();
            break;
        case 0:
        default:
            drawTest1();
            break;
        }

        m_window->flush();
    }


    void setupGraphics()
    {
        skNewContext();

        m_font = skNewFont(SK_FONT_DEFAULT, 72, 128);
        skSetFont1i(m_font, SK_FONT_FILTER, SK_FILTER_BI_LINEAR);
        skSetFont1i(m_font, SK_FONT_MIPMAP, 1);
        skSelectFont(m_font);

        skSetFont1f(m_font, SK_FONT_SIZE, 32);
        m_cstring = skNewCachedString();
        skBuildCachedString(m_cstring, "0123456789!@#$%^&*()_+");
        initTest();
    }

    int run()
    {
        m_manager = new skWindowManager(WM_CTX_PLATFORM);
        m_window  = m_manager->create("MiscTest",
                                     800,
                                     600,
                                     WindowFlags);
        m_keyboard = m_window->getKeyboard();
        m_mouse    = m_window->getMouse();
        m_manager->addHandler(this);

        setupGraphics();

        m_manager->broadcastEvent(SK_WIN_SIZE);
        m_manager->process();
        return 0;
    }
};


int main(int, char**)
{
    try
    {
        skLogger log;
        log.setFlags(LF_STDOUT);

        Application app;
        return app.run();
    }
    catch (...)
    {
        printf("An exception occurred\n");
    }
    return 0;
}
