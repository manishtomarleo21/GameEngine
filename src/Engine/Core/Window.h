#pragma once

#include <SDL3/SDL.h>
#include <string>

class Window {
    public:
        Window(const std::string& title, int width, int height);
        ~Window();

        bool ShouldClose() const { return m_ShouldClose; }
        void PollEvents();
        void Clear(float r, float g, float b);
        void Present();
        void SwapBuffers();
        void GetSize(int& width, int& height) const;

        SDL_Window* GetNativeWindow() const { return m_Window; }
        SDL_Renderer* GetRenderer() const { return m_Renderer; } 
    private:
        SDL_Window* m_Window = nullptr;
        SDL_Renderer* m_Renderer = nullptr;
        bool m_ShouldClose = false;
};