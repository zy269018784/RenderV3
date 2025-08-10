#ifndef MATERIAL_H
#define MATERIAL_H

/*
	玻璃材质 
	p1.x: reflectivity
	p1.y: transmissivity;
	p2.x: texture id
	p2.y: normal map id
	p2.z: material type
	p3.xyz = RGB
*/
/*
	导体材质
	p1.x: uRoughness
	p1.y: vRoughness;
	p1.z: remaproughness;
	p2.x: texture id
	p2.y: normal map id
	p2.z: material type
	p3.xyz = RGB
*/
/*
	disney材质
	p1.x : roughness;	p1.y : metallic;	p1.z : specular;	p1.w : specularTint; 
	p2.x : texture id;	p2.y : normal map id;	p2.z : material type;	p2.w : bsdf_props; 
	p3.xyz : albedo; 	p3.w : transmission;
	p4.xyz : k;		p4.w : ior;
	p5.x : sheen;	p5.y : sheenTint;	p5.z : clearcoat;	p5.w : clearcoatGloss;
	p6.x : subsurface;		p6.y : flatness;		p6.z : anisotropy; 		p6.w : null;
	p7.xyz : emission;		p7.w : emissionTint;
	p8.x : roughness map id;	p8.y : metallic map id；	p8.z : height map id;	p8.w : null;
*/
struct Material 
{ 
	vec4 p1;
	ivec4 p2;
	vec4 p3;
	vec4 p4;
	vec4 p5;
	vec4 p6;
	vec4 p7;
	ivec4 p8;
};

//#define Diffuse  1
//#define Glass 2
//#define Conductor 3

layout(binding = 7, set = 0) buffer Materials { Material[] matls; }; 


int GetTextureIndex(Material m)
{
	return m.p2.x;
}

int GetNormalMapTextureIndex(Material m)
{
	return m.p2.y;
}

int GetMaterialType(Material m)
{
	return m.p2.z;
}

vec3 GetAlbedo(Material m)
{
	return m.p3.xyz;
}

void SetAlbedo(inout Material m, vec3 a)
{
	m.p3.xyz = a;
}

float GlassGetReflectivity(Material m)
{
	return m.p1.x;
}

float GlassGetTransmissivity(Material m)
{
	return m.p1.y;
}


float GetURoughness(Material m)
{
	//if(m.p1.x <= 0.001)
	//	m.p1.x = 0.001;
	return m.p1.x;
}

float GetVRoughness(Material m)
{
	//if(m.p1.y <= 0.001)
	//	m.p1.y = 0.001;
	return m.p1.y;
}

float GetRemaproughness(Material m)
{
	return m.p1.z;
}

float ConductorGetURoughness(Material m)
{
	if(m.p1.x <= 0.001)
		m.p1.x = 0.001;
	return m.p1.x;
}

float ConductorGetVRoughness(Material m)
{
	if(m.p1.y <= 0.001)
		m.p1.y = 0.001;
	return m.p1.y;
}

float ConductorGetRemaproughness(Material m)
{
	return m.p1.z;
}

float DielectricGetURoughness(Material m)
{
	return m.p1.x;
}

float DielectricGetVRoughness(Material m)
{
	return m.p1.y;
}

float DielectricGetRemaproughness(Material m)
{
	return m.p1.z;
}

float DielectricGetReflectivity(Material m)
{
	return m.p1.w;
}



#include "MaterialDisneyImpl.glsl"


#endif