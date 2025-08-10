#include "ErrorCode/ErrorCode.h"
#include "Utils/Filesystem.h"
#include "Scene/Vertex.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>	
#include <boost/iostreams/device/mapped_file.hpp>
#if 0
RtResult ResizeVBO(std::vector<float>& VBO)
{
	if (VBO.size() % 3 != 0)
	{
		auto Left = VBO.size() % 3;
		VBO.erase(VBO.end() - Left, VBO.end());
	}
	return RT_SUCCESS;
}

RtResult WriteVBOToFile(std::string File, std::vector<float>& VBO)
{
	ResizeVBO(VBO);

	if (VBO.size() == 0)
	{
		std::cout << "VBO is 0" << std::endl;
		return RT_VBO_EMPTY;
	}

	return WriteBinaryFile(File, reinterpret_cast<char*>(VBO.data()), VBO.size() * sizeof (float));
}
#endif
RtResult ReadEBOFromFile(std::string FileName, std::vector<std::uint32_t>& EBO)
{
	try {
		boost::iostreams::mapped_file_source File;
		File.open(FileName);

		if (File.is_open())
		{
			// 直接访问文件内容
			const char* data = File.data();
			std::size_t size = File.size();

			EBO.resize(size / sizeof(std::uint32_t));
			std::memcpy(EBO.data(), File.data(), EBO.size() * sizeof(std::uint32_t));

			File.close();
		}
		else {
			std::cerr << "无法映射文件: " << FileName << std::endl;
			return RT_FILE_OPEN_FAILED;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "错误: " << e.what() << std::endl;
		return RT_FILE_OPEN_FAILED;
	}
	return RT_SUCCESS;
}

RtResult ReadVBOFromFile(std::string FileName, std::vector<Rt::Vertex>& VBO)
{
	try {
		boost::iostreams::mapped_file_source File;
		File.open(FileName);

		if (File.is_open())
		{
			// 直接访问文件内容
			const char* data = File.data();
			std::size_t size = File.size();

			VBO.resize(size / sizeof(Rt::Vertex));
			std::memcpy(VBO.data(), File.data(), VBO.size() * sizeof(Rt::Vertex));

			File.close();
		}
		else {
			std::cerr << "无法映射文件: " << FileName << std::endl;
			return RT_FILE_OPEN_FAILED;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "错误: " << e.what() << std::endl;
		return RT_FILE_OPEN_FAILED;
	}
	return RT_SUCCESS;
}
#if 0
RtResult ReadVBOFromFile(std::string FileName, std::vector<float> &VBO)
{
	std::ifstream Infile(FileName, std::ios::binary);
	if (!Infile.is_open())
	{
		std::cerr << "无法打开文件写入: " << FileName << std::endl;
		return RT_FILE_OPEN_FAILED;
	}
	/*
		获取文件字节数
	*/
	Infile.seekg(0, std::ios::end);
	std::streamsize Size = Infile.tellg();
	Infile.seekg(0, std::ios::beg);  // 回到文


	/*
		计算VBO大小
	*/
	VBO.resize(Size / sizeof(float));
	/*
		调整VBO大小为3的倍数
	*/
	ResizeVBO(VBO);

	Infile.read((char*)VBO.data(), VBO.size() * sizeof(float));
	Infile.close();
	return RT_SUCCESS;
}


RtResult TestSceneFileFormat()
{
	RtResult Ret = RT_SUCCESS;

	std::string FileName = "1.vbo";

	std::vector<float> VBO;
	VBO.push_back(1.3);
	VBO.push_back(2.3);
	VBO.push_back(4.3);
	VBO.push_back(422.3);
	VBO.push_back(432.3);
	VBO.push_back(42452.3);

	Ret = WriteVBOToFile(FileName, VBO);

	std::vector<float> VBO2;
	Ret = ReadVBOFromFile(FileName, VBO2);
	if (Ret == RT_FILE_OPEN_FAILED)
	{
		std::cout << "文件打开失败" << std::endl;
	}
 	for (int i = 0; i < VBO2.size(); i++)
	{
		std::cout << VBO2[i] << std::endl;
	}

	return Ret;
}
#endif