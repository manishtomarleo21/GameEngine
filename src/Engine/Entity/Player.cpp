#include "Engine/Entity/Player.h"
#include "Engine/Input/Input.h"

Player::Player(float x, float y, float w, float h)
    :Entity(x,y,w,h){
        SetColor(220, 50, 50);
    }

void Player::Update(float deltaTime)
{
    float moveX = 0.0f;
    float moveY = 0.0f;

    if (Input::IsKeyPressed(SDL_SCANCODE_LEFT)  || Input::IsKeyPressed(SDL_SCANCODE_A)) moveX -= 1.0f;
    if (Input::IsKeyPressed(SDL_SCANCODE_RIGHT) || Input::IsKeyPressed(SDL_SCANCODE_D)) moveX += 1.0f;
    if (Input::IsKeyPressed(SDL_SCANCODE_UP)    || Input::IsKeyPressed(SDL_SCANCODE_W)) moveY -= 1.0f;
    if (Input::IsKeyPressed(SDL_SCANCODE_DOWN)  || Input::IsKeyPressed(SDL_SCANCODE_S)) moveY += 1.0f;

    if (moveX != 0.0f && moveY != 0.0f) {
        moveX *= 0.7071f;
        moveY *= 0.7071f;
    }

    SetVelocity(moveX * m_speed, moveY * m_speed);

    Entity::Update(deltaTime);
}

