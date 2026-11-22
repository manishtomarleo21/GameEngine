#include "Engine/Entity/Player.h"
#include "Engine/Input/Input.h"
#include <cmath>

Player::Player(float x, float y, float w, float h)
    :Entity(x,y,w,h){
        SetColor(220, 50, 50);
    }

void Player::Update(float deltaTime)
{
    UpdateControlledEntity(*this, deltaTime);
}

void Player::UpdateControlledEntity(Entity& entity, float deltaTime)
{
    float speed = 200.0f;
    float vx = 0.0f;
    float vy = 0.0f;

    if (Input::IsKeyPressed(SDL_SCANCODE_LEFT)  || Input::IsKeyPressed(SDL_SCANCODE_A)) vx -= 1.0f;
    if (Input::IsKeyPressed(SDL_SCANCODE_RIGHT) || Input::IsKeyPressed(SDL_SCANCODE_D)) vx += 1.0f;
    if (Input::IsKeyPressed(SDL_SCANCODE_UP)    || Input::IsKeyPressed(SDL_SCANCODE_W)) vy -= 1.0f;
    if (Input::IsKeyPressed(SDL_SCANCODE_DOWN)  || Input::IsKeyPressed(SDL_SCANCODE_S)) vy += 1.0f;

    const float directionLength = std::sqrt(vx * vx + vy * vy);
    if (directionLength > 1.0f) {
        vx /= directionLength;
        vy /= directionLength;
    }

    entity.SetVelocity(vx * speed, vy * speed);
    if (vx < 0.0f) entity.SetFlipHorizontal(true);
    if (vx > 0.0f) entity.SetFlipHorizontal(false);

    const bool moving = (vx != 0.0f || vy != 0.0f);

    if (moving) {
        if (!entity.HasAnimation()) {
            entity.PlayAnimation("run");
        }
        entity.PauseAnimation(false);
    } else {
        if (!entity.HasAnimation()) {
            entity.PlayAnimation("run");
        }
        entity.PauseAnimation(true);
    }

    entity.Entity::Update(deltaTime);
}

