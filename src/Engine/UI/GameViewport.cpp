#include "Engine/UI/GameViewport.h"
#include <iostream>

GameViewport::~GameViewport(){
    Destroy();
}

bool GameViewport::Create(SDL_Renderer* renderer, int width, int height) {
    m_Renderer = renderer;
    return Resize(width, height);
}

bool GameViewport::Resize(int width, int height) {
    if (width <= 0 || height <= 0) return false;
    if (width == m_Width && height == m_Height && m_Texture) return true;

    Destroy();

    m_Width  = width;
    m_Height = height;

    m_Texture = SDL_CreateTexture(
        m_Renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        m_Width,
        m_Height
    );

    if (!m_Texture) {
        std::cerr << "Failed to create viewport texture: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void GameViewport::Destroy() {
    if (m_Texture) {
        SDL_DestroyTexture(m_Texture);
        m_Texture = nullptr;
    }
    m_Width = m_Height = 0;
}

void GameViewport::Begin() {
    if (m_Texture) {
        SDL_SetRenderTarget(m_Renderer, m_Texture);
    }
}

void GameViewport::End() {
    SDL_SetRenderTarget(m_Renderer, nullptr);
}