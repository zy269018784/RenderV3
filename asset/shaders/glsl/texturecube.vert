#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;

layout(binding = 0) uniform UniformBufferObject {
  mat4 view;
  mat4 proj;
}
ubo;

 
layout( location = 0 ) out vec3 vert_texcoord;

void main() {
  vec3 position = mat3(ubo.view) * inPosition;
  gl_Position = (ubo.proj * vec4( position, 0.0 )).xyzz;
  vert_texcoord = inPosition;
}