#include <DirectXMath.h>
#include <windows.h>

#include <exception>

#include "Error.h"
#include "Gfx/Camera.h"
#include "Gfx/Gfx.h"
#include "Window.h"
#include "models/Cube.h"

namespace Global
{
bool running = true;
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    Window window;

    try
    {
        Gfx gfx(window.get_handle());

        Camera camera(gfx);

        Cube cube(gfx);

        float color[] = {0.5f, 0.2f, 0.2f, 1.0f};

        while (Global::running)
        {
            window.handle_messages();

            gfx.clear(color);

            cube.bind();

            cube.update();

            camera.update();

            gfx.end_frame();
        }

        return 0;
    }
    catch (Error &exc)
    {
        MessageBoxA(window.get_handle(), exc.what(), exc.title(), MB_OK);
    }
    catch (std::exception &exc)
    {
        MessageBoxA(window.get_handle(), exc.what(), "Unknown Error", MB_OK);
    }
    catch (...)
    {
        MessageBoxA(window.get_handle(), "Something went wrong.", "Unknown Error", MB_OK);
    }

    return -1;
}
