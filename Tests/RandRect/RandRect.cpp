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
#include <algorithm>
#include <cmath>
#include "Math/skMath.h"
#include "Math/skRectangle.h"
#include "Math/skVector2.h"
#include "Utils/skDisableWarnings.h"
#include "Utils/skFixedArray.h"
#include "Utils/skRandom.h"
#include "Window/skMouse.h"
#include "Window/skWindow.h"
#include "Window/skWindowHandler.h"
#include "Window/skWindowManager.h"
#include "Graphics/skGraphics.h"

typedef struct SortRect
{
    skRectangle  m_rect;
    int          m_used;
    unsigned int m_color;
} SortRect;

static const SKuint32 COLORS[] = {
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

SKuint16 ColorSize = sizeof COLORS / sizeof(SKuint32);

typedef skFixedArray<skRectangle, 120> RandArray;
typedef skFixedArray<SortRect, 120>    SortArray;

class Application : public skWindowHandler
{
private:
    skWindowManager* m_manager;
    skWindow*        m_window;
    skVector2        m_size, m_mouseCo, m_scale;
    SKuint32         m_lastFill{};
    bool             m_done;

    RandArray m_rand;
    SortArray m_sort;

public:
    Application();
    ~Application() override;

    void initialize(void);
    void update(void) const;

    SK_INLINE bool isDone(void) const
    {
        return m_done;
    }

private:
    void handle(const skEventType& evt, skWindow* caller) override;
    void draw(void);

    skVector2 initializeRect(void);
    skVector2 initializeRectSort(void);

    void doFill(skScalar x, skScalar y, skScalar w, skScalar h, unsigned int color) const;

    static skScalar unitRandom(void);
};

int main(int argc, char** argv)
{
    Application app;
    app.initialize();

    while (!app.isDone())
        app.update();
}

Application::Application() :
    m_manager(nullptr),
    m_window(nullptr),
    m_done(false)
{
}

Application::~Application()
{
    skDeleteContext(skGetCurrentContext());
    delete m_manager;
}

skScalar Application::unitRandom(void)
{
    return (skScalar)skUnitRandom();
}

void Application::initialize(void)
{
    skRandInit();
    m_manager = new skWindowManager();
    m_window  = m_manager->create("", 800, 600, WM_WF_CENTER | WM_WF_MAXIMIZE | WM_WF_SHOWN);
    m_manager->addHandler(this);

    skSetCurrentContext(skNewContext());
    m_manager->broadcastEvent(SK_WIN_SIZE);

    m_lastFill = 0x00000000;
}

skVector2 Application::initializeRect(void)
{
    skScalar  x, y;
    skVector2 maxRects = skVector2::Zero;

    for (SKuint16 i = 0; i < 120; ++i)
    {
        m_sort[i].m_used = 0;
        m_sort[i].m_rect = m_rand[i];

        x = m_sort[i].m_rect.width;
        y = m_sort[i].m_rect.height;

        if (maxRects.x < x)
            maxRects.x = x;
        if (maxRects.y < y)
            maxRects.y = y;
    }

    return maxRects;
}

int CompSortGT(SortRect p1, SortRect p2)
{
    if (p1.m_rect.height > p2.m_rect.height)
        return 1;
    return 0;
}

int CompSortEnd(SortRect p1, SortRect p2)
{
    if (p1.m_used > p2.m_used)
        return 1;
    return 0;
}

skVector2 Application::initializeRectSort(void)
{
    skScalar  x, y;
    skVector2 maxRects = skVector2::Zero;

    for (SKuint16 i = 0; i < ColorSize; ++i)
    {
        m_sort[i].m_used  = 0;
        m_sort[i].m_rect  = m_rand[i];
        m_sort[i].m_color = COLORS[i];

        x = m_sort[i].m_rect.width;
        y = m_sort[i].m_rect.height;

        if (maxRects.x < x)
            maxRects.x = x;
        if (maxRects.y < y)
            maxRects.y = y;
    }

    m_sort.sort(CompSortGT);
    return maxRects;
}

void Application::handle(const skEventType& evt, skWindow* caller)
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
    case SK_WIN_EVT_UNKNOWN:
    case SK_KEY_PRESSED:
    case SK_KEY_RELEASED:
    case SK_MOUSE_PRESSED:
    case SK_MOUSE_RELEASED:
    case SK_MOUSE_WHEEL:
    case SK_WIN_SHOWN:
    case SK_WIN_HIDDEN:
        break;
    }
}

void Application::update(void) const
{
    m_manager->processInteractive(true);
}

void Application::doFill(skScalar x, skScalar y, skScalar w, skScalar h, unsigned int color) const
{
    skColor1ui(color);
    skRect(x, y, w, h);
    skFill();
    skColor1ui(CS_Grey00);
    skStroke();
}

void Application::draw(void)
{
    skClearColor1i(CS_Grey02);
    skClearContext();
    skProjectRect(0, 0, m_size.x, m_size.y);

    skScalar x, y, w, h;
    m_scale.x = m_size.x / 20;
    m_scale.y = m_size.y / 20;

    skScalar yoffs;
    skScalar shelf;
    SKuint16 i, j;

    m_rand.resize(0);
    m_sort.resize(0);

    for (i = 0; i < ColorSize; ++i)
    {
        m_rand.push_back(skRectangle());
        m_sort.push_back(SortRect());

        m_rand[i].x = unitRandom() * (m_size.x / 2.f - m_scale.x);
        m_rand[i].y = unitRandom() * m_size.y;

        if (m_rand[i].x > m_size.x / 2.f)
            m_rand[i].x = 0;

        m_sort[i].m_color = COLORS[i];
        m_rand[i].width   = unitRandom() * m_scale.x;
        m_rand[i].height  = unitRandom() * m_scale.y;

        if (m_rand[i].width < 10)
            m_rand[i].width = 10;
        if (m_rand[i].height < 10)
            m_rand[i].height = 10;

        if (m_rand[i].width > 50)
            m_rand[i].width = 50;
        if (m_rand[i].height > 50)
            m_rand[i].height = 50;

        doFill(
            m_rand[i].x,
            m_rand[i].y,
            m_rand[i].width,
            m_rand[i].height,
            COLORS[i]);
    }

    skVector2 maxRects = initializeRect();

    i = 0;

    for (shelf = 0; shelf < m_size.y && i < ColorSize; shelf += (SKuint16)maxRects.y)
    {
        x = m_size.x / 2;
        while (i < ColorSize && x + m_sort[i].m_rect.width < m_size.x)
        {
            if (m_sort[i].m_used == 0)
            {
                y = shelf;
                w = m_sort[i].m_rect.width;
                h = m_sort[i].m_rect.height;

                m_sort[i].m_used = 1;

                doFill(x, y, w, h, m_sort[i].m_color);
                x += m_sort[i].m_rect.width;
            }
            ++i;
        }
    }

    skColor1ui(CS_Grey00);
    skLine(m_size.x / 2, shelf, m_size.x, shelf);
    skStroke();

    maxRects = initializeRectSort();

    i = 0;

    shelf += 10;
    skScalar mx = 0;

    for (; shelf < m_size.y && i<ColorSize; shelf += (SKuint16)mx> 0 ? maxRects.y : mx)
    {
        x  = m_size.x / 2;
        mx = 0;
        while (i < ColorSize && x + m_sort[i].m_rect.width < m_size.x)
        {
            if (m_sort[i].m_used != 0)
            {
                ++i;
                continue;
            }

            y = shelf;
            w = m_sort[i].m_rect.width;
            h = m_sort[i].m_rect.height;

            m_sort[i].m_used = 1;

            if (mx < h)
                mx = h;

            doFill(x, y, w, h, m_sort[i].m_color);

            yoffs = m_sort[i].m_rect.height;

            for (j = i + 1; j < ColorSize && yoffs < maxRects.y; ++j)
            {
                if (m_sort[j].m_used == 0)
                {
                    h = m_sort[j].m_rect.height;
                    w = m_sort[j].m_rect.width;
                    if (mx < h)
                        mx = h;

                    if (yoffs + h < maxRects.y && w < m_sort[i].m_rect.width)
                    {
                        m_sort[j].m_used = 1;
                        doFill(x, y + yoffs, w, h, m_sort[j].m_color);
                        yoffs += h;
                    }
                }
            }
            x += m_sort[i].m_rect.width;
            // rectSubSort();
            ++i;
        }
    }

    skColor1ui(CS_Grey00);
    skLine(m_size.x / 2, shelf, m_size.x, shelf);
    skStroke();

    skColor1ui(CS_Grey00);
    skLine(m_size.x / 2, 0, m_size.x / 2, m_size.y);
    skStroke();

    m_window->flush();
}
