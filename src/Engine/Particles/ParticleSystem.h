#pragma once

#include "Engine/Particles/Particles.h"
#include <vector>

enum class ParticlePreset {
    Spark,
    Fire,
    Smoke,
    Explosion
};

class ParticleSystem {
public:
    ParticleSystem(size_t maxParticles = 500);

    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer, float cameraX, float cameraY);

    // Emit a burst of particles
    void Emit(float x, float y, int count,
              float minSpeed, float maxSpeed,
              float life,
              Uint8 r, Uint8 g, Uint8 b,
              float size = 4.0f);
    void EmitPreset(ParticlePreset preset, float x, float y);

    void Clear();

private:
    std::vector<Particle> m_Particles;
};