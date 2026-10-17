#include "Engine/Physics/Physics.h"

void Physics::ApplyFloorCollision(std::vector<std::unique_ptr<Entity>>& entities, float floorY){
    for (auto& entity : entities) {
        if (!entity) continue;
        if (entity->GetGravityScale() <= 0.0f) continue;

        float bottom = entity->GetY() + entity->GetH();
        if(bottom > floorY){
            entity->SetPosition(entity->GetX(), floorY - entity->GetH());

            float velX = entity->GetVelX();
            float velY = entity->GetVelY();

            // Bounce on Y
            velY = -velY * entity->GetBounciness();

            // stop tiny bouncing
            if (std::abs(velY) < 40.0f) {
                velY = 0.0f;
            }

            // Apply friction on X when on ground
            if (velY == 0.0f) {  // only when resting on floor
                float friction = entity->GetFriction();
                velX *= (1.0f - friction);

                // Stop tiny sliding
                if (std::abs(velX) < 15.0f) {
                    velX = 0.0f;
                }
            }

            entity->SetVelocity(velX, velY);
        }

    }
}