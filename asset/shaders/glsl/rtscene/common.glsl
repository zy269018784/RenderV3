#ifndef COMMON_H
#define COMMON_H
const highp float M_PI = 3.14159265358979323846;
#define TWO_PI    6.28318530717958647692 
#define INV_PI    0.31830988618379067154
#define INV_2PI   0.15915494309189533577

#define EPS 0.001
#define INFINITY  100000000.0
//#define MINIMUM   0.0001
#define MINIMUM   0.02


struct UBORenderParams
{
	//mat4 viewInverse;
	//mat4 projInverse;
 
	mat4 cameraFromWorld;
    mat4 worldFromCamera;
    mat4 screenFromCamera;     // ��֪: Perspective(fov, 1e-2f, 1000.f);
    mat4 screenFromRaster;     // Inverse(rasterFromScreen) 
    mat4 screenFromNDC;        // Inverse(NDCFromScreen)
    mat4 rasterFromScreen;     // rasterFromNDC * NDCFromScreen;
    mat4 rasterFromNDC;        // ��֪: Scale(film.FullResolution().x, -film.FullResolution().y, 1);
    mat4 rasterFromCamera;     // Inverse(cameraFromRaster)
    mat4 cameraFromRaster;     // Inverse(screenFromCamera) * screenFromRaster;
    mat4 cameraFromNDC;        // cameraFromRaster * rasterFromNDC; 
    mat4 cameraFromScreen;     // cameraFromRaster * rasterFromScreen;
    mat4 NDCFromScreen;        // ��֪:
    mat4 NDCFromRaster;        // Inverse(rasterFromNDC) = NDCFromScreen * screenFromRaster
    mat4 NDCFromCamera;        // Inverse(cameraFromNDC) = NDCFromScreen * screenFromCamera
    uint RenderWidth;
    uint RenderHeight; 
    uint frame;
    uint spp;
    uint maxDepth;
    uint lightCount;
	uint areaLightCount;
    bool useHDR;
    float hdrResolution;
    float hdrMultiplier;
    float exposure; 
    uint tonemapType; 
    float outputGamma; 
};

struct BSDFSample1 //Bidirectional Scattered Distribution Function ˫��ɢ��ֲ��������� ����BRDF��BTDF
{
	vec3 dir; 
	float pdf; 
};

struct HitPayload
{
    vec3 rayOrigin;         // 射线起点
    vec3 rayDir;            // 射线方向
    vec3 position;          // 交点
  //vec3 dir;
    vec3 GeometryNormal;    // 交点几何法线
    vec3 ShadingNormal;     // 交点着色法线     
    vec3 radiance;          // 没用到
    vec3 L;                 // 最终光照
    vec3 beta;              // 积分
  //BSDFSample1 bsdf;       
  //vec3 absorption; //����
  //float eta;
    uint depth;
    bool done;              // 与HDR求交
    uint frame;
	bool hit;
  //int MaterialType;
  //uint uMatId;
    uint seed;
    uint uSpaceColorType;    
};


uint pcg(inout uint state)
{
    uint prev = state * 747796405u + 2891336453u;
    uint word = ((prev >> ((prev >> 28u) + 4u)) ^ prev) * 277803737u;
    state = prev;
    return (word >> 22u) ^ word;
}

uvec2 pcg2d(uvec2 v)
{
    v = v * 1664525u + 1013904223u;
    v.x += v.y * 1664525u;
    v.y += v.x * 1664525u;
    v = v ^ (v >> 16u);
    v.x += v.y * 1664525u;
    v.y += v.x * 1664525u;
    v = v ^ (v >> 16u);
    return v;
}

float rand(inout uint seed)
{
    uint val = pcg(seed);
    return (float(val) * (1.0 / float(0xffffffffu)));
}


float max3(vec3 v) 
{
	return max (max (v.x, v.y), v.z);
}

/*
 * Power heuristic often reduces variance even further for multiple importance sampling
 * Chapter 13.10.1 of pbrbook
 */ 
float powerHeuristic(float a, float b)
{
	float t = a * a;
	return t / (b * b + t);
}

vec3 faceforward(vec3 normal,vec3 rayDir){
    return dot(normal,rayDir) <= 0.0 ? normal * -1.0 : normal;
}
#endif