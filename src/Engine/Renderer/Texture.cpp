#include "Engine/Renderer/Texture.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>

Texture::~Texture(){
    Destroy();
}

bool Texture::LoadfromFile(SDL_Renderer* renderer, const std::string& path){
    Destroy(); //This will clear previous ones if any

    m_Texture = IMG_LoadTexture(renderer, path.c_str());
    if (!m_Texture) {
        std::cerr << "Failed to load texture: " << path << " | " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GetTextureSize(m_Texture, &m_Width, &m_Height);
    return true;
}

void Texture::Destroy() {
    if (m_Texture) {
        SDL_DestroyTexture(m_Texture);
        m_Texture = nullptr;
        m_Width = m_Height = 0;
    }
}
