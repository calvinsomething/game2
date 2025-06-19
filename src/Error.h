#include <exception>
#include <windows.h>

#ifndef HANDLE_HRESULT
#define HANDLE_HRESULT(hr)                                                                                             \
    {                                                                                                                  \
        if (FAILED(hr))                                                                                                \
        {                                                                                                              \
            throw WindowsError(__FILE__, __LINE__);                                                                    \
        }                                                                                                              \
    }
#endif

class Error : public std::exception
{
  public:
    virtual const char *title() const = 0;

  protected:
    char message[1024] = {};
};

class WindowsError : public Error
{
  public:
    WindowsError(const char *file_name, int line_number);
    ~WindowsError();

    const char *title() const override;
    const char *what() const override;
};
