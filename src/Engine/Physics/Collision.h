#pragma once

#include "Engine/Entity/Entity.h"
#include <vector>
#include <memory>

class Collision {
    public:
        static bool Check(const Entity& a, const Entity& b);
        static void Resolve(Entity& a, Entity& b);
        static void ProcessPlayerCollisions(Entity& a, const std::vector<std::unique_ptr<Entity>>& entities);
};