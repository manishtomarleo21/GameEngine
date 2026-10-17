#include "Engine/Scene/Scene.h"
#include "Engine/Physics/Collision.h"
#include "Engine/Physics/Physics.h"

Scene::Scene() {
    // Create Player
    auto player = std::make_unique<Player>(100.0f, 200.0f, 80.0f, 80.0f);
    m_Entities.push_back(std::move(player));

    if (m_Entities.size() > 1)
    {
        m_Entities[1]->SetGravityScale(1.0f);
        m_Entities[1]->SetBounciness(0.7f);
        m_Entities[1]->SetVelocity(100.0f, -200.0f); // optional starting velocity
    }
}

void Scene::Update(float deltaTime){
    for(auto& entity : m_Entities){
        entity->Update(deltaTime);
    }

    //Gravity Test
   Physics::ApplyFloorCollision(m_Entities, m_FloorY);

    // Collision
    if (!m_Entities.empty()) {
        Collision::ProcessPlayerCollisions(*m_Entities[0], m_Entities);
    }
}

void Scene::Renderer(SDL_Renderer* renderer) {
    for (auto& entity : m_Entities) {
        // Temporary offset by camera
        float originalX = entity->GetX();
        float originalY = entity->GetY();

        entity->SetPosition(originalX - m_Camera.GetX(), originalY - m_Camera.GetY());
        entity->Renderer(renderer);
        entity->SetPosition(originalX, originalY); // restore
    }
}

void Scene::KeepPlayerInBounds(int windowWidth, int windowHeight) {
    if (m_Entities.empty()) return;

    auto& player = m_Entities[0];
    float x = player->GetX();
    float y = player->GetY();
    float w = player->GetW();
    float h = player->GetH();

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + w > windowWidth)  x = windowWidth  - w;
    if (y + h > windowHeight) y = windowHeight - h;

    player->SetPosition(x, y);
}

void Scene::SetPlayerTexture(std::shared_ptr<Texture> texture){
    if (!m_Entities.empty() && texture) {
        m_Entities[0]->SetTexture(texture);
    }
}

void Scene::AddEntity(float x, float y, std::shared_ptr<Texture> texture){
    if(!texture) return;

    auto entity = std::make_unique<Entity>(x, y, texture->GetWidth(), texture->GetHeight());
    entity->SetTexture(texture);
    m_Entities.push_back(std::move(entity));
}

void Scene::UpdateCamera(float windowWidth, float windowHeight, float deltaTime) {
    if (m_Entities.empty()) return;

    float playerX = m_Entities[0]->GetX() + m_Entities[0]->GetW() / 2.0f;
    float playerY = m_Entities[0]->GetY() + m_Entities[0]->GetH() / 2.0f;

    m_Camera.Follow(playerX, playerY, windowWidth, windowHeight, deltaTime);
}

void Scene::EnableGravityOnEntity(size_t index, float gravityScale, float bounciness) {
    if (index >= m_Entities.size()) return;

    m_Entities[index]->SetGravityScale(gravityScale);
    m_Entities[index]->SetBounciness(bounciness);
}

void Scene::SetFloorY(float y) {
    m_FloorY = y;
}

void Scene::SetEntityFriction(size_t index, float friction){
    if(index < m_Entities.size()){
        m_Entities[index]->SetFriction(friction);
    }
}