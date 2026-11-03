#include "Engine/Core/Application.h"
#include <iostream>
#include "Engine/Input/Input.h"
#include "Engine/Core/Time.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Entity/Player.h"
#include "Engine/Physics/Collision.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Resources/TextureManager.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Animation/Animation.h"
#include "imgui.h"
#include <memory>
#include <cmath>

Application::Application(){
    Time::Init();
    m_Window = std::make_unique<Window>("Game Engine", 1280, 720);
    m_GameViewport.Create(m_Window->GetRenderer(), 1280, 720);
    Audio::Get().Init();
    m_ImGui = std::make_unique<ImGuiLayer>();
    m_ImGui->Init(m_Window->GetNativeWindow(), m_Window->GetRenderer());
    m_Scene = std::make_unique<Scene>();

    auto& tm = TextureManager::Get();
    SDL_Renderer* renderer = m_Window->GetRenderer();

    auto tileset = TextureManager::Get().Load(
        m_Window->GetRenderer(),
        "assets/textures/tileset.png");
    if (tileset)
    {
        m_Scene->GetTilemap().SetTileset(tileset, 8);
    }
   auto sheet = tm.Load(renderer, "assets/textures/players_sheet.jpg"); 
   auto playerTex = tm.Load(renderer, "assets/textures/player.png");
   if(playerTex) m_Scene->SetPlayerTexture(playerTex);

   auto enemyTex = tm.Load(renderer, "assets/textures/fly.png");
   if(enemyTex){
        m_Scene->AddEntity(500.0f, 200.0f, enemyTex);
   }

 Entity* player = m_Scene->GetEntity(0);
if (player && sheet) {
    Animation run("run", sheet);

    // Row 1 (top)
    run.AddFrame(0,    0, 1344, 1792, 0.10f);
    run.AddFrame(1344, 0, 1344, 1792, 0.10f);
    run.AddFrame(2688, 0, 1344, 1792, 0.10f);
    run.AddFrame(4032, 0, 1344, 1792, 0.10f);

    // Row 2 (bottom)
    run.AddFrame(0,    1792, 1344, 1792, 0.10f);
    run.AddFrame(1344, 1792, 1344, 1792, 0.10f);
    run.AddFrame(2688, 1792, 1344, 1792, 0.10f);
    run.AddFrame(4032, 1792, 1344, 1792, 0.10f);

    run.SetLoop(true);

    player->AddAnimation("run", run);
    player->PlayAnimation("run");

    // Display size on screen (scaled down)
    player->SetSize(96, 128);
}

    Audio::Get().LoadSound("click", "assets/audio/click.wav");
    Audio::Get().LoadMusic("bgm", "assets/audio/bgm.mp3");

    m_Scene->SetCameraBounds(0.0f, 0.0f, 2000.0f, 1200.0f); 
    m_Scene->EnableGravityOnEntity(1, 1.0f, 0.7f);
    // m_Scene->EnableGravityOnEntity(2, 1.0f, 0.6f); // optional
    m_Scene->SetEntityFriction(1, 0.15f);
}

Application::~Application(){
    Audio::Get().Shutdown();
}

void Application::Run(){
    while (m_Running)
    {
        Time::Update();
        float deltaTime = Time::GetDeltaTime();

        // m_Window->PollEvents();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            m_ImGui->ProcessEvent(event);
            if (event.type == SDL_EVENT_QUIT) {
                m_Running = false;
            }
        }
        
        if(m_Window->ShouldClose()){
            m_Running = false;
        }

        Input::Update();

        m_Scene->Update(deltaTime);

        int windowW, windowH;
        m_Window->GetSize(windowW, windowH);
        m_Scene->SetFloorY(static_cast<float>(windowH));
        m_Scene->KeepPlayerInBounds(windowW, windowH);

        // Camera follows player
        m_Scene->UpdateCamera(static_cast<float>(windowW), static_cast<float>(windowH), deltaTime);

        m_ImGui->BeginFrame();
        m_EditorUI.Draw(*m_Scene, deltaTime);

        const EditorViewportBounds& viewportBounds = m_EditorUI.GetViewportBounds();
        ImGui::SetNextWindowPos(ImVec2(viewportBounds.x, viewportBounds.y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(viewportBounds.width, viewportBounds.height), ImGuiCond_Always);
        ImGui::Begin("Game View", nullptr,
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse);

        ImVec2 canvasPos  = ImGui::GetCursorScreenPos(); // top-left of the game Window
        ImVec2 size = ImGui::GetContentRegionAvail();
        int vpW = static_cast<int>(size.x);
        int vpH = static_cast<int>(size.y);

        if (vpW > 0 && vpH > 0) {
            m_GameViewport.Resize(vpW, vpH);

            // Render scene into the texture
            m_GameViewport.Begin();

            float r, g, b;
            m_Scene->GetBackgroundColor(r, g, b);
            SDL_SetRenderDrawColor(m_Window->GetRenderer(),
                static_cast<Uint8>(r * 255),
                static_cast<Uint8>(g * 255),
                static_cast<Uint8>(b * 255), 255);
            SDL_RenderClear(m_Window->GetRenderer());

            m_Scene->Renderer(m_Window->GetRenderer(), static_cast<float>(vpW), static_cast<float>(vpH));

            m_Scene->RenderGrid(
                m_Window->GetRenderer(),
                static_cast<float>(vpW),
                static_cast<float>(vpH)
            );

            m_GameViewport.End();

            // Show the texture inside ImGui
            ImGui::Image((ImTextureID)(intptr_t)m_GameViewport.GetTexture(), size);

            //FIx to stop the Drag of ImGui window when moving dragging entity
            ImGui::SetCursorScreenPos(canvasPos);
            ImGui::InvisibleButton("GameViewportInput", size);
            // Handle click + drag (clean call)
            HandleViewportInput(canvasPos.x, canvasPos.y, size.x, size.y);
        }

        ImGui::End();

        m_ImGui->EndFrame();

        SDL_SetRenderDrawColor(m_Window->GetRenderer(), 20, 20, 22, 255);
        SDL_RenderClear(m_Window->GetRenderer());
        m_ImGui->Renderer(m_Window->GetRenderer());
        m_Window->Present();

        Input::EndFrame();
    }
    
}

static float Snap(float value, float grid) {
    if (grid <= 0.0f) return value;
    return std::round(value / grid) * grid;
}

void Application::HandleViewportInput(float canvasX, float canvasY, float canvasW, float canvasH) {
    ImVec2 mouse = ImGui::GetMousePos();
    bool hovered = ImGui::IsItemHovered();
    bool active  = ImGui::IsItemActive();

    float localX = mouse.x - canvasX;
    float localY = mouse.y - canvasY;

    // Ignore if mouse is outside the image
    if (localX < 0 || localY < 0 || localX > canvasW || localY > canvasH) {
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            m_Dragging = false;
        }
        return;
    }

    float worldX = localX + m_Scene->GetCamera().GetX();
    float worldY = localY + m_Scene->GetCamera().GetY();

    // Mouse pressed → select + start drag
    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        int hit = m_Scene->PickEntityAt(worldX, worldY);
        m_Scene->SetSelectedEntity(hit);

        if (hit >= 0) {
            Entity* e = m_Scene->GetEntity(hit);
            if (e) {
                m_Dragging = true;
                m_DragOffsetX = worldX - e->GetX();
                m_DragOffsetY = worldY - e->GetY();
            }
        } else {
            m_Dragging = false;
        }
    }

    // Mouse held → drag
    if (m_Dragging && active) {
        Entity* selected = m_Scene->GetSelectedEntityPtr();
        if (selected) {
            float newX = worldX - m_DragOffsetX;
            float newY = worldY - m_DragOffsetY;

            if (m_Scene->GetSnapToGrid()) {
                newX = Snap(newX, m_Scene->GetGridSize());
                newY = Snap(newY, m_Scene->GetGridSize());
            }

            selected->SetPosition(newX, newY);
        }
    }

    if (!m_Dragging && hovered) {
        // Right mouse = paint
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            m_Scene->GetTilemap().PaintTile(
                worldX,
                worldY,
                m_Scene->GetPaintTileId()
            );
        }

        // Middle mouse = erase
        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
            m_Scene->GetTilemap().PaintTile(worldX, worldY, 0);
        }
    }

    // Mouse released → stop drag
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        m_Dragging = false;
    }
}