#pragma once
#include <RHI.h>
#include <VulkanRHI1/VulkanRHI.h>
#include <Scene/UniformBufferObject.h>
#include <Scene/Material.h>
#include <Scene/Instance.h>
#include <Scene/GeometryNode.h>
#include <Scene/Scene.h>
#include <Scene/Camera.h>
#include <Scene/HDRParams.h>

#include <memory>

#define USE_MESH_CACHE

#ifdef USE_MESH_CACHE
struct MeshCache {
	int meshId;
	uint32_t vertexOffset;
	uint32_t indexOffset;
};
#endif

class BaseRender
{
public:
	BaseRender();
	virtual ~BaseRender();

	struct PostProcessingParams {
		glm::vec4 params;
	};
public:
	/*
		渲染
	*/
	virtual int Run() = 0;
protected:
	/*
		初始化RHI
	*/
	virtual void InitRHI() = 0;
	/*
		创建Storage Image
	*/
	virtual void CreateStorageImages() = 0;
	/*
		更新相机UBO
	*/
	virtual void UpdateCameraUBO(uint32_t currentFrame) = 0;
	/*
		渲染一帧
	*/
	virtual void DrawFrame() = 0;
	/*
		保存到文件
	*/
	virtual void SaveFile(const char* filename) = 0;
protected:
	/*
		加载场景文件
	*/
	int LoadSceneFile(std::string File);
	/*
		创建渲染资源
	*/
	int PrepareForRendering();
	/*
		创建采样器
	*/
	void CreateSampler();
	void InitTexture();
	void InitMaterials();
	void InitHDR();
	void InitLights();
	void InitRaytracing();
	void CreateTLAS();
	void CreateMergedVertexBuffer();
	void CreateBLASs();
	void CreateUniformBuffer();
	void CreateRaytracingPipeline();
	void CreateShaderBindingTables();
	void CreatePostProcessingParamsUBO();
public:
	/*
		GPU Index
	*/
	int GpuIndex = -1;
	/*
		RHI
	*/
	VulkanRHI *m_pRHI = nullptr;
	/*
		VBO
	*/
	VulkanBuffer* VBO = nullptr;
	/*
		EBO
	*/
	VulkanBuffer* EBO = nullptr;
	VulkanBuffer* OffsetBuffer = nullptr;
	/*
		相机参数
	*/
	VulkanBuffer* UBO = nullptr;
	UniformBufferObject ubo = {};
	/*
		HRT Shaders
	*/
	RHIRayGenShader* RGShader = nullptr;
	RHIRayMissShader* RMShader = nullptr;
	RHIRayMissShader* RMShaderShadow = nullptr;
	RHIRayHitGroupShader* RHShader = nullptr;
	RHIRayHitGroupShader* RHShaderShadow = nullptr;
	/*
		HRT Pipeline 
	*/
	RHIRaytracingPipeline* pRaytracingPipeline = nullptr;
	/*
		光源参数SSBO
	*/
	VulkanBuffer* SSBOLight = nullptr;
	std::vector<Rt::RenderLight> Lights{};

	VulkanBuffer* SSBOAreaLight = nullptr;
	std::vector<Rt::RenderLight> AreaLights{};

	/*
		光源实例参数 
	*/	
	struct AreaLightInstance {
		glm::uvec4 ins;	//x : instanceId y: lightId, z: 网格三角形数量
	};
	VulkanBuffer* AreaLightInstanceSSBO = nullptr;
	std::vector< AreaLightInstance > areaLightInstances; 


	/*
		材质参数SSBO
	*/
	VulkanBuffer* SSBOMaterial = nullptr;
	std::vector<Rt::Material> Materials{};
	/*
		HDR参数UBO
		struct HDRParams {
		    glm::vec4 Bound;			 //(w, h, min, max)
		    glm::vec4 MarginalData;		 // R: min   G: max B: funcInt  A: func_size = h
		    glm::mat4 RotateY;
		};
	*/
	VulkanBuffer* HDRParamsUBO = nullptr;
	HDRParams hdrUBO = {};
	/*
		加速结构
	*/
	RHIRaytracingTLAS* TLAS = nullptr;
	std::vector< RHIRaytracingBLAS* > BLASs;
	/*
		实例SSBO
	*/
	VulkanBuffer* SSBOInstance = nullptr;
	std::vector<Instance> instances{};
	std::vector< glm::mat4 > transforms; //实例矩阵变换
	/*
		Sampler
	*/
	RHISampler* DefaultSampler = nullptr;
	RHISampler* HDRSampler = nullptr;
	/*
		Shader Binding Tables
	*/
	RHIShaderBindingTables* SBTs = nullptr;
	/*
		
	*/
	RHIShaderBindings* ShaderBindings;
	/*
		GeometryNodeSSBO
	*/
	VulkanBuffer* SSBO = nullptr;
	std::vector<GeometryNode> geometryNodes{};
	/*
		float func[h][w + 1], cdf[h][w + 1]
		R: func   G: cdf 
	*/
	RHITexture* ConditionalRG = nullptr; //2d
	/*
		vec4 ConditionaldataR[h]
		R: min   G: max B: funcInt  A: func_size = w + 1 
	*/
	RHITexture* ConditionalDataRGBA = nullptr; //1d
	std::vector<glm::vec4> ConditionalData;
	/*
		float func[h + 1], cdf[h + 1]
		R: func   G: cdf
	*/
	RHITexture* MarginalRG = nullptr; //1d  
	/*
		纹理
	*/
	std::vector<RHITexture*> TextureMaps;
	/*
		HDR
	*/
	std::vector<RHITexture*> HDRs;
	/*
		Frame Number
	*/
	int frame = 0;
	/*
	
	*/
	Rt::Scene* m_pMainScene = nullptr;
	Rt::Camera* m_pMainCamera = nullptr;
	std::map<std::string, int> textureMap;
	/*
		材质名称 <---> 材质索引
	*/
	std::map<std::string, int> materialMap;
	/*
		Storage Image
	*/
	RHITexture* StorageImageOut = nullptr;
	RHITexture* StorageImageOutFloat = nullptr;

	/*
		Post Processing
	*/
	RHIBuffer* PostProcessingParamsUBO;
	PostProcessingParams postprocessingData;

	/*
	*	优化
	*/
#ifdef USE_MESH_CACHE
	std::vector<MeshCache> meshCaches;
#endif

	int m_width   = 1920;
	int m_height = 1280;
	int32_t hdrResolution = 0;

	unsigned int nLightCount = 0;
	unsigned int nAreaLightCount = 0;
	unsigned int nAreaLightInstanceCount = 0;
};