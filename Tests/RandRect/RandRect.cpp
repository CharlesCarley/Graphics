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
#include "ColorTable.inl"
#include "Graphics/skGraphics.h"
#include "Math/skMath.h"
#include "Math/skRectangle.h"
#include "Math/skVector2.h"
#include "Utils/skDisableWarnings.h"
#include "Utils/skFixedArray.h"
#include "Utils/skLogger.h"
#include "Utils/skRandom.h"
#include "Window/skMouse.h"
#include "Window/skWindow.h"
#include "Window/skWindowHandler.h"
#include "Window/skWindowManager.h"

#if SK_PLATFORM == SK_PLATFORM_EMSCRIPTEN
constexpr int WindowX     = 320;
constexpr int WindowY     = 240;
constexpr int WindowFlags = WM_WF_CENTER | WM_WF_SHOWN;
#else
constexpr int WindowX     = 800;
constexpr int WindowY     = 600;
constexpr int WindowFlags = WM_WF_CENTER | WM_WF_MAXIMIZE | WM_WF_SHOWN;
#endif

struct SortRect
{
    SortRect() :
        rect(0, 0, 0, 0),
        used(0),
        color(CS_Transparent)
    {
    }
    skRectangle rect;
    int         used;
    SKuint32    color;
};

typedef skFixedArray<skRectangle, 120> RandArray;
typedef skFixedArray<SortRect, 120>    SortArray;

class Application final : public skWindowHandler
{
private:
    skWindowManager* m_manager;
    skWindow*        m_window;
    skVector2        m_size, m_mouseCo, m_scale;
    bool             m_done;

    RandArray m_rand;
    SortArray m_sort;

public:
    Application();
    ~Application() override;

    void run();

private:
    void handle(const skEventType& evt, skWindow* caller) override;
    void draw();

    skVector2 initializeRect();
    skVector2 initializeRectSort();

    void doFill(skScalar x,
                skScalar y,
                skScalar w,
                skScalar h,
                SKuint32 color) const;

    static skScalar unitRandom();
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
    }
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

skScalar Application::unitRandom()
{
    return (skScalar)skUnitRandom();
}

void Application::run()
{
    skRandInit();
    m_manager = new skWindowManager(WM_CTX_SDL, this);
    m_window  = m_manager->create("Random Rectangle", WindowX, WindowY, WindowFlags);

    skNewContext();
    m_manager->process();
}

skVector2 Application::initializeRect()
{
    skVector2 maxRects = skVector2::Zero;

    for (SKuint16 i = 0; i < 120; ++i)
    {
        m_sort[i].used = 0;
        m_sort[i].rect = m_rand[i];

        const skScalar x = m_sort[i].rect.width;
        const skScalar y = m_sort[i].rect.height;

        if (maxRects.x < x)
            maxRects.x = x;
        if (maxRects.y < y)
            maxRects.y = y;
    }
    return maxRects;
}

int CompSortGT(SortRect p1, SortRect p2)
{
    if (p1.rect.height > p2.rect.height)
        return 1;
    return 0;
}

int CompSortEnd(SortRect p1, SortRect p2)
{
    if (p1.used > p2.used)
        return 1;
    return 0;
}

skVector2 Application::initializeRectSort()
{
    skVector2 maxRects = skVector2::Zero;

    for (SKuint16 i = 0; i < COLOR_SIZE; ++i)
    {
        m_sort[i].used  = 0;
        m_sort[i].rect  = m_rand[i];
        m_sort[i].color = COLOR_TABLE[i];

        const skScalar x = m_sort[i].rect.width;
        const skScalar y = m_sort[i].rect.height;

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

void Application::doFill(skScalar       x,
                         skScalar       y,
                         skScalar       w,
                         skScalar       h,
                         const SKuint32 color) const
{
    skColor1ui(color);
    skRect(x, y, w, h);
    skFill();
    skColor1ui(CS_Grey00);
    skStroke();
}

void Application::draw()
{
    skClearColor1i(CS_Grey02);
    skClearContext();
    skProjectRect(0, 0, m_size.x, m_size.y);

    skScalar x, y, w, h;
    m_scale.x = m_size.x / 20;
    m_scale.y = m_size.y / 20;

    skScalar shelf;
    SKuint16 i;

    m_rand.resize(0);
    m_sort.resize(0);

    for (i = 0; i < COLOR_SIZE; ++i)
    {
        m_rand.push_back(skRectangle());
        m_sort.push_back(SortRect());

        m_rand[i].x = unitRandom() * (m_size.x / 2.f - m_scale.x);
        m_rand[i].y = unitRandom() * m_size.y;

        if (m_rand[i].x > m_size.x / 2.f)
            m_rand[i].x = 0;

        m_sort[i].color  = COLOR_TABLE[i];
        m_rand[i].width  = unitRandom() * m_scale.x;
        m_rand[i].height = unitRandom() * m_scale.y;

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
            COLOR_TABLE[i]);
    }

    skVector2 maxRects = initializeRect();

    i = 0;

    for (shelf = 0; shelf < m_size.y && i < COLOR_SIZE; shelf += (SKuint16)maxRects.y)
    {
        x = m_size.x / 2;
        while (i < COLOR_SIZE && x + m_sort[i].rect.width < m_size.x)
        {
            if (m_sort[i].used == 0)
            {
                y = shelf;
                w = m_sort[i].rect.width;
                h = m_sort[i].rect.height;

                m_sort[i].used = 1;

                doFill(x, y, w, h, m_sort[i].color);
                x += m_sort[i].rect.width;
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

    for (; shelf < m_size.y && i<COLOR_SIZE; shelf += (SKuint16)mx> 0 ? maxRects.y : mx)
    {
        x  = m_size.x / 2;
        mx = 0;
        while (i < COLOR_SIZE && x + m_sort[i].rect.width < m_size.x)
        {
            if (m_sort[i].used != 0)
            {
                ++i;
                continue;
            }

            y = shelf;
            w = m_sort[i].rect.width;
            h = m_sort[i].rect.height;

            m_sort[i].used = 1;

            if (mx < h)
                mx = h;

            doFill(x, y, w, h, m_sort[i].color);

            skScalar yOffs = m_sort[i].rect.height;

            for (SKuint16 j = i + 1; j < COLOR_SIZE && yOffs < maxRects.y; ++j)
            {
                if (m_sort[j].used == 0)
                {
                    h = m_sort[j].rect.height;
                    w = m_sort[j].rect.width;
                    if (mx < h)
                        mx = h;

                    if (yOffs + h < maxRects.y && w < m_sort[i].rect.width)
                    {
                        m_sort[j].used = 1;
                        doFill(x, y + yOffs, w, h, m_sort[j].color);
                        yOffs += h;
                    }
                }
            }
            x += m_sort[i].rect.width;
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
