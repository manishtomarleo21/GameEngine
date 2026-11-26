#include "Engine/Physics/Collision.h"
#include <algorithm>

bool Collision::Check(const Entity& a, const Entity& b) {
    float ax, ay, aw, ah;
    float bx, by, bw, bh;
    a.GetColliderRect(ax, ay, aw, ah);
    b.GetColliderRect(bx, by, bw, bh);

    return ax < bx + bw &&
           ax + aw > bx &&
           ay < by + bh &&
           ay + ah > by;
}

void Collision::Resolve(Entity& a, Entity& b) {
    float ax, ay, aw, ah;
    float bx, by, bw, bh;
    a.GetColliderRect(ax, ay, aw, ah);
    b.GetColliderRect(bx, by, bw, bh);

    float overlapX = std::min(ax + aw, bx + bw) - std::max(ax, bx);
    float overlapY = std::min(ay + ah, by + bh) - std::max(ay, by);

    if (overlapX <= 0.0f || overlapY <= 0.0f) return;

    if (overlapX < overlapY) {
        // Resolve on X axis
        float centerA = ax + aw * 0.5f;
        float centerB = bx + bw * 0.5f;

        float newColX = ax;
        if (centerA < centerB) newColX = ax - overlapX;
        else                   newColX = ax + overlapX;

        // convert collider x back to entity x
        float offsetX = ax - a.GetX(); // same as col offset if custom
        a.SetPosition(newColX - offsetX, a.GetY());
        a.SetVelocity(0.0f, a.GetVelY());
    } else {
        // Resolve on Y axis
        float centerA = ay + ah * 0.5f;
        float centerB = by + bh * 0.5f;

        float newColY = ay;
        if (centerA < centerB) newColY = ay - overlapY; // a above b → land on top
        else                   newColY = ay + overlapY;

        float offsetY = ay - a.GetY();
        a.SetPosition(a.GetX(), newColY - offsetY);
        a.SetVelocity(a.GetVelX(), 0.0f);

        // mark grounded when landing on top
        // if (centerA < centerB) {
        //     a.SetGrounded(true); // if you have this
        // }
    }
}

void Collision::ProcessPlayerCollisions(Entity& player, 
                                        const std::vector<std::unique_ptr<Entity>>& entities) {
    for (const auto& entity : entities) {
        Entity* other = entity.get();
        if (!other || other == &player || !other->IsSolid()) continue;
        if (Check(player, *other)) {
            Resolve(player, *other);
        }
    }
}