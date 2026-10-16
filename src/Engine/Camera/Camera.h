#pragma once

class Camera {
    public:
        Camera() = default;

        void SetPosition(float x, float y);
        void Move(float dx, float dy);

        float GetX() const { return m_X; }
        float GetY() const { return m_Y; }

        // Make the camera follow a target (usually the player)
        void Follow(float targetX, float targetY, float windowWidth, float windowHeight);

    private:
        float m_X = 0.0f;
        float m_Y = 0.0f;
};