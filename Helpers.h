#pragma once
#include <unordered_set>

class Helpers {
public:
	static std::unordered_set<wchar_t> GetDefinedSymbols(const DirectX::SpriteFont* font);
	static void DisplayDefinedSymbols(const DirectX::SpriteFont* font);

};
