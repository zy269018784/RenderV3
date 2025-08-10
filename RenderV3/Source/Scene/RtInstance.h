#pragma once
#include <glm/glm.hpp>

struct RtInstance 
{
    glm::vec4 matrix;
    uint32_t instanceCustomIndex;
    uint8_t mask;
    uint32_t SBTRecordOffset;
    uint8_t flags;
    uint64_t blasDeviceAddress;
};