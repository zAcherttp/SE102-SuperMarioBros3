#pragma once

#include <map>
#include <string>
#include <memory>
#include <vector>
#include <SpriteBatch.h>
#include "SpriteSheet.h"

struct AnimationSequence
{
    std::vector<const SpriteSheet::SpriteFrame*> frames;
    bool loop;
    float baseTimePerFrame;
    float minTimePerFrame;
    float maxTimePerFrame;
    bool useVelocityScaling;
    float velocityScaleFactor;
};

class Animator
{
public:

    Animator() noexcept :
        mCurrentSequence(""),
        mCurrentFrame(0),
        mTotalElapsed(0.f),
        mPaused(false),
        mEffects(DirectX::SpriteEffects_None),
        mDepth(0.f),
        mRotation(0.f),
        mScale(1.f, 1.f)
    {
    }

    void LoadSpriteSheet(ID3D11ShaderResourceView* texture, const wchar_t* sheetDataFile);

    void DefineAnimation(const std::string& name,
        const std::vector<const wchar_t*>& frameNames,
        bool loop = true,
        float baseTimePerFrame = 0.1f,
        bool useVelocityScaling = false,
        float minTimePerFrame = 0.03f,
        float maxTimePerFrame = 0.2f,
        float velocityScaleFactor = 1.0f);

    // Set the current animation and optionally reset it
    void SetAnimation(const std::string& name, bool reset = true);

    // Update the animation based on elapsed time and current velocity
    void Update(float elapsed, float velocity = 0.0f);

    // Draw the current animation frame
    void Draw(DirectX::SpriteBatch* batch, const DirectX::XMFLOAT2& position);

    // Direct access to sprite effects for flipping
    void SetFlipHorizontal(bool flip);

    void SetFlipVertical(bool flip);

    bool IsFlippedHorizontally() const;

    bool IsFlippedVertically() const;

    // Allow direct access to sprite effects for advanced usage
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

    // Status methods
    bool IsPaused() const;
    std::string GetCurrentAnimation() const;
    int GetCurrentFrame() const;
    bool IsFinished() const;

    const std::map<std::string, AnimationSequence>& GetAnimations() const;

    bool HasAnimation(const std::string& name) const;


private:
    std::unique_ptr<SpriteSheet> mSpriteSheet;
    std::map<std::string, AnimationSequence> mAnimations;

    std::string mCurrentSequence;
    int mCurrentFrame;
    float mTotalElapsed;
    bool mPaused;
    DirectX::SpriteEffects mEffects;
    float mDepth;
    float mRotation;
    DirectX::XMFLOAT2 mScale;
    DirectX::FXMVECTOR mColor = DirectX::Colors::White;
};

