#include "Input.h"

#include "../Error.h"

constexpr std::chrono::milliseconds MAX_CLICK_DURATION(400);

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
    keyboard_state = {};
    reset_mouse_state();

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

Input::MouseState Input::get_mouse_state()
{
    return mouse_state;
}

Input::KeyboardState Input::get_keyboard_state()
{
    return keyboard_state;
}

void Input::handle_keyboard_input(bool key_is_down, USHORT v_key)
{
    keyboard_state.keys_down[v_key] = key_is_down;
}

void Input::reset_mouse_state()
{
    mouse_state = MouseState{{}, 0, mouse_state.left_button, mouse_state.right_button, mouse_sensitivity};
    mouse_state.left_button.was_clicked = 0;
    mouse_state.right_button.was_clicked = 0;
}

void Input::handle_mouse_button(bool is_down, bool was_released, Input::MouseState::ButtonState &button_state)
{
    if (is_down)
    {
        if (!mouse_state.left_button.is_down)
        {
            button_state.is_down = true;
            button_state.click_deadline = std::chrono::steady_clock::now() + MAX_CLICK_DURATION;
        }
    }
    else if (was_released)
    {
        button_state.is_down = false;
        if (std::chrono::steady_clock::now() < button_state.click_deadline)
        {
            button_state.was_clicked = true;
        }
    }
}

void Input::handle_mouse_input(RAWINPUT *raw)
{
    RAWMOUSE &mouse = raw->data.mouse;

    if (!(mouse.usFlags & MOUSE_MOVE_ABSOLUTE))
    {
        mouse_state.movement = {raw->data.mouse.lLastX, raw->data.mouse.lLastY};
    }

    handle_mouse_button(raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN,
                        raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP, mouse_state.left_button);
    handle_mouse_button(raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN,
                        raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP, mouse_state.right_button);

    if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
    {
        mouse_state.scroll = *reinterpret_cast<short *>(&raw->data.mouse.usButtonData);
    }
}

void Input::process_data(RAWINPUT *raw)
{
    switch (raw->header.dwType)
    {
    case RIM_TYPEMOUSE:
        handle_mouse_input(raw);
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
