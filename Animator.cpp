#include "pch.h"
#include "Debug.h"
#include "Animator.h"

void Animator::SetSpriteSheet(SpriteSheet* spriteSheet)
{
	m_spriteSheet = spriteSheet;
}

void Animator::DefineAnimation(const int& name, const std::vector<const wchar_t*>& frameNames, bool loop, float baseTimePerFrame, bool useVelocityScaling, float minTimePerFrame, float maxTimePerFrame, float velocityScaleFactor)
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
<<<<<<< HEAD
		//std::wstring fname = std::wstring(frameName);
		//std::string fstr = std::string(fname.begin(), fname.end());
		// Log(__FUNCTION__, "Finding frame: " + fstr); // Updated to use 'fstr' instead of 'name.c_str()'
=======
		std::wstring name = std::wstring(frameName);
		std::string str = std::string(name.begin(), name.end());
		Log(__FUNCTION__, "Finding frame: " + str); // Updated to use 'str' instead of 'name.c_str()'
>>>>>>> nhan
		auto frame = m_spriteSheet->Find(frameName);
		if (frame)
		{
			sequence.frames.push_back(frame);
			//Log(__FUNCTION__, "Found frame:" + str);
		}
		else {
			// Log(__FUNCTION__, "Frame not found");
		}
	}

	// Save the animation
	m_animations[name] = sequence;

<<<<<<< HEAD
	// Log(__FUNCTION__, "Loaded animation: " + std::to_string(name));

=======
	Log(__FUNCTION__, "Loaded animation: " + std::to_string(name));
>>>>>>> nhan
}

// Set the current animation and optionally reset it
void Animator::SetAnimation(const int& id, bool reset)
{

	// Don't change if it's the same animation unless reset is requested
	if (m_currentSequence == id && !reset)
	{
		return;
	}

	// Find the animation in the unordered map
	auto it = m_animations.find(id);


<<<<<<< HEAD
	m_currentSequence = id;
	//Log(__FUNCTION__, "Animation set: " + std::to_string(id));
	//Log(__FUNCTION__, "Animation frames: " + std::to_string(m_animations[id].frames.size()));
=======
		m_currentSequence = id;
		// Log(__FUNCTION__, "Animation set: " + std::to_string(id));
		// Log(__FUNCTION__, "Animation frames: " + std::to_string(m_animations[id].frames.size()));
	
>>>>>>> nhan



	if (reset)
	{
		m_currentFrame = 0;
		m_totalElapsed = 0.f;
		m_paused = false;
	}
}

// Update the animation based on elapsed time and current velocity
void Animator::Update(float elapsed, float velocity)
{
	if (m_paused || m_currentSequence < 0)
		return;

	auto it = m_animations.find(m_currentSequence);
	if (it == m_animations.end() || it->second.frames.empty())
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
	m_totalElapsed += elapsed;

	// Time to advance to next frame?
	if (m_totalElapsed >= timePerFrame)
	{
		m_currentFrame++;
		m_totalElapsed -= timePerFrame;

		// Handle end of animation
		if (m_currentFrame >= sequence.frames.size())
		{
			if (sequence.loop)
			{
				m_currentFrame = 0;
			}
			else
			{
				m_currentFrame = static_cast<int>(sequence.frames.size()) - 1;
				m_paused = true;
			}
		}
	}
}

void Animator::Draw(DirectX::SpriteBatch* batch, const DirectX::XMFLOAT2& position, const float& depth)
{
	if (m_currentSequence < 0 || !m_spriteSheet)

		return;

	auto it = m_animations.find(m_currentSequence);

	if (it->second.frames.empty())
		// Log(__FUNCTION__, "Animation frames empty for: " + std::to_string(m_currentSequence));

		if (it == m_animations.end() || it->second.frames.empty())
			return;

	const AnimationSequence& sequence = it->second;

	//Log(__FUNCTION__, "drawing current frame ");
	if (m_currentFrame < 0 || m_currentFrame >= sequence.frames.size())
		return;

	// Get the current frame to draw
	const SpriteSheet::SpriteFrame* frame = sequence.frames[m_currentFrame];

	// Draw the sprite with current effects (flipping)
	m_spriteSheet->Draw(batch, *frame, position, DirectX::Colors::White,
		m_rotation, m_scale, m_spriteEffects, depth == 1.0f ? m_depth : depth);
	//  Log(LOG_INFO, "Drawing frame: " + std::to_string(m_currentFrame) + " of animation: " + std::to_string(m_currentSequence) + " at position: " + std::to_string(position.x) + ", " + std::to_string(position.y));
}

/// <summary>
/// Draws an animation frame from a sprite sheet at a specified position (default = 0).
/// </summary>
/// <param name="batch">A pointer to the DirectX::SpriteBatch used for rendering.</param>
/// <param name="id">The identifier of the animation sequence to draw.</param>
/// <param name="position">The position on the screen where the animation frame should be drawn.</param>
void Animator::Draw(DirectX::SpriteBatch* batch, const int& id, const DirectX::XMFLOAT2& position, const float& depth, const float& scale)
{
	if (!m_spriteSheet)
		return;

	auto it = m_animations.find(id);

	if (it == m_animations.end() || it->second.frames.empty())
		return;

	const AnimationSequence& sequence = it->second;
	const SpriteSheet::SpriteFrame* frame = sequence.frames[0];

	m_spriteSheet->Draw(batch, *frame, position, DirectX::Colors::White,
		m_rotation, scale, m_spriteEffects, depth);
}


void Animator::SetFlipHorizontal(bool flip)
{
	if (flip)
		m_spriteEffects = static_cast<DirectX::SpriteEffects>(m_spriteEffects | DirectX::SpriteEffects_FlipHorizontally);
	else
		m_spriteEffects = static_cast<DirectX::SpriteEffects>(m_spriteEffects & ~DirectX::SpriteEffects_FlipHorizontally);
}

void Animator::SetFlipVertical(bool flip)
{
	if (flip)
		m_spriteEffects = static_cast<DirectX::SpriteEffects>(m_spriteEffects | DirectX::SpriteEffects_FlipVertically);
	else
		m_spriteEffects = static_cast<DirectX::SpriteEffects>(m_spriteEffects & ~DirectX::SpriteEffects_FlipVertically);
}

bool Animator::IsFlippedHorizontally() const
{
	return (m_spriteEffects & DirectX::SpriteEffects_FlipHorizontally) != 0;
}

bool Animator::IsFlippedVertically() const
{
	return (m_spriteEffects & DirectX::SpriteEffects_FlipVertically) != 0;
}

// Allow direct access to sprite effects for advanced usage
void Animator::SetSpriteEffects(DirectX::SpriteEffects effects)
{
	m_spriteEffects = effects;
}

DirectX::SpriteEffects Animator::GetSpriteEffects() const
{
	return m_spriteEffects;
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
	m_currentFrame = 0;
	m_totalElapsed = 0.f;
}

void Animator::Stop()
{
	m_paused = true;
	m_currentFrame = 0;
	m_totalElapsed = 0.f;
}

void Animator::Play() { m_paused = false; }

void Animator::Pause() { m_paused = true; }

void Animator::SetScale(float scale) { m_scale = { scale, scale }; }

void Animator::SetScale(float scaleX, float scaleY) { m_scale = { scaleX, scaleY }; }

void Animator::SetRotation(float rotation) { m_rotation = rotation; }

// Status methods
bool Animator::IsPaused() const { return m_paused; }

int Animator::GetCurrentAnimation() const { return m_currentSequence; }

int Animator::GetCurrentFrame() const { return m_currentFrame; }

bool Animator::IsFinished() const
{
	if (m_currentSequence < 0)
		return true;

	auto it = m_animations.find(m_currentSequence);
	if (it == m_animations.end())
		return true;

	return !it->second.loop && m_currentFrame == it->second.frames.size() - 1;
}

const std::unordered_map<int, AnimationSequence>& Animator::GetAnimations() const
{
	return m_animations;
}

bool Animator::HasAnimation(const int& id) const
{
	return m_animations.find(id) != m_animations.end();
}