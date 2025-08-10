#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "ErrorCode.h"

RtResult WriteBinaryFile(std::string FileName, const char* Data, std::streamsize Size);