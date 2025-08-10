#pragma once
#include <Math/Math.h>
#include <Scene/Model.h>
#include <Scene/SceneData.h>


//#include <Scene/SceneData.h>
#include <Image/Image.h>
#include <Primitive/BVHAggregate.h>
#include <Mesh/Mesh.h>
//#include <Material/Material.h>
#include <Integrator/Integrator.h>
#include <Camera/Camera.h>
#include <Film/Film.h>
#include <Light/Light.h>
#include <Math/Sampling.h>
#include <Sampler/Sampler.h>
#include <Texture/SpectrumTexture.h>
#include <Shape/Shape.h>
#include <Memory/MemoryBuffer.h>
#include <Render/RenderBuffer.h>
#include <Init.h>


#include <Texture/SpectrumRandomImageTexture.h>


/*
	cuda不能使用std::vector
*/
namespace Render {
	class Triangle;
	class TriangleMesh;
	class Primitive;
	class BVHAggregate;
	class BasicSceneBuilder;
	class Weather;
	class Material;
	class Scene {
	public:
		Scene() = default;
		~Scene();
	public: // API
		void UpdateCamera();
		void CameraMoveLeft();
		void CameraMoveRight();
		void CameraMoveForward();
		void CameraMoveBackward();
		void CameraMoveUp();
		void CameraMoveDown();
		void ShowCamera();
		void ClearSamples();
		void* GetColorBufferPointer();
		size_t GetColorBufferSize();
		void RenderOneSample();
		void ReRender();
		void UpdateCameraThenRender();
		void ResizeThenRender(int w, int h);
		void SetSampleCount(int n);
		int GetSampleCount();
		void SampleCountAddOne();
		Image* GetImagePointer(int index);
		void SetSceneData(SceneData *sd);
		SceneData* GetSceneData();
		Image* GetOutputImagePointer();
		Medium GetMedium();
		//Film GetFilm();
		Camera GetCamera();
		void SetScreenWindow(Bounds2f b);
		Bounds2f GetScreenWindow();
		void SetFov(float f);
		float GetFov();
		void SetEye(Point3f e);
		Point3f GetEye();
		void SetLookat(Point3f e);
		Point3f GetLookat();
		void SetUp(Vector3f s);
		Vector3f GetUp();
		void SetCameraFromWorld(Transform m);
		Transform GetCameraFromWorld();
		void SetWorldFromCamera(Transform m);
		Transform GetWorldFromCamera();
		void RasterToCamera(double x, double y);
		void SaveOutputImageToFile(std::string filename);
		std::string GetOutputImageFileName();
		int GetWidth();
		int GetHeight();
		float GetFrame();

		void SetSkyBaseColor(float r, float g, float b);
	public:
		void SetTextureTaggedPointer(int index, SpectrumTexture p);
		SpectrumTexture* GetTextureTaggedPointer(int index);
		SpectrumImageTexture* GetImageTexturePointer(int index);
		SpectrumConstTexture* GetConstTexturePointer(int index);
		SpectrumNoiseTexture* GetNoiseTexturePointer(int index);
		SpectrumRandomImageTexture* GetRandomTexturePointer(int index);
		int GetTextureIndexFromImageTextureIndex(int index);
		int GetTextureIndexFromConstTextureIndex(int index);
		int GetTextureIndexFromNoiseTextureIndex(int index);
		int GetTextureIndexFromRandomTextureIndex(int index);

		int GetImageTextureOffset();
		int GetConstTextureOffset();
		int GetNoiseTextureOffset();
		int GetRandomTextureOffset();
		
		void SetImageTexture(int index, Image *img);
		void SetConstTexture(int index, RGB color);
		void SetConstTexture(int index, float r, float g, float b);
		void SetNoiseTexture(int index, float scale, float freq);
		void SetRandomTexture(int index, Image* img);

		void SetMaterialTaggedPointer(int index, Material m);
		void SetDiffuseMaterial(int index, SpectrumTexture texture, Image *normalMap);
		void SetGlassMaterial(int index, float r, float t);
		void SetGlassMaterial(int index, SpectrumTexture texture, float r, float t, Image* normalMap);
		void SetMirrorMaterial(int index, SpectrumTexture texture, Image* normalMap);
		void SetConductorMaterial(int index, SpectrumTexture texture, Image* normalMap);

		void BuildScene(BasicSceneBuilder *builder); 
	private: // 内部API
		void Resize(int w, int h);
		void ResizeRendBuffer(int w, int h);
		void ResizeOutputImage(int w, int h);
		void ResizeScreenWindow(int w, int h);
		void ResizeFilm(int w, int h);
	private:
		void BuildRenderBuffer(BasicSceneBuilder* builder);
		void BuildMedia();
		void BuildFilm(BasicSceneBuilder* builder);
		void BuildCamera(BasicSceneBuilder* builder);
		void BuildImage(BasicSceneBuilder* builder);
		void BuildTexture(BasicSceneBuilder* builder);
		void BuildMaterial(BasicSceneBuilder* builder);
		void BuildGPUMemory(BasicSceneBuilder* builder);
		void BuildGPUMemoryForTriangleMeshes(const vector<unsigned int>	&cpuMeshIndexCount, const vector<unsigned int>	&cpuMeshFirstIndex);

		void BuildGPUMemoryForVertexs(const std::vector<Point3f>& vertexs);
		void BuildGPUMemoryForIndices(const std::vector<int> &indices);
		void BuildGPUMemoryForNormals(const std::vector<Normal3f> &normals);
		void BuildGPUMemoryForUV(const std::vector<Point2f>& uv);	
		void BuildGPUMemoryForTangents(const std::vector<Vector3f>& uv);

		void BuildGPUMemoryForTriangle(const std::vector<int> &indices);
		void BuildGPUMemoryForSphere(const std::vector<float>	&radius,
									 const std::vector<float>	&zMin,
									 const std::vector<float>	&zMax,
									 const std::vector<float>	&phiMax);

		void BuildGPUMemoryForPrimitives();
		void BuildGPUMemoryForBVHAggregate();

		void BuildShape(BasicSceneBuilder* builder, vector<Shape>& shape);
		void BuildPrimitive(vector<Shape>& shape);
		void BuildAccel(BasicSceneBuilder* builder, vector<Shape>& shape);
		void BuildCpuAccel(BasicSceneBuilder* builder, vector<Shape>& shape);
		void BuildGpuAccel(BasicSceneBuilder* builder, vector<Shape>& shape);
		void BilldIntegrator(BasicSceneBuilder* builder);
		void BuildSampler();		
		void BuildLight(BasicSceneBuilder* builder);
		void BuildRay();
		void BuildWeather();
		void BuildHDR();

	public:
		// 所有网格的顶点, 法线, UV, 索引.
		Point3f* gpuVertexs = nullptr;															// 顶点数组 
		Normal3f* gpuNormals = nullptr;															// 法线数组 
		Vector3f* gpuTangents = nullptr;														// 切线数组
		Point2f* gpuUV = nullptr;																// uv数组 
		unsigned int nVertices = 0;																// 顶点数量 == 法线数量 == uv数量
		unsigned int* gpuIndices = nullptr;														// 索引数组 
		unsigned int nIndices = 0;																// 索引数量
		Integrator integrator;																	// 积分器TaggedPointer
		Camera camera;																			// 相机 TaggedPointer
		Sampler sampler;																		// 采样器 TaggedPointer
		Light* light;																			// 光源 TaggedPointer
		unsigned int nLight;																	// 光源数量
		HDRLight* hdrLight;
		RenderBuffer* renderBuffer = nullptr;													// 渲染缓冲区
		int SampleCount = 0;																	// 样本数量
		// 天气
		Weather* weather;																		// 天气		
	public: // GPU 内存
		Film film;																				// Film TaggedPointer		
	private: // GPU 内存
		Medium Media;																			// 介质 TaggedPointer
		Material* Materials;																	// 材质 TaggedPointer
		MemoryBuffer<SpectrumTexture>* Textures;												// 纹理 TaggedPointer
		MemoryBuffer<SpectrumImageTexture>* ImageTextures;
		MemoryBuffer<SpectrumConstTexture>* ConstTextures;
		MemoryBuffer<SpectrumNoiseTexture>* NoiseTextures;
		MemoryBuffer<SpectrumRandomImageTexture>* RandomTextures;
		MemoryBuffer<Image>* ImageBuffer;														// Image 缓存
		MemoryBuffer<uint8_t>* ImagePixelBuffer;												// Image的像素缓存
		size_t ImageOffset = 0;
		size_t ConstOffset = 0;
		size_t NoiseOffset = 0;
		size_t RandomOffset = 0;
		Image* OutputImage = nullptr;															// 输出图像
		Bounds2f ScreenWindow;																	// 窗口宽高比
		Float Speed;																			// 相机 移动速度
		Float Fov;																				// 相机 field of view
		Point3f Eye;																			// 相机 位置
		Point3f Look;																			// 相机 lookat
		Vector3f Up;																			// 相机 上向量
		Float CameraDistance;
		Float cameraPhi;
		Float cameraTheta;
		Transform CameraFromWorld;																// 世界坐标转相机坐标 矩阵
		Transform WorldFromCamera;																// 相机坐标转世界坐标 矩阵
			
		// 网格 
		TriangleMesh* gpuTriangleMeshes;														// 三角形网格
		unsigned int nTriangleMeshes = 0;														// 三角形网格数量
		// 三角形
		Triangle* gpuTriangles = nullptr;														// 三角形的gpu内存
		unsigned int nTriangles = 0;															// 三角形数量

		// 球形
		Sphere* gpuSpheres = nullptr;															// 球的gpu内存
		unsigned int nSpheres = 0;																// 球的数量			

		// 图元
		Primitive* gpuPrimitiveForShapes;														// 所有形状的图元, 数量nShapes.	

		// 数量统计
		int nShapes = 0;																		// 形状数量 = 三角形数量 + 球数量 + 其他形状数量
		int nTriangleOffset = 0;																// 球的开始索引 = 三角形0
		int nSphereOffset = 0;																	// 球的开始索引 = 三角形数量
		int nObjectDefintions = 0;																// 对象定义数量
		int nObjectShapes = 0;																	// 所有对象形状的数量
		int nInstances = 0;																		// 实例数量
	
		// GPU内存
		MemoryBuffer<BVHAggregate>* gpuBVHAggregateBuffer = nullptr;							// nBVHAggregate个BVHAggregate
		int nBVHAggregate = 0;																	// BVHAggregate数量 = (nTriangleMeshes + nSpheres  + 其他形状数量) + nObjectDefintions + 1;
		int gpuShapeBVHAggregateOffset = 0;														// 0
		int gpuObjectBVHAggregateOffset = 0;													// nTriangleMeshes + nSpheres
		int gpuSceneBVHAggregateOffset = 0;														// nTriangleMeshes + nSpheres + nObjects	

		MemoryBuffer<Primitive>* gpuPrimitiveBuffer = nullptr;									// nPrimitives个Primitive
		int nPrimitives = 0;																	// Primitive数量	
		int gpuShapeBVHAggregatePrimitiveOffset = 0;											// 0
		int gpuObjectBVHAggregatePrimitiveOffset = 0;											// (nTriangleMeshes + nSpheres  + 其他形状数量)
		int gpuObjectShapePrimitiveOffset = 0;													// (nTriangleMeshes + nSpheres  + 其他形状数量) + nObjectDefinitions
		int gpuInstanceBVHAggregatePrimitiveOffset = 0;											// (nTriangleMeshes + nSpheres  + 其他形状数量) + nObjectDefinitions + nObjectDefinitions
	
		MemoryBuffer<TransformedPrimitive>* gpuTransformedPrimitiveBuffer = nullptr;			// nTransformedPrimitives个TransformedPrimitive
		int nTransformedPrimitives = 0;															// TransformedPrimitive数量
		int gpuObjectShapeTransformedPrimitiveOffset = 0;										// 0
		int gpuInstanceTransformedPrimitiveOffset = 0;											// nObjectDefintions

		MemoryBuffer<BVHBuildNode>* gpuBVHNodeBuffer = nullptr;									// BVH node
		int gpuShapeBVHAggregateNodeOffset = 0;
		int gpuObjectBVHAggregateNodeOffset = 0;
		int gpuSceneBVHAggregateNodeOffset = 0;

		MemoryBuffer<unsigned int>* gpuOrderedPrimitiveIndicesBuffer = nullptr;					// BVH node 图元索引	
		int gpuShapeBVHAggregateOrderedPrimitiveIndicesOffset = 0;
		int gpuObjectBVHAggregateOrderedPrimitiveIndicesOffset = 0;
		int gpuSceneBVHAggregateOrderedPrimitiveIndicesOffset = 0;
	private:
		SceneData* sceneData;											// 场景数据(CPU内存)
	private: //CPU 内存
		MemoryBuffer<BVHAggregate>* cpuShapeBVHAggregateBuffer = nullptr;						// 形状BVH, 数量nTriangleMeshes + nSpheres
		MemoryBuffer<Primitive>* cpuShapeBVHAggregatePrimitiveBuffer = nullptr;					// 形状BVH的图元, 数量nTriangleMeshes + nSpheres

		MemoryBuffer<BVHAggregate>* cpuObjectBVHAggregateBuffer = nullptr;						// 对象BVH
		MemoryBuffer<Primitive>* cpuObjectBVHAggregatePrimitiveBuffer = nullptr;				// 对象BVH的图元
		MemoryBuffer<Primitive>* cpuObjectShapePrimitiveBuffer = nullptr;						// 对象的形状的图元
		MemoryBuffer<TransformedPrimitive>* cpuObjectShapeTransformedPrimitiveBuffer = nullptr; // 对象的形状的Transform图元

		MemoryBuffer<Primitive>* cpuInstanceBVHAggregatePrimitiveBuffer = nullptr;				// 实例BVH的图元
		MemoryBuffer<TransformedPrimitive>* cpuInstanceTransformedPrimitiveBuffer = nullptr;	// 实例BVH的Transform图元

		BVHAggregate* cpuSceneBVHAggregate;
		Primitive cpuSceneBVHAggregatePrimitive;
	public:
		Image* hdr_image = nullptr;
		PiecewiseConstant2D* pc2d = nullptr;
		Point2f* u = nullptr;
	};
}