#include "Engine/Animation/Animation.h"

Animation::Animation(const std::string& name, std::shared_ptr<Texture> texture)
    : m_Name(name), m_Texture(texture) {
}

void Animation::AddFrame(float u, float v, float w, float h, float duration) {
    m_Frames.push_back({ u, v, w, h, duration });
}

void Animation::Update(float deltaTime) {
    if (m_Frames.empty() || m_Finished || m_Paused) return;

    m_Timer += deltaTime;

    if (m_Timer >= m_Frames[m_CurrentFrame].duration) {
        m_Timer = 0.0f;
        m_CurrentFrame++;

        if (m_CurrentFrame >= static_cast<int>(m_Frames.size())) {
            if (m_Loop) {
                m_CurrentFrame = 0;
            } else {
                m_CurrentFrame = static_cast<int>(m_Frames.size()) - 1;
                m_Finished = true;
            }
        }
    }
}

void Animation::Reset() {
    m_CurrentFrame = 0;
    m_Timer = 0.0f;
    m_Finished = false;
    m_Paused = false;
}

const AnimationFrame* Animation::GetCurrentFrame() const {
    if (m_Frames.empty()) return nullptr;
    return &m_Frames[m_CurrentFrame];
}

void Animation::SetCurrentFrameIndex(int index) {
    if (m_Frames.empty()) return;
    if (index < 0) index = 0;
    if (index >= static_cast<int>(m_Frames.size())) {
        index = static_cast<int>(m_Frames.size()) - 1;
    }
    m_CurrentFrame = index;
    m_Timer = 0.0f;
}

const AnimationFrame* Animation::GetFrame(int index) const {
    if (index < 0 || index >= static_cast<int>(m_Frames.size())) return nullptr;
    return &m_Frames[index];
}