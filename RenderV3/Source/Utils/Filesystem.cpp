#include "Filesystem.h"
#include <vector>
#include <boost/iostreams/device/mapped_file.hpp>

RtResult ResizeVBO(std::vector<float>& VBO);

RtResult WriteBinaryFile(std::string FileName, const char* Data, std::streamsize Size)
{
	std::ofstream Outfile(FileName, std::ios::binary);
	if (!Outfile.is_open())
	{
		std::cerr << "无法打开文件写入: " << FileName << std::endl;
		return RT_FILE_OPEN_FAILED;
	}

	//std::cout << "write size " << Size << std::endl;
	Outfile.write(Data, Size);
	Outfile.close();
	return RT_SUCCESS;
}

RtResult ReadFile(std::string FileName, const char* Data, std::size_t *Size)
{
	return RT_SUCCESS;
}



