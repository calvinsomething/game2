#pragma once

#include "../Global.h"
#include "../models/Model.h"
#include <optional>

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
    enum class State
    {
        at_rest,
        running,
        attacking,
    };

    class DirectionPad
    {
      public:
        static constexpr uint8_t BACKWARD = 0, LEFT = 0, CENTER = 1, FORWARD = 2, RIGHT = 2;

        DirectX::XMVECTOR get_direction(uint8_t y, uint8_t x);

        DirectX::XMVECTOR twelve_oclock = Global::north;

        void set_twelve_oclock(DirectX::XMVECTOR twelve_oclock)
        {
            DirectionPad::twelve_oclock = twelve_oclock;
            directions[0][0] = {};
            directions[0][1] = {};
            directions[0][2] = {};
            directions[1][0] = {};
            directions[1][1] = {};
            directions[1][2] = {};
            directions[2][0] = {};
            directions[2][1] = {twelve_oclock};
            directions[2][2] = {};
        }

      private:
        static const DirectX::XMVECTOR rotations[3][3];
        std::optional<DirectX::XMVECTOR> directions[3][3] = {{}, {Global::north}};
    };

    DirectX::XMFLOAT3 position;

    DirectionPad direction_pad = {};

    float movement_speed_mph = 30.0f;

    State state = State::at_rest;

    DirectX::XMVECTOR get_orientation();
};
