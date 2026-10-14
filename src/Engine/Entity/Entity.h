#pragma once

#include <SDL3/SDL.h>

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

    private:
        float m_X, m_Y;
        float m_W, m_H;
        float m_VelX = 0.0f;
        float m_VelY = 0.0f;

        Uint8 m_ColorR = 255;
        Uint8 m_ColorG = 255;
        Uint8 m_ColorB = 255;
};