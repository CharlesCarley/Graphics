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
#include "Utils/skLogger.h"
#include "Window/skMouse.h"
#include "Window/skWindow.h"
#include "Window/skWindowEnums.h"
#include "Window/skWindowHandler.h"
#include "Window/skWindowManager.h"
#include "Utils/skDisableWarnings.h"
#include "skGraphics.h"

const SKuint32 COLOR_TABLE[] = {
    CS_Grey00,
    CS_Grey01,
    CS_Grey02,
    CS_Grey03,
    CS_Grey04,
    CS_Grey05,
    CS_Grey06,
    CS_Grey07,
    CS_Grey08,
    CS_Grey09,
    CS_Grey10,
    CS_Grey04HL,
    CS_Color01,
    CS_Color01HL,
    CS_Color02,
    CS_Color02HL,
    CS_Color03,
    CS_Color03HL,
    CS_Color04,
    CS_Color04HL,
    CS_Color05,
    CS_Color05HL,
    CS_Color06,
    CS_Color06HL,
    CS_Color07,
    CS_Color07HL,
    CS_Color08,
    CS_Color08HL,
    CS_Color09,
    CS_Color09HL,
    CS_Color10,
    CS_Color10HL,
    CS_Color11,
    CS_Color11HL,
    CS_Indicator,
    CS_Green,
    CS_BaseRed,
    CS_BaseGreen,
    CS_BaseRedHL,
    CS_BaseGreenHL,
    CS_Color081,
    CS_Color082,
    CS_Color083,
    CS_Color084,
    CS_Color085,
    CS_Macab031,
    CS_Macab032,
    CS_Macab033,
    CS_Macab034,
    CS_Macab035,
    CS_Macab051,
    CS_Macab052,
    CS_Macab053,
    CS_Macab054,
    CS_Macab055,
    CS_LightBackground,
    CS_LabelText,
    CS_LabelAccent,
    CS_LabelEmphasis3,
    CS_LabelEmphasis2,
    CS_LabelEmphasis1,
    CS_LabelEmphasisA3,
    CS_LabelEmphasisA2,
    CS_LabelEmphasisA1,
    CS_ButtonBackground,
    CS_ButtonText,
    CS_Tiny01,
    CS_Tiny02,
    CS_Tiny03,
    CS_Tiny04,
    CS_Tiny05,
    CS_Macab01,
    CS_Macab02,
    CS_Macab03,
    CS_Macab04,
    CS_Macab05,
    CS_Pismo01,
    CS_Pismo02,
    CS_Pismo03,
    CS_Pismo04,
    CS_Pismo05,
    CS_OBContrast01,
    CS_OBContrast02,
    CS_OBContrast03,
    CS_OBContrast04,
    CS_OBContrast05,
    CS_OffWhite,
    CS_OffWhiteM,
    CS_OffWhiteD0,
    CS_OffWhiteD1,
    CS_OffWhiteD2,
    CS_OffWhiteD3,
    CS_Transparent,
    CS_Glass,
    CS_Macab011,
    CS_Macab012,
    CS_Macab013,
    CS_Macab014,
    CS_Macab015,
    CS_Macab021,
    CS_Macab022,
    CS_Macab023,
    CS_Macab024,
    CS_Macab025,
    CS_Macab041,
    CS_Macab042,
    CS_Macab043,
    CS_Macab044,
    CS_Macab045,
    CS_DarkBackground,
    CS_LabelText,
    CS_LabelAccent,
    CS_LabelEmphasis3,
    CS_LabelEmphasis2,
    CS_LabelEmphasis1,
    CS_LabelEmphasisA3,
    CS_LabelEmphasisA2,
    CS_LabelEmphasisA1,
    CS_ButtonBackground,
    CS_ButtonText,
};
const SKuint32 COLOR_SIZE = sizeof COLOR_TABLE / sizeof(SKuint32);

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

        skScalar x, y;
        x = 20;
        y = 20;

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

        const skScalar max  = m_size.y - (25 + y);
        const skScalar max2 = max - 50;

        skRect(25, 50 + y, max2, max2);

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

    void initialize(void)
    {
        m_manager = new skWindowManager(WM_CTX_PLATFORM);

        m_window = m_manager->create("Color Test",
                                     800,
                                     600,
                                     WM_WF_CENTER | WM_WF_MAXIMIZE | WM_WF_SHOWN);

        if (!m_window)
        {
            m_done = true;
            delete m_manager;
            m_manager = nullptr;
            return;
        }

        m_manager->addHandler(this);
        skSetCurrentContext(skNewBackEndContext(SK_BE_OpenGL));
        m_manager->broadcastEvent(SK_WIN_SIZE);

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
                              SK_SOUTH | SK_EAST);
    }

    void update(void) const
    {
        if (m_manager)
            m_manager->processInteractive(true);
    }

    bool isDone() const
    {
        return m_done;
    }
};

int main(int, char**)
{
    try
    {
        skLogger* log = new skLogger();
        log->setFlags(LF_STDOUT);

        Application app;
        app.initialize();
        while (!app.isDone())
            app.update();
    }
    catch (...)
    {
        skLogf(LD_ERROR, "An exception occurred\n");
    }

    delete skLogger::getSingletonPtr();
}
