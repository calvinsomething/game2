#include <exception>
#include <windows.h>

#ifndef GET_HRESULT_HANDLER

#define GET_HRESULT_HANDLER(hr, error_type)                                                                            \
    {                                                                                                                  \
        if (FAILED(hr))                                                                                                \
        {                                                                                                              \
            throw error_type(__FILE__, __LINE__);                                                                      \
        }                                                                                                              \
    }

#define HANDLE_HRESULT(hr) GET_HRESULT_HANDLER(hr, WindowsError)

#endif

class Error : public std::exception
{
  public:
    virtual const char *title() const = 0;

  protected:
    char message[1024] = {};
};

// WindowsError
//
class WindowsError : public Error
{
  public:
    WindowsError(const char *file_name, int line_number);
    ~WindowsError();

    const char *title() const override;
    const char *what() const override;
};

// GfxError
//
#ifndef NDEBUG
class GfxError : public Error
{
  public:
    GfxError(const char *file_name, int line_number);
    ~GfxError();

    const char *title() const override;
    const char *what() const override;
};
#endif
