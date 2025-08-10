#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_GOOGLE_include_directive : enable
#include "common.glsl"
#include "shaderlib/Light.glsl"
#include "shaderlib/Transform.glsl"
#include "shaderlib/HDRParams.glsl"
 #include "intersect.glsl"
#include "shaderlib/EqualAreaMapping.glsl"

layout(binding = 2, set = 0) uniform RenderParams { UBORenderParams rp; }; 
//layout(binding = 5, set = 0) readonly buffer LightArray {  Light[] Lights;  };
layout(binding = 6, set = 0) uniform sampler2D[] HDRs;  
layout(binding = 12, set = 0) uniform Params { HDRParams Hdr; }; 

layout(location = 0) rayPayloadInEXT HitPayload payload;

#if 0
float envPdf() {
	vec3 direction = payload.rayDir;//gl_WorldRayDirectionEXT;
	float theta = acos(clamp(direction.y, -1.0, 1.0));
	vec2 uv = vec2((M_PI + atan(direction.z, direction.x)) * INV_2PI, theta * INV_PI);
	float pdf = texture(HDRs[1], uv).y * texture(HDRs[2], vec2(0., uv.y)).y; 
	return (pdf * rp.hdrResolution) / (TWO_PI * M_PI * sin(theta));
}   

vec3 evalEnvmap(in mat4 envTransform, in float intensity, in vec3 dir) {
	//dir = transformDirection(transpose(envTransform), dir);

	float theta = acos(clamp(dir.y, -1.0, 1.0));
	vec2 uv = vec2((M_PI + atan(dir.z, dir.x)) * INV_2PI, theta * INV_PI);
	return texture(HDRs[0], uv).rgb * intensity ;
}
#endif 

/*
	HDR求交
*/
void main()
{
    payload.done = true;  
	// HDR旋转
	vec3 wLight = Mul(Hdr.RotateY, payload.rayDir);
	// Y朝上改为Z朝上（EXR坐标系Z朝上）
	float a = wLight.y; 
	wLight.y = wLight.z;
	wLight.z = a;
	// 生成UV
	vec2 uv = EqualAreaSphereToSquare(wLight);	
	uv.y = 1 - uv.y;  

	vec3 L 		= payload.L;
	vec3 beta 	= payload.beta;
	vec3 Le 	= texture(HDRs[0], uv).rgb * rp.hdrMultiplier;
	//Le *= vec3(0.97254, 0.55294, 0.117647);
	Le = clamp(Le,0,200); 
	L 			+= beta * Le;
	payload.L 	= L;
}
