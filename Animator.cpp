#include "pch.h"
#include "Animator.h"

void Animator::LoadSpriteSheet(ID3D11ShaderResourceView* texture, const wchar_t* sheetDataFile)
{
	mSpriteSheet = std::make_unique<SpriteSheet>();
	mSpriteSheet->Load(texture, sheetDataFile);
}

void Animator::DefineAnimation(const std::string& name, const std::vector<const wchar_t*>& frameNames, bool loop, float baseTimePerFrame, bool useVelocityScaling, float minTimePerFrame, float maxTimePerFrame, float velocityScaleFactor)
{
	AnimationSequence sequence;
	sequence.loop = loop;
	sequence.baseTimePerFrame = baseTimePerFrame;
	sequence.minTimePerFrame = minTimePerFrame;
	sequence.maxTimePerFrame = maxTimePerFrame;
	sequence.useVelocityScaling = useVelocityScaling;
	sequence.velocityScaleFactor = velocityScaleFactor;

	// Load all frames from the sprite sheet
	for (auto frameName : frameNames)
	{
		auto frame = mSpriteSheet->Find(frameName);
		if (frame)
		{
			sequence.frames.push_back(frame);
		}
	}

	// Save the animation
	mAnimations[name] = sequence;
}

// Set the current animation and optionally reset it
void Animator::SetAnimation(const std::string& name, bool reset)
{
	// Don't change if it's the same animation unless reset is requested
	if (mCurrentSequence == name && !reset)
		return;

	auto it = mAnimations.find(name);
	if (it != mAnimations.end())
	{
		mCurrentSequence = name;

		if (reset)
		{
			mCurrentFrame = 0;
			mTotalElapsed = 0.f;
		}
	}
}

// Update the animation based on elapsed time and current velocity
void Animator::Update(float elapsed, float velocity)
{
	if (mPaused || mCurrentSequence.empty())
		return;

	auto it = mAnimations.find(mCurrentSequence);
	if (it == mAnimations.end() || it->second.frames.empty())
		return;

	const AnimationSequence& sequence = it->second;

	// Calculate the current time per frame based on velocity
	float timePerFrame = sequence.baseTimePerFrame;

	if (sequence.useVelocityScaling && velocity != 0.0f)
	{
		// The faster the entity moves, the faster the animation
		float absVelocity = std::abs(velocity);
		timePerFrame = sequence.baseTimePerFrame / (absVelocity * sequence.velocityScaleFactor);

		// Clamp to min/max frame rate
		timePerFrame = std::max(sequence.minTimePerFrame,
			std::min(timePerFrame, sequence.maxTimePerFrame));
	}

	// Update animation timing
	mTotalElapsed += elapsed;

	// Time to advance to next frame?
	if (mTotalElapsed >= timePerFrame)
	{
		mCurrentFrame++;
		mTotalElapsed -= timePerFrame;

		// Handle end of animation
		if (mCurrentFrame >= sequence.frames.size())
		{
			if (sequence.loop)
			{
				mCurrentFrame = 0;
			}
			else
			{
				mCurrentFrame = static_cast<int>(sequence.frames.size()) - 1;
				mPaused = true;
			}
		}
	}
}

void Animator::Draw(DirectX::SpriteBatch* batch, const DirectX::XMFLOAT2& position)
{
	if (mCurrentSequence.empty())
		return;

	auto it = mAnimations.find(mCurrentSequence);
	if (it == mAnimations.end() || it->second.frames.empty())
		return;

	const AnimationSequence& sequence = it->second;

	if (mCurrentFrame < 0 || mCurrentFrame >= sequence.frames.size())
		return;

	// Get the current frame to draw
	const SpriteSheet::SpriteFrame* frame = sequence.frames[mCurrentFrame];

	// Draw the sprite with current effects (flipping)
	mSpriteSheet->Draw(batch, *frame, position, DirectX::Colors::White,
		mRotation, mScale, mEffects, mDepth);
}

void Animator::SetFlipHorizontal(bool flip)
{
	if (flip)
		mEffects = static_cast<DirectX::SpriteEffects>(mEffects | DirectX::SpriteEffects_FlipHorizontally);
	else
		mEffects = static_cast<DirectX::SpriteEffects>(mEffects & ~DirectX::SpriteEffects_FlipHorizontally);
}

void Animator::SetFlipVertical(bool flip)
{
	if (flip)
		mEffects = static_cast<DirectX::SpriteEffects>(mEffects | DirectX::SpriteEffects_FlipVertically);
	else
		mEffects = static_cast<DirectX::SpriteEffects>(mEffects & ~DirectX::SpriteEffects_FlipVertically);
}

bool Animator::IsFlippedHorizontally() const
{
	return (mEffects & DirectX::SpriteEffects_FlipHorizontally) != 0;
}

bool Animator::IsFlippedVertically() const
{
	return (mEffects & DirectX::SpriteEffects_FlipVertically) != 0;
}

// Allow direct access to sprite effects for advanced usage
void Animator::SetSpriteEffects(DirectX::SpriteEffects effects)
{
	mEffects = effects;
}

DirectX::SpriteEffects Animator::GetSpriteEffects() const
{
	return mEffects;
}

// Direction helper - common use case for side-scrollers
void Animator::SetDirection(int direction)
{
	// direction < 0 means facing left, which typically requires horizontal flipping
	SetFlipHorizontal(direction < 0);
}

// Control methods
void Animator::Reset()
{
	mCurrentFrame = 0;
	mTotalElapsed = 0.f;
}

void Animator::Stop()
{
	mPaused = true;
	mCurrentFrame = 0;
	mTotalElapsed = 0.f;
}

void Animator::Play() { mPaused = false; }

void Animator::Pause() { mPaused = true; }

void Animator::SetScale(float scale) { mScale = { scale, scale }; }

void Animator::SetScale(float scaleX, float scaleY) { mScale = { scaleX, scaleY }; }

void Animator::SetRotation(float rotation) { mRotation = rotation; }

// Status methods
bool Animator::IsPaused() const { return mPaused; }

std::string Animator::GetCurrentAnimation() const { return mCurrentSequence; }

int Animator::GetCurrentFrame() const { return mCurrentFrame; }

bool Animator::IsFinished() const
{
	if (mCurrentSequence.empty())
		return true;

	auto it = mAnimations.find(mCurrentSequence);
	if (it == mAnimations.end())
		return true;

	return !it->second.loop && mCurrentFrame == it->second.frames.size() - 1;
}

const std::map<std::string, AnimationSequence>& Animator::GetAnimations() const
{
	return mAnimations;
}

bool Animator::HasAnimation(const std::string& name) const
{
	return mAnimations.find(name) != mAnimations.end();
}