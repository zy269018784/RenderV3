#ifndef LIGHTSAMPLING_H
#define LIGHTSAMPLING_H
#include "shaderlib/Sampling.glsl"
LightLiSample SampleSpotLight(LightSampleContext ctx, Light light)
{ 
    vec3 From = GetFrom(light);
    vec3 To = GetTo(light);
    vec3 Li = GetColor(light);
    float range = GetRange(light);
    float power = GetPower(light); 
    vec3 pLight = From;  
    vec3 wiLight = normalize(ctx.p - pLight); 
    vec3 wi = -wiLight;
    //vec3 wi = normalize(pLight - ctx.p);

    float d = length(pLight - ctx.p);
    float cosFalloffStart = GetCosFalloffStart(light);
    float cosFalloffEnd = GetCosFalloffEnd(light);
    if (d > range)
        power *= exp(range - d);
    //Li *= SmoothStep(CosTheta(wiLight), cosFalloffEnd, cosFalloffStart);
    //Li *= power;
    float pdf = 1;
    return LightLiSample(Li, wi, pLight, pdf);
}

LightLiSample SamplePointLight(LightSampleContext ctx, Light light)
{
   //return LightLiSample(GetColor(light), normalize(GetFrom(light) - ctx.p), GetFrom(light) , 1);
    //return LightLiSample(GetColor(light), normalize(vec3(49.2874, -8.94025, 21.7952) - ctx.p), vec3(49.2874, -8.94025, 21.7952), 1);
    //return LightLiSample(GetColor(light), normalize(vec3(34.3792, -6.86014, 23.4608) - ctx.p), vec3( 34.3792, -6.86014, 23.4608), 1);

    //vec3 pLight = MulPoint(light.WorldFromLight, vec3(0));
    // 光源的世界坐标
    vec3 pLight = GetFrom(light);
    // 交点到光源的向量
    vec3 wLight = normalize(pLight - ctx.p);
    // 交点到光源的距离
    float d = length(pLight - ctx.p);
    // 光源颜色
    vec3 Li     = GetColor(light);

    // 衰减范围
    float range = GetRange(light);
    // 亮度
    float power = GetPower(light);
    // 计算距离衰减
    if (d > range)
        power *= exp(range - d);
    Li *= power;
    float pdf = 1;
    return LightLiSample(Li, wLight, pLight, pdf);
}
 
LightLiSample SampleDistantLight(LightSampleContext ctx, Light light)
{
    // 光源的世界坐标
    vec3 From = GetFrom(light);
    vec3 To = GetTo(light);
    vec3 wLight = normalize(From - To);
    vec3 pLight = From - To + ctx.p;  
    // 光源颜色
    vec3 Li     = GetColor(light);
    // 亮度
    float power = GetPower(light);
    Li *= power;
    float pdf   = 1.0;

    return LightLiSample(Li, wLight, pLight, pdf);
} 

uint seed2 ;
// Random int [0, max]
int getRandomInteger(int max) {
    return int(rnd(seed2) * (max + 1));
}

LightLiSample SampleAreaLight2(LightSampleContext ctx, Light light)
{
    vec3 pLight = vec3(0,800,0);
    vec3 wLight = normalize(pLight - ctx.p);
    vec3 Li = vec3(1);
    float pdf = 1.0;
    return LightLiSample(Li, wLight, pLight, pdf);
}


LightLiSample SampleAreaLight(LightSampleContext ctx, Light light)
{ 
    float u = 0;
    /*
        采样面光源
    */
    uint Index = UniformSample(uint(rp.areaLightCount), u);
    
    //灯光关联的实例信息
    uint instanceIndex = areaLightInstance[Index].v.x;
    uint faceCount = areaLightInstance[Index].v.z;
    //网格信息
    MeshInstance instance = instances[instanceIndex]; 
    uvec2 offsets = Offsets[instanceIndex]; 
    Vertices vertices = Vertices(instance.vertexBufferDeviceAddress);
    Indices indices   = Indices(instance.indexBufferDeviceAddress);
    //变换信息
    InstanceTransform iTrans = transforms[instanceIndex];   
    /*
        采样网格三角形
    */
    u = rnd(payload.seed);
    uint TriRandomIndex = UniformSample(faceCount, u);
    TriRandomIndex = 0;
    
    Triangle tri;
    tri.uMatId = instance.uMatId;
    const uint triIndex = TriRandomIndex * 3;
    for (uint i = 0; i < 3; i++) {
        const uint offset = (offsets.x + indices.i[offsets.y + triIndex + i]) * 2;
        vec4 d0 = vertices.v[offset + 0]; 
        vec4 d1 = vertices.v[offset + 1]; 
        tri.vertices[i].pos = d0.xyz;
        tri.vertices[i].normal = vec3(d0.w,d1.xy); 
    }   
    /*
        获取三角形上的 随机采样点
    */
    vec2 randomXY = vec2(rnd(payload.seed), rnd(payload.seed));
    vec3 barycentricCoords = SampleUniformTriangle(randomXY); 


    // 计算三角形中 采样点的 世界坐标 跟 法线 
    vec3 LightSample_WorldPos = tri.vertices[0].pos * barycentricCoords.x + tri.vertices[1].pos * barycentricCoords.y + tri.vertices[2].pos * barycentricCoords.z;
    vec3 LightSample_Normal = tri.vertices[0].normal * barycentricCoords.x + tri.vertices[1].normal * barycentricCoords.y + tri.vertices[2].normal * barycentricCoords.z; 
    
    // 顶点 跟 法线 变换到 世界坐标系
    mat4 transform = iTrans.matrix;
    mat4 normalTransform = iTrans.normalMatrix;
    LightSample_WorldPos = (transform * vec4(LightSample_WorldPos, 1.0)).xyz;
    LightSample_Normal = (normalTransform * vec4(LightSample_Normal, 0.0)).xyz; 

    // 矩阵变换有缩放，需要再归一化
    LightSample_Normal = normalize(LightSample_Normal);
 
    //光照亮度
    vec3 Li = GetColor(light) * GetPower(light) ;  

    //光照点
    vec3 pLight = LightSample_WorldPos;    
    
    // 交点到光源的向量
    vec3 wLight = normalize(pLight - ctx.p);

    //概率
    float pdf = 0.5f;
  
    return LightLiSample(Li, wLight, pLight, pdf);
} 

LightLiSample SampleLights(LightSampleContext ctx, Light light)
{     
    LightLiSample LightSample;
    LightSample.L = vec3(0, 0, 0);
    int Type = GetLightType(light);
    switch(Type)
    {
        case Spot:
            LightSample = SampleSpotLight(ctx, light);
            break;
        case Distant:
            LightSample = SampleDistantLight(ctx, light);
            break;       
        case Point:
            LightSample = SamplePointLight(ctx, light);
            break;
        case Area:
            LightSample = SampleAreaLight(ctx, light);
            break;
    } 
    return LightSample;
}

vec3 LightSampling(vec3 wpos, BSDF bsdf, float u)
{
    uint TotalLightCount = rp.lightCount + rp.areaLightCount;
    if(TotalLightCount == uint(0)) return vec3(0);
    uint LightIndex = UniformSample(TotalLightCount, u);
    Light SampledLight;
    if (LightIndex < rp.lightCount)
    {
        /* 普通光源 */
        SampledLight = Lights[LightIndex];
    }
    else
    {
        /* 面光源 */    
        LightIndex -= rp.lightCount;
        SampledLight = AreaLights[LightIndex]; 
    }

    vec3 radiance = vec3(0);  
    LightSampleContext ctx;
    ctx.p = wpos;
    vec3 wo = -payload.rayDir;
    LightLiSample LightSample1 = SampleLights(ctx, SampledLight);
    vec3 light_wo = ToLocal(bsdf.ShadingFrame, normalize(wo));
    vec3 light_wi = ToLocal(bsdf.ShadingFrame, LightSample1.wi); 
    if (SameHemisphere(light_wo, light_wi)) 
    { 
        if (Unoccluded(wpos, LightSample1.pLight)) 
        { 
	        radiance += LightSample1.L;
        } 		
    }  
    return radiance;
}


#endif