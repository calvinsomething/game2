#include <Windows.h>

#include <array>

class Window
{
  public:
    Window();
    ~Window();

    RECT get_rect();
    RECT get_full_screen_size();

    void set_full_screen();
    void set_rect(RECT rect);

    void handle_messages();

    HWND get_handle();

  private:
    HWND hwnd;
    WINDOWINFO info;

    MSG msg = {};

    static LRESULT window_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
};
