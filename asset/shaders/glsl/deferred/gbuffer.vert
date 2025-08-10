#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

layout (binding = 0) uniform UBO 
{
	mat4 projection;
	mat4 model;
	mat4 view; 
} ubo;

layout (location = 0) out vec3 outWorldPos;  
layout (location = 1) out vec2 outUV; 
layout (location = 2) out vec3 outNormal;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
	vec4 tmpPos = vec4(inPos.xyz, 1.0); 
	gl_Position = ubo.projection * ubo.view * ubo.model * tmpPos;
	
	outWorldPos = vec3(ubo.model * tmpPos);		// Vertex position in world space	
	outUV = inUV;  
	outNormal = transpose(inverse(mat3(ubo.model))) * normalize(inNormal);		// Normal in world space 
}
