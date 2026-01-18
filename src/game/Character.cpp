#include "Character.h"

void Character::update()
{
    model.update();
}

void Character::update(const Controls &controls)
{
    float travel = Global::clock.speed_to_distance(movement_speed_mph);

    if (controls.rotation)
    {
        twelve_oclock = DirectX::XMVector3Transform(
            twelve_oclock, DirectX::XMMatrixRotationQuaternion(DirectX::XMVector4Normalize(
                               DirectX::XMVECTOR{0.0f, 0.1f * controls.rotation, 0.0f, 1.0f})));
    }

    DirectX::XMVECTOR position_delta = DirectX::XMVectorScale(twelve_oclock, controls.movement.y * travel);

    if (controls.movement.x)
    {
        position_delta = DirectX::XMVectorAdd(position_delta,
                                              DirectX::XMVectorScale(get_three_oclock(), controls.movement.x * travel));
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
    DirectX::XMStoreFloat3(&f3, twelve_oclock);

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
    twelve_oclock = direction;
    three_oclock_synced = false;
}

DirectX::XMVECTOR Character::get_three_oclock()
{
    static DirectX::XMVECTOR vec;

    if (!three_oclock_synced)
    {
        vec = DirectX::XMVector3Transform(
            twelve_oclock, DirectX::XMMatrixRotationQuaternion(DirectX::XMVECTOR{0.0f, SIN_1_4_PI, 0.0f, SIN_1_4_PI}));
        three_oclock_synced = true;
    }

    return vec;
}
