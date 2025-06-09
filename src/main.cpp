#include <windows.h>

#include <exception>

#include "Gfx.h"
#include "Window.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    try
    {
        Window window;

        Gfx gfx(window.get_handle());

        window.handle_messages();

        return 0;
    }
    catch (std::exception &exc)
    {
        MessageBoxA(nullptr, exc.what(), "Unknown Error", MB_OK);
    }
    catch (...)
    {
        MessageBoxW(nullptr, L"Something went wrong.", L"Unknown Error", MB_OK);
    }

    return -1;
}
