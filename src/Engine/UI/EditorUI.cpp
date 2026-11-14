#include "Engine/UI/EditorUI.h"
#include "Engine/Core/Application.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Tilemap/Tilemap.h"
#include "Engine/Particles/ParticleSystem.h"
#include "Engine/Resources/TextureManager.h"
#include "imgui.h"

#include <filesystem>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <string>
#include <cctype>

namespace {
constexpr float kPanelGap = 8.0f;
constexpr float kOuterMargin = 10.0f;
constexpr float kTopOffset = 28.0f;
constexpr float kBottomMargin =  34.0f; //10.0f;
constexpr float kMinimumViewportWidth = 320.0f;
constexpr float kMinimumPanelWidth = 170.0f;
constexpr float kMaximumPanelWidth = 280.0f;
}
namespace fs = std::filesystem;

static const char* GetEntityIcon(const Entity* e) {
    if (!e) return "?";

    // Player by name or index 0 if you prefer
    if (e->GetName() == "Player") return "P";

    if (e->GetTexture()) return "S";          // Sprite
    if (e->GetShape() == ShapeType::Circle) return "C";
    return "R";                               // Rectangle
}

static ImVec4 GetEntityIconColor(const Entity* e) {
    if (!e) return ImVec4(0.6f, 0.6f, 0.6f, 1.0f);

    if (e->GetName() == "Player")
        return ImVec4(0.35f, 0.75f, 1.0f, 1.0f); // blue
    if (e->GetTexture())
        return ImVec4(0.45f, 0.90f, 0.55f, 1.0f); // green sprite
    if (e->GetShape() == ShapeType::Circle)
        return ImVec4(0.95f, 0.75f, 0.30f, 1.0f); // yellow circle
    return ImVec4(0.95f, 0.45f, 0.45f, 1.0f);     // red rect
}

static void DrawResourceGrid(
    const std::string& dir,
    const std::vector<std::string>& files,
    SDL_Renderer* renderer,
    Scene& scene,
    Entity* selected,
    bool isTextureFolder
) {
    if (files.empty()) {
        ImGui::TextDisabled("Empty");
        return;
    }

    const float thumb = 64.0f;
    const float padding = 8.0f;
    const float cell = thumb + padding;
    const float panelWidth = ImGui::GetContentRegionAvail().x;
    const int columns = std::max(1, static_cast<int>(panelWidth / cell));
    if (!ImGui::BeginTable(
            "ResourceGrid",
            columns,
            ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoSavedSettings)) {
        return;
    }

    for (int column = 0; column < columns; ++column) {
        const std::string columnID = "##Resource" + std::to_string(column);
        ImGui::TableSetupColumn(columnID.c_str(), ImGuiTableColumnFlags_WidthFixed, cell);
    }

    for (const auto& name : files) {
        ImGui::TableNextColumn();
        ImGui::PushID(name.c_str());

        ImVec2 cursor = ImGui::GetCursorScreenPos();

        bool clicked = false;

        if (isTextureFolder) {
            std::string path = dir + "/" + name;
            auto tex = TextureManager::Get().Load(renderer, path);

            if (tex && tex->GetSDLTexture()) {
                ImGui::Image(
                    (ImTextureID)(intptr_t)tex->GetSDLTexture(),
                    ImVec2(thumb, thumb)
                );
                clicked = ImGui::IsItemClicked();

                // hover border
                if (ImGui::IsItemHovered()) {
                    ImGui::GetWindowDrawList()->AddRect(
                        cursor,
                        ImVec2(cursor.x + thumb, cursor.y + thumb),
                        IM_COL32(255, 220, 50, 255),
                        0.0f, 0, 2.0f
                    );
                }
            } else {
                // fallback tile
                ImGui::Dummy(ImVec2(thumb, thumb));
                ImGui::GetWindowDrawList()->AddRectFilled(
                    cursor,
                    ImVec2(cursor.x + thumb, cursor.y + thumb),
                    IM_COL32(50, 50, 55, 255)
                );
            }
        } else {
            // non-texture badge
            ImGui::Dummy(ImVec2(thumb, thumb));
            ImDrawList* dl = ImGui::GetWindowDrawList();
            dl->AddRectFilled(cursor, ImVec2(cursor.x + thumb, cursor.y + thumb), IM_COL32(45, 48, 58, 255));
            dl->AddRect(cursor, ImVec2(cursor.x + thumb, cursor.y + thumb), IM_COL32(90, 90, 110, 255));

            // extension label
            std::string ext = fs::path(name).extension().string();
            ImVec2 ts = ImGui::CalcTextSize(ext.c_str());
            dl->AddText(
                ImVec2(cursor.x + (thumb - ts.x) * 0.5f, cursor.y + (thumb - ts.y) * 0.5f),
                IM_COL32(180, 180, 200, 255),
                ext.c_str()
            );
            clicked = ImGui::IsItemClicked();
        }

        // filename under thumbnail
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x);
        ImGui::TextUnformatted(name.c_str());
        ImGui::PopTextWrapPos();

        // click actions
        if (clicked) {
            if (isTextureFolder && selected) {
                std::string path = dir + "/" + name;
                auto tex = TextureManager::Get().Load(renderer, path);
                if (tex) {
                    selected->StopAnimation();
                    selected->SetTexture(tex);
                    selected->SetSize(64, 64);
                }
            } else if (!isTextureFolder) {
                // scenes folder: load on click
                if (dir.find("scenes") != std::string::npos) {
                    scene.LoadFromFile(dir + "/" + name);
                }
            }
        }

        // drag source for textures
        if (isTextureFolder && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("ASSET_TEXTURE", name.c_str(), name.size() + 1);
            ImGui::Text("%s", name.c_str());
            ImGui::EndDragDropSource();
        }

        ImGui::PopID();
    }

    ImGui::EndTable();
}

static std::vector<std::string> ListFiles(const std::string& dir, const std::vector<std::string>& exts) {
    std::vector<std::string> result;

    std::error_code ec;
    if (!fs::exists(dir, ec)) return result;

    for (const auto& entry : fs::directory_iterator(dir, ec)) {
        if (ec) break;
        if (!entry.is_regular_file()) continue;

        std::string filename = entry.path().filename().string();
        std::string ext = entry.path().extension().string();

        // lowercase extension check
        for (char& c : ext) c = static_cast<char>(tolower(c));

        for (const auto& allowed : exts) {
            if (ext == allowed) {
                result.push_back(filename);
                break;
            }
        }
    }

    std::sort(result.begin(), result.end());
    return result;
}

static std::string ResolveAssetDir(const std::string& relativeDir) {
    if (fs::exists(relativeDir)) return relativeDir;
    std::string alt = std::string("../") + relativeDir;
    if (fs::exists(alt)) return alt;
    return relativeDir;
}

void EditorUI::Draw(Scene& scene, float deltaTime, Application& app, SDL_Renderer* renderer) {
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
            ImGui::PushID(static_cast<int>(i));
            const char* icon = GetEntityIcon(entity);
            ImVec4 iconCol = GetEntityIconColor(entity);

            ImGui::PushStyleColor(ImGuiCol_Text, iconCol);
            ImGui::Text("%s", icon);
            ImGui::PopStyleColor();

            ImGui::SameLine();

            char label[128];
            std::snprintf(label, sizeof(label), "%s##%zu",  entity->GetName().c_str(), i);
            bool selected = (scene.GetSelectedEntity() == static_cast<int>(i));
            if (ImGui::Selectable(label, selected)) {
                scene.SetSelectedEntity(static_cast<int>(i));
            }

            // Drop texture onto this entity
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE")) {
                    const char* filename = static_cast<const char*>(payload->Data);

                    std::string texDir = ResolveAssetDir("assets/textures");
                    std::string path = texDir + "/" + filename;

                    auto tex = TextureManager::Get().Load(renderer, path);
                    if (tex) {
                        entity->StopAnimation();
                        entity->SetTexture(tex);
                        entity->SetSize(64, 64);
                        scene.SetSelectedEntity(static_cast<int>(i));
                    }
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::PopID();
        }

        if (ImGui::Button("+ Rect")) {
        scene.CreateRectangleEntity(300, 200, 80, 80, 0.9f, 0.3f, 0.3f);
        }
        ImGui::SameLine();
        if (ImGui::Button("+ Circle")) {
            scene.CreateCircleEntity(300, 200, 40, 0.3f, 0.8f, 0.9f);
        }

    ImGui::Separator();
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(kOuterMargin, bottomY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, bottomHeight), ImGuiCond_Always);

    if (ImGui::Begin("Resources", nullptr, panelFlags)) {
        ImGui::TextDisabled("Project Assets");
        ImGui::Separator();

        Entity* selected = scene.GetSelectedEntityPtr();

        std::string texDir = ResolveAssetDir("assets/textures");
        if (ImGui::TreeNodeEx("textures", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto files = ListFiles(texDir, {".png", ".jpg", ".jpeg", ".bmp"});
            DrawResourceGrid(texDir, files, renderer, scene, selected, true);
            ImGui::TreePop();
        }

        std::string sceneDir = ResolveAssetDir("assets/scenes");
        if (ImGui::TreeNode("scenes")) {
            auto files = ListFiles(sceneDir, {".txt", ".scene"});
            DrawResourceGrid(sceneDir, files, renderer, scene, selected, false);
            ImGui::TreePop();
        }

        std::string audioDir = ResolveAssetDir("assets/audio");
        if (ImGui::TreeNode("audio")) {
            auto files = ListFiles(audioDir, {".wav", ".ogg", ".mp3"});
            DrawResourceGrid(audioDir, files, renderer, scene, selected, false);
            ImGui::TreePop();
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
                    char nameBuf[64];
                    std::snprintf(nameBuf, sizeof(nameBuf), "%s", selected->GetName().c_str());

                    if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) {
                        selected->SetName(nameBuf);
                    }
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

                        ImGui::Separator();
                        ImGui::Text("Pivot");

                        float pivot[2] = { selected->GetPivotX(), selected->GetPivotY() };
                        if (ImGui::DragFloat2("Pivot", pivot, 0.01f, 0.0f, 1.0f)) {
                            selected->SetPivot(pivot[0], pivot[1]);
                        }

                        // Quick presets
                        if (ImGui::Button("Top-Left"))    selected->SetPivot(0.0f, 0.0f);
                        ImGui::SameLine();
                        if (ImGui::Button("Center"))      selected->SetPivot(0.5f, 0.5f);
                        ImGui::SameLine();
                        if (ImGui::Button("Bottom"))      selected->SetPivot(0.5f, 1.0f);

                        bool flipH = selected->GetFlipHorizontal();
                        if (ImGui::Checkbox("Flip X", &flipH)) {
                            selected->SetFlipHorizontal(flipH);
                        }
                        bool flipV = selected->GetFlipVertical();
                        if (ImGui::Checkbox("Flip Y", &flipV)) {
                            selected->SetFlipVertical(flipV);
                        }

                        if (selected->GetTexture()) {
                            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.5f, 1.0f), "Texture: assigned");
                        } else {
                            ImGui::TextDisabled("Texture: none");
                        }

                        ImGui::Separator();
                        ImGui::Text("Sprite Frames");

                        Animation* anim = selected->GetCurrentAnimation();
                        if (!anim) {
                            ImGui::TextDisabled("No animation on this entity.");
                            ImGui::TextDisabled("Assign/play an animation from code first.");
                        } else {
                            int frameCount = selected->GetAnimationFrameCount();
                            int current = selected->GetCurrentFrameIndex();

                            // Frames dropdown
                            if (frameCount > 0) {
                                // Build labels: Frame 0, Frame 1, ...
                                if (ImGui::BeginCombo("Frames", ("Frame " + std::to_string(current)).c_str())) {
                                    for (int i = 0; i < frameCount; ++i) {
                                        bool isSelected = (i == current);
                                        std::string label = "Frame " + std::to_string(i);
                                        if (ImGui::Selectable(label.c_str(), isSelected)) {
                                            selected->SetCurrentFrameIndex(i);
                                            selected->PauseAnimation(true); // hold on chosen frame
                                        }
                                        if (isSelected) ImGui::SetItemDefaultFocus();
                                    }
                                    ImGui::EndCombo();
                                }
                            } else {
                                ImGui::TextDisabled("No frames yet");
                            }

                            ImGui::Spacing();

                            // + Add Frame
                            static float newU = 0.0f, newV = 0.0f, newW = 32.0f, newH = 32.0f, newDur = 0.12f;
                            if (ImGui::TreeNode("Add Frame Settings")) {
                                ImGui::DragFloat("U", &newU, 1.0f, 0.0f, 10000.0f);
                                ImGui::DragFloat("V", &newV, 1.0f, 0.0f, 10000.0f);
                                ImGui::DragFloat("W", &newW, 1.0f, 1.0f, 10000.0f);
                                ImGui::DragFloat("H", &newH, 1.0f, 1.0f, 10000.0f);
                                ImGui::DragFloat("Duration", &newDur, 0.01f, 0.01f, 2.0f);
                                ImGui::TreePop();
                            }

                            if (ImGui::Button("+ Add Frame", ImVec2(-1, 0))) {
                                selected->AddAnimationFrame(newU, newV, newW, newH, newDur);
                            }

                            // Simple Slicer Tool
                            if (ImGui::Button("Slicer Tool", ImVec2(-1, 0))) {
                                ImGui::OpenPopup("SlicerToolPopup");
                            }

                            if (ImGui::BeginPopupModal("SlicerToolPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                                static int sliceCols = 4;
                                static int sliceRows = 2;
                                static float sliceW = 1344.0f;
                                static float sliceH = 1792.0f;
                                static float sliceDur = 0.10f;

                                ImGui::Text("Auto-slice a grid into frames");
                                ImGui::DragInt("Columns", &sliceCols, 1, 1, 64);
                                ImGui::DragInt("Rows", &sliceRows, 1, 1, 64);
                                ImGui::DragFloat("Frame W", &sliceW, 1.0f, 1.0f, 10000.0f);
                                ImGui::DragFloat("Frame H", &sliceH, 1.0f, 1.0f, 10000.0f);
                                ImGui::DragFloat("Duration", &sliceDur, 0.01f, 0.01f, 2.0f);

                                if (ImGui::Button("Generate Frames", ImVec2(160, 0))) {
                                    for (int row = 0; row < sliceRows; ++row) {
                                        for (int col = 0; col < sliceCols; ++col) {
                                            float u = col * sliceW;
                                            float v = row * sliceH;
                                            selected->AddAnimationFrame(u, v, sliceW, sliceH, sliceDur);
                                        }
                                    }
                                    ImGui::CloseCurrentPopup();
                                }
                                ImGui::SameLine();
                                if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                                    ImGui::CloseCurrentPopup();
                                }
                                ImGui::EndPopup();
                            }

                            ImGui::TextDisabled("Frames: %d", frameCount);
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

    // ===== Bottom Status Bar =====
    const float statusH = 28.0f;
    ImGui::SetNextWindowPos(ImVec2(0.0f, displaySize.y - statusH), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(displaySize.x, statusH), ImGuiCond_Always);

    ImGuiWindowFlags statusFlags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoNav;

    if (ImGui::Begin("##StatusBar", nullptr, statusFlags)) {
        // Left side
        ImGui::TextColored(ImVec4(0.45f, 0.90f, 0.55f, 1.0f), "Ready");
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();

        ImGui::Text("Scene: scene1");
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();

        int selectedIndex = scene.GetSelectedEntity();
        if (selectedIndex >= 0) {
            Entity* e = scene.GetSelectedEntityPtr();
            if (e) {
                ImGui::Text("Selected: %s", e->GetName().c_str());
            } else {
                ImGui::Text("Selected: Entity %d", selectedIndex);
            }
        } else {
            ImGui::TextDisabled("Selected: None");
        }

        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();

        // Mode
        EditorMode mode = app.GetMode();
        if (mode == EditorMode::Play) {
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.5f, 1.0f), "Playing");
        } else if (mode == EditorMode::Paused) {
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f), "Paused");
        } else {
            ImGui::Text("Edit");
        }

        // Right side: FPS
        static float displayFps = 0.0f;
        static float displayMs = 0.0f;
        static float fpsTimer = 0.0f;
        fpsTimer += deltaTime;
        if (fpsTimer >= 0.25f) {
            displayFps = Time::GetFPS();
            displayMs = deltaTime * 1000.0f;
            fpsTimer = 0.0f;
        }

        char rightText[64];
        std::snprintf(rightText, sizeof(rightText), "FPS: %.1f | %.2f ms", displayFps, displayMs);

        float rightWidth = ImGui::CalcTextSize(rightText).x;
        ImGui::SameLine(ImGui::GetWindowWidth() - rightWidth - 12.0f);
        ImGui::TextUnformatted(rightText);
    }
    ImGui::End();
}
