#pragma once

#include "Engine/Core/Window.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Scene/Scene.h"
#include <memory>
#include <vector>

class Application {
    public: 
        Application();
        ~Application();
        
        void Run();
    
    private:
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<Scene> m_Scene;
        bool m_Running = true;

};
