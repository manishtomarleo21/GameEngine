#include "Engine/Physics/Physics.h"

void Physics::ApplyFloorCollision(std::vector<std::unique_ptr<Entity>>& entities, float floorY){
    for (auto& entity : entities) {
        if (!entity) continue;
        if (entity->GetGravityScale() <= 0.0f) continue;

        float bottom = entity->GetY() + entity->GetH();
        if(bottom > floorY){
            entity->SetPosition(entity->GetX(), floorY - entity->GetH());

            //Bounce
            float velY = entity->GetVelY();
            float newVelY = -velY * entity->GetBounciness();

            // stop tiny bouncing
            if (std::abs(newVelY) < 40.0f) {
                newVelY = 0.0f;
            }
            entity->SetVelocity(entity->GetVelX(), newVelY);
        }

    }
}