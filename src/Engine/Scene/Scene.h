#pragma once

#include "Engine/Entity/Entity.h"
#include "Engine/Entity/Player.h"
#include "Engine/Renderer/Texture.h"
#include <vector>
#include <memory>

class Scene
{
    public:
        Scene();
        ~Scene() = default;

        void Update(float deltaTime);
        void Renderer(SDL_Renderer* renderer);

        // Helper to keep player inside window
        void KeepPlayerInBounds(int windowWidth, int windowHeight);
        void SetPlayerTexture(std::shared_ptr<Texture> texture);
    
    private:
        std::vector<std::unique_ptr<Entity>> m_Entities;
};

