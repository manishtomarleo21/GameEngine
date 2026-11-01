#include "Engine/Entity/Entity.h"
#include <algorithm>

Entity::Entity(float x, float y, float w, float h)
    : m_X(x), m_Y(y), m_W(w), m_H(h){

    }

void Entity::Update(float deltaTime){
    if (m_CurrentAnim) {
        m_CurrentAnim->Update(deltaTime);
    }
    // Apply gravity
    const float GRAVITY =  980.0f;
    m_VelY += GRAVITY * m_GravityScale * deltaTime;

    //Apply Velocity
    m_X += m_VelX * deltaTime;
    m_Y += m_VelY * deltaTime;
}

void Entity::Renderer(SDL_Renderer* renderer){
    SDL_FRect dest = { m_X, m_Y, m_W, m_H };

    if(m_Texture && m_Texture->GetSDLTexture()){
        SDL_FRect src;
        const SDL_FRect* srcPtr = nullptr;
        if (m_CurrentAnim) {
            const AnimationFrame* frame = m_CurrentAnim->GetCurrentFrame();
            if (frame) {
                src = { frame->u, frame->v, frame->w, frame->h };
                srcPtr = &src;
            }
        }
        SDL_FlipMode flip = m_FlipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderTextureRotated(renderer, m_Texture->GetSDLTexture(), srcPtr, &dest, 0.0, nullptr, flip);
        return;
    }
    else{
        SDL_SetRenderDrawColor(renderer, m_ColorR, m_ColorG, m_ColorB, 255);
        if (m_Shape == ShapeType::Circle) {
            // Simple filled circle approximation
            float radius = m_W * 0.5f;
            float centerX = m_X + radius;
            float centerY = m_Y + radius;

            for (float y = -radius; y <= radius; y += 1.0f) {
                for (float x = -radius; x <= radius; x += 1.0f) {
                    if (x*x + y*y <= radius*radius) {
                        SDL_RenderPoint(renderer, centerX + x, centerY + y);
                    }
                }
            }
        } 
        else {
            // Rectangle (default)
            SDL_RenderFillRect(renderer, &dest);
        }
    }
}

void Entity::SetPosition(float x, float y){
    m_X = x;
    m_Y = y;
}

void Entity::SetSize(float w, float h) {
    m_W = w;
    m_H = h;
}

void Entity::SetColor(Uint8 r, Uint8 g, Uint8 b) {
    m_ColorR = r;
    m_ColorG = g;
    m_ColorB = b;
}

void Entity::SetVelocity(float vx, float vy) {
    m_VelX = vx;
    m_VelY = vy;
}

void Entity::SetTexture(std::shared_ptr<Texture> texture) {
    m_Texture = texture;
    if (m_Texture) {
        // Automatically match entity size to the texture
        // m_W = m_Texture->GetWidth();
        // m_H = m_Texture->GetHeight();
    }
}

void Entity::AddVelocity(float vx, float vy) {
    m_VelX += vx;
    m_VelY += vy;
}

void Entity::SetGravityScale(float scale) {
    m_GravityScale = scale;
}

void Entity::SetBounciness(float value) {
    m_Bounciness = value;
}

void Entity::SetFriction(float friction) {
    m_Friction = friction;
}

void Entity::SetShape(ShapeType shape) {
    m_Shape = shape;
}

void Entity::AddAnimation(const std::string& name, Animation anim) {
    m_Animations[name] = std::move(anim);
}

void Entity::PlayAnimation(const std::string& name) {
    auto it = m_Animations.find(name);
    if (it == m_Animations.end()) return;

    m_CurrentAnim = &it->second;
    m_CurrentAnim->Reset();

    // Use animation texture
    if (m_CurrentAnim->GetTexture()) {
        m_Texture = m_CurrentAnim->GetTexture();
    }
}

void Entity::StopAnimation() {
    m_CurrentAnim = nullptr;
}

void Entity::PauseAnimation(bool pause) {
    if (m_CurrentAnim) {
        m_CurrentAnim->SetPaused(pause);
        if (pause) {
            m_CurrentAnim->Reset();          // go to first frame
            m_CurrentAnim->SetPaused(true);  // keep paused after reset
        }
    }
}