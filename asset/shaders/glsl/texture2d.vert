#version 450  

layout(binding = 0) uniform UniformBufferObject { 
    mat4 proj; 
	mat4 view; 
}
ubo;

//layout(std430, binding = 2) buffer StorageBufferObject { 
//    mat4 proj; 
//    mat4 view; 
//}
//ssbo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;

out gl_PerVertex { vec4 gl_Position; };

void main() {
  gl_Position = ubo.proj *  ubo.view * vec4(inPosition, 1.0);
  fragNormal = inNormal;
  fragTexCoord = inTexCoord;
}

