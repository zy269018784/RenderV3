#pragma once
#include "ErrorCode.h"
#include "Scene/Vertex.h"

#include <vector>
#include <string>
//RtResult TestSceneFileFormat();
RtResult ReadEBOFromFile(std::string FileName, std::vector<std::uint32_t>& EBO);
RtResult ReadVBOFromFile(std::string FileName, std::vector<Rt::Vertex>& VBO);