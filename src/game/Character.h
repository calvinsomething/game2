#pragma once

#include "../Global.h"
#include "../models/Model.h"

constexpr float SIN_1_4_PI = 0.7071067811865475f;

class Character
{
  public:
    struct Controls
    {
        enum class Action
        {
            none,
            action_1,
            action_2,
            action_3,
            action_4,
            action_5,
        };

        DirectX::XMFLOAT2 movement = {};

        float rotation = 0;

        Action action = Action::none;
    };

    Character(Model<Vertex> &model) : model(model)
    {
    }

    void update();
    void update(const Controls &controls);

    DirectX::XMFLOAT3 get_position();

    void set_position(DirectX::XMFLOAT3 position);
    void set_twelve_oclock(DirectX::XMVECTOR direction);

    // TODO move to protected and move model update logic to Character
    Model<Vertex> &model;

  protected:
    DirectX::XMFLOAT3 position;

    DirectX::XMVECTOR twelve_oclock = Global::north;
    DirectX::XMVECTOR three_oclock = DirectX::XMVector3Transform(
        twelve_oclock, DirectX::XMMatrixRotationQuaternion(DirectX::XMVECTOR{0.0f, SIN_1_4_PI, 0.0f, SIN_1_4_PI}));
    bool three_oclock_synced = true;

    float movement_speed_mph = 30.0f;

    DirectX::XMVECTOR get_three_oclock();
    DirectX::XMVECTOR get_orientation();
};
