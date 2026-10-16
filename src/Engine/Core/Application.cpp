#include "Engine/Core/Application.h"
#include <iostream>
#include "Engine/Input/Input.h"
#include "Engine/Core/Time.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Entity/Player.h"
#include "Engine/Physics/Collision.h"
#include "Engine/Renderer/Texture.h"
#include <memory>

Application::Application(){
    Time::Init();
    m_Window = std::make_unique<Window>("Game Engine", 1920, 1080);
    m_Scene = std::make_unique<Scene>();

    auto playerTex = std::make_shared<Texture>();
    if (playerTex->LoadfromFile(m_Window->GetRenderer(), "assets/textures/player.png")) {
        m_Scene->SetPlayerTexture(playerTex);
        std::cout << "Player texture loaded successfully!" << std::endl;
    } else {
        std::cout << "Could not load player texture (using colored rectangle instead)" << std::endl;
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