#pragma once

#include "Engine/Renderer/Texture.h"
#include <memory>
#include <string>
#include <vector>

struct AnimationFrame
{
    float u, v;       // top-left of frame in texture (pixels)
    float w, h;       // frame size
    float duration;   // seconds to show this frame
};

class Animation
{
    public:
        Animation() = default;
        Animation(const std::string& name, std::shared_ptr<Texture> texture);
        
        void AddFrame(float u, float v, float w, float h, float duration);
        void SetLoop(bool loop) { m_Loop = loop; }

        void Update(float deltaTime);
        void Reset();
        void SetPaused(bool paused) { m_Paused = paused; }
        bool IsPaused() const { return m_Paused; }

        const AnimationFrame* GetCurrentFrame() const;
        std::shared_ptr<Texture> GetTexture() const { return m_Texture; }
        const std::string& GetName() const { return m_Name; }
        bool IsFinished() const { return m_Finished; }

    private:
        std::string m_Name;
        std::shared_ptr<Texture> m_Texture;
        std::vector<AnimationFrame> m_Frames;

        int   m_CurrentFrame = 0;
        float m_Timer = 0.0f;
        bool  m_Loop = true;
        bool  m_Finished = false;
        bool m_Paused = false;
};


