#include "pch.h"
#include "Block.h"

Block::Block(Vector2 position, Vector2 size, SpriteSheet* spriteSheet) : 
	Entity(position, size, spriteSheet)
{
}

Block::Block(Vector2 position, SpriteSheet* spriteSheet) :
	Entity(position, spriteSheet)
{
}

