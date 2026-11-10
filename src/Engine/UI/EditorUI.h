#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Core/Time.h"

class Application;

struct EditorViewportBounds {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
};

class EditorUI {
public:
    void Draw(Scene& scene, float deltaTime, Application& app);
    const EditorViewportBounds& GetViewportBounds() const { return m_ViewportBounds; }

private:
    EditorViewportBounds m_ViewportBounds;
};