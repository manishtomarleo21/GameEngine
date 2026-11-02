#include "Engine/UI/EditorUI.h"

#include "Engine/Audio/Audio.h"
#include "Engine/Particles/ParticleSystem.h"
#include "imgui.h"

#include <algorithm>
#include <cstdio>

namespace {
constexpr float kPanelGap = 8.0f;
constexpr float kOuterMargin = 10.0f;
constexpr float kTopOffset = 28.0f;
constexpr float kBottomMargin = 10.0f;
constexpr float kMinimumViewportWidth = 320.0f;
constexpr float kMinimumPanelWidth = 170.0f;
constexpr float kMaximumPanelWidth = 280.0f;
}

void EditorUI::Draw(Scene& scene, float deltaTime) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Scene")) {
                scene.SaveToFile("assets/scenes/scene1.txt");
            }
            if (ImGui::MenuItem("Load Scene")) {
                scene.LoadFromFile("assets/scenes/scene1.txt");
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            const bool hasSelection = scene.GetSelectedEntityPtr() != nullptr;
            ImGui::BeginDisabled(!hasSelection);
            if (ImGui::MenuItem("Duplicate", "Cmd+D")) {
                scene.DuplicateSelectedEntity();
            }
            if (ImGui::MenuItem("Delete", "Backspace")) {
                scene.DeleteSelectedEntity();
            }
            ImGui::EndDisabled();
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    const ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    const float availableWidth = std::max(0.0f, displaySize.x - 2.0f * kOuterMargin - 2.0f * kPanelGap);
    const float panelWidth = std::clamp(
        displaySize.x * 0.22f,
        kMinimumPanelWidth,
        std::min(kMaximumPanelWidth, std::max(kMinimumPanelWidth, (availableWidth - kMinimumViewportWidth) * 0.5f)));
    const float viewportWidth = std::max(0.0f, availableWidth - 2.0f * panelWidth);
    const float panelHeight = std::max(0.0f, displaySize.y - kTopOffset - kBottomMargin);
    const float bottomY = kTopOffset + panelHeight * 0.46f + kPanelGap;
    const float bottomHeight = std::max(0.0f, displaySize.y - bottomY - kBottomMargin);

    m_ViewportBounds = {
        kOuterMargin + panelWidth + kPanelGap,
        kTopOffset,
        viewportWidth,
        panelHeight
    };

    ImGuiWindowFlags panelFlags = ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowPos(ImVec2(kOuterMargin, kTopOffset), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, panelHeight * 0.46f), ImGuiCond_Always);
    if (ImGui::Begin("Hierarchy", nullptr, panelFlags)) {
        ImGui::TextDisabled("%zu OBJECTS", scene.GetEntityCount());
        ImGui::Separator();

        for (size_t i = 0; i < scene.GetEntityCount(); ++i) {
            Entity* entity = scene.GetEntity(i);
            if (!entity) {
                continue;
            }

            char label[80];
            std::snprintf(label, sizeof(label), "Entity %zu  (%.0f, %.0f)",
                i, entity->GetX(), entity->GetY());
            if (ImGui::Selectable(label, scene.GetSelectedEntity() == static_cast<int>(i))) {
                scene.SetSelectedEntity(static_cast<int>(i));
            }
        }
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(kOuterMargin, bottomY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, bottomHeight), ImGuiCond_Always);
    if (ImGui::Begin("Create", nullptr, panelFlags)) {
        static float newPos[2] = {300.0f, 200.0f};
        static float newSize[2] = {80.0f, 80.0f};
        static float newColor[3] = {0.9f, 0.3f, 0.3f};

        ImGui::DragFloat2("Position", newPos, 1.0f);
        ImGui::DragFloat2("Size", newSize, 1.0f, 1.0f, 2000.0f);
        ImGui::ColorEdit3("Color", newColor);

        if (ImGui::Button("Rectangle", ImVec2(-1, 0))) {
            scene.CreateRectangleEntity(newPos[0], newPos[1], newSize[0], newSize[1],
                newColor[0], newColor[1], newColor[2]);
        }
        if (ImGui::Button("Circle", ImVec2(-1, 0))) {
            scene.CreateCircleEntity(newPos[0], newPos[1], newSize[0] * 0.5f,
                newColor[0], newColor[1], newColor[2]);
        }
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(displaySize.x - kOuterMargin - panelWidth, kTopOffset), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, panelHeight * 0.62f), ImGuiCond_Always);
    if (ImGui::Begin("Inspector", nullptr, panelFlags)) {
        Entity* selected = scene.GetSelectedEntityPtr();
        if (selected) {
            ImGui::Text("Entity %d", scene.GetSelectedEntity());
            ImGui::Separator();

            float position[2] = {selected->GetX(), selected->GetY()};
            float size[2] = {selected->GetW(), selected->GetH()};
            if (ImGui::DragFloat2("Position", position, 1.0f)) {
                selected->SetPosition(position[0], position[1]);
            }
            if (ImGui::DragFloat2("Size", size, 1.0f, 1.0f, 2000.0f)) {
                selected->SetSize(size[0], size[1]);
            }

            bool hasGravity = selected->GetGravityScale() > 0.0f;
            if (ImGui::Checkbox("Gravity", &hasGravity)) {
                selected->SetGravityScale(hasGravity ? 1.0f : 0.0f);
            }

            float friction = selected->GetFriction();
            if (ImGui::SliderFloat("Friction", &friction, 0.0f, 1.0f)) {
                selected->SetFriction(friction);
            }

            float bounciness = selected->GetBounciness();
            if (ImGui::SliderFloat("Bounciness", &bounciness, 0.0f, 1.0f)) {
                selected->SetBounciness(bounciness);
            }

            if (ImGui::Button("Reset Velocity", ImVec2(-1, 0))) {
                selected->SetVelocity(0.0f, 0.0f);
            }
            if (ImGui::Button("Duplicate", ImVec2(-1, 0))) {
                scene.DuplicateSelectedEntity();
            }
            if (ImGui::Button("Delete", ImVec2(-1, 0))) {
                scene.DeleteSelectedEntity();
            }
        } else {
            ImGui::TextDisabled("Select an entity in the Hierarchy or Game View.");
        }
    }
    ImGui::End();

    const float toolsY = kTopOffset + panelHeight * 0.62f + kPanelGap;
    const float toolsHeight = std::max(0.0f, displaySize.y - toolsY - kBottomMargin);
    ImGui::SetNextWindowPos(ImVec2(displaySize.x - kOuterMargin - panelWidth, toolsY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, toolsHeight), ImGuiCond_Always);
    if (ImGui::Begin("Scene & Tools", nullptr, panelFlags)) {
        ImGui::Text("Performance");
        ImGui::Text("FPS: %.1f", Time::GetFPS());
        ImGui::Text("Frame: %.2f ms", deltaTime * 1000.0f);

        ImGui::Separator();
        ImGui::Text("Camera: %.0f, %.0f",
            scene.GetCamera().GetX(), scene.GetCamera().GetY());
        static float bgColor[3] = {0.1f, 0.1f, 0.2f};
        if (ImGui::ColorEdit3("Background", bgColor)) {
            scene.SetBackgroundColor(bgColor[0], bgColor[1], bgColor[2]);
        }

        bool snap = scene.GetSnapToGrid();
        if (ImGui::Checkbox("Snap to Grid", &snap)) {
            scene.SetSnapToGrid(snap);
        }
        float gridSize = scene.GetGridSize();
        if (ImGui::DragFloat("Grid Size", &gridSize, 1.0f, 8.0f, 128.0f)) {
            scene.SetGridSize(gridSize);
        }

        ImGui::Separator();
        if (ImGui::Button("Save Scene", ImVec2(-1, 0))) {
            scene.SaveToFile("assets/scenes/scene1.txt");
        }
        if (ImGui::Button("Load Scene", ImVec2(-1, 0))) {
            scene.LoadFromFile("assets/scenes/scene1.txt");
        }

        ImGui::Separator();
        if (ImGui::Button("Play Click Sound", ImVec2(-1, 0))) {
            Audio::Get().PlaySound("click");
        }
        if (ImGui::Button("Play Music", ImVec2(-1, 0))) {
            Audio::Get().PlayMusic("bgm", true);
        }
        if (ImGui::Button("Stop Music", ImVec2(-1, 0))) {
            Audio::Get().StopMusic();
        }

        ImGui::Separator();
        ImGui::Text("Particles");
        if (ImGui::Button("Spark", ImVec2(-1, 0))) {
            scene.GetParticles().EmitPreset(ParticlePreset::Spark, 400.0f, 300.0f);
        }
        if (ImGui::Button("Fire", ImVec2(-1, 0))) {
            scene.GetParticles().EmitPreset(ParticlePreset::Fire, 400.0f, 300.0f);
        }
        if (ImGui::Button("Smoke", ImVec2(-1, 0))) {
            scene.GetParticles().EmitPreset(ParticlePreset::Smoke, 400.0f, 300.0f);
        }
        if (ImGui::Button("Explosion", ImVec2(-1, 0))) {
            scene.GetParticles().EmitPreset(ParticlePreset::Explosion, 400.0f, 300.0f);
        }
    }
    ImGui::End();
}
