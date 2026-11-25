#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Core/Time.h"
#include <string>

class Application;

struct EditorViewportBounds {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
};

class EditorUI {
public:
    void Draw(Scene& scene, float deltaTime, Application& app, SDL_Renderer* renderer);
    const EditorViewportBounds& GetViewportBounds() const { return m_ViewportBounds; }

private:
    EditorViewportBounds m_ViewportBounds;
    char m_SaveSceneFilename[128] = "scene1.txt";
    std::string m_LoadSceneFilename = "scene1.txt";
    std::string m_PendingSavePath;
    std::string m_SceneDialogError;
    bool m_ConfirmSceneOverwrite = false;
    bool m_OpenSaveSceneDialog = false;
    bool m_OpenLoadSceneDialog = false;
};