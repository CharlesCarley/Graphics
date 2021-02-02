#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "skGraphicsWindow.h"

#ifdef Graphics_BUILD_WINDOW

typedef struct Program
{
    SKwindowManager manager;
    SKwindow        window;
    int             exitRequest;
    SKimage         texture;
    SKint32         texWidth;
    SKint32         texHeight;
    SKuint8         leftDown;
    SKscalar        m_co;
    SKscalar        m_lco;
} Program;

void InitializeContext(SKcontext ctx, Program* program)
{
    program->texture = skNewImage();
    skImageLoad(program->texture, "test9.png");

    SKint32 bpp, fmt;
    skGetImage1i(program->texture, SK_IMAGE_WIDTH, &program->texWidth);
    skGetImage1i(program->texture, SK_IMAGE_HEIGHT, &program->texHeight);
    skGetImage1i(program->texture, SK_IMAGE_BPP, &bpp);
    skGetImage1i(program->texture, SK_IMAGE_FORMAT, &fmt);
    skSetImage1i(program->texture, SK_IMAGE_FILTER, SK_FILTER_BI_LINEAR);

    skSetContext1i(SK_PROJECT, SK_STD);

    printf("Image : %d, %d\n", program->texWidth, program->texHeight);
}

void UpdateMouseCo(void* user, SKint32 x, SKint32 y)
{
    Program* data = user;
    if (!data)
        return;


    if (data->m_lco < 0)
        data->m_lco = (SKscalar)y;

    data->m_co += (SKscalar)y - data->m_lco;
    data->m_lco = (SKscalar)y;
    if (data->m_co > 360)
        data->m_co = 0;
}

void WindowClosed(SKwindow window, void* user)
{
    Program* data = user;
    if (data != NULL)
        data->exitRequest = 1;
}

void WindowResized(SKwindow window, void* user, SKuint32 width, SKuint32 height)
{
    skSetContext2i(SK_CONTEXT_SIZE, width, height);
    skRedrawWindow(window);
}

void WindowPaint(SKwindow window, void* user)
{
    Program* data = user;
    if (!data)
        return;

    skClearColor1i(CS_Grey02);
    skClearContext();

    skProjectContext(SK_STD);

    SKint32 s[3] = {0, 0, 0};
    skGetContext2i(SK_CONTEXT_SIZE, (SKint32*)&s);
    SKscalar w, h, x, y, wx, wy, sz;

    wx = (SKscalar)(s[0] >> 1);
    wy = (SKscalar)(s[1] >> 1);
    w  = (SKscalar)data->texWidth;
    h  = (SKscalar)data->texHeight;


    if (w < h)
        sz = w;
    else 
        sz = h;

    x = wx - sz / 2.f;
    y = wy - sz / 2.f;

    skSelectImage(data->texture);
    skRect(0, 0, sz, sz);
    skColor1ui(CS_Grey10);

    skLoadIdentity();

    skTranslate(-sz / 2.f, -sz / 2.f);
    skRotate(data->m_co / 180.f * 3.1459f);
    skTranslate(x + sz / 2.f, y + sz / 2.f);

    skFill();
    skSelectImage(NULL);
}

void WindowMotion(SKwindow window, void* user, SKint32 x, SKint32 y)
{
    if (skGetMouseState(window, MBT_L) && skGetKeyCodeState(window, KC_CTRL))
    {
        skGetMouseCo(window, &x, &y);
        UpdateMouseCo(user, x, y);
        skRedrawWindow(window);
    }
}

void WindowButtonPressed(SKwindow    window,
                         void*       user,
                         skMouseCode code,
                         SKint32     x,
                         SKint32     y)
{
    ((Program*)user)->leftDown = skGetMouseState(window, MBT_L);
    skRedrawWindow(window);
}

void WindowButtonReleased(SKwindow    window,
                          void*       user,
                          skMouseCode code,
                          SKint32     x,
                          SKint32     y)
{
    ((Program*)user)->leftDown = skGetMouseState(window, MBT_L);
    skRedrawWindow(window);
}


void WindowKeyDown(SKwindow window, void* user, skScanCode code)
{
    if (code == KC_ESC)
        ((Program*)user)->exitRequest = 1;
}

void WindowKeyUp(SKwindow window, void* user, skScanCode code)
{
    if (code == KC_Q)
        ((Program*)user)->exitRequest = 1;
}



SKwindowCallbacks Events = {
    WindowClosed,
    WindowResized,
    WindowPaint,
    WindowMotion,
    WindowButtonPressed,
    WindowButtonReleased,
    WindowKeyDown,
    WindowKeyUp,
    NULL,
};

int main(int argc, char** argv)
{
    Program p;
    p.manager = skNewWindowManager(WM_CTX_PLATFORM);
    p.m_lco   = -1;

    p.m_co        = 0;
    p.exitRequest = 0;
    Events.user   = &p;

    p.window = skNewWindow(p.manager,
                           "C-API-Test",
                           800,
                           600,
                           WM_WF_CENTER | WM_WF_SHOWN,
                           &Events);

    SKcontext ctx = skNewContext();
    skBroadcast(p.manager, SK_WIN_SIZE);

    InitializeContext(ctx, &p);

    while (!p.exitRequest)
    {
        skDispatch(p.manager);
    }

    skDeleteContext(ctx);
    skDelWindowManager(p.manager);
    return 0;
}
#else

int main(int argc, char** argv)
{
    return 0;
}

#endif