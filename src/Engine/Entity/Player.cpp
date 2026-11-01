#include "Engine/Entity/Player.h"
#include "Engine/Input/Input.h"

Player::Player(float x, float y, float w, float h)
    :Entity(x,y,w,h){
        SetColor(220, 50, 50);
    }

void Player::Update(float deltaTime)
{
    float speed = 200.0f;
    float vx = 0.0f;
    float vy = 0.0f;

    if (Input::IsKeyPressed(SDL_SCANCODE_LEFT)  || Input::IsKeyPressed(SDL_SCANCODE_A)) vx -= 1.0f;
    if (Input::IsKeyPressed(SDL_SCANCODE_RIGHT) || Input::IsKeyPressed(SDL_SCANCODE_D)) vx += 1.0f;
    if (Input::IsKeyPressed(SDL_SCANCODE_UP)    || Input::IsKeyPressed(SDL_SCANCODE_W)) vy -= 1.0f;
    if (Input::IsKeyPressed(SDL_SCANCODE_DOWN)  || Input::IsKeyPressed(SDL_SCANCODE_S)) vy += 1.0f;

    SetVelocity(vx, vy);
    if (vx < 0.0f) SetFlipHorizontal(true);
    if (vx > 0.0f) SetFlipHorizontal(false);

   // Play run only when moving
   bool moving = (vx != 0.0f || vy != 0.0f);

    if (moving) {
        PlayAnimation("run");          // ensure run is active
        PauseAnimation(false);         // unpause
    } else {
        // stay on run animation, but freeze first frame
        if (!HasAnimation()) {
            PlayAnimation("run");
        }
        PauseAnimation(true);
    }

    Entity::Update(deltaTime);
}

