#include "Engine/Scene/Scene.h"
#include "Engine/Physics/Collision.h"

Scene::Scene() {
    // Create Player
    auto player = std::make_unique<Player>(100.0f, 200.0f, 80.0f, 80.0f);
    m_Entities.push_back(std::move(player));

    auto other = std::make_unique<Entity>(400.0f, 300.0f, 60.0f, 60.0f);
    other->SetColor(50, 200, 80);
    m_Entities.push_back(std::move(other));
}

void Scene::Update(float deltaTime){
    for(auto& entity : m_Entities){
        entity->Update(deltaTime);
    }

    // Collision
    if (!m_Entities.empty()) {
        Collision::ProcessPlayerCollisions(*m_Entities[0], m_Entities);
    }
}

void Scene::Renderer(SDL_Renderer* renderer) {
    for (auto& entity : m_Entities) {
        entity->Renderer(renderer);
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