#pragma once

#include <SDL3/SDL.h>
#include "Engine/Renderer/Texture.h"
#include <memory>

class Entity {
    public:
        Entity(float x, float y, float w, float h);
        virtual ~Entity() = default;

        virtual void Update(float deltaTime);
        virtual void Renderer(SDL_Renderer* renderer);

        float GetX() const { return m_X; }
        float GetY() const { return m_Y; }
        float GetW() const { return m_W; }
        float GetH() const { return m_H; }

        void SetPosition(float x, float y);
        void SetSize(float w, float h);
        void SetColor(Uint8 r, Uint8 g, Uint8 b);
        void SetVelocity(float vx, float vy);

        void SetTexture(std::shared_ptr<Texture> texture);

        void AddVelocity(float vx, float vy);
        void SetGravityScale(float scale);     // 1.0 = normal gravity, 0.0 = no gravity
        void SetBounciness(float value);        // 0.0 = no bounce, 1.0 = perfect bounce
        
        float GetVelX() const { return m_VelX; }
        float GetVelY() const { return m_VelY; }
        float GetBounciness() const { return m_Bounciness; }
        float GetGravityScale() const { return m_GravityScale; } 

        void SetFriction(float friction);          // 0.0 = no friction, 1.0 = full stop
        float GetFriction() const { return m_Friction; }

    private:
        float m_X, m_Y;
        float m_W, m_H;
        float m_VelX = 0.0f;
        float m_VelY = 0.0f;
        float m_GravityScale = 0.0f;   // 0 by default (so existing entities don't fall)
        float m_Bounciness = 0.5f;  
        float m_Friction = 0.0f;

        Uint8 m_ColorR = 255;
        Uint8 m_ColorG = 255;
        Uint8 m_ColorB = 255;

        std::shared_ptr<Texture> m_Texture;
};