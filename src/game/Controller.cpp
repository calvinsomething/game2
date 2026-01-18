#include "Controller.h"

void Controller::poll_input(const Input &input)
{
    character_controls = {};
    camera_controls = {};

    auto keyboard = input.get_keyboard_state();
    auto mouse = input.get_mouse_state();

    _face_away_from_camera = mouse.right_button.is_down;

    handle_movement_and_rotation(keyboard, mouse);

    handle_actions(keyboard);

    handle_dragging(mouse);
}

Character::Controls Controller::get_character_controls()
{
    return character_controls;
}

Camera::Controls Controller::get_camera_controls()
{
    return camera_controls;
}

bool Controller::face_away_from_camera()
{
    return _face_away_from_camera;
}

void Controller::handle_movement_and_rotation(Input::KeyboardState &keyboard, Input::MouseState &mouse)
{
    if (keyboard.keys_down['W'])
    {
        character_controls.movement.y += 1;
    }
    if (keyboard.keys_down['S'])
    {
        character_controls.movement.y -= 1;
    }
    if (keyboard.keys_down['D'])
    {
        if (_face_away_from_camera)
        {
            character_controls.movement.x += 1;
        }
        else
        {
            character_controls.rotation += 1;
        }
    }
    if (keyboard.keys_down['A'])
    {
        if (mouse.right_button.is_down)
        {
            character_controls.movement.x -= 1;
        }
        else
        {
            character_controls.rotation -= 1;
        }
    }
    if (keyboard.keys_down[VK_ESCAPE])
    {
        Global::running = false;
    }
}

void Controller::handle_actions(Input::KeyboardState &keyboard)
{
    if (keyboard.keys_down['1'])
    {
        character_controls.action = Character::Controls::Action::action_1;
    }
    else if (keyboard.keys_down['2'])
    {
        character_controls.action = Character::Controls::Action::action_2;
    }
    else if (keyboard.keys_down['3'])
    {
        character_controls.action = Character::Controls::Action::action_3;
    }
    else if (keyboard.keys_down['4'])
    {
        character_controls.action = Character::Controls::Action::action_4;
    }
    else if (keyboard.keys_down['5'])
    {
        character_controls.action = Character::Controls::Action::action_5;
    }
}

void Controller::handle_dragging(Input::MouseState &mouse)
{

    static struct
    {
        float x[3], y[3];
        bool active;
    } mouse_movement{};

    if (mouse.left_button.is_down || mouse.right_button.is_down)
    {
        mouse_movement.active = true;

        mouse_movement.x[0] = mouse.movement.x * mouse.sensitivity;
        mouse_movement.y[0] = mouse.movement.y * mouse.sensitivity;
    }

    if (mouse_movement.active)
    {
        camera_controls.yaw_delta =
            mouse_movement.x[0] * 0.5f + mouse_movement.x[1] * 0.33f + mouse_movement.x[2] * 0.17f;
        camera_controls.pitch_delta =
            mouse_movement.y[0] * 0.5f + mouse_movement.y[1] * 0.33f + mouse_movement.y[2] * 0.17f;

        // shift previous deltas
        mouse_movement.x[2] = mouse_movement.x[1];
        mouse_movement.x[1] = mouse_movement.x[0];
        mouse_movement.x[0] = 0.0f;

        mouse_movement.y[2] = mouse_movement.y[1];
        mouse_movement.y[1] = mouse_movement.y[0];
        mouse_movement.y[0] = 0.0f;

        mouse_movement.active =
            mouse_movement.x[1] || mouse_movement.x[2] || mouse_movement.y[1] || mouse_movement.y[2];
    }

    camera_controls.distance_delta = -mouse.scroll * 0.01f;
}
