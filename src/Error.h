#pragma once

#include <d3d11.h>
#include <dxgidebug.h>
#include <windows.h>

#include <exception>

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
#define HANDLE_GFX_ERR(hr) GET_HRESULT_HANDLER(hr, GfxError)

// Gfx Info Handler
#ifdef NDEBUG
#define HANDLE_GFX_INFO(call) call
#else
#define HANDLE_GFX_INFO(call)                                                                                          \
    {                                                                                                                  \
        GfxError::set_mark();                                                                                          \
                                                                                                                       \
        call;                                                                                                          \
                                                                                                                       \
        if (GfxError::found_new_messages())                                                                            \
        {                                                                                                              \
            throw GfxError(__FILE__, __LINE__);                                                                        \
        }                                                                                                              \
    }
#endif

class Error : public std::exception
{
  public:
    virtual const char *title() const = 0;

  protected:
    static char message[1024];
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

    static void init_dxgi_debug();
    static void init_d3d11_debug(ID3D11Device *device);

    static void set_mark();
    static bool found_new_messages();

  private:
    static IDXGIDebug *dxgi_debug;
    static IDXGIInfoQueue *dxgi_info_queue;
    static ID3D11Debug *d3d11_debug;
    static ID3D11InfoQueue *d3d11_info_queue;

    static UINT64 dxgi_message_count;
    static UINT64 d3d11_message_count;
};
#endif
