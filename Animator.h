#pragma once

#include <map>
#include <string>
#include <memory>
#include <vector>
#include <SpriteBatch.h>
#include "SpriteSheet.h"

struct AnimationSequence {
    std::vector<const SpriteSheet::SpriteFrame*> frames;
    bool loop = true;                  
    float baseTimePerFrame = 0.1f; 
    float minTimePerFrame = 0.05f;
    float maxTimePerFrame = 0.2f;         
    bool useVelocityScaling = false;
    float velocityScaleFactor = 1.0f;     
};

class Animator {
public:
    Animator() = default;

    ~Animator() {
        if (m_ownsSpriteSheet && m_spriteSheet) {
            delete m_spriteSheet;
        }
    }

    void SetSpriteSheet(SpriteSheet* spriteSheet);

    void DefineAnimation(const int& name, const std::vector<const wchar_t*>& frameNames,
        bool loop = true, float baseTimePerFrame = 0.1f,
        bool useVelocityScaling = false, float minTimePerFrame = 0.05f,
        float maxTimePerFrame = 0.2f, float velocityScaleFactor = 1.0f);

    void SetAnimation(const int& id, bool reset = false);
    void Update(float elapsed, float velocity = 0.0f);
    void Draw(DirectX::SpriteBatch* batch, const DirectX::XMFLOAT2& position, const float& depth = 0.0f);
    void Draw(DirectX::SpriteBatch* batch, const int& id , const DirectX::XMFLOAT2& position, const float& depth = 0.5f);
    void Draw(DirectX::SpriteBatch* batch, const int& id, const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size);

    void SetFlipHorizontal(bool flip);
    void SetFlipVertical(bool flip);
    bool IsFlippedHorizontally() const;
    bool IsFlippedVertically() const;
    void SetSpriteEffects(DirectX::SpriteEffects effects);
    DirectX::SpriteEffects GetSpriteEffects() const;

    void SetDirection(int direction);

    // Control methods
    void Reset();
    void Stop();
    void Play();
    void Pause();
    void SetScale(float scale);
    void SetScale(float scaleX, float scaleY);
    void SetRotation(float rotation);
    void SetDepth(float depth) { m_depth = depth; }
    float GetDepth() const { return m_depth; }

    // Status methods
    bool IsPaused() const;
    int GetCurrentAnimation() const;
    int GetCurrentFrame() const;
    bool IsFinished() const;
    const std::unordered_map<int, AnimationSequence>& GetAnimations() const;
    bool HasAnimation(const int& id) const;

    float GetTimePerFrame();

private:
    SpriteSheet* m_spriteSheet = nullptr;
    bool m_ownsSpriteSheet = false;  

    std::unordered_map<int, AnimationSequence> m_animations;
    int m_currentSequence = -1;
    int m_currentFrame = 0;
    float m_totalElapsed = 0.0f;
    bool m_paused = false;

    DirectX::SpriteEffects m_spriteEffects = DirectX::SpriteEffects_None;
    DirectX::XMFLOAT2 m_scale = { 1.0f, 1.0f };
    float m_rotation = 0.0f;
    float m_depth = 0.5f;
};
