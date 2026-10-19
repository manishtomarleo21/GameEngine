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
    m_ImGui->Init(m_Window->GetNativeWindow(), m_Window->GetRenderer());˳
    m_Scene = std::make_unique<Scene>();

    auto& tm = TextureManager::Get();
    SDL_Renderer* renderer = m_Window->GetRenderer();

   auto playerTex = tm.Load(renderer, "assets/textures/player.png");
   if(playerTex) m_Scene->SetPlayerTexture(playerTex);

   auto enemyTex = tm.Load(renderer, "assets/textures/fly.png");
   if(enemyTex){
        m_Scene->AddEntity(500.0f, 200.0f, enemyTex);
        m_Scene->AddEntity(700.0f, 400.0f, enemyTex);   // same texture reused
   }

//    auto boxTex = tm.Load(renderer, "assets/textures/bird.png");
//     if (boxTex) {
//         m_Scene->AddEntity(300.0f, 500.0f, boxTex);
//     }
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
        // Show the official Demo window (very useful)
        ImGui::Begin("Editor");

        // --- Background Color ---
        ImGui::Text("Background Color");
        static float bgColor[3] = { 0.1f, 0.1f, 0.2f };
        if (ImGui::ColorEdit3("##bg", bgColor)) {
            m_Scene->SetBackgroundColor(bgColor[0], bgColor[1], bgColor[2]);
        }

        ImGui::Separator();

        // --- Create Entity ---
        ImGui::Text("Create Entity");
        static float newPos[2] = { 200.0f, 200.0f };
        static float newSize[2] = { 80.0f, 80.0f };
        static float newColor[3] = { 0.9f, 0.3f, 0.3f };

        ImGui::DragFloat2("Position", newPos, 1.0f);
        ImGui::DragFloat2("Size", newSize, 1.0f);
        ImGui::ColorEdit3("Color", newColor);

        if (ImGui::Button("Create Rectangle", ImVec2(-1, 0))) {
            m_Scene->CreateRectangleEntity(
                newPos[0], newPos[1],
                newSize[0], newSize[1],
                newColor[0], newColor[1], newColor[2]
            );
        }

        ImGui::Separator();
        ImGui::Text("Entities: %zu", m_Scene->GetEntityCount());

        ImGui::End();
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