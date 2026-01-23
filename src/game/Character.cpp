#include "Character.h"

constexpr float SIN_PI_DIV_8 = 0.3826834323650897717284599840304;
constexpr float SIN_PI_DIV_4 = 0.70710678118654752440084436210485;

constexpr float COS_PI_DIV_8 = 0.92387953251128675612818318939679;

const DirectX::XMVECTOR Character::DirectionPad::rotations[3][3] = {
    {
        {0.0f, COS_PI_DIV_8, 0.0f, SIN_PI_DIV_8},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, -COS_PI_DIV_8, 0.0f, SIN_PI_DIV_8},
    },
    {
        {0.0f, -SIN_PI_DIV_4, 0.0f, SIN_PI_DIV_4},
        {},
        {0.0f, SIN_PI_DIV_4, 0.0f, SIN_PI_DIV_4},
    },
    {
        {0.0f, -SIN_PI_DIV_8, 0.0f, COS_PI_DIV_8},
        DirectX::XMQuaternionIdentity(),
        {0.0f, SIN_PI_DIV_8, 0.0f, COS_PI_DIV_8},
    },
};

void Character::update()
{
    model.update();
}

void Character::update(const Controls &controls)
{
    if (controls.rotation)
    {
        direction_pad.set_twelve_oclock(DirectX::XMVector3Transform(
            direction_pad.twelve_oclock, DirectX::XMMatrixRotationQuaternion(DirectX::XMVector4Normalize(
                                             DirectX::XMVECTOR{0.0f, 0.03f * controls.rotation, 0.0f, 1.0f}))));
    }

    DirectX::XMVECTOR position_delta = {};

    if (controls.movement.y || controls.movement.x)
    {
        DirectX::XMVECTOR direction = direction_pad.get_direction(DirectionPad::CENTER + controls.movement.y,
                                                                  DirectionPad::CENTER + controls.movement.x);
        position_delta = DirectX::XMVectorScale(direction, Global::clock.speed_to_distance(movement_speed_mph));

        if (Character::state != State::running)
        {
            Character::state = State::running;
            model.loop_animation("Run");
        }
    }
    else if (controls.action == Controls::Action::action_1)
    {
        Character::state = State::attacking;
        model.start_animation("Punch");
    }
    else
    {
        switch (Character::state)
        {
        case State::attacking:
            if (model.is_animating())
            {
                break;
            }
        case State::running:
            Character::state = State::at_rest;
            model.loop_animation("Idle");
        default:
            break;
        }
    }

    DirectX::XMFLOAT3 f3;
    DirectX::XMStoreFloat3(&f3, position_delta);

    set_position(position + f3);

    model.set_orientation(get_orientation());

    Character::update();
}

DirectX::XMVECTOR Character::get_orientation()
{
    DirectX::XMFLOAT3 f3;
    DirectX::XMStoreFloat3(&f3, direction_pad.twelve_oclock);

    float radian = DirectX::XMScalarASin(f3.x);
    if (f3.z < 0)
    {
        radian = DirectX::XM_PI - radian;
    }

    return DirectX::XMQuaternionRotationNormal(DirectX::XMVECTOR{0.0f, 1.0f}, radian);
}

DirectX::XMFLOAT3 Character::get_position()
{
    return position;
}

void Character::set_position(DirectX::XMFLOAT3 position)
{
    Character::position = position;
    model.set_position(position);
}

void Character::set_twelve_oclock(DirectX::XMVECTOR direction)
{
    direction_pad.set_twelve_oclock(direction);
}

DirectX::XMVECTOR Character::DirectionPad::get_direction(uint8_t y, uint8_t x)
{
    if (!directions[y][x].has_value())
    {
        directions[y][x].emplace(
            DirectX::XMVector3Transform(twelve_oclock, DirectX::XMMatrixRotationQuaternion(rotations[y][x])));
    }

    return directions[y][x].value();
}
