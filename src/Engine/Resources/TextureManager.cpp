#include "Engine/Resources/TextureManager.h"
#include <iostream>

TextureManager& TextureManager::Get(){
    static TextureManager instance;
    return instance;
}

std::shared_ptr<Texture> TextureManager::Load(SDL_Renderer* renderer, const std::string& path){
    auto it = m_Textures.find(path);
    if(it!=m_Textures.end()){
        return it->second;
    }

    auto texture = std::make_shared<Texture>();
    if(!texture->LoadfromFile(renderer, path)){
        std::cerr << "TextureManager: Failed to load " << path << std::endl;
        return nullptr;
    }

    m_Textures[path] = texture;
    std::cout << "TextureManager: Loaded " << path << std::endl;
    return texture;
}

void TextureManager::Clear() {
    m_Textures.clear();
}