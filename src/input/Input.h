#pragma once

#include <stdexcept>
#include <windows.h>
// have be imported after windows.h
#include <hidusage.h>
#include <winDNS.h>

#include "../Error.h"

class Input
{
  public:
    void register_devices()
    {
        RAWINPUTDEVICE devices[] = {{HID_USAGE_PAGE_GENERIC, HID_USAGE_GENERIC_MOUSE, 0, 0},
                                    {HID_USAGE_PAGE_GENERIC, HID_USAGE_GENERIC_KEYBOARD, 0, 0}};

        if (!RegisterRawInputDevices(devices, ARRAYSIZE(devices), sizeof(devices[0])))
        {
            throw WindowsError(__FILE__, __LINE__);
        }
    }

    void handle_input()
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

    void handle_keyboard_input(bool key_is_down, USHORT scan_code)
    {
        unsigned char usage = LOBYTE(scan_code);
        if (usage >= HID_USAGE_KEYBOARD_aA && usage <= HID_USAGE_KEYBOARD_zZ)
        {
            throw std::runtime_error("USAGE OK");
        }
    }

    void process_data(RAWINPUT *raw)
    {
        switch (raw->header.dwType)
        {
        case RIM_TYPEMOUSE:
            break;
        case RIM_TYPEKEYBOARD:
            if (raw->data.keyboard.MakeCode && raw->data.keyboard.MakeCode != KEYBOARD_OVERRUN_MAKE_CODE)
            {
                USHORT scan_code = MAKEWORD(raw->data.keyboard.MakeCode & 0x7f,
                                            ((raw->data.keyboard.Flags & RI_KEY_E0)
                                                 ? 0xe0
                                                 : ((raw->data.keyboard.Flags & RI_KEY_E1) ? 0xe1 : 0x00)));

                handle_keyboard_input(!(raw->data.keyboard.Flags & RI_KEY_BREAK), scan_code);
            }
        }
    }
};
