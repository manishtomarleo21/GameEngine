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

        size_t GetEntityCount() const;
        Entity* GetEntity(size_t index);
        Camera& GetCamera() { return m_Camera; }
        void UpdateCamera(float windowWidth, float windowHeight, float deltaTime);
        void EnableGravityOnEntity(size_t index, float gravityScale = 1.0f, float bounciness = 0.7f);
        void SetEntityFriction(size_t index, float friction);
        void SetFloorY(float y);
        void SetCameraBounds(float minX, float minY, float maxX, float maxY);

        void SetBackgroundColor(float r, float g, float b);
        void GetBackgroundColor(float& r, float& g, float& b) const;

        void CreateRectangleEntity(float x, float y, float w, float h, float r, float g, float b);
        void CreateCircleEntity(float x, float y, float radius, float r, float g, float b);

        void SetSelectedEntity(int index);
        int  GetSelectedEntity() const { return m_SelectedEntity; }
        Entity* GetSelectedEntityPtr();

        void DeleteSelectedEntity();
        void DuplicateSelectedEntity();

    
    private:
        std::vector<std::unique_ptr<Entity>> m_Entities;
        Camera m_Camera;
        float m_FloorY = 700.0f;
        float m_BgR = 0.1f;
        float m_BgG = 0.1f;
        float m_BgB = 0.2f;
        int m_SelectedEntity = -1;   // -1 means nothing selected
};

