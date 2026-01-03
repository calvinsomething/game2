#pragma once

#include <chrono>

#include <windows.h>
// have be imported after windows.h
#include <hidusage.h>
#include <winDNS.h>

class Input
{
  public:
    struct KeyboardState
    {
        bool keys_down[256] = {};
    };

    struct MouseState
    {
        struct Button
        {
            friend class Input;

            bool is_down, was_clicked;

          private:
            std::chrono::time_point<std::chrono::steady_clock> click_deadline;
        };

        struct
        {
            int x, y;
        } movement;

        int scroll;

        Button left_button, right_button;

        float sensitivity;
    };

    void register_devices();

    KeyboardState get_keyboard_state();

    void handle_input();

    MouseState get_mouse_state();

    float mouse_sensitivity = 0.08f;

  private:
    void handle_keyboard_input(bool key_is_down, USHORT scan_code);

    void reset_mouse_state();
    void handle_mouse_input(RAWINPUT *raw);

    void process_data(RAWINPUT *raw);

    KeyboardState keyboard_state = {};
    MouseState mouse_state = {};
};
