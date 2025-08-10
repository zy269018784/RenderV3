#pragma once
#include <glm/glm.hpp>
namespace Rt
{
    struct Vertex
    {
        glm::vec3 pos;      // 位置 
        glm::vec3 normal;   // 法线   
        glm::vec2 uv;       // 纹理坐标
        //glm::vec3 tangent;  // 切线
    };
}