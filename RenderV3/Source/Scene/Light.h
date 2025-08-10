#pragma once
#include <glm/glm.hpp>

namespace Rt
{
    class Light
    {
    public:
        enum class Type
        {
            Spot = 0,
            Distant,
            Point,
            Area,
            HDR, 
        };
    public:
        void CreatePointLight(glm::vec3 Color, float Power, float Range, glm::vec3 From);
        void CreateDistantLight(glm::vec3 Color, float Power, glm::vec3 From, glm::vec3 To);
        void CreateSpotLight(glm::vec3 Color, float Power, float Range, glm::vec3 From, glm::vec3 To, float CosFalloffStart, float CosFalloffEnd);
        void CreateHDRLight(float Phi, float Theta, float Power);
        void CreateAreaLight(glm::vec3 Color, float Power, int ObjectIndex);

        void SetColor(glm::vec3 Color);
        void SetPower(float Power);
        void SetRange(float Range);
        void SetMeshIndex(int Index);
        void SetFalloff(float CosFalloffStart, float CosFalloffEnd);
        void SetLightType(Light::Type Type);
        void SetLightVisible(bool visible = true);
        void SetDirection(glm::vec3 From, glm::vec3 To);
        void SetWorldFromLight(glm::mat4 WorldFromLight);
        
        glm::vec3 GetColor() const {
            return p1;
        }
        uint32_t GetType() {
            return p2.r;
        }

        void Print() {
            //printf("light=======================================\n");
            //printf("type: %d\n", p2.r);
            //printf("color:%f,%f,%f,%f\n", p1.r, p1.g, p1.b,p1.a);
            //printf("pos:%f,%f,%f\n", p3.x, p3.y, p3.z);
            //printf("to:%f,%f,%f\n", p4.x, p4.y, p4.z);
        }
    public:
        glm::mat4 WorldFromLight;
        glm::vec4 p1;   // rgb: color,  a: power
        glm::ivec4 p2;  // r: type,     g: MeshIndex,    b:faceCount(三角形的总数量)
        glm::vec4 p3;   // rgb: from,   a: range
        glm::vec4 p4;   // rgb: to      a: area(光源的总表面积)
        glm::vec4 p5;   // r: cosFalloffStart g: cosFalloffEnd
    };

    struct RenderLight {
        glm::mat4 WorldFromLight;
        glm::vec4 p1;  // rgb: color,  a: power
        glm::ivec4 p2; // r: type,     g: MeshIndex,    b:faceCount(三角形的总数量)
        glm::vec4 p3;  // rgb: from,   a: range
        glm::vec4 p4;  // rgb: to      a: area(光源的总表面积)
        glm::vec4 p5;  // r: cosFalloffStart g: cosFalloffEnd
    };
}