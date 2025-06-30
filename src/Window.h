#include <Windows.h>

class Window
{
  public:
    Window();
    ~Window();

    void handle_message();

    HWND get_handle();

  private:
    HWND hwnd;
    MSG msg = {};

    static LRESULT window_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
};
