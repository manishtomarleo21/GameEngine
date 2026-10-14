#pragma once

#include "Engine/Core/Window.h"
#include "Engine/Entity/Entity.h"
#include <memory>
#include <vector>

class Application {
    public: 
        Application();
        ~Application();
        
        void Run();
    
    private:
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;

       std::vector<std::unique_ptr<Entity>> m_Entities;

};
