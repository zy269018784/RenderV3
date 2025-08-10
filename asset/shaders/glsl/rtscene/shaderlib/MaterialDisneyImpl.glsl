#ifndef MATERIAL_DISNEY_IMPL_H
#define MATERIAL_DISNEY_IMPL_H
 
// BSDF Props
#define BSDF_FLAG_DIFFUSE 		1 << 0
#define BSDF_FLAG_SPECULAR 		1 << 1
#define BSDF_FLAG_GLOSSY 		1 << 2
#define BSDF_FLAG_REFLECTION 	1 << 3
#define BSDF_FLAG_TRANSMISSION 	1 << 4

#define BSDF_FLAG_DIFFUSE_REFLECTION 	BSDF_FLAG_DIFFUSE | BSDF_FLAG_REFLECTION
#define BSDF_FLAG_DIFFUSE_TRANSMISSION 	BSDF_FLAG_DIFFUSE | BSDF_FLAG_TRANSMISSION
#define BSDF_FLAG_SPECULAR_REFLECTION 	BSDF_FLAG_SPECULAR | BSDF_FLAG_REFLECTION
#define BSDF_FLAG_SPECULAR_TRANSMISSION BSDF_FLAG_SPECULAR | BSDF_FLAG_TRANSMISSION

#define BSDF_FLAG_ALL BSDF_FLAG_DIFFUSE | BSDF_FLAG_SPECULAR | BSDF_FLAG_REFLECTION | BSDF_FLAG_TRANSMISSION

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
//p1
float GetDisneyRoughness(Material m) {
	return max(m.p1.x,0.001);
}

void SetDisneyRoughness(inout Material m, float v) {
	m.p1.x = v;
} 

float GetDisneyMetallic(Material m) {
	return m.p1.y;
}

void SetDisneyMetallic(inout Material m, float v) {
	m.p1.y = v;
}

float GetDisneySpecular(Material m) {
	return m.p1.z;
}

float GetDisneySpecularTint(Material m) {
	return m.p1.w;
} 

//p2
int GetDisneyAlbedoMap(Material m) {
	return m.p2.x;
}

int GetDisneyNormalMap(Material m) {
	return m.p2.y;
}

int GetDisneyMaterialType(Material m) {
	return m.p2.z;
}

int GetDisneyBSDFProperties(Material m) {
	return m.p2.w;
}
 
//p3
vec3 GetDisneyAlbedo(Material m) {
	return m.p3.xyz;
} 

float GetDisneyTransmission(Material m) {
	return m.p3.w;
} 

//p4
vec3 GetDisneyK(Material m) {
	return m.p4.xyz;
}

float GetDisneyIOR(Material m) {
	return m.p4.w;
}

//p5
float GetDisneySheen(Material m) {
	return m.p5.x;
}

float GetDisneySheenTint(Material m) {
	return m.p5.y;
}

float GetDisneyClearcoat(Material m) {
	return m.p5.z;
}

float GetDisneyClearcoatGloss(Material m) {
	return m.p5.w;
}

float GetDisneyClearcoatRoughness(Material m) {
	return 1.0 - m.p5.w;
}

//p6
float GetDisneySubsurface(Material m) {
	return m.p6.x;
}

float GetDisneyFlatness(Material m) {
	return m.p6.y;
}

float GetDisneyAnisotropy(Material m) {
	return m.p6.z;
}

float GetDisneyAtDistance(Material m) { //暂时不用
	return m.p6.w;
}

//p7
vec3 GetDisneyEmission(Material m) {
	return m.p7.xyz;
}

float GetDisneyEmissionTint(Material m) {
	return m.p7.w;
}

//p8
int GetDisneyRoughnessMap(Material m) {
	return m.p8.x;
}

int GetDisneyMetallicMap(Material m) {
	return m.p8.y;
}

int GetDisneyHeightMap(Material m) {
	return m.p8.z;
}

int GetDisneyThin(Material m) {
	return m.p8.w;
}

 
bool isSpecular(Material m) 
{ 
	return ( GetDisneyBSDFProperties(m) & BSDF_FLAG_SPECULAR) != 0; 
}

bool isGlossy(Material m) 
{ 
	return (GetDisneyBSDFProperties(m) & BSDF_FLAG_GLOSSY) != 0; 
}

bool isDiffuse(Material m) 
{ 
	return (GetDisneyBSDFProperties(m) & BSDF_FLAG_DIFFUSE) != 0; 
}

#endif