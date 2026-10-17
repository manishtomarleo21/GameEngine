#pragma once

#include <SDL3/SDL.h>

class ImGuiLayer {
    public:
        ImGuiLayer() = default;
        ~ImGuiLayer();

        bool Init(SDL_Window* window, SDL_Renderer* renderer);
        void ShutDown();

        void BeginFrame();
        void EndFrame();
        void Renderer(SDL_Renderer* renderer);
        
        // Call this for every SDL event
        void ProcessEvent(const SDL_Event& event);

    private:
        bool m_Initialized = false;
};