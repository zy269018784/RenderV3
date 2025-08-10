#ifndef LIGHT_H
#define LIGHT_H
#include "Math.glsl"
#include "shaderlib/Tonemapping.glsl"

#define Spot    0
#define Distant 1
#define Point   2
#define Area    3
struct Light
{
    mat4 WorldFromLight;  
    vec4 p1;   // rgb: color, a: power
    ivec4 p2;  // r: type, g: visible
    vec4 p3;   // rgb: from, a: range
    vec4 p4;   // rgb: to
    vec4 p5;   // r: cosFalloffStart g: cosFalloffEnd

};
/*
    x: InstanceIndex
    y: LightIndex
    z：网格三角形数量
*/
struct AreaLightInstance{
	uvec4 v;
};

layout(binding = 5, set = 0)   readonly buffer LightArray {  Light[] Lights;  };
layout(binding = 17, set = 0)  readonly buffer AreaLightArray {  Light[] AreaLights;  };
layout(binding = 16, set = 0) readonly buffer AreaLightInstanceArray { AreaLightInstance[] areaLightInstance; }; 

struct LightLiSample
{
    vec3 L;             // 光照
    vec3 wi;            // 光照点到光源的方向
    vec3 pLight;        // 光源的世界坐标
    float pdf;          // pdf
};

struct LightSampleContext
{
    vec3 p;
};

vec3 GetColor(Light light)
{
    return light.p1.rgb;
}

float GetPower(Light light)
{
    return light.p1.a;
}

int GetLightType(Light light)
{
    return light.p2.r;
}

int GetLightVisible(Light light)
{
    return light.p2.g;
}

float GetRange(Light light)
{
	return light.p3.a;
}

float GetCosFalloffStart(Light light)
{
	return light.p5.r;
}

float GetCosFalloffEnd(Light light)
{
	return light.p5.g;
}

vec3 GetFrom(Light light)
{
    return light.p3.rgb;
}

vec3 GetTo(Light light)
{
    return light.p4.rgb;
}

 

 #endif 