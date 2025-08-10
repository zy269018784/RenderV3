#version 460 core
layout (local_size_x = 8,local_size_y = 8,local_size_z = 1) in;
//layout (std430, binding = 0) buffer shader_storage_block_data
//{
//	vec4 CC[512][512];
//}data;

//layout(rgba32f, binding = 0) uniform image3D RWVoxelLighting;
layout(rgba8_snorm, binding = 0) uniform image3D RWVoxelLighting;

void main()
{
	vec4 color;
	if (gl_GlobalInvocationID.x > 15)
	{
		color = vec4(1, 0, 0, 0);
		if (gl_GlobalInvocationID.y > 15)
		{
			color = vec4(0, 1, 0, 0);
		}
	} 
	else 
	{
		color = vec4(0, 0, 1, 0);
		if (gl_GlobalInvocationID.y > 15)
		{
			color = vec4(1, 1, 0, 0);
		}
	}
	if (gl_GlobalInvocationID.z > 15)
		color = vec4(1, 0, 1, 0);
    imageStore(RWVoxelLighting, ivec3(gl_GlobalInvocationID), color);

};
