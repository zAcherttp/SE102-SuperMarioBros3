#pragma once

constexpr auto LOG_INFO = 0;
constexpr auto LOG_WARN = 1;

void Log(const char* label, std::string value);

void Log(const int& label, std::string value);
