#ifndef GEOMETRY_H
#define GEOMETRY_H

//几何节点数据（网格地址索引 材质索引）
struct MeshInstance {
	uint64_t vertexBufferDeviceAddress;
	uint64_t indexBufferDeviceAddress;
	uint uMatId;
}; 

struct Vertex
{
  vec3 pos;
  vec3 normal;
  vec2 uv;
 // int flags;
};

struct Triangle {
	Vertex vertices[3];
	vec3 pos;
	vec3 normal;
	vec2 uv;
	uint uMatId;
}; 

struct InstanceTransform {
    mat4 matrix;
	mat4 normalMatrix;
};

layout(buffer_reference, scalar) buffer Vertices {vec4 v[]; };
layout(buffer_reference, scalar) buffer Indices {uint i[]; };

Triangle unpackTriangle(uint index, int vertexSize,uint vertexOffset,uint indexOffset, MeshInstance meshInstance, vec2 attribs)
{  
	Vertices vertices = Vertices(meshInstance.vertexBufferDeviceAddress);
	Indices indices   = Indices(meshInstance.indexBufferDeviceAddress);

	Triangle tri;
	tri.uMatId = meshInstance.uMatId;
 	const uint triIndex = index * 3;
	for (uint i = 0; i < 3; i++) {
		const uint offset = (vertexOffset + indices.i[indexOffset + triIndex + i]) * vertexSize/16;
		vec4 d0 = vertices.v[offset + 0]; 
		vec4 d1 = vertices.v[offset + 1]; 
		tri.vertices[i].pos = d0.xyz;
		tri.vertices[i].normal = vec3(d0.w,d1.xy);
		tri.vertices[i].uv = vec2(d1.z,1.0-d1.w);
	}

	// Calculate values at barycentric coordinates
	vec3 barycentricCoords = vec3(1.0f - attribs.x - attribs.y, attribs.x, attribs.y);
	tri.pos = tri.vertices[0].pos * barycentricCoords.x + tri.vertices[1].pos * barycentricCoords.y + tri.vertices[2].pos * barycentricCoords.z;
	tri.normal = tri.vertices[0].normal * barycentricCoords.x + tri.vertices[1].normal * barycentricCoords.y + tri.vertices[2].normal * barycentricCoords.z;
	tri.uv = tri.vertices[0].uv * barycentricCoords.x + tri.vertices[1].uv * barycentricCoords.y + tri.vertices[2].uv * barycentricCoords.z;
	return tri;
} 

#endif //GEOMETRY_H
