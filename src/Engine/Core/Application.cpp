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
#include "imgui.h"
#include <memory>

Application::Application(){
    Time::Init();
    m_Window = std::make_unique<Window>("Game Engine", 1280, 720);
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

    m_Scene->SetCameraBounds(0.0f, 0.0f, 2000.0f, 1200.0f); 
    m_Scene->EnableGravityOnEntity(1, 1.0f, 0.7f);
    // m_Scene->EnableGravityOnEntity(2, 1.0f, 0.6f); // optional
    m_Scene->SetEntityFriction(1, 0.15f);
}

Application::~Application(){

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
        m_ImGui->EndFrame();

        float r, g, b;
        m_Scene->GetBackgroundColor(r, g, b);
        m_Window->Clear(r, g, b);
        m_Scene->Renderer(m_Window->GetRenderer());

        // Render ImGui on top
        m_ImGui->Renderer(m_Window->GetRenderer());

        m_Window->Present();

        Input::EndFrame();
    }
    
}