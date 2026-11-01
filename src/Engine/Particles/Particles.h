#pragma once

#include <SDL3/SDL.h>

class Particle {
    public:
        float x = 0.0f;
        float y = 0.0f;
        float velX = 0.0f;
        float velY = 0.0f;
        float life = 0.0f; 
        float maxLife = 1.0f;
        float size = 4.0f;
        Uint8 r = 255, g = 255, b = 255, a = 255;
        bool active = false;
};