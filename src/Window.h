#include <Windows.h>

class Window
{
  public:
    Window();
    ~Window();

    void handle_messages();

    HWND get_handle();

  private:
    HWND hwnd;

    static LRESULT window_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
};
