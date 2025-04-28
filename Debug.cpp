#include "pch.h"
#include "Debug.h"

void Log(const char* label, std::string value)
{
	OutputDebugStringA(("[" + std::string(label) + "] " + value + "\n").c_str());
}
