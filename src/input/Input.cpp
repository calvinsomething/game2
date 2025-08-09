#include "Input.h"

#include <utility>

#include "../Error.h"

void Input::register_devices()
{
    RAWINPUTDEVICE devices[] = {{HID_USAGE_PAGE_GENERIC, HID_USAGE_GENERIC_MOUSE, 0, 0},
                                {HID_USAGE_PAGE_GENERIC, HID_USAGE_GENERIC_KEYBOARD, 0, 0}};

    if (!RegisterRawInputDevices(devices, ARRAYSIZE(devices), sizeof(devices[0])))
    {
        throw WindowsError(__FILE__, __LINE__);
    }
}

void Input::handle_input()
{
    UINT buffer_size = 0;

    UINT result = GetRawInputBuffer(nullptr, &buffer_size, sizeof(RAWINPUTHEADER));

    if (result == static_cast<UINT>(-1))
    {
        throw WindowsError(__FILE__, __LINE__);
    }

    buffer_size *= 8;

    RAWINPUT *buffer = static_cast<RAWINPUT *>(alloca(buffer_size));

    UINT size = buffer_size;
    while ((result = GetRawInputBuffer(buffer, &size, sizeof(RAWINPUTHEADER))) > 0)
    {
        if (result == static_cast<UINT>(-1))
        {
            throw WindowsError(__FILE__, __LINE__);
        }

        RAWINPUT *raw = buffer;
        for (size_t i = 0; i < result; ++i)
        {
            process_data(raw);

            raw = NEXTRAWINPUTBLOCK(raw);
        }
    }
}

void Input::set_control_handler(std::function<void(unsigned char)> &&handler)
{
    control_handler = std::move(handler);
}

void Input::handle_keyboard_input(bool key_is_down, USHORT v_key)
{
    control_handler(v_key);
}

void Input::process_data(RAWINPUT *raw)
{
    switch (raw->header.dwType)
    {
    case RIM_TYPEMOUSE:
        break;
    case RIM_TYPEKEYBOARD:
        if (raw->data.keyboard.VKey)
        {
            handle_keyboard_input(!(raw->data.keyboard.Flags & RI_KEY_BREAK),
                                  static_cast<unsigned char>(raw->data.keyboard.VKey));
            break;
        }
    }
}
