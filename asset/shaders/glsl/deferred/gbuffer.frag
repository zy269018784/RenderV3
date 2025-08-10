#version 450

layout (binding = 1) uniform sampler2D samplerColor; 

layout (location = 0) in vec3 inWorldPos;  
layout (location = 1) in vec2 inUV; 
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outAlbedo;

void main() 
{
	outPosition = vec4(inWorldPos, 1.0);

	// Calculate normal in tangent space
	vec3 N = normalize(inNormal); 
	outNormal = vec4(N, 1.0);

	outAlbedo = texture(samplerColor, inUV);
}