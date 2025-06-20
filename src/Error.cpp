#include "Error.h"

#include <cstring>
#include <stdio.h>

#include <d3d11.h>

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
#ifndef NDEBUG

namespace Global
{
extern ID3D11InfoQueue *info_queue;
};

GfxError::GfxError(const char *file_name, int line_number)
{
    UINT64 message_count = Global::info_queue->GetNumStoredMessages();

    size_t j = 0;
    for (UINT64 i = 0; i < message_count; i++)
    {
        SIZE_T n = 0;
        Global::info_queue->GetMessage(0, nullptr, &n);

        D3D11_MESSAGE *pm = static_cast<D3D11_MESSAGE *>(malloc(n));

        Global::info_queue->GetMessage(0, pm, &n);

        char msg[512] = {};
        int len = sprintf_s(msg, "%s:%d: %s (Error #%d)", file_name, line_number, pm->pDescription, pm->ID);

        if (i)
        {
            message[j++] = '\n';
        }

        std::memcpy(message + j, msg, len);
        j += len;

        free(pm);
    }

    Global::info_queue->ClearStoredMessages();
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
#endif
