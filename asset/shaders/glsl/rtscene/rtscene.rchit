#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

#include "CompileConfig.glsl"
#include "common.glsl"
#include "shaderlib/Geometry.glsl" 
#include "shaderlib/Material.glsl"
#include "shaderlib/GammaCorrection.glsl"
#include "shaderlib/ColorSpace.glsl"

//bindings
layout(binding = 2, set = 0) uniform RenderParams { UBORenderParams rp; }; 
layout(binding = 3, set = 0) uniform sampler2D[] TextureMaps;  
layout(binding = 4, set = 0) buffer MeshInstances { MeshInstance instances[]; };   
layout(binding = 6, set = 0) uniform sampler2D[] HDRs;   
layout(binding = 8, set = 0) buffer InstanceTransforms { InstanceTransform transforms[]; };  
layout(binding = 14, set = 0) readonly buffer OffsetArray { uvec2[] Offsets; }; 
#if 1
layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
//payload 
layout(location = 0) rayPayloadInEXT HitPayload payload; 
layout(location = 2) rayPayloadEXT bool shadowed;
#endif

/*
	解析材质贴图
*/
vec3 GetMaterialAlbedo(Material M, vec2 UV)
{
	vec3 Albedo = GetAlbedo(M);
	int TextureIndex = GetTextureIndex(M);
	if (TextureIndex >= 0)
	{					
		Albedo = texture(TextureMaps[TextureIndex], UV).xyz;
		Albedo = GammaCorrection(Albedo, 2.2);
		if (payload.uSpaceColorType == 1)
		{ 
			Albedo = Linear_to_ACEScg(Albedo);
		}
	}   
	return Albedo;
}

hitAttributeEXT vec2 attribs;

#include "random.glsl"
#include "shaderlib/Ray.glsl"



#include "shaderlib/Light.glsl"
#include "shaderlib/Frame.glsl"

#include "shaderlib/Math.glsl" 
#include "shaderlib/BSDF.glsl"
#include "shaderlib/EqualAreaMapping.glsl"
#include "shaderlib/PiecewiseConstant2D.glsl"
#include "shaderlib/Transform.glsl"

/*
	场景求交函数
*/
#include "shaderlib/SceneIntersection.glsl"
#include "shaderlib/LightSampling.glsl"

uint seed = 0; 

vec3 directLight(in Material material)
{
	vec3 L = vec3(0);

	L += vec3(0.3);
	return L;
}


vec3 sampleEnvironmentLight(vec2 r, vec3 p, vec3 beta) {	
	vec3 radiance = vec3(0.0);
	float pdf;
	vec2 u = vec2(r.x, r.y);
	u = vec2(0.5, 0.5); 
	vec2 hdrUV = PiecewiseConstant2D_Sample(u, pdf);
	hdrUV.y = 1 - hdrUV.y;
	vec3 wLight = EqualAreaSquareToSphere(hdrUV); 
	float tmp = wLight.y;
	wLight.y = wLight.z;
	wLight.z = tmp;
	wLight = Mul(Hdr.RotateYInv, wLight.xyz);
			
	uint flags = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT;
	// 判断HDR是否被遮挡
	Ray LightRay;
	LightRay.o = p;
	LightRay.d = normalize(wLight);
	{
		vec3 L = vec3(0); 
		shadowed = true;  
		traceRayEXT(topLevelAS, flags, 0xFF, 0, 0, 1, LightRay.o, MINIMUM, LightRay.d, INFINITY, 2); 
		if (!shadowed) {
			vec2 uv = EqualAreaSphereToSquare(LightRay.d.xzy);	
			uv.y = 1 - uv.y;
			radiance += beta * clamp(texture(HDRs[0], uv).rgb, 0, 1);
			//radiance = texture(HDRs[0], uv).rgb; 
		} 
	} 

	return radiance;
}

vec3 SampleHDRLight(vec3 p, vec2 u, BSDF bsdf, Material m)
{
	vec3 radiance = vec3(0);
	float pdf;
	// 采样
	vec2 uv = PiecewiseConstant2D_Sample(u, pdf);
	pdf = pdf / (4 * M_PI);
	uv.y = 1 - uv.y;
	// HDR射线方向
	vec3 wLight = EqualAreaSquareToSphere(uv);

	// 转EXR坐标系
	float tmp 	= wLight.y;
	wLight.y 	= wLight.z;
	wLight.z 	= tmp;
	// HDR旋转
	wLight = Mul(Hdr.RotateYInv, wLight.xyz);
	
	
	// 判断HDR是否被遮挡
	Ray LightRay;
	LightRay.o = p;
	LightRay.d = normalize(wLight);
	
	// 默认被遮挡
	shadowed = true; 
	// 跳过close hit
	uint flags = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT;
	// HDR遮挡测试
	traceRayEXT(topLevelAS, flags, 0xFF, 0, 0, 1, LightRay.o, MINIMUM, LightRay.d, INFINITY, 2); 

    vec3 wo = -payload.rayDir; 
	vec3 wi = LightRay.d;
    vec3 light_wo = ToLocal(bsdf.ShadingFrame, normalize(wo));
    vec3 light_wi = ToLocal(bsdf.ShadingFrame, normalize(wi)); 
    if (SameHemisphere(light_wo, light_wi))  {  	
		if (!shadowed) {
			//vec3 EXRRay = LightRay.d.xzy; 
			vec3 EXRRay = Mul(Hdr.RotateY, LightRay.d).xzy;
			vec2 uv = EqualAreaSphereToSquare(EXRRay);	
			uv.y = 1 - uv.y;
			// radiance += clamp(texture(HDRs[0], uv).rgb, 0, 1);// * vec3(1, 0.635, 0.0078);

			vec3 lightSample_L = clamp(texture(HDRs[0], uv).rgb * rp.hdrMultiplier, 0, 200);
			float cosTheta = dot(wi, payload.ShadingNormal);
            if(cosTheta < 0) return vec3(0);
			vec3 f = Sample_F(light_wo, light_wi, m) * cosTheta;
			float p_b = max(50, Sample_PDF(light_wo, light_wi, m));  //PDF函数有问题，临时加个限制值
			if(f.x <= 0 && f.y <= 0 && f.z <= 0) return vec3(0); 
			float light_p = 1.0;  //选中这个光源的概率？
			float p_l = light_p * pdf; // pdf: distribution u v 方向概率的积

            float w_l = PowerHeuristic(1, p_l, 1, p_b);
			return w_l * lightSample_L * f / p_l * lightSample_L;

		} 
    }    
	return radiance;
}

vec3 SampleHDRLight2(vec3 p, vec2 u, BSDF bsdf, Material m)
{
	vec3 radiance = vec3(0);
	float pdf;

	// 采样hdr方向
	vec2 uv = PiecewiseConstant2D_Sample(u, pdf);
	pdf = pdf / (4 * M_PI);
	uv.y = 1 - uv.y; 
	vec3 wLight = EqualAreaSquareToSphere(uv);

	// 转EXR坐标系
	float tmp 	= wLight.y;
	wLight.y 	= wLight.z;
	wLight.z 	= tmp;
	// HDR旋转
	wLight = Mul(Hdr.RotateYInv, wLight.xyz);
	
	
	// 判断HDR是否被遮挡
	Ray LightRay;
	LightRay.o = p;
	LightRay.d = normalize(wLight);
	
	// 默认被遮挡
	shadowed = true;  
	uint flags = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT; 
	traceRayEXT(topLevelAS, flags, 0xFF, 0, 0, 1, LightRay.o, MINIMUM, LightRay.d, INFINITY, 2); 

    vec3 wo = -payload.rayDir; 
	vec3 wi = LightRay.d;
    vec3 light_wo = ToLocal(bsdf.ShadingFrame, normalize(wo));
    vec3 light_wi = ToLocal(bsdf.ShadingFrame, normalize(wi)); 
 	
	if (!shadowed) { 
		vec3 EXRRay = Mul(Hdr.RotateY, LightRay.d).xzy;
		vec2 uv = EqualAreaSphereToSquare(EXRRay);	
		uv.y = 1 - uv.y; 
		vec3 lightSample_L = texture(HDRs[0], uv).rgb * rp.hdrMultiplier; //* vec3(1.0,1.0,0.0); 改变色相偏黄 
		float cosTheta = AbsDot(wi, payload.ShadingNormal);
		vec3 F = Sample_F(light_wo, light_wi, m); 	//DisneyEval(material, lightDir, bsdfSample.pdf);  
		float BSDF_SamplePdf = Sample_PDF(light_wo, light_wi, m);
		float light_p = 1.0;
		float Light_SamplePDF = light_p * pdf;

		float misWeight = PowerHeuristic(1, Light_SamplePDF, 1, BSDF_SamplePdf); 
		if (misWeight > 0.0)
			radiance += misWeight * F * cosTheta * lightSample_L / (Light_SamplePDF + EPS); 
	}  

	return radiance;
}

vec3 sampleDistantLight(vec2 r, vec3 wpos, vec3 beta)
{
	vec3 radiance = vec3(0.0);
	LightSampleContext ctx;
	ctx.p = wpos;
	LightLiSample liSample;
	//SampleLights(ctx, Lights[0],liSample); 
	liSample = SampleLights(ctx, Lights[0]); 
	vec3 L = vec3(0);
	Ray LightRay;
	LightRay.o = wpos;
	LightRay.d = liSample.wi;
	//if(dot(payload.ShadingNormal,LightRay.d) < 0.0){
	//	return radiance;
	//}
	// 跳过close hit
	uint flags = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT;  
	shadowed = true;   
	traceRayEXT(topLevelAS, flags, 0xFF, 0, 0, 1, LightRay.o, MINIMUM, LightRay.d, INFINITY, 2); 
	if(!shadowed) {
		radiance += liSample.L * beta;
	} 
	else
		radiance += liSample.L * beta * 0.3;
	return radiance;
}



/*
	解析法线贴图
*/
vec3 GetMaterialNormal(Material M, vec2 UV, vec3 ShadingNormal)
{
	/*
		默认着色法线
	*/
	vec3 Normal = ShadingNormal;
	/*
		法线贴图索引大于0, 从法线贴图获取法线
	*/
	int NormalMapTextureIndex = GetNormalMapTextureIndex(M);
	if (NormalMapTextureIndex >= 0)
    { 
		Frame frame = FromZ(ShadingNormal);
		Normal = texture(TextureMaps[NormalMapTextureIndex], UV).xyz;
		Normal = FromLocal(frame, normalize(Normal * 2.0 - 1.0));
	} 
	return Normal;
}


void main()
{ 		 
	MeshInstance instance = instances[gl_InstanceCustomIndexEXT]; 
	uvec2 offsets = Offsets[gl_InstanceCustomIndexEXT];
	Triangle tri = unpackTriangle(gl_PrimitiveID, 32, offsets.x, offsets.y, instance, attribs);  //16 字节对齐数据    
	InstanceTransform iTrans = transforms[gl_InstanceCustomIndexEXT];  
  
	mat4 matrix = iTrans.matrix;
	vec3 wpos = vec3(matrix * vec4(tri.pos,1.0)); 
	mat3 normalMatrix = transpose(inverse(mat3(matrix)));
	vec3 GeometryNormal = normalize(normalMatrix * tri.normal);
	
	//todo: GeometryNormal存在反向问题，强制反转(临时方案)
	/*
		着色法线
	*/
	vec3 ShadingNormal = faceforward(GeometryNormal, -gl_WorldRayDirectionEXT);

	vec2 uv = vec2(tri.uv.x, tri.uv.y);

	//Material
	uint uMatId = instance.uMatId;
	Material material = matls[uMatId];
	//uv *= 0.01;
	
	/*
		解析纹理
	*/
	vec3 Albedo = GetMaterialAlbedo(material, uv);

	/*
		解析法线贴图
	*/
	vec3 Normal = GetMaterialNormal(material, uv, ShadingNormal);
	GeometryNormal = Normal;
	ShadingNormal  = GeometryNormal;


    payload.position = wpos;
    payload.GeometryNormal = GeometryNormal; 
	payload.ShadingNormal  = ShadingNormal;

	uint sampleIndex = gl_LaunchIDEXT.y * gl_LaunchSizeEXT.x + gl_LaunchIDEXT.x;
	uint numSamples = gl_LaunchSizeEXT.x * gl_LaunchSizeEXT.y;
 
	vec2 xi = vec2(rnd(payload.seed), rnd(payload.seed));//hammersley(sampleIndex, numSamples);
	float u2 = rnd(payload.seed);	
	float u = rnd(payload.seed);

	vec3 wo = -payload.rayDir;
	vec3 n = normalize(payload.ShadingNormal);

	vec3 L 		= payload.L;
	vec3 beta 	= payload.beta; 

	BSDF bsdf = ConstructBSDF(n, n); // 构造BSDF  
	BSDFSampleInput SampleInput;
	SampleInput.m = material;
	SampleInput.uv = uv;
	if (GetMaterialType(material) != Disney) 
	{   
		BSDFSample bsdfsample = SampleBSDF(bsdf, wo, u2, xi, SampleInput); // 采样BSDF  
		vec3 wi = bsdfsample.wi;	 // 新射线反向  
		
		//if(payload.depth == 0) 
		{//自发光
			for(int i = 0;i < rp.areaLightCount; i++){
				if(uint(areaLightInstance[i].v.x) == gl_InstanceCustomIndexEXT){
					uint lightIndex = uint(areaLightInstance[i].v.y);
					Light areaLight = AreaLights[lightIndex];
					vec3 Le = GetColor(areaLight) * GetPower(areaLight); 
					L += Le * payload.beta;
					break;
				}
			}
		} 

		//beta *=  Albedo; 
		 
		//if(bsdfsample.f.x > 0 )
			beta *= bsdfsample.f * AbsDot(wi, n) / (bsdfsample.pdf + EPS);
		
		if(bsdfsample.wi == vec3(0)){
			payload.done = true;
			return;
		}
		vec3 AllLight = vec3(0); 
		if (GetMaterialType(material) != Glass)
			AllLight += SampleHDRLight(wpos, vec2(rnd(payload.seed)), bsdf, material);
		
		// 计算光源
		AllLight += LightSampling(wpos, bsdf, u); 

		//环境光
		vec3 ambient = beta * vec3(0.03);
		AllLight += ambient;
	
		L += beta * AllLight; 	
		payload.beta 		= beta;
		payload.L 			= L;
		payload.rayOrigin 	= payload.position; 	
		payload.rayDir 		= wi;  
	}
	else { //Disney 材质  
		int RoughnessMapTextureIndex = GetDisneyRoughnessMap(material);
		if (RoughnessMapTextureIndex >= 0) { // 金属度贴图
			float roughness = texture(TextureMaps[RoughnessMapTextureIndex], uv).x;
			SetDisneyRoughness(material, roughness);
		}

		int MetallicMapTextureIndex = GetDisneyMetallicMap(material);
		if (MetallicMapTextureIndex >= 0) { // 金属度贴图
			float metallic = texture(TextureMaps[MetallicMapTextureIndex], uv).x;
			SetDisneyMetallic(material, metallic);
		}

		vec3 Le = GetDisneyEmission(material) * GetDisneyEmissionTint(material) * beta; 
		L += Le; 

		// 计算光源 （直接光照）
		vec3 L_amb = vec3(0.03);
		vec3 L_hdr = SampleHDRLight2(wpos, vec2(rnd(payload.seed)), bsdf, material);
		vec3 L_lights = LightSampling(wpos, bsdf, u);  
		vec3 Li = (L_amb + L_hdr +  L_lights) * beta;  
		L += Li; 

 
		//（间接光照）
		//bsdf 采样 积分
		BSDFSample bsdfsample = SampleBSDF(bsdf, wo, u2, xi, SampleInput);  
		// 新射线反向
		vec3 wi = bsdfsample.wi;	
		vec3 f = bsdfsample.f;
		float pdf = bsdfsample.pdf; 
		float cosTheta = AbsDot(wi, n);

		if(wi == vec3(0) || pdf <= 0.0){
			payload.done = true;
			return;
		}  

		beta *= f * cosTheta / pdf;	 

		//if (dot(n, wi) < 0.0)
		//	payload.absorption = -log(material.extinction.xyz) / (material.atDistance + EPS);
 #if 1
        if (payload.depth > 1) {
            float q = min(0.95f, Luminance(beta));
            if (q == 0 || q < rand(seed))
                payload.done = true;
            else
                beta /= q;
        }
#endif
		payload.beta 		= beta;
		payload.L 			= L;
		payload.rayOrigin 	= payload.position; 	
		payload.rayDir 		= wi;  
	}

}
