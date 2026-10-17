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
        m_Scene->AddEntity(700.0f, 400.0f, enemyTex);   // same texture reused
   }

   auto boxTex = tm.Load(renderer, "assets/textures/bird.png");
    if (boxTex) {
        m_Scene->AddEntity(300.0f, 500.0f, boxTex);
    }
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
        m_Scene->KeepPlayerInBounds(windowW, windowH);

        // Camera follows player
        m_Scene->UpdateCamera(static_cast<float>(windowW), static_cast<float>(windowH), deltaTime);

        m_ImGui->BeginFrame();
        // Show the official Demo window (very useful)
        ImGui::ShowDemoWindow();

        ImGui::Begin("Engine Debug");
        ImGui::Text("FPS: %.1f", Time::GetFPS());
        ImGui::Text("Delta Time: %.4f", deltaTime);
        ImGui::End();

        m_ImGui->EndFrame();

        m_Window->Clear(0.1f, 0.1f, 0.2f); //Dark Blue Color Window
        m_Scene->Renderer(m_Window->GetRenderer());

        // Render ImGui on top
        m_ImGui->Renderer(m_Window->GetRenderer());

        m_Window->Present();

        Input::EndFrame();
    }
    
}