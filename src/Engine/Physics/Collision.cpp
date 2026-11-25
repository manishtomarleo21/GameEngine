#include "Engine/Physics/Collision.h"
#include <algorithm>

bool Collision::Check(const Entity& a, const Entity& b) {
    return (a.GetX() < b.GetX() + b.GetW() &&
            a.GetX() + a.GetW() > b.GetX() &&
            a.GetY() < b.GetY() + b.GetH() &&
            a.GetY() + a.GetH() > b.GetY());
}

void Collision::Resolve(Entity& a, Entity& b) {
    float overlapX = std::min(a.GetX() + a.GetW(), b.GetX() + b.GetW()) - std::max(a.GetX(), b.GetX());
    float overlapY = std::min(a.GetY() + a.GetH(), b.GetY() + b.GetH()) - std::max(a.GetY(), b.GetY());

    if (overlapX < overlapY) {
        // Resolve on X axis
        float centerA = a.GetX() + a.GetW() * 0.5f;
        float centerB = b.GetX() + b.GetW() * 0.5f;
        if (centerA < centerB) {
            a.SetPosition(a.GetX() - overlapX, a.GetY());
        } else {
            a.SetPosition(a.GetX() + overlapX, a.GetY());
        }
        a.SetVelocity(0.0f, a.GetVelY());
    } else {
        // Resolve on Y axis
        float centerA = a.GetY() + a.GetH() * 0.5f;
        float centerB = b.GetY() + b.GetH() * 0.5f;
        if (centerA < centerB) {
            a.SetPosition(a.GetX(), a.GetY() - overlapY);
        } else {
            a.SetPosition(a.GetX(), a.GetY() + overlapY);
        }
        a.SetVelocity(a.GetVelX(), 0.0f);
    }
}

void Collision::ProcessPlayerCollisions(Entity& player, 
                                        const std::vector<std::unique_ptr<Entity>>& entities) {
    for (const auto& entity : entities) {
        Entity* other = entity.get();
        if (!other || other == &player) continue;
        if (Check(player, *other)) {
            Resolve(player, *other);
        }
    }
}