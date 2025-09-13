#include "Error.h"

#include <cstring>
#include <stdio.h>

#include <d3d11.h>

// Error static
//
char Error::message[1024] = {};

// WindowsError
//
WindowsError::WindowsError(const char *file_name, int line_number)
{
    DWORD error_code = GetLastError();

    char *buffer = 0;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error_code, 0,
                   reinterpret_cast<LPSTR>(&buffer), 0, nullptr);

    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error_code, 0, buffer, static_cast<DWORD>(LocalSize(buffer)),
                   nullptr);

    sprintf_s(message, "%s:%d: %s", file_name, line_number, buffer);

    LocalFree(buffer);
}

WindowsError::~WindowsError()
{
}

const char *WindowsError::title() const
{
    return "Windows Error";
}

const char *WindowsError::what() const
{
    return message;
}

// GfxError
//
bool GfxError::write_message(char *msg, int len, size_t &j, bool first_line)
{
    const char *ellipses = 0;

    if (j + len > sizeof(message) - 1)
    {
        len = static_cast<int>(sizeof(message) - j - 4);
        ellipses = "...";
    }

    if (!first_line)
    {
        message[j++] = '\n';
    }

    std::memcpy(message + j, msg, len);

    j += len;

    if (ellipses)
    {
        std::memcpy(message + j, ellipses, 3);

        return false;
    }

    return true;
}

GfxError::GfxError(const char *file_name, int line_number)
{
    set_mark();

    size_t j = 0;

    int encountered_ids[10] = {};
    size_t encountered_index = 0;

    for (UINT64 i = 0; i < dxgi_message_count; ++i)
    {
        SIZE_T n = 0;
        dxgi_info_queue->GetMessageW(DXGI_DEBUG_ALL, i, nullptr, &n);

        DXGI_INFO_QUEUE_MESSAGE *pm = static_cast<DXGI_INFO_QUEUE_MESSAGE *>(malloc(n));

        dxgi_info_queue->GetMessageW(DXGI_DEBUG_ALL, i, pm, &n);

        char msg[sizeof(message)] = {};
        int len = sprintf_s(msg, "%s:%d: %s (Error #%d %d %d)", file_name, line_number, pm->pDescription, pm->ID,
                            pm->Category, pm->Severity);

        for (size_t i = 0; i < encountered_index; ++i)
        {
            if (pm->ID == encountered_ids[i])
            {
                free(pm);
                continue;
            }
        }

        encountered_ids[encountered_index++] = pm->ID;

        if (!write_message(msg, len, j, !i))
        {
            return;
        }

        free(pm);
    }

    if (d3d11_info_queue)
    {
        for (UINT64 i = 0; i < d3d11_message_count; i++)
        {
            SIZE_T n = 0;
            d3d11_info_queue->GetMessageW(i, nullptr, &n);

            D3D11_MESSAGE *pm = static_cast<D3D11_MESSAGE *>(malloc(n));

            d3d11_info_queue->GetMessageW(i, pm, &n);

            char new_line[2] = {};
            if (!i && j)
            {
                new_line[0] = '\n';
            }

            char msg[512] = {};
            int len = sprintf_s(msg, "%s%s:%d: %s (Error #%d %d %d)", new_line, file_name, line_number,
                                pm->pDescription, pm->ID, pm->Category, pm->Severity);

            for (size_t i = 0; i < encountered_index; ++i)
            {
                if (pm->ID == encountered_ids[i])
                {
                    free(pm);
                    continue;
                }
            }

            encountered_ids[encountered_index++] = pm->ID;

            if (!write_message(msg, len, j, !i))
            {
                return;
            }

            free(pm);
        }
    }
}

GfxError::~GfxError()
{
}

const char *GfxError::title() const
{
    return "Gfx Error";
}

const char *GfxError::what() const
{
    return message;
}

// GfxError static
//
void GfxError::init_dxgi_debug()
{
    HMODULE lib = LoadLibraryA("DXGIDebug.dll");

    auto get_debug_interface =
        reinterpret_cast<decltype(DXGIGetDebugInterface) *>(GetProcAddress(lib, "DXGIGetDebugInterface"));

    HANDLE_WIN_ERR(get_debug_interface(__uuidof(IDXGIDebug), reinterpret_cast<void **>(&dxgi_debug)));

    HANDLE_WIN_ERR(get_debug_interface(__uuidof(IDXGIInfoQueue), reinterpret_cast<void **>(&dxgi_info_queue)));
}

void GfxError::init_d3d11_debug(ID3D11Device *device)
{
    HANDLE_WIN_ERR(device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void **>(&d3d11_debug)));

    HANDLE_WIN_ERR(device->QueryInterface(__uuidof(ID3D11InfoQueue), reinterpret_cast<void **>(&d3d11_info_queue)));
}

UINT64 GfxError::dxgi_message_count = 0;
UINT64 GfxError::d3d11_message_count = 0;

IDXGIDebug *GfxError::dxgi_debug = 0;
IDXGIInfoQueue *GfxError::dxgi_info_queue = 0;
ID3D11Debug *GfxError::d3d11_debug = 0;
ID3D11InfoQueue *GfxError::d3d11_info_queue = 0;

void GfxError::set_mark()
{
    dxgi_message_count = GfxError::dxgi_info_queue->GetNumStoredMessagesAllowedByRetrievalFilters(DXGI_DEBUG_ALL);

    if (d3d11_info_queue)
    {
        d3d11_message_count = GfxError::d3d11_info_queue->GetNumStoredMessagesAllowedByRetrievalFilter();
    }
}

bool GfxError::found_new_messages()
{
    return GfxError::dxgi_info_queue->GetNumStoredMessagesAllowedByRetrievalFilters(DXGI_DEBUG_ALL) >
               dxgi_message_count ||
           GfxError::d3d11_info_queue->GetNumStoredMessagesAllowedByRetrievalFilter() > d3d11_message_count;
}
