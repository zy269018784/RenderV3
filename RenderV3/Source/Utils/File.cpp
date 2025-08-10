#include <string>
#include <iostream>
#include <fstream>

#include <vector>
#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

int WriteFile(std::string FileName, const char *Data, std::streamsize Size)
{
	std::ofstream Outfile(FileName, std::ios::binary);
	if (!Outfile) {
		std::cerr << "无法打开文件写入: " << FileName << std::endl;
		return -1;
	}
	//std::cout << "write size " << Size << std::endl;
	Outfile.write(Data, Size);
	Outfile.close();
	return 0;
}

int ReadOffsetFile(std::string FileName, std::vector<glm::uvec2> &Offsets)
{
	std::ifstream Infile(FileName, std::ios::binary);
	if (!Infile) {
		std::cerr << "无法打开文件写入: " << FileName << std::endl;
		return -1;
	}
	Infile.seekg(0, std::ios::end);
	std::streamsize Size = Infile.tellg();
	Infile.seekg(0, std::ios::beg);  // 回到文


	std::cout << "read size " << Size << std::endl;

	int Count = Size / sizeof(std::uint32_t);
	Offsets.resize(Count);

	//std::uint32_t* Data = new std::uint32_t[Count];
	Infile.read((char *)Offsets.data(), Size);
	for (int i = 0; i < 10; i++)
	{
		std::cout << Offsets[i].x << " " << Offsets[i].y << std::endl;
	}

	//delete[] Data;
	return 0;
}