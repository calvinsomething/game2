#include "Window.h"

Window::Window()
{
    WNDCLASS wc = {};
    wc.lpszClassName = L"Main Window";
    wc.hInstance = GetModuleHandleW(0);
    wc.lpfnWndProc = window_proc;

    RegisterClass(&wc);

    hwnd = CreateWindowExW(0, wc.lpszClassName, wc.lpszClassName, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                           // Size and position
                           CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                           NULL, // Parent window
                           NULL, // Menu
                           wc.hInstance,
                           NULL // Additional application data
    );
}

Window::~Window(){};

LRESULT __stdcall Window::window_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT: {
        // PAINTSTRUCT ps;
        // HDC hdc = BeginPaint(hwnd, &ps);

        // FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        // EndPaint(hwnd, &ps);
    }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

void Window::handle_messages()
{
    if (hwnd)
    {
        MSG msg = {};

        while (GetMessageW(&msg, hwnd, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

HWND Window::get_handle()
{
    return hwnd;
}
