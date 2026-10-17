#pragma once

#include "Engine/Entity/Entity.h"
#include "Engine/Entity/Player.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Camera/Camera.h"
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
        
        void AddEntity(float x, float y, std::shared_ptr<Texture> texture);

        Camera& GetCamera() { return m_Camera; }
        void UpdateCamera(float windowWidth, float windowHeight, float deltaTime);
    
    private:
        std::vector<std::unique_ptr<Entity>> m_Entities;
        Camera m_Camera;
};

