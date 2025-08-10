/* Copyright (c) 2023, Sascha Willems
 *
 * SPDX-License-Identifier: MIT
 *
 */



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

#ifdef FOR_MESHINSTANCE_

Triangle unpackTriangle(uint index, int vertexSize,uint vertexOffset,uint indexOffset) 
{ 
	GeometryNode geometryNode = geometryNodes.nodes[gl_InstanceCustomIndexEXT];  
	Vertices vertices = Vertices(geometryNode.vertexBufferDeviceAddress);
	Indices indices   = Indices(geometryNode.indexBufferDeviceAddress);

	Triangle tri;
	tri.uMatId = geometryNode.uMatId;
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
#else
// This function will unpack our vertex buffer data into a single triangle and calculates uv coordinates
Triangle unpackTriangle(uint index, int vertexSize) {
	Triangle tri;
	const uint triIndex = index * 3;
	GeometryNode geometryNode = geometryNodes.nodes[gl_GeometryIndexEXT]; 
	Indices indices   = Indices(geometryNode.indexBufferDeviceAddress);
	Vertices vertices = Vertices(geometryNode.vertexBufferDeviceAddress);
	tri.uMatId = geometryNode.uMatId;
 
	for (uint i = 0; i < 3; i++) {
		const uint offset = indices.i[triIndex + i] * vertexSize/16;
		vec4 d0 = vertices.v[offset + 0]; // pos.xyz, n.x
		vec4 d1 = vertices.v[offset + 1]; // n.yz, uv.xy
		tri.vertices[i].pos = d0.xyz;
		tri.vertices[i].normal = vec3(d0.w,d1.xy);
		tri.vertices[i].uv = d1.zw;
	}
	// Calculate values at barycentric coordinates
	vec3 barycentricCoords = vec3(1.0f - attribs.x - attribs.y, attribs.x, attribs.y);
	tri.pos = tri.vertices[0].pos * barycentricCoords.x + tri.vertices[1].pos * barycentricCoords.y + tri.vertices[2].pos * barycentricCoords.z;
	tri.normal = tri.vertices[0].normal * barycentricCoords.x + tri.vertices[1].normal * barycentricCoords.y + tri.vertices[2].normal * barycentricCoords.z;
	tri.uv = tri.vertices[0].uv * barycentricCoords.x + tri.vertices[1].uv * barycentricCoords.y + tri.vertices[2].uv * barycentricCoords.z;
	return tri;
}
#endif