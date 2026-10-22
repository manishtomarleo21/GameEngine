#pragma once

#include <SDL3/SDL.h>

class GameViewport {
    public:
        GameViewport() = default;
        ~GameViewport();

        bool Create(SDL_Renderer* renderer, int width, int height);
        bool Resize(int width, int height);
        void Destroy();

        void Begin();   // Start rendering into the texture
        void End();     // Stop rendering into the texture

        SDL_Texture* GetTexture() const { return m_Texture; }
        int GetWidth()  const { return m_Width; }
        int GetHeight() const { return m_Height; }

    private:
        SDL_Renderer* m_Renderer = nullptr;
        SDL_Texture*  m_Texture  = nullptr;
        int m_Width  = 0;
        int m_Height = 0;
};