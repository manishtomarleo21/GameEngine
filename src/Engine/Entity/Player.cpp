#include "Engine/Entity/Player.h"
#include "Engine/Input/Input.h"
#include <cmath>

Player::Player(float x, float y, float w, float h)
    :Entity(x,y,w,h){
        SetColor(220, 50, 50);
    }

void Player::Update(float deltaTime, MovementMode mode)
{
    UpdateControlledEntity(*this, deltaTime, mode);
}

void Player::UpdateControlledEntity(Entity& entity, float deltaTime, MovementMode mode)
{
    const float speed = 220.0f;

    float vx = 0.0f;
    float vy = 0.0f;

    if (mode == MovementMode::Free)
    {
        // FREE MOVEMENT: Top-down / flying movement

        if (Input::IsKeyPressed(SDL_SCANCODE_A) ||
            Input::IsKeyPressed(SDL_SCANCODE_LEFT))
            vx -= 1.0f;

        if (Input::IsKeyPressed(SDL_SCANCODE_D) ||
            Input::IsKeyPressed(SDL_SCANCODE_RIGHT))
            vx += 1.0f;

        if (Input::IsKeyPressed(SDL_SCANCODE_W) ||
            Input::IsKeyPressed(SDL_SCANCODE_UP))
            vy -= 1.0f;

        if (Input::IsKeyPressed(SDL_SCANCODE_S) ||
            Input::IsKeyPressed(SDL_SCANCODE_DOWN))
            vy += 1.0f;

        // Normalize diagonal movement
        float length = std::sqrt(vx * vx + vy * vy);

        if (length > 1.0f)
        {
            vx /= length;
            vy /= length;
        }

        entity.SetVelocity(vx * speed, vy * speed);

        entity.SetGravityScale(0.0f);
    }
    else if (mode == MovementMode::Platformer)
    {
        // PLATFORMER: Horizontal movement + jumping

        if (Input::IsKeyPressed(SDL_SCANCODE_A) ||
            Input::IsKeyPressed(SDL_SCANCODE_LEFT))
            vx -= speed;

        if (Input::IsKeyPressed(SDL_SCANCODE_D) ||
            Input::IsKeyPressed(SDL_SCANCODE_RIGHT))
            vx += speed;

        // Preserve vertical velocity from gravity/jumping
        vy = entity.GetVelY();

        if (entity.GetVelY() == 0.0f &&
            (Input::IsKeyPressed(SDL_SCANCODE_SPACE) ||
             Input::IsKeyPressed(SDL_SCANCODE_W) ||
             Input::IsKeyPressed(SDL_SCANCODE_UP)))
        {
            vy = -420.0f;
        }

        entity.SetVelocity(vx, vy);

        // Gravity remains enabled in platformer mode.
    }

    // Update horizontal sprite direction
    if (vx < 0.0f)
        entity.SetFlipHorizontal(true);

    if (vx > 0.0f)
        entity.SetFlipHorizontal(false);

    // Animation handling
    const bool moving = (vx != 0.0f || vy != 0.0f);

    if (moving)
    {
        if (!entity.HasAnimation())
            entity.PlayAnimation("run");

        entity.PauseAnimation(false);
    }
    else
    {
        entity.PauseAnimation(true);
    }

    // Run base Entity update
    entity.Entity::Update(deltaTime);
}


