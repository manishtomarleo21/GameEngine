#include "Engine/Scene/Scene.h"
#include "Engine/Physics/Collision.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Audio/Audio.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

Scene::Scene() {
    // Create Player
    auto player = std::make_unique<Player>(100.0f, 200.0f, 80.0f, 80.0f);
    m_Entities.push_back(std::move(player));
    m_Tilemap.Create(40, 30, 32); // 40x30 tiles, 32px each
    m_Tilemap.SetAllNonZeroSolid(true);
    if (m_Entities.size() > 1)
    {
        m_Entities[1]->SetGravityScale(1.0f);
        m_Entities[1]->SetBounciness(0.7f);
        m_Entities[1]->SetVelocity(100.0f, -200.0f); // optional starting velocity
    }
}

void Scene::Update(float deltaTime){
    for(auto& entity : m_Entities){
        if (!entity) continue;
        entity->Update(deltaTime);
        float x = entity->GetX();
        float y = entity->GetY();
        float w = entity->GetW();
        float h = entity->GetH();
        float vx = entity->GetVelX();
        float vy = entity->GetVelY();

        if (m_Tilemap.ResolveCollision(x, y, w, h, vx, vy)) {
            entity->SetPosition(x, y);
            entity->SetVelocity(vx, vy);
        }
    }
    m_Particles.Update(deltaTime);
    //Gravity Test
   Physics::ApplyFloorCollision(m_Entities, m_FloorY);

    // Collision
    if (!m_Entities.empty()) {
        Collision::ProcessPlayerCollisions(*m_Entities[0], m_Entities);
    }
}

void Scene::Renderer(SDL_Renderer* renderer, float viewW, float viewH) {
    m_Tilemap.Render(renderer, m_Camera.GetX(), m_Camera.GetY(), viewW, viewH);
    for (auto& entity : m_Entities) {
        // Temporary offset by camera
        float originalX = entity->GetX();
        float originalY = entity->GetY();

        entity->SetPosition(originalX - m_Camera.GetX(), originalY - m_Camera.GetY());
        entity->Renderer(renderer);
        entity->SetPosition(originalX, originalY); // restore
    }
    m_Particles.Render(renderer, m_Camera.GetX(), m_Camera.GetY());
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

    Entity* e = m_Entities[m_SelectedEntity].get();
    if (e) {
        // Spawn destroy effect at entity center
        float cx = e->GetX() + e->GetW() * 0.5f;
        float cy = e->GetY() + e->GetH() * 0.5f;
        m_Particles.EmitPreset(ParticlePreset::Explosion, cx, cy);

        // Optional: play sound
        Audio::Get().PlaySound("click");
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

bool Scene::SaveToFile(const std::string& path){
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to save scene: " << path << std::endl;
        return false;
    }
    // Header
    file << "SCENE_VERSION 1\n";
    // Background
    file << "BG " << m_BgR << " " << m_BgG << " " << m_BgB << "\n";
    
    for (auto& entity : m_Entities) {
        if (!entity) continue;

        file << "ENTITY "
             << entity->GetX() << " "
             << entity->GetY() << " "
             << entity->GetW() << " "
             << entity->GetH() << " "
             << static_cast<int>(entity->GetShape()) << " "
             << static_cast<int>(entity->GetColorR()) << " "
             << static_cast<int>(entity->GetColorG()) << " "
             << static_cast<int>(entity->GetColorB()) << " "
             << entity->GetGravityScale() << " "
             << entity->GetFriction() << " "
             << entity->GetBounciness()
             << "\n";
    }
    m_Tilemap.Save(file);
    file.close();
    std::cout << "Scene saved: " << path << std::endl;
    return true;
}

bool Scene::LoadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to load scene: " << path << std::endl;
        return false;
    }

    // Clear current entities
    m_Entities.clear();
    m_SelectedEntity = -1;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "BG") {
            ss >> m_BgR >> m_BgG >> m_BgB;
        }
        else if (type == "ENTITY") {
            float x, y, w, h;
            int shape, r, g, b;
            float gravity, friction, bounciness;

            ss >> x >> y >> w >> h >> shape >> r >> g >> b >> gravity >> friction >> bounciness;

            auto entity = std::make_unique<Entity>(x, y, w, h);
            entity->SetShape(static_cast<ShapeType>(shape));
            entity->SetColor(static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b));
            entity->SetGravityScale(gravity);
            entity->SetFriction(friction);
            entity->SetBounciness(bounciness);

            m_Entities.push_back(std::move(entity));
        }
        else if (type == "TILEMAP") {
            int w, h, tileSize;
            ss >> w >> h >> tileSize;

            // next line should be TILES ...
            std::string tilesLine;
            if (std::getline(file, tilesLine)) {
                std::istringstream tilesStream(tilesLine);
                std::string tilesTag;
                tilesStream >> tilesTag; // "TILES"

                std::vector<int> tiles;
                int id;
                while (tilesStream >> id) {
                    tiles.push_back(id);
                }

                m_Tilemap.LoadFromValues(w, h, tileSize, tiles);
            }
        }
    }

    file.close();
    std::cout << "Scene loaded: " << path << std::endl;
    return true;
}

int Scene::PickEntityAt(float worldX, float worldY) const {
    for (int i = static_cast<int>(m_Entities.size()) - 1; i >= 0; --i) {
        const Entity* e = m_Entities[i].get();
        if (!e) continue;

        if (worldX >= e->GetX() && worldX <= e->GetX() + e->GetW() &&
            worldY >= e->GetY() && worldY <= e->GetY() + e->GetH()) {
            return i;
        }
    }
    return -1;
}

void Scene::RenderGrid(SDL_Renderer* renderer, float viewW, float viewH) {
    if (!m_SnapToGrid || m_GridSize <= 0.0f) return;

    const float grid = m_GridSize;
    const float camX = m_Camera.GetX();
    const float camY = m_Camera.GetY();

    // Start from first visible grid line
    float startX = std::floor(camX / grid) * grid;
    float startY = std::floor(camY / grid) * grid;

    float endX = camX + viewW;
    float endY = camY + viewH;

    SDL_SetRenderDrawColor(renderer, 80, 80, 90, 100); // soft gray

    // Vertical lines
    for (float x = startX; x <= endX; x += grid) {
        float screenX = x - camX;
        SDL_RenderLine(renderer, screenX, 0.0f, screenX, viewH);
    }

    // Horizontal lines
    for (float y = startY; y <= endY; y += grid) {
        float screenY = y - camY;
        SDL_RenderLine(renderer, 0.0f, screenY, viewW, screenY);
    }
}

void Scene::SetPlayerStart(float x, float y) {
    m_PlayerStartX = x;
    m_PlayerStartY = y;
}

void Scene::ResetPlayerToStart() {
    if (m_Entities.empty()) return;

    m_Entities[0]->SetPosition(m_PlayerStartX, m_PlayerStartY);
    m_Entities[0]->SetVelocity(0.0f, 0.0f);
    m_Win = false;
}

void Scene::ClearWin() {
    m_Win = false;
}