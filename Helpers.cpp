#include "pch.h"
#include "Helpers.h"


/// <summary>
/// Retrieves a set of defined symbols (characters) supported by the given DirectX SpriteFont.
/// </summary>
/// <param name="font">A pointer to the DirectX::SpriteFont object to check for supported characters.</param>
/// <returns>An unordered set of wide characters (wchar_t) representing the symbols supported by the font.</returns>
std::unordered_set<wchar_t> Helpers::GetDefinedSymbols(const DirectX::SpriteFont* font)
{
    std::unordered_set<wchar_t> definedSymbols;
    // Check for standard ASCII characters (32-126)
    for (wchar_t c = 32; c <= 126; c++)
    {
        if (font->ContainsCharacter(c))
        {
            definedSymbols.insert(c);
        }
    }
    // Check for extended characters if needed
    // This range covers most Latin, Greek, Cyrillic, and common symbols
    for (wchar_t c = 128; c <= 0x2000; c++)
    {
        if (font->ContainsCharacter(c))
        {
            definedSymbols.insert(c);
        }
    }
    return definedSymbols;
}

/// <summary>
/// Displays the defined symbols in a given DirectX SpriteFont object.
/// </summary>
/// <param name="font">A pointer to the DirectX::SpriteFont object whose defined symbols will be displayed.</param>
void Helpers::DisplayDefinedSymbols(const DirectX::SpriteFont* font)
{
    auto symbols = GetDefinedSymbols(font);
    OutputDebugString((L"Defined symbols in font (" + std::to_wstring(symbols.size()) + L" total):\n").c_str());
    int count = 0;
    for (wchar_t c : symbols)
    {
        OutputDebugStringW((L"'" + std::wstring(1, c) + L"' (U+" + std::to_wstring(static_cast<int>(c)) + L") ").c_str());
        // Line break every 8 characters for readability
        if (++count % 8 == 0)
            OutputDebugStringW(L"\n");
    }
    OutputDebugStringW(L"\n");
}
/// <usage> DisplayDefinedSymbols(spriteFont.get()); </usage>