#include "Engine/Camera/Camera.h"

void Camera::SetPosition(float x, float y){
    m_X = x;
    m_Y = y;
}

void Camera::Move(float dx, float dy) {
    m_X += dx;
    m_Y += dy;
}

void Camera::Follow(float targetX, float targetY, float windowWidth, float windowHeight) {
    // Center the camera on the target
    m_X = targetX - windowWidth  / 2.0f;
    m_Y = targetY - windowHeight / 2.0f;
}