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

    // Highlight selected entity
    Entity* selected = GetSelectedEntityPtr();
    if (selected) {
        float x = selected->GetX() - m_Camera.GetX();
        float y = selected->GetY() - m_Camera.GetY();
        float w = selected->GetW();
        float h = selected->GetH();

        SDL_FRect outline = { x - 3.0f, y - 3.0f, w + 6.0f, h + 6.0f };

        SDL_SetRenderDrawColor(renderer, 255, 220, 50, 255); // yellow
        SDL_RenderRect(renderer, &outline);
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

void Scene::SetCameraBounds(float minX, float minY, float maxX, float maxY){
    m_Camera.SetBounds(minX, minY, maxX, maxY);
    m_Camera.EnableBounds(true);
}

size_t Scene::GetEntityCount() const {
    return m_Entities.size();
}

Entity* Scene::GetEntity(size_t index) {
    if (index >= m_Entities.size()) return nullptr;
    return m_Entities[index].get();
}

void Scene::SetBackgroundColor(float r, float g, float b) {
    m_BgR = r;
    m_BgG = g;
    m_BgB = b;
}

void Scene::GetBackgroundColor(float& r, float& g, float& b) const {
    r = m_BgR;
    g = m_BgG;
    b = m_BgB;
}

void Scene::CreateRectangleEntity(float x, float y, float w, float h, float r, float g, float b) {
    auto entity = std::make_unique<Entity>(x, y, w, h);
    entity->SetColor(
        static_cast<Uint8>(r * 255),
        static_cast<Uint8>(g * 255),
        static_cast<Uint8>(b * 255)
    );
    m_Entities.push_back(std::move(entity));
}

void Scene::SetSelectedEntity(int index) {
    if (index >= 0 && index < static_cast<int>(m_Entities.size())) {
        m_SelectedEntity = index;
    } else {
        m_SelectedEntity = -1;
    }
}

Entity* Scene::GetSelectedEntityPtr() {
    if (m_SelectedEntity >= 0 && m_SelectedEntity < static_cast<int>(m_Entities.size())) {
        return m_Entities[m_SelectedEntity].get();
    }
    return nullptr;
}

void Scene::CreateCircleEntity(float x, float y, float radius, float r, float g, float b) {
    auto entity = std::make_unique<Entity>(x, y, radius * 2.0f, radius * 2.0f);
    entity->SetShape(ShapeType::Circle);
    entity->SetColor(
        static_cast<Uint8>(r * 255),
        static_cast<Uint8>(g * 255),
        static_cast<Uint8>(b * 255)
    );
    m_Entities.push_back(std::move(entity));
}

void Scene::DeleteSelectedEntity() {
    if (m_SelectedEntity < 0 || m_SelectedEntity >= static_cast<int>(m_Entities.size())) {
        return;
    }

    m_Entities.erase(m_Entities.begin() + m_SelectedEntity);
    m_SelectedEntity = -1; // clear selection
}

void Scene::DuplicateSelectedEntity() {
    Entity* selected = GetSelectedEntityPtr();
    if (!selected) return;

    // Create a copy slightly offset
    auto copy = std::make_unique<Entity>(
        selected->GetX() + 30.0f,
        selected->GetY() + 30.0f,
        selected->GetW(),
        selected->GetH()
    );

    copy->SetShape(selected->GetShape());
    copy->SetColor(selected->GetColorR(), selected->GetColorG(), selected->GetColorB());
    copy->SetGravityScale(selected->GetGravityScale());
    copy->SetBounciness(selected->GetBounciness());
    copy->SetFriction(selected->GetFriction());
    copy->SetVelocity(selected->GetVelX(), selected->GetVelY());

    // Optional: copy texture if it has one
    // (We can improve this later)

    m_Entities.push_back(std::move(copy));

    // Select the new duplicated entity
    m_SelectedEntity = static_cast<int>(m_Entities.size()) - 1;
}