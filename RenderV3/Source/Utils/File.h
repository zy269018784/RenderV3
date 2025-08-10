#pragma once
#include <string>
#include <fstream>

int WriteFile(std::string FileName, const char* Data, std::streamsize Size);
int ReadOffsetFile(std::string FileName, std::vector<glm::uvec2>& Offsets);