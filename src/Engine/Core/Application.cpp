#include "Engine/Core/Application.h"
#include <iostream>
#include "Engine/Input/Input.h"
#include "Engine/Core/Time.h"

Application::Application(){
    Time::Init();
    m_Window = std::make_unique<Window>("Game Engine", 1280, 720);
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

        m_RectX += moveX* m_Speed * deltaTime;
        m_RectY += moveY* m_Speed * deltaTime;

        //To Keep Rectangle inside the window
        int windowW, windowH;
        m_Window->GetSize(windowW, windowH);
        if (m_RectX < 0) m_RectX = 0;
        if (m_RectY < 0) m_RectY = 0;
        if (m_RectX + m_RectW > windowW) m_RectX = windowW - m_RectW;
        if (m_RectY + m_RectH > windowH) m_RectY = windowH - m_RectH;

        // m_Window->SwapBuffers();
        m_Window->Clear(0.1f, 0.1f, 0.2f); //Dark Blue Color Window

        SDL_FRect rect = { m_RectX, m_RectY, m_RectW, m_RectH };
        SDL_SetRenderDrawColor(m_Window->GetRenderer(), 220, 50, 50, 255);
        SDL_RenderFillRect(m_Window->GetRenderer(), &rect);

        m_Window->Present();
        Input::EndFrame();
    }
    
}