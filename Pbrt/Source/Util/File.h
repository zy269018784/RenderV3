#pragma once
#include <string>

namespace Render
{
	bool FileExists(std::string filename);
	std::string ReadFileContents(std::string filename);
	bool WriteFileContents(std::string filename, const std::string& contents);
	bool HasExtension(std::string filename, std::string e);
}
