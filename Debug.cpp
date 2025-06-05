#include "pch.h"
#include "Debug.h"

void Log(const char* label, std::string value)
{
	OutputDebugStringA(("[" + std::string(label) + "] " + value + "\n").c_str());
}

void Log(const char* label, const DirectX::SimpleMath::Vector2& velocity) {
	Log(label, "velocity x: " + std::to_string(velocity.x) + ", y: " + std::to_string(velocity.y));
}

void Log(const int& label, const DirectX::SimpleMath::Vector2& velocity) {
	Log(label, "velocity x: " + std::to_string(velocity.x) + ", y: " + std::to_string(velocity.y));
}

void Log(const int& labelType, std::string value)
{
	std::string lbl;
	switch (labelType) {
	case 0:
		lbl = "INFO";
		break;
	case 1:
		lbl = "WARN";
		break;
	case 2:
		lbl = "ERROR";
		break;
	default:
		lbl = "LOG";
		break;
	}
	OutputDebugStringA(("[" + lbl + "] " + value + "\n").c_str());
}
