#pragma once

#include "Engine/Entity/Entity.h"

class Player : public Entity {
    public:
        Player(float x, float y, float w, float h);
        void Update(float deltaTime) override;

    private:
        float m_speed = 400.0f;

};