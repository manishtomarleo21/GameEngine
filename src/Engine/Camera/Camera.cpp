#include "Engine/Camera/Camera.h"

void Camera::SetPosition(float x, float y){
    m_X = x;
    m_Y = y;
}

void Camera::Move(float dx, float dy) {
    m_X += dx;
    m_Y += dy;
}

void Camera::Follow(float targetX, float targetY, float windowWidth, float windowHeight, float deltaTime) {
    // Desired camera position (center on target)
    float desiredX = targetX - windowWidth  / 2.0f;
    float desiredY = targetY - windowHeight / 2.0f;

    // Smoothly move towards the desired position (lerp)
    m_X += (desiredX - m_X) * m_SmoothSpeed * deltaTime;
    m_Y += (desiredY - m_Y) * m_SmoothSpeed * deltaTime;
}