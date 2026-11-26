#include "Engine/Renderer/Texture.h"
#include <SDL3_image/SDL_image.h>
#include <algorithm>
#include <iostream>

Texture::~Texture(){
    Destroy();
}

bool Texture::LoadfromFile(SDL_Renderer* renderer, const std::string& path){
    Destroy(); //This will clear previous ones if any

    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load texture: " << path << " | " << SDL_GetError() << std::endl;
        return false;
    }

    int minX = surface->w;
    int minY = surface->h;
    int maxX = -1;
    int maxY = -1;
    bool alphaRead = true;
    for (int y = 0; y < surface->h && alphaRead; ++y) {
        for (int x = 0; x < surface->w; ++x) {
            Uint8 r, g, b, a;
            if (!SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a)) {
                alphaRead = false;
                break;
            }
            if (a > 0) {
                minX = std::min(minX, x);
                minY = std::min(minY, y);
                maxX = std::max(maxX, x);
                maxY = std::max(maxY, y);
            }
        }
    }

    if (alphaRead) {
        m_HasOpaqueBounds = true;
        if (maxX >= minX && maxY >= minY) {
            m_OpaqueX = static_cast<float>(minX) / surface->w;
            m_OpaqueY = static_cast<float>(minY) / surface->h;
            m_OpaqueW = static_cast<float>(maxX - minX + 1) / surface->w;
            m_OpaqueH = static_cast<float>(maxY - minY + 1) / surface->h;
        } else {
            m_OpaqueX = 0.0f;
            m_OpaqueY = 0.0f;
            m_OpaqueW = 0.0f;
            m_OpaqueH = 0.0f;
        }
    } else {
        std::cerr << "Failed to inspect texture alpha bounds: " << path << " | "
                  << SDL_GetError() << std::endl;
    }

    m_Texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (!m_Texture) {
        std::cerr << "Failed to create texture: " << path << " | " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GetTextureSize(m_Texture, &m_Width, &m_Height);
    m_Path = path;
    return true;
}

void Texture::Destroy() {
    if (m_Texture) {
        SDL_DestroyTexture(m_Texture);
        m_Texture = nullptr;
        m_Width = m_Height = 0;
    }
    m_Path.clear();
    m_OpaqueX = 0.0f;
    m_OpaqueY = 0.0f;
    m_OpaqueW = 1.0f;
    m_OpaqueH = 1.0f;
    m_HasOpaqueBounds = false;
}

bool Texture::GetOpaqueBounds(float& x, float& y, float& w, float& h) const {
    if (!m_HasOpaqueBounds) return false;
    x = m_OpaqueX;
    y = m_OpaqueY;
    w = m_OpaqueW;
    h = m_OpaqueH;
    return true;
}
