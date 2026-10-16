#include "Engine/Core/Application.h"
#include <iostream>
#include "Engine/Input/Input.h"
#include "Engine/Core/Time.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Entity/Player.h"
#include "Engine/Physics/Collision.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Resources/TextureManager.h"
#include <memory>

Application::Application(){
    Time::Init();
    m_Window = std::make_unique<Window>("Game Engine", 1280, 720);
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

        m_Window->PollEvents();
        if(m_Window->ShouldClose()){
            m_Running = false;
        }

        Input::Update();

        m_Scene->Update(deltaTime);

        int windowW, windowH;
        m_Window->GetSize(windowW, windowH);
        m_Scene->KeepPlayerInBounds(windowW, windowH);

        m_Window->Clear(0.1f, 0.1f, 0.2f); //Dark Blue Color Window
        m_Scene->Renderer(m_Window->GetRenderer());
        m_Window->Present();

        Input::EndFrame();
    }
    
}