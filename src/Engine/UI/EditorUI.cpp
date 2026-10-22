#include "Engine/UI/EditorUI.h"
#include "Engine/Audio/Audio.h"
#include "imgui.h"

void EditorUI::Draw(Scene& scene, float deltaTime) {

    // ========== MAIN MENU BAR ==========
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene"))  { /* future */ }
            if (ImGui::MenuItem("Open Scene")) { /* future */ }
            if (ImGui::MenuItem("Save Scene")) { /* future */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                // We can handle exit later
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Duplicate", "Ctrl+D")) { /* future */ }
            if (ImGui::MenuItem("Delete", "Del"))       { /* future */ }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Hierarchy", nullptr, true);
            ImGui::MenuItem("Inspector", nullptr, true);
            ImGui::MenuItem("Performance", nullptr, true);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Create")) {
            if (ImGui::MenuItem("Rectangle")) { /* wire this later */ }
            if (ImGui::MenuItem("Circle"))    { /* wire this later */ }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::SetNextWindowPos(ImVec2(940, 40), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(360, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin("Game Engine Editor", nullptr, ImGuiWindowFlags_NoCollapse);

    // ========== HEADER ==========
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Engine Editor");
    ImGui::Separator();

    // ========== PERFORMANCE ==========
    if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("FPS: %.1f", Time::GetFPS());
        ImGui::Text("Delta: %.3f ms", deltaTime * 1000.0f);
    }

    // ========== CAMERA ==========
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Position: (%.1f, %.1f)",
            scene.GetCamera().GetX(),
            scene.GetCamera().GetY());

        static float bgColor[3] = { 0.1f, 0.1f, 0.2f };
        if (ImGui::ColorEdit3("Background", bgColor)) {
            scene.SetBackgroundColor(bgColor[0], bgColor[1], bgColor[2]);
        }
    }

    // ========== CREATE ==========
    if (ImGui::CollapsingHeader("Create", ImGuiTreeNodeFlags_DefaultOpen)) {
        static float newPos[2]  = { 300.0f, 200.0f };
        static float newSize[2] = { 80.0f, 80.0f };
        static float newColor[3] = { 0.9f, 0.3f, 0.3f };

        ImGui::DragFloat2("Position", newPos, 1.0f);
        ImGui::DragFloat2("Size", newSize, 1.0f);
        ImGui::ColorEdit3("Color", newColor);

        if (ImGui::Button("Create Rectangle", ImVec2(-1, 0))) {
            scene.CreateRectangleEntity(newPos[0], newPos[1], newSize[0], newSize[1],
                                        newColor[0], newColor[1], newColor[2]);
        }

        if (ImGui::Button("Create Circle", ImVec2(-1, 0))) {
            float radius = newSize[0] * 0.5f;
            scene.CreateCircleEntity(newPos[0], newPos[1], radius,
                                     newColor[0], newColor[1], newColor[2]);
        }
    }

    // ========== ENTITIES ==========
    if (ImGui::CollapsingHeader("Entities", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Total: %zu", scene.GetEntityCount());
        ImGui::Separator();

        for (size_t i = 0; i < scene.GetEntityCount(); ++i) {
            Entity* e = scene.GetEntity(i);
            if (!e) continue;

            bool isSelected = (scene.GetSelectedEntity() == static_cast<int>(i));

            char label[64];
            snprintf(label, sizeof(label), "Entity %zu  (%.0f, %.0f)", i, e->GetX(), e->GetY());

            if (ImGui::Selectable(label, isSelected)) {
                scene.SetSelectedEntity(static_cast<int>(i));
            }
        }
    }

    // ========== SELECTED ENTITY ==========
    if (ImGui::CollapsingHeader("Selected Entity", ImGuiTreeNodeFlags_DefaultOpen)) {
        Entity* selected = scene.GetSelectedEntityPtr();

        if (selected) {
            float pos[2]  = { selected->GetX(), selected->GetY() };
            float size[2] = { selected->GetW(), selected->GetH() };

            if (ImGui::DragFloat2("Position", pos, 1.0f)) {
                selected->SetPosition(pos[0], pos[1]);
            }
            if (ImGui::DragFloat2("Size", size, 1.0f)) {
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

            ImGui::Spacing();

            if (ImGui::Button("Duplicate", ImVec2(-1, 0))) {
                scene.DuplicateSelectedEntity();
            }

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.75f, 0.15f, 0.15f, 1.0f));
            if (ImGui::Button("Delete", ImVec2(-1, 0))) {
                scene.DeleteSelectedEntity();
            }
            ImGui::PopStyleColor();
        } else {
            ImGui::TextDisabled("No entity selected");
        }
    }

    //Audio
    if (ImGui::CollapsingHeader("Audio", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("Play Click Sound", ImVec2(-1, 0))) {
            Audio::Get().PlaySound("click");
        }
        if (ImGui::Button("Play Music", ImVec2(-1, 0))) {
            Audio::Get().PlayMusic("bgm", true);
        }
        if (ImGui::Button("Stop Music", ImVec2(-1, 0))) {
            Audio::Get().StopMusic();
        }
    }

    // ========== UPCOMING FEATURES ==========
    if (ImGui::CollapsingHeader("Upcoming Features")) {
        ImGui::BeginDisabled();
        ImGui::Button("Audio System", ImVec2(-1, 0));
        ImGui::Button("Particle System", ImVec2(-1, 0));
        ImGui::Button("Tilemap Editor", ImVec2(-1, 0));
        ImGui::Button("Save / Load Scene", ImVec2(-1, 0));
        ImGui::Button("Animation System", ImVec2(-1, 0));
        ImGui::EndDisabled();

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "These features are planned next.");
    }

    ImGui::End();
}