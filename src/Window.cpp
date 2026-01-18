#include "Window.h"

#include "Global.h"

Window::Window()
{
    WNDCLASS wc = {};
    wc.lpszClassName = L"Main Window";
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpfnWndProc = window_proc;

    RegisterClass(&wc);

    hwnd = CreateWindowExW(0, wc.lpszClassName, wc.lpszClassName, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                           // Size and position
                           CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                           nullptr, // Parent window
                           nullptr, // Menu
                           wc.hInstance,
                           nullptr // Additional application data
    );

    GetWindowInfo(hwnd, &info);
}

Window::~Window() {};

RECT Window::get_rect()
{
    return info.rcClient;
}

void Window::set_rect(RECT rect)
{
    SetWindowPos(hwnd, HWND_TOP, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
}

void Window::set_full_screen()
{
    set_rect(get_full_screen_size());
    SetWindowLongPtrW(hwnd, GWL_STYLE, (info.dwStyle & ~WS_OVERLAPPEDWINDOW) | WS_POPUP);
}

RECT Window::get_full_screen_size()
{
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFO monitor_info = {};
    monitor_info.cbSize = sizeof(MONITORINFO);

    GetMonitorInfoW(monitor, &monitor_info);

    return monitor_info.rcMonitor;
}

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
