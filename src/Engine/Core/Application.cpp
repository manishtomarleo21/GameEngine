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
#include "imgui.h"
#include <memory>

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

   auto playerTex = tm.Load(renderer, "assets/textures/player.png");
   if(playerTex) m_Scene->SetPlayerTexture(playerTex);

   auto enemyTex = tm.Load(renderer, "assets/textures/fly.png");
   if(enemyTex){
        m_Scene->AddEntity(500.0f, 200.0f, enemyTex);
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

        // === Game Viewport Window ===
        ImGui::SetNextWindowPos(ImVec2(20, 40), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);
        ImGui::Begin("Game");

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

            m_Scene->Renderer(m_Window->GetRenderer());

            m_GameViewport.End();

            // Show the texture inside ImGui
            ImGui::Image((ImTextureID)(intptr_t)m_GameViewport.GetTexture(), size);
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