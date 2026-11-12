#pragma once

#include <SDL3/SDL.h>
#include "Engine/Renderer/Texture.h"
#include "Engine/Animation/Animation.h"
#include <unordered_map>
#include <memory>

enum class ShapeType {
    Rectangle,
    Circle
};

class Entity {
    public:
        Entity(float x, float y, float w, float h);
        virtual ~Entity() = default;

        virtual void Update(float deltaTime);
        virtual void Renderer(SDL_Renderer* renderer);

        float GetX() const { return m_X; }
        float GetY() const { return m_Y; }
        float GetW() const { return m_W; }
        float GetH() const { return m_H; }

        void SetPosition(float x, float y);
        void SetSize(float w, float h);
        void SetColor(Uint8 r, Uint8 g, Uint8 b);
        void SetVelocity(float vx, float vy);

        void SetTexture(std::shared_ptr<Texture> texture);

        void AddVelocity(float vx, float vy);
        void SetGravityScale(float scale);     // 1.0 = normal gravity, 0.0 = no gravity
        void SetBounciness(float value);        // 0.0 = no bounce, 1.0 = perfect bounce
        
        float GetVelX() const { return m_VelX; }
        float GetVelY() const { return m_VelY; }
        float GetBounciness() const { return m_Bounciness; }
        float GetGravityScale() const { return m_GravityScale; } 

        void SetFriction(float friction);          // 0.0 = no friction, 1.0 = full stop
        float GetFriction() const { return m_Friction; }

        void SetShape(ShapeType shape);
        ShapeType GetShape() const { return m_Shape; }

        Uint8 GetColorR() const { return m_ColorR; }
        Uint8 GetColorG() const { return m_ColorG; }
        Uint8 GetColorB() const { return m_ColorB; }

        void AddAnimation(const std::string& name, Animation anim);
        void PlayAnimation(const std::string& name);
        void StopAnimation();
        bool HasAnimation() const { return m_CurrentAnim != nullptr; }
        void SetFlipHorizontal(bool flip) { m_FlipH = flip; }
        bool GetFlipHorizontal() const { return m_FlipH; }
        void PauseAnimation(bool pause);

        void SetRotation(float degrees);
        float GetRotation() const { return m_Rotation; }
        void SetVisible(bool visible) { m_Visible = visible; }
        bool IsVisible() const { return m_Visible; }
        void SetFlipVertical(bool flip) { m_FlipV = flip; }
        bool GetFlipVertical() const { return m_FlipV; }

        void SetPivot(float x, float y);   // 0..1 range
        float GetPivotX() const { return m_PivotX; }
        float GetPivotY() const { return m_PivotY; }

        Animation* GetCurrentAnimation() { return m_CurrentAnim; }
        int GetAnimationFrameCount() const;
        int GetCurrentFrameIndex() const;
        void SetCurrentFrameIndex(int index);
        bool AddAnimationFrame(float u, float v, float w, float h, float duration);

    private:
        float m_X, m_Y;
        float m_W, m_H;
        float m_VelX = 0.0f;
        float m_VelY = 0.0f;
        float m_GravityScale = 0.0f;   // 0 by default (so existing entities don't fall)
        float m_Bounciness = 0.5f;  
        float m_Friction = 0.0f;
        float m_Rotation = 0.0f; //degree
        float m_PivotX = 0.5f;  // center by default
        float m_PivotY = 0.5f;
        bool m_FlipH = false;
        bool m_Visible = true;
        bool m_FlipV = false;

        Uint8 m_ColorR = 255;
        Uint8 m_ColorG = 255;
        Uint8 m_ColorB = 255;

        std::shared_ptr<Texture> m_Texture;
        ShapeType m_Shape = ShapeType::Rectangle;
        
        std::unordered_map<std::string, Animation> m_Animations;
        Animation* m_CurrentAnim = nullptr;
};