#pragma once

#include "../input/Input.h"
#include "Camera.h"
#include "Character.h"

class Controller
{
  public:
    Controller()
    {
    }

    void poll_input(const Input &input);

    Character::Controls get_character_controls();

    Camera::Controls get_camera_controls();

    bool face_away_from_camera();

  private:
    Character::Controls character_controls = {};
    Camera::Controls camera_controls = {};
    bool _face_away_from_camera = 0;

    void handle_movement_and_rotation(Input::KeyboardState &keyboard, Input::MouseState &mouse);

    void handle_actions(Input::KeyboardState &keyboard);

    void handle_dragging(Input::MouseState &mouse);
};
