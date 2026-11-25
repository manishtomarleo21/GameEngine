#pragma once

#include "Engine/Entity/Entity.h"
#include "Engine/Core/MovementMode.h"

class Player : public Entity {
    public:
        Player(float x, float y, float w, float h);
        using Entity::Update;
        void Update(float deltaTime, MovementMode mode);
        static void UpdateControlledEntity(Entity& entity, float deltaTime, MovementMode mode);

    private:
        float m_speed = 400.0f;

};