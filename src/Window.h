#include <Windows.h>

class Window
{
  public:
    Window();
    ~Window();

    operator bool();

  private:
    HWND hwnd;

    static LRESULT window_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
};
