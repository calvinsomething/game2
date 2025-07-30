#pragma once

#include <functional>
#include <windows.h>
// have be imported after windows.h
#include <hidusage.h>
#include <winDNS.h>

class Input
{
  public:
    void register_devices();

    void set_control_handler(std::function<void(unsigned char)> &&handler);

    void handle_input();

  private:
    void handle_keyboard_input(bool key_is_down, USHORT scan_code);

    void process_data(RAWINPUT *raw);

    std::function<void(unsigned char)> control_handler;
};
