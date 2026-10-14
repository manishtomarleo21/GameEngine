#include "Engine/Entity/Entity.h"

Entity::Entity(float x, float y, float w, float h)
    : m_X(x), m_Y(y), m_W(w), m_H(h){

    }

void Entity::Update(float deltaTime){
    m_X += m_VelX * deltaTime;
    m_Y += m_VelY * deltaTime;
}

void Entity::Renderer(SDL_Renderer* renderer){
    SDL_FRect rect = { m_X, m_Y, m_W, m_H };
    SDL_SetRenderDrawColor(renderer, m_ColorR, m_ColorG, m_ColorB, 255);
    SDL_RenderFillRect(renderer, &rect);
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