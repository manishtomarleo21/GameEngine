#include "Engine/Particles/ParticleSystem.h"
#include <cstdlib>
#include <algorithm>
#include <cmath>

static float RandomFloat(float minV, float maxV) {
    return minV + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxV - minV);
}

ParticleSystem::ParticleSystem(size_t maxParticles) {
    m_Particles.resize(maxParticles);
}

void ParticleSystem::Emit(float x, float y, int count,
                          float minSpeed, float maxSpeed,
                          float life,
                          Uint8 r, Uint8 g, Uint8 b,
                          float size) {
    int spawned = 0;

    for (auto& p : m_Particles) {
        if (spawned >= count) break;
        if (p.active) continue;

        float angle = RandomFloat(0.0f, 6.28318f); // 0 to 2π
        float speed = RandomFloat(minSpeed, maxSpeed);

        p.x = x;
        p.y = y;
        p.velX = std::cos(angle) * speed;
        p.velY = std::sin(angle) * speed;
        p.life = life;
        p.maxLife = life;
        p.size = size;
        p.r = r;
        p.g = g;
        p.b = b;
        p.a = 255;
        p.active = true;

        spawned++;
    }
}

void ParticleSystem::Update(float deltaTime) {
    for (auto& p : m_Particles) {
        if (!p.active) continue;

        p.x += p.velX * deltaTime;
        p.y += p.velY * deltaTime;
        p.life -= deltaTime;

        // Fade out
        float t = std::max(p.life / p.maxLife, 0.0f);
        p.a = static_cast<Uint8>(t * 255.0f);

        if (p.life <= 0.0f) {
            p.active = false;
        }
    }
}

void ParticleSystem::Render(SDL_Renderer* renderer, float cameraX, float cameraY) {
    for (const auto& p : m_Particles) {
        if (!p.active) continue;

        SDL_FRect rect = {
            p.x - cameraX - p.size * 0.5f,
            p.y - cameraY - p.size * 0.5f,
            p.size,
            p.size
        };

        SDL_SetRenderDrawColor(renderer, p.r, p.g, p.b, p.a);
        SDL_RenderFillRect(renderer, &rect);
    }
}

void ParticleSystem::Clear() {
    for (auto& p : m_Particles) {
        p.active = false;
    }
}

void ParticleSystem::EmitPreset(ParticlePreset preset, float x, float y) {
    switch (preset) {
        case ParticlePreset::Spark:
            // Fast bright yellow/white sparks
            Emit(x, y, 20, 120.0f, 280.0f, 0.4f, 255, 230, 80, 3.0f);
            Emit(x, y, 10, 80.0f, 160.0f, 0.3f, 255, 255, 255, 2.0f);
            break;

        case ParticlePreset::Fire:
            // Upward orange/red flames
            for (int i = 0; i < 25; ++i) {
                // Find free particle manually for upward bias
                for (auto& p : m_Particles) {
                    if (p.active) continue;

                    float angle = RandomFloat(-0.6f, 0.6f) - 1.57f; // mostly upward
                    float speed = RandomFloat(40.0f, 120.0f);

                    p.x = x + RandomFloat(-8.0f, 8.0f);
                    p.y = y;
                    p.velX = std::cos(angle) * speed;
                    p.velY = std::sin(angle) * speed;
                    p.life = RandomFloat(0.4f, 0.9f);
                    p.maxLife = p.life;
                    p.size = RandomFloat(4.0f, 8.0f);
                    p.r = 255;
                    p.g = static_cast<Uint8>(RandomFloat(80, 160));
                    p.b = 20;
                    p.a = 255;
                    p.active = true;
                    break;
                }
            }
            break;

        case ParticlePreset::Smoke:
            // Slow gray rising smoke
            for (int i = 0; i < 15; ++i) {
                for (auto& p : m_Particles) {
                    if (p.active) continue;

                    float angle = RandomFloat(-0.5f, 0.5f) - 1.57f;
                    float speed = RandomFloat(20.0f, 60.0f);

                    p.x = x + RandomFloat(-10.0f, 10.0f);
                    p.y = y;
                    p.velX = std::cos(angle) * speed;
                    p.velY = std::sin(angle) * speed;
                    p.life = RandomFloat(1.0f, 1.8f);
                    p.maxLife = p.life;
                    p.size = RandomFloat(8.0f, 16.0f);
                    Uint8 gray = static_cast<Uint8>(RandomFloat(80, 140));
                    p.r = gray;
                    p.g = gray;
                    p.b = gray;
                    p.a = 180;
                    p.active = true;
                    break;
                }
            }
            break;

        case ParticlePreset::Explosion:
            // Big burst + fire colors
            Emit(x, y, 40, 100.0f, 320.0f, 0.7f, 255, 120, 30, 6.0f);
            Emit(x, y, 20, 60.0f, 180.0f, 0.9f, 255, 200, 50, 4.0f);
            Emit(x, y, 15, 40.0f, 100.0f, 1.1f, 120, 120, 120, 8.0f); // smoke-ish
            break;
    }
}