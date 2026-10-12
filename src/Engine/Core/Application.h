#pragma once

#include "Engine/Core/Window.h"
#include <memory>

class Application {
    public: 
        Application();
        ~Application();
        
        void Run();
    
    private:
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;

        float m_RectX = 100.0f;
        float m_RectY = 200.0f;
        float m_RectW = 80.0f;
        float m_RectH = 80.0f;
        float m_Speed = 300.0f;   // pixels per second

};
