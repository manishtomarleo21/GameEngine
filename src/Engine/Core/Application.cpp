#include "Engine/Core/Application.h"
#include <iostream>
#include "Engine/Input/Input.h"
#include "Engine/Core/Time.h"
#include "Engine/Entity/Entity.h"

Application::Application(){
    Time::Init();
    m_Window = std::make_unique<Window>("Game Engine", 1280, 720);

    auto player = std::make_unique<Entity>(100.0f, 200.0f, 80.0f, 80.0f);
    player->SetColor(220, 50, 50);
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

        if(!m_Entities.empty()){
            
            float moveX = 0.0f;
            float moveY = 0.0f;

            if (Input::IsKeyPressed(SDL_SCANCODE_LEFT)  || Input::IsKeyPressed(SDL_SCANCODE_A)) moveX -= 1.0f;
            if (Input::IsKeyPressed(SDL_SCANCODE_RIGHT) || Input::IsKeyPressed(SDL_SCANCODE_D)) moveX += 1.0f;
            if (Input::IsKeyPressed(SDL_SCANCODE_UP)    || Input::IsKeyPressed(SDL_SCANCODE_W)) moveY -= 1.0f;
            if (Input::IsKeyPressed(SDL_SCANCODE_DOWN)  || Input::IsKeyPressed(SDL_SCANCODE_S)) moveY += 1.0f;

            //Diagonal Movement
            if (moveX != 0.0f && moveY != 0.0f) {
                moveX *= 0.7071f;
                moveY *= 0.7071f;
            }

            float speed = 400.0f;
            m_Entities[0]->SetVelocity(moveX * speed, moveY * speed);
        }

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

        // m_Window->SwapBuffers();
        m_Window->Clear(0.1f, 0.1f, 0.2f); //Dark Blue Color Window

        for(auto& entity : m_Entities){
            entity->Renderer(m_Window->GetRenderer());
        }

        m_Window->Present();
        Input::EndFrame();
    }
    
}