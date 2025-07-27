#include "Window.h"

Window::Window()
{
    WNDCLASS wc = {};
    wc.lpszClassName = L"Main Window";
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpfnWndProc = window_proc;

    RegisterClass(&wc);

    hwnd = CreateWindowExW(0, wc.lpszClassName, wc.lpszClassName, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                           // Size and position
                           CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
                           nullptr, // Parent window
                           nullptr, // Menu
                           wc.hInstance,
                           nullptr // Additional application data
    );
}

Window::~Window(){};

namespace Global
{
extern bool running;
}; // namespace Global

LRESULT __stdcall Window::window_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    // case WM_INPUT: ignored
    case WM_DESTROY:
        Global::running = false;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

void Window::handle_messages()
{
    static UINT max = WM_INPUT - 1, min = WM_INPUT + 1;

    while (PeekMessageW(&msg, hwnd, 0, max, PM_REMOVE) ||
           PeekMessageW(&msg, hwnd, min, static_cast<UINT>(-1), PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_QUIT)
        {
            return;
        }
    }
}

HWND Window::get_handle()
{
    return hwnd;
}
