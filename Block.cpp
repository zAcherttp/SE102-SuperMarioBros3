#include "pch.h"
#include "Block.h"

Block::Block(Vector2 position, Vector2 size, SpriteSheet* spriteSheet) : 
	Entity(position, size, spriteSheet), m_isSolid(false), m_isCollectible(false), m_isClaimed(false)
{
}

Block::Block(Vector2 position, SpriteSheet* spriteSheet) :
	Entity(position, spriteSheet), m_isSolid(false), m_isCollectible(false), m_isClaimed(false)
{
}

void Block::SetSolid(bool solid) { m_isSolid = solid; }

bool Block::IsSolid() const { return m_isSolid; }

bool Block::IsCollectible() const
{
	return m_isCollectible;
}

void Block::SetIsClaimed(bool claimed)
{
	m_isClaimed = claimed;
}

void Block::Collect()
{
}

void Block::Bump()
{
	// bump
}

void Block::Break()
{
}

