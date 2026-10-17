#include "Engine/UI/ImGuiLayer.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

ImGuiLayer::~ImGuiLayer(){
    ShutDown();
}

bool ImGuiLayer::Init(SDL_Window* window, SDL_Renderer* renderer){
    if (m_Initialized) return true;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    m_Initialized = true;
    return true;
}

void ImGuiLayer::ShutDown(){
    if(!m_Initialized) return;

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    m_Initialized = false;
}

void ImGuiLayer::ProcessEvent(const SDL_Event& event){
    if(!m_Initialized) return;
    ImGui_ImplSDL3_ProcessEvent(&event);
}

void ImGuiLayer::BeginFrame() {
    if (!m_Initialized) return;

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::EndFrame() {
    if (!m_Initialized) return;
    ImGui::Render();
}

void ImGuiLayer::Renderer(SDL_Renderer* renderer) {
    if (!m_Initialized) return;
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}
