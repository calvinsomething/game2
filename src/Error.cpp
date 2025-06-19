#include "Error.h"

#include <stdio.h>

#include <cstring>

WindowsError::WindowsError(const char *file_name, int line_number)
{
    DWORD error_code = GetLastError();

    LPWSTR buffer = 0;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error_code, 0,
                   reinterpret_cast<LPWSTR>(&buffer), 0, nullptr);

    DWORD n = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error_code, 0, buffer,
                             static_cast<DWORD>(LocalSize(buffer)), nullptr);

    wcstombs_s(nullptr, message, buffer, n);

    LocalFree(buffer);

    char runtime_info[512];
    int i = sprintf_s(runtime_info, "%s:%d: ", file_name, line_number);

    std::memmove(message + i, message, n);

    std::memcpy(message, runtime_info, i);
}

const char *WindowsError::title() const
{
    return "Windows Error";
}

WindowsError::~WindowsError()
{
}

const char *WindowsError::what() const
{
    return message;
}
