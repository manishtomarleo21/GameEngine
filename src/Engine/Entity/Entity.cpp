#include "Engine/Entity/Entity.h"
#include <algorithm>

Entity::Entity(float x, float y, float w, float h)
    : m_X(x), m_Y(y), m_W(w), m_H(h){

    }
void Entity::SetFrameTextures(const std::vector<std::shared_ptr<Texture>>& frames, float frameTime) {
    m_FrameTextures = frames;
    m_FrameTime = frameTime;
    m_FrameIndex = 0;
    m_FrameTimer = 0.0f;
    if (!m_FrameTextures.empty()) {
        m_Texture = m_FrameTextures[0];
    }
}

void Entity::PlayFrames(bool loop) {
    m_FramesPlaying = !m_FrameTextures.empty();
    m_FramesLoop = loop;
}

void Entity::StopFrames() {
    m_FramesPlaying = false;
    m_FrameIndex = 0;
    m_FrameTimer = 0.0f;
    if (!m_FrameTextures.empty()) {
        m_Texture = m_FrameTextures[0]; // idle = first frame
    }
}

void Entity::Update(float deltaTime){
    if (m_CurrentAnim) {
        m_CurrentAnim->Update(deltaTime);
        if (m_FramesPlaying && m_FrameTextures.size() > 1) {
            m_FrameTimer += deltaTime;
            if (m_FrameTimer >= m_FrameTime) {
                m_FrameTimer = 0.0f;
                m_FrameIndex++;
                if (m_FrameIndex >= (int)m_FrameTextures.size()) {
                    if (m_FramesLoop) m_FrameIndex = 0;
                    else {
                        m_FrameIndex = (int)m_FrameTextures.size() - 1;
                        m_FramesPlaying = false;
                    }
                }
                m_Texture = m_FrameTextures[m_FrameIndex];
                m_CurrentAnim = nullptr;
            }
        }
    }
    // Apply gravity
    const float GRAVITY =  980.0f;
    m_VelY += GRAVITY * m_GravityScale * deltaTime;

    //Apply Velocity
    m_X += m_VelX * deltaTime;
    m_Y += m_VelY * deltaTime;
}

void Entity::Renderer(SDL_Renderer* renderer){
    if (!m_Visible) return;
    SDL_FRect dest = { m_X, m_Y, m_W, m_H };

    if(m_Texture && m_Texture->GetSDLTexture()){
        dest.x += m_SpriteOffsetX;
        dest.y += m_SpriteOffsetY;
        SDL_FRect src;
        const SDL_FRect* srcPtr = nullptr;
        if (m_CurrentAnim) {
            const AnimationFrame* frame = m_CurrentAnim->GetCurrentFrame();
            if (frame) {
                src = { frame->u, frame->v, frame->w, frame->h };
                srcPtr = &src;
            }
        }
        SDL_FlipMode flip = SDL_FLIP_NONE;
        if (m_FlipH && m_FlipV) {
            flip = static_cast<SDL_FlipMode>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
        } else if (m_FlipH) {
            flip = SDL_FLIP_HORIZONTAL;
        } else if (m_FlipV) {
            flip = SDL_FLIP_VERTICAL;
        }
        SDL_FPoint center = { m_W * m_PivotX, m_H * m_PivotY };
        SDL_RenderTextureRotated(renderer, m_Texture->GetSDLTexture(), srcPtr, &dest, m_Rotation, &center, flip);
        return;
    }
    else{
        SDL_SetRenderDrawColor(renderer, m_ColorR, m_ColorG, m_ColorB, 255);
        if (m_Shape == ShapeType::Circle) {
            // Simple filled circle approximation
            float radius = m_W * 0.5f;
            float centerX = m_X + radius;
            float centerY = m_Y + radius;

            for (float y = -radius; y <= radius; y += 1.0f) {
                for (float x = -radius; x <= radius; x += 1.0f) {
                    if (x*x + y*y <= radius*radius) {
                        SDL_RenderPoint(renderer, centerX + x, centerY + y);
                    }
                }
            }
        } 
        else {
            // Rectangle (default)
            SDL_RenderFillRect(renderer, &dest);
        }
    }
}

void Entity::SetPosition(float x, float y){
    m_X = x;
    m_Y = y;
}

void Entity::SetSize(float w, float h) {
    m_W = w;
    m_H = h;
}

void Entity::SetColor(Uint8 r, Uint8 g, Uint8 b) {
    m_ColorR = r;
    m_ColorG = g;
    m_ColorB = b;
}

void Entity::SetVelocity(float vx, float vy) {
    m_VelX = vx;
    m_VelY = vy;
}

void Entity::SetTexture(std::shared_ptr<Texture> texture) {
    m_Texture = texture;
    if (m_Texture) {
        // Automatically match entity size to the texture
        // m_W = m_Texture->GetWidth();
        // m_H = m_Texture->GetHeight();
    }
}

void Entity::AddVelocity(float vx, float vy) {
    m_VelX += vx;
    m_VelY += vy;
}

void Entity::SetGravityScale(float scale) {
    m_GravityScale = scale;
}

void Entity::SetBounciness(float value) {
    m_Bounciness = value;
}

void Entity::SetFriction(float friction) {
    m_Friction = friction;
}

void Entity::SetShape(ShapeType shape) {
    m_Shape = shape;
}

void Entity::AddAnimation(const std::string& name, Animation anim) {
    m_Animations[name] = std::move(anim);
}

void Entity::PlayAnimation(const std::string& name) {
    auto it = m_Animations.find(name);
    if (it == m_Animations.end()) return;

    m_CurrentAnim = &it->second;
    m_CurrentAnim->Reset();

    // Use animation texture
    if (m_CurrentAnim->GetTexture()) {
        m_Texture = m_CurrentAnim->GetTexture();
    }
}

void Entity::StopAnimation() {
    m_CurrentAnim = nullptr;
}

void Entity::PauseAnimation(bool pause) {
    if (m_CurrentAnim) {
        m_CurrentAnim->SetPaused(pause);
        if (pause) {
            m_CurrentAnim->Reset();          // go to first frame
            m_CurrentAnim->SetPaused(true);  // keep paused after reset
        }
    }
}

void Entity::SetRotation(float degrees) {
    m_Rotation = degrees;
}

void Entity::SetPivot(float x, float y) {
    m_PivotX = std::clamp(x, 0.0f, 1.0f);
    m_PivotY = std::clamp(y, 0.0f, 1.0f);
}

int Entity::GetAnimationFrameCount() const {
    if (!m_CurrentAnim) return 0;
    return m_CurrentAnim->GetFrameCount();
}

int Entity::GetCurrentFrameIndex() const {
    if (!m_CurrentAnim) return 0;
    return m_CurrentAnim->GetCurrentFrameIndex();
}

void Entity::SetCurrentFrameIndex(int index) {
    if (!m_CurrentAnim) return;
    m_CurrentAnim->SetCurrentFrameIndex(index);
}

bool Entity::AddAnimationFrame(float u, float v, float w, float h, float duration) {
    if (!m_CurrentAnim) return false;
    m_CurrentAnim->AddFrame(u, v, w, h, duration);
    return true;
}

void Entity::SetCollider(float offsetX, float offsetY, float colW, float colH) {
    m_AutoColliderFromImage = false;
    m_ColOffsetX = offsetX;
    m_ColOffsetY = offsetY;
    m_ColW = colW;
    m_ColH = colH;
    m_UseCustomCollider = true;
}

void Entity::SetAutoColliderFromImage(bool enabled) {
    if (m_AutoColliderFromImage && !enabled) {
        float x, y, w, h;
        GetColliderRect(x, y, w, h);
        m_ColOffsetX = x - m_X;
        m_ColOffsetY = y - m_Y;
        m_ColW = w;
        m_ColH = h;
    }
    m_AutoColliderFromImage = enabled;
}

void Entity::GetColliderRect(float& outX, float& outY, float& outW, float& outH) const {
    if (m_UseCustomCollider && m_AutoColliderFromImage && m_Texture) {
        float x, y, w, h;
        if (m_Texture->GetOpaqueBounds(x, y, w, h)) {
            outX = m_X + m_SpriteOffsetX + x * m_W;
            outY = m_Y + m_SpriteOffsetY + y * m_H;
            outW = w * m_W;
            outH = h * m_H;
            return;
        }
    }

    if (m_UseCustomCollider && m_ColW > 0.0f && m_ColH > 0.0f) {
        outX = m_X + m_ColOffsetX;
        outY = m_Y + m_ColOffsetY;
        outW = m_ColW;
        outH = m_ColH;
    } else {
        outX = m_X;
        outY = m_Y;
        outW = m_W;
        outH = m_H;
    }
}