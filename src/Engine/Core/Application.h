#pragma once

#include "Engine/Core/Window.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Scene/Scene.h"
#include "Engine/UI/ImGuiLayer.h"
#include "Engine/UI/EditorUI.h"
#include "Engine/UI/GameViewport.h"
#include <memory>
#include <vector>

enum class EditorMode {
    Edit,
    Play,
    Paused
};

enum class EditorTool {
    Select,
    Pan,
    Paint
};
class Application {
    public: 
        Application();
        ~Application();
        
        void Run();

        EditorMode GetMode() const { return m_Mode; }
        void SetMode(EditorMode mode);
        void Play();
        void Pause();
        void Stop();

        EditorTool GetTool() const { return m_Tool; }
        void SetTool(EditorTool tool) { m_Tool = tool; }
    
    private:
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<Scene> m_Scene;
        std::unique_ptr<ImGuiLayer> m_ImGui;
        bool m_Running = true;
        EditorUI m_EditorUI;
        GameViewport m_GameViewport;
        EditorMode m_Mode = EditorMode::Edit;
        EditorTool m_Tool = EditorTool::Select;

        void HandleViewportInput(float canvasX, float canvasY, float canvasW, float canvasH);
        bool  m_Dragging = false;
        float m_DragOffsetX = 0.0f;
        float m_DragOffsetY = 0.0f;

};
