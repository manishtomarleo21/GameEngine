#include "Engine/Camera/Camera.h"
#include <iostream>

void Camera::SetPosition(float x, float y){
    m_X = x;
    m_Y = y;
}

void Camera::Move(float dx, float dy) {
    m_X += dx;
    m_Y += dy;
}

void Camera::SetBounds(float minX, float minY, float maxX, float maxY) {
    m_MinX = minX;
    m_MinY = minY;
    m_MaxX = maxX;
    m_MaxY = maxY;
}

void Camera::ClampToBounds(float windowWidth, float windowHeight) {
    if (!m_BoundsEnabled) return;

    // Camera position is top-left of the view
    float maxCamX = m_MaxX - windowWidth;
    float maxCamY = m_MaxY - windowHeight;

    m_X = std::clamp(m_X, m_MinX, maxCamX);
    m_Y = std::clamp(m_Y, m_MinY, maxCamY);
}

void Camera::Follow(float targetX, float targetY, float windowWidth, float windowHeight, float deltaTime) {
    // Desired camera position (center on target)
    float desiredX = targetX - windowWidth  / 2.0f;
    float desiredY = targetY - windowHeight / 2.0f;

    // Smoothly move towards the desired position (lerp)
    m_X += (desiredX - m_X) * m_SmoothSpeed * deltaTime;
    m_Y += (desiredY - m_Y) * m_SmoothSpeed * deltaTime;

    ClampToBounds(windowWidth, windowHeight);
}