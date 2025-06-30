#pragma once

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

// Windows Error Handler
#define HANDLE_WIN_ERR(hr) GET_HRESULT_HANDLER(hr, WindowsError)

// Gfx Error Handler
#ifdef NDEBUG
#define HANDLE_GFX_ERR(hr) GET_HRESULT_HANDLER(hr, WindowsError)
#else
#define HANDLE_GFX_ERR(hr) GET_HRESULT_HANDLER(hr, GfxError)
#endif

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
