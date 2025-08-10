#pragma once

struct GeometryNode 
{
    //glm::mat4 matrix;
    uint64_t vertexBufferDeviceAddress;
    uint64_t indexBufferDeviceAddress;
    uint32_t uMatId = 0;
};