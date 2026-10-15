#include "Engine/Core/Application.h"
#include <iostream>
#include "Engine/Input/Input.h"
#include "Engine/Core/Time.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Entity/Player.h"
#include "Engine/Physics/Collision.h"

Application::Application(){
    Time::Init();
    m_Window = std::make_unique<Window>("Game Engine", 1280, 720);

    //Create Player
    auto player = std::make_unique<Player>(100.0f, 200.0f, 80.0f, 80.0f);
    m_Entities.push_back(std::move(player));

    //Second entity for testing only
    auto other = std::make_unique<Entity>(400.0f, 300.0f, 60.0f, 60.0f);
    other->SetColor(50, 200, 80);
    m_Entities.push_back(std::move(other));
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

        //Update entities
        for(auto& entity : m_Entities){
            entity->Update(deltaTime);
        }


        if(!m_Entities.empty()){
            //To Keep Rectangle inside the window
            int windowW, windowH;
            m_Window->GetSize(windowW, windowH);

            auto& player = m_Entities[0];
            float x = player->GetX();
            float y = player->GetY();
            float w = player->GetW();
            float h = player->GetH();

            if (x < 0) x = 0;
            if (y < 0) y = 0;
            if (x + w > windowW) x = windowW - w;
            if (y + h > windowH) y = windowH - h;

            player->SetPosition(x, y);
        }

        if (!m_Entities.empty()){
            Collision::ProcessPlayerCollisions(*m_Entities[0], m_Entities);
        }

        // m_Window->SwapBuffers();
        m_Window->Clear(0.1f, 0.1f, 0.2f); //Dark Blue Color Window

        for(auto& entity : m_Entities){
            entity->Renderer(m_Window->GetRenderer());
        }

        m_Window->Present();
        Input::EndFrame();
    }
    
}