#include "pch.h"
#include "Ground.h"

Ground::Ground(Vector2 position, Vector2 size, SpriteSheet* spriteSheet) :
	Block(position, size, spriteSheet)
{
}

void Ground::Render(DirectX::SpriteBatch* spriteBatch)
{
	//using size, render first block then repeating middle parts, then end part
}
