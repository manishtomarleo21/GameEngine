#pragma once

#include <SDL3/SDL.h>
#include <string>

class Texture {
    public:
        Texture() = default;
        ~Texture();

        bool LoadfromFile(SDL_Renderer* renderer, const std::string& path);
        void Destroy();

        SDL_Texture* GetSDLTexture() const { return m_Texture; };
        float GetWidth()  const { return m_Width; }
        float GetHeight() const { return m_Height; }
        const std::string& GetPath() const { return m_Path; }
        bool GetOpaqueBounds(float& x, float& y, float& w, float& h) const;

    private:
        SDL_Texture* m_Texture = nullptr;
        float m_Width  = 0.0f;
        float m_Height = 0.0f;
        std::string m_Path;
        float m_OpaqueX = 0.0f;
        float m_OpaqueY = 0.0f;
        float m_OpaqueW = 1.0f;
        float m_OpaqueH = 1.0f;
        bool m_HasOpaqueBounds = false;
};