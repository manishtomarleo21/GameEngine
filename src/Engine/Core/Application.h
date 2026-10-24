#pragma once

#include "Engine/Core/Window.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Scene/Scene.h"
#include "Engine/UI/ImGuiLayer.h"
#include "Engine/UI/EditorUI.h"
#include "Engine/UI/GameViewport.h"
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
        std::unique_ptr<ImGuiLayer> m_ImGui;
        bool m_Running = true;
        EditorUI m_EditorUI;
        GameViewport m_GameViewport;

        void HandleViewportInput(float canvasX, float canvasY, float canvasW, float canvasH);
        bool  m_Dragging = false;
        float m_DragOffsetX = 0.0f;
        float m_DragOffsetY = 0.0f;

};
