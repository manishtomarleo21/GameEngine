#include "Engine/UI/EditorUI.h"
#include "Engine/Core/Application.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Tilemap/Tilemap.h"
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

void EditorUI::Draw(Scene& scene, float deltaTime, Application& app) {
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

    const float toolbarH = 34.0f;
    m_ViewportBounds = {
        kOuterMargin + panelWidth + kPanelGap,
        kTopOffset + toolbarH,
        viewportWidth,
        std::max(0.0f, panelHeight - toolbarH)
    };
    // ===== Toolbar: Play / Pause / Stop =====
    ImGui::SetNextWindowPos(ImVec2(m_ViewportBounds.x, kTopOffset),
    ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_ViewportBounds.width, toolbarH),
    ImGuiCond_Always);

    ImGuiWindowFlags toolbarFlags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoSavedSettings;

    if (ImGui::Begin("##Toolbar", nullptr, toolbarFlags)) {
        EditorMode mode = app.GetMode();

        // Play
        if (ImGui::Button("▶ Play")) {
            app.Play();
        }
        ImGui::SameLine();

        // Pause
        if (ImGui::Button(mode == EditorMode::Paused ? "▶ Resume" : "⏸ Pause")) {
            app.Pause();
        }
        ImGui::SameLine();

        // Stop
        if (ImGui::Button("⏹ Stop")) {
            app.Stop();
        }
        ImGui::SameLine(); 
        //   static int activeTool = 0; // 0 select, 1 pan, 2 paint
        // if (ImGui::Button("↖")) activeTool = 0; ImGui::SameLine(); // select
        // // if (ImGui::Button("✋")) activeTool = 1; ImGui::SameLine(); // pan
        // if (ImGui::Button("▦")) activeTool = 2; ImGui::SameLine(); // tile paint
        // if (ImGui::Button("＋")) { /* create later */ } ImGui::SameLine();
        // if (ImGui::Button("↺")) { scene.LoadFromFile("assets/scenes/scene1.txt"); } ImGui::SameLine();
        // if (ImGui::Button("⛶")) { /* fullscreen later */ } ImGui::SameLine();
        // if (ImGui::Button("🌐")) { /* world/grid later */ } ImGui::SameLine();
        // // if (ImGui::Button("📍")) { /* focus selected later */ } ImGui::SameLine();
        // if (ImGui::Button("⚙")) { /* settings later */ }

        if (ImGui::Button(app.GetTool() == EditorTool::Select ? "[Select]" : "Select")) {
            app.SetTool(EditorTool::Select);
        }
        ImGui::SameLine();
        if (ImGui::Button(app.GetTool() == EditorTool::Pan ? "[Pan]" : "Pan")) {
            app.SetTool(EditorTool::Pan);
        }
        ImGui::SameLine();
        if (ImGui::Button(app.GetTool() == EditorTool::Paint ? "[Paint]" : "Paint")) {
            app.SetTool(EditorTool::Paint);
        }

        ImGui::SameLine();
        ImGui::TextDisabled("  |  ");
        ImGui::SameLine();

        if (mode == EditorMode::Edit) {
            ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "Mode: Edit");
        } else if (mode == EditorMode::Play) {
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.5f, 1.0f), "Mode: Play");
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f), "Mode: Paused");
        }

    }
    ImGui::End();

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
    ImGui::SetNextWindowSize(ImVec2(panelWidth, panelHeight), ImGuiCond_Always);

    if (ImGui::Begin("Inspector", nullptr, panelFlags)) {
        if (ImGui::BeginTabBar("InspectorTabs")) {
            if (ImGui::BeginTabItem("Inspector")) {
                Entity* selected = scene.GetSelectedEntityPtr();

                if (!selected) {
                    ImGui::TextDisabled("Select an entity in the Hierarchy or Game View.");
                } else {
                    ImGui::Text("Entity %d", scene.GetSelectedEntity());
                    ImGui::Separator();

                    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                        const ImVec4 axisColors[] = {
                            ImVec4(0.9f, 0.3f, 0.3f, 1.0f),
                            ImVec4(0.3f, 0.8f, 0.35f, 1.0f),
                            ImVec4(0.35f, 0.45f, 0.95f, 1.0f)
                        };
                        const float labelWidth = 55.0f;
                        const float axisWidth = (ImGui::GetContentRegionAvail().x -
                            labelWidth - ImGui::GetStyle().ItemSpacing.x * 2.0f) / 3.0f;
                        const auto startAxisRow = [&](const char* label) {
                            const float rowStartX = ImGui::GetCursorPosX();
                            ImGui::Text("%s", label);
                            ImGui::SameLine();
                            ImGui::SetCursorPosX(rowStartX + labelWidth);
                        };
                        const auto drawAxis = [&](const char* row, int axis, float& value,
                                                  int colorIndex, bool disabled, bool first) {
                            if (!first) {
                                ImGui::SameLine();
                            }
                            ImGui::PushID(row);
                            ImGui::PushID(axis);
                            ImGui::BeginDisabled(disabled);
                            const ImVec2 markerPos = ImGui::GetCursorScreenPos();
                            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                                ImVec2(ImGui::GetStyle().FramePadding.x + 7.0f,
                                       ImGui::GetStyle().FramePadding.y));
                            ImGui::SetNextItemWidth(axisWidth);
                            const bool changed = ImGui::DragFloat("##value", &value, 0.1f, 0.0f, 0.0f, "%.2f");
                            ImGui::GetWindowDrawList()->AddRectFilled(
                                ImVec2(markerPos.x + 3.0f, markerPos.y + 3.0f),
                                ImVec2(markerPos.x + 6.0f, markerPos.y + ImGui::GetFrameHeight() - 3.0f),
                                ImGui::ColorConvertFloat4ToU32(axisColors[colorIndex]));
                            ImGui::PopStyleVar();
                            ImGui::EndDisabled();
                            ImGui::PopID();
                            ImGui::PopID();
                            return changed;
                        };

                        float positionX = selected->GetX();
                        float positionY = selected->GetY();
                        float positionZ = 0.0f;
                        startAxisRow("Position");
                        if (drawAxis("Position", 0, positionX, 0, false, true)) {
                            selected->SetPosition(positionX, positionY);
                        }
                        if (drawAxis("Position", 1, positionY, 1, false, false)) {
                            selected->SetPosition(positionX, positionY);
                        }
                        drawAxis("Position", 2, positionZ, 2, true, false);

                        float rotation = selected->GetRotation();
                        startAxisRow("Rotation");

                        // X/Y disabled placeholders, Z = actual 2D rotation
                        float rotX = 0.0f;
                        float rotY = 0.0f;
                        drawAxis("Rotation", 0, rotX, 0, true, true);
                        drawAxis("Rotation", 1, rotY, 1, true, false);

                        if (drawAxis("Rotation", 2, rotation, 2, false, false)) {
                            selected->SetRotation(rotation);
                        }

                        float scaleX = selected->GetW();
                        float scaleY = selected->GetH();
                        float scaleZ = 1.0f;
                        startAxisRow("Scale");
                        if (drawAxis("Scale", 0, scaleX, 0, false, true)) {
                            selected->SetSize(std::max(1.0f, scaleX), selected->GetH());
                        }
                        if (drawAxis("Scale", 1, scaleY, 1, false, false)) {
                            selected->SetSize(selected->GetW(), std::max(1.0f, scaleY));
                        }
                        drawAxis("Scale", 2, scaleZ, 2, true, false);

                        ImGui::TextDisabled("X/Y scale maps to 2D size in pixels.");
                        bool visible = selected->IsVisible();
                        if (ImGui::Checkbox("Visible", &visible)) {
                            selected->SetVisible(visible);
                        }
                       ImGui::BeginDisabled();
                        bool billboard = false;
                        ImGui::Checkbox("Billboard", &billboard);
                        ImGui::EndDisabled();
                        ImGui::TextDisabled("Billboard is for 3D cameras (not used in 2D).");
                    }

                    if (ImGui::CollapsingHeader("Mesh Component", ImGuiTreeNodeFlags_None)) {
                        ImGui::TextDisabled("3D mesh components are not implemented.");
                        ImGui::BeginDisabled();
                        ImGui::Button("Create Shape", ImVec2(-1, 0));
                        bool castShadows = false;
                        bool receiveShadows = false;
                        ImGui::Checkbox("Cast Shadows", &castShadows);
                        ImGui::Checkbox("Receive Shadows", &receiveShadows);
                        ImGui::Button("+ Add Submesh", ImVec2(-1, 0));
                        ImGui::Button("< Not defined >", ImVec2(-1, 0));
                        bool faceCulling = false;
                        bool textureShadow = true;
                        ImGui::Checkbox("Face Culling", &faceCulling);
                        ImGui::Checkbox("Texture Shadow", &textureShadow);
                        int primitive = 0;
                        const char* primitiveTypes[] = {"Triangles", "Lines", "Points"};
                        ImGui::Combo("Primitive", &primitive, primitiveTypes, 3);
                        float textureRect[] = {0.0f, 0.0f, 1.0f, 1.0f};
                        ImGui::DragFloat4("Texture Rect", textureRect, 0.01f);
                        ImGui::EndDisabled();
                    }

                    if (ImGui::CollapsingHeader("Sprite Component", ImGuiTreeNodeFlags_DefaultOpen)) {
                        ImGui::Text("Size: %.0f x %.0f px", selected->GetW(), selected->GetH());

                        int shape = selected->GetShape() == ShapeType::Circle ? 1 : 0;
                        const char* shapeNames[] = {"Rectangle", "Circle"};
                        if (ImGui::Combo("Shape", &shape, shapeNames, 2)) {
                            selected->SetShape(shape == 1 ? ShapeType::Circle : ShapeType::Rectangle);
                        }

                        float color[] = {
                            selected->GetColorR() / 255.0f,
                            selected->GetColorG() / 255.0f,
                            selected->GetColorB() / 255.0f
                        };
                        if (ImGui::ColorEdit3("Shape Color", color)) {
                            selected->SetColor(
                                static_cast<Uint8>(color[0] * 255.0f),
                                static_cast<Uint8>(color[1] * 255.0f),
                                static_cast<Uint8>(color[2] * 255.0f));
                        }

                        bool flipH = selected->GetFlipHorizontal();
                        if (ImGui::Checkbox("Flip X", &flipH)) {
                            selected->SetFlipHorizontal(flipH);
                        }
                        bool flipV = selected->GetFlipVertical();
                        if (ImGui::Checkbox("Flip Y", &flipV)) {
                            selected->SetFlipVertical(flipV);
                        }
                        ImGui::TextDisabled("Texture and animation assignment: code only");
                    }

                    if (ImGui::CollapsingHeader("Physics", ImGuiTreeNodeFlags_DefaultOpen)) {
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

                        ImGui::Text("Velocity: (%.1f, %.1f)", selected->GetVelX(), selected->GetVelY());
                        if (ImGui::Button("Reset Velocity", ImVec2(-1, 0))) {
                            selected->SetVelocity(0.0f, 0.0f);
                        }
                    }

                    if (ImGui::CollapsingHeader("Actions", ImGuiTreeNodeFlags_DefaultOpen)) {
                        if (ImGui::Button("Duplicate", ImVec2(-1, 0))) {
                            scene.DuplicateSelectedEntity();
                        }
                        if (ImGui::Button("Delete", ImVec2(-1, 0))) {
                            scene.DeleteSelectedEntity();
                        }
                    }
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Scene & Tools")) {
                ImGui::Text("Performance");
                // Smoothed display (updates ~4 times/sec)
                static float displayFps = 0.0f;
                static float displayMs  = 0.0f;
                static float fpsTimer   = 0.0f;

                fpsTimer += deltaTime;
                if (fpsTimer >= 0.25f) {          // update every 0.25 seconds
                    displayFps = Time::GetFPS();
                    displayMs  = deltaTime * 1000.0f;
                    fpsTimer   = 0.0f;
                }

                ImGui::Text("FPS: %.1f", displayFps);
                ImGui::Text("Frame: %.2f ms", displayMs);

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

                ImGui::Separator();
                ImGui::Text("Tilemap");

                Tilemap& tilemap = scene.GetTilemap();
                ImGui::Text("Map: %d x %d | Tile: %d px",
                    tilemap.GetWidth(), tilemap.GetHeight(), tilemap.GetTileSize());

                int paintId = scene.GetPaintTileId();
                ImGui::Text("Selected Tile ID: %d", paintId);

                if (ImGui::Button("Clear Tilemap", ImVec2(-1, 0))) {
                    tilemap.Clear(0);
                }

                // ===== Tileset Palette =====
                auto tileset = tilemap.GetTileset();
                if (tileset && tileset->GetSDLTexture()) {
                    ImGui::Separator();
                    ImGui::Text("Tileset Palette");

                    const float tileSize = static_cast<float>(tilemap.GetTileSize());
                    const int tilesPerRow = tilemap.GetTilesPerRow();

                    // How big each tile appears in the UI
                    const float previewScale = 1.0f;
                    const float previewTile = tileSize * previewScale;

                    // Texture size in pixels
                    const float texW = tileset->GetWidth();
                    const float texH = tileset->GetHeight();

                    ImVec2 imageSize(texW * previewScale, texH * previewScale);

                    ImVec2 cursor = ImGui::GetCursorScreenPos();
                    ImGui::Image((ImTextureID)(intptr_t)tileset->GetSDLTexture(), imageSize);

                    // Click to pick tile
                    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                        ImVec2 mouse = ImGui::GetMousePos();
                        float localX = mouse.x - cursor.x;
                        float localY = mouse.y - cursor.y;

                        int col = static_cast<int>(localX / previewTile);
                        int row = static_cast<int>(localY / previewTile);

                        if (col >= 0 && row >= 0 && col < tilesPerRow) {
                            int id = row * tilesPerRow + col + 1; // 0 = empty
                            scene.SetPaintTileId(id);
                        }
                    }

                    // Optional: highlight selected tile
                    if (paintId > 0) {
                        int idx = paintId - 1;
                        int col = idx % tilesPerRow;
                        int row = idx / tilesPerRow;

                        ImDrawList* draw = ImGui::GetWindowDrawList();
                        ImVec2 minPos(
                            cursor.x + col * previewTile,
                            cursor.y + row * previewTile
                        );
                        ImVec2 maxPos(
                            minPos.x + previewTile,
                            minPos.y + previewTile
                        );
                        draw->AddRect(minPos, maxPos, IM_COL32(255, 220, 50, 255), 0.0f, 0, 2.0f);
                    }
                } else {
                    ImGui::TextDisabled("No tileset loaded");
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}
