#include <stdlib.h>
#include <string.h>
#include "Graphics/skGraphicsWindow.h"

#ifdef Graphics_BUILD_WINDOW

typedef struct Program
{
    SKwindowManager manager;
    SKwindow        window;
    SKimage         texture;
    SKint32         texWidth;
    SKint32         texHeight;
    SKuint8         leftDown;
    SKscalar        m_co;
    SKscalar        m_lco;
} Program;

void InitializeContext(Program* program)
{
    program->texture = skImageLoad("test2.png");

    SKint32 bpp, fmt;
    skGetImage1i(program->texture, SK_IMAGE_WIDTH, &program->texWidth);
    skGetImage1i(program->texture, SK_IMAGE_HEIGHT, &program->texHeight);
    skGetImage1i(program->texture, SK_IMAGE_BPP, &bpp);
    skGetImage1i(program->texture, SK_IMAGE_PIXEL_FORMAT, &fmt);
    skSetImage1i(program->texture, SK_IMAGE_FILTER, SK_FILTER_BI_LINEAR);
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
}

void WindowResized(SKwindow window, void* user, SKuint32 width, SKuint32 height)
{
    skSetContext2i(SK_CONTEXT_SIZE, width, height);
    skWindowRedraw(window);
}

void WindowPaint(SKwindow window, void* user)
{
    Program* data = user;
    if (!data)
        return;

    skClearColor1i(CS_Grey02);
    skClearContext();
    skProjectContext(SK_STANDARD);

    SKint32 s[3] = {0, 0, 0};
    skGetContext2i(SK_CONTEXT_SIZE, (SKint32*)&s);
    SKscalar sz;

    const SKscalar wx = (SKscalar)(s[0] >> 1);
    const SKscalar wy = (SKscalar)(s[1] >> 1);
    const SKscalar w  = (SKscalar)data->texWidth;
    const SKscalar h  = (SKscalar)data->texHeight;

    if (w < h)
        sz = w;
    else
        sz = h;

    const SKscalar x = wx - sz * .5f;
    const SKscalar y = wy - sz * .5f;

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
        skWindowRedraw(window);
    }
}

void WindowButtonPressed(SKwindow    window,
                         void*       user,
                         skMouseCode code,
                         SKint32     x,
                         SKint32     y)
{
    ((Program*)user)->leftDown = skGetMouseState(window, MBT_L);
    skWindowRedraw(window);
}

void WindowButtonReleased(SKwindow    window,
                          void*       user,
                          skMouseCode code,
                          SKint32     x,
                          SKint32     y)
{
    ((Program*)user)->leftDown = skGetMouseState(window, MBT_L);
    skWindowRedraw(window);
}

void WindowKeyDown(SKwindow window, void* user, skScanCode code)
{
    if (code == KC_ESC)
        skWindowClose(window);
}

void WindowKeyUp(SKwindow window, void* user, skScanCode code)
{
    if (code == KC_Q)
        skWindowClose(window);
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
    memset(&p, 0, sizeof(Program));

    p.manager   = skNewWindowManager(WM_CTX_SDL);
    p.m_lco     = -1;
    Events.user = &p;
    p.window    = skNewWindow(p.manager,
                           "C-API-Test",
                           800,
                           600,
                           WM_WF_CENTER | WM_WF_SHOWN,
                           &Events);

    SKcontext ctx = skNewContext();
    skBroadcast(p.manager, SK_WIN_SIZE);

    InitializeContext(&p);

    skProcess(p.manager);

    if (p.texture)
        skDeleteImage(p.texture);

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
