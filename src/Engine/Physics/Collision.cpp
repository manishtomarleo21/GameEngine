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
        if (a.GetX() < b.GetX()) {
            a.SetPosition(a.GetX() - overlapX, a.GetY());
        } else {
            a.SetPosition(a.GetX() + overlapX, a.GetY());
        }
    } else {
        // Resolve on Y axis
        if (a.GetY() < b.GetY()) {
            a.SetPosition(a.GetX(), a.GetY() - overlapY);
        } else {
            a.SetPosition(a.GetX(), a.GetY() + overlapY);
        }
    }
}

void Collision::ProcessPlayerCollisions(Entity& player, 
                                        const std::vector<std::unique_ptr<Entity>>& entities) {
    for (size_t i = 1; i < entities.size(); ++i) {   // skip index 0 (the player itself)
        Entity* other = entities[i].get();
        if (Check(player, *other)) {
            Resolve(player, *other);
        }
    }
}