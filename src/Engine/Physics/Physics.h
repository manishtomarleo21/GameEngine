#pragma once

#include "Engine/Entity/Entity.h"
#include <vector>
#include <memory>

class Physics {
    public:
        static void ApplyFloorCollision(std::vector<std::unique_ptr<Entity>>& entities, float floorY);
};