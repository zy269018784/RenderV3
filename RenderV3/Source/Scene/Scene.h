#pragma once
#include <RHIBuffer.h>
#include <Scene/BasicSceneBuilder.h>

#include <vector>
#include <string>

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/hash.hpp>

#include <Scene/Material.h>
#include <Scene/RtTexture.h>
#include <Scene/Camera.h>
#include <Scene/RtMesh.h>
#include <Scene/Light.h>

#define FOR_MESHINSTANCE_

#define BEGIN_NAMESPACE_RT  namespace Rt{
#define END_NAMEPSPACE_RT   }


namespace Rt 
{

    struct InstanceIndexToLightIndex
    {
        std::uint32_t InstanceIndex;
        std::uint32_t LightIndex;
        //std::uint32_t MeshIndex;
        std::uint32_t MeshTriangleCount;        // 网格三角形数量
    };

    class Scene {
    public:
        Scene(const char* filename); 
        ~Scene();

        void Build(BasicSceneBuilder* builder);
        void BuildFilm(BasicSceneBuilder* builder);
        void BuildCamera(BasicSceneBuilder* builder);
        void BuildImage(BasicSceneBuilder* builder);
        void BuildTexture(BasicSceneBuilder* builder);
        void BuildMaterial(BasicSceneBuilder* builder);

        void BuildShape(BasicSceneBuilder* builder);
      //  void BuildPrimitive(vector<Shape>& shape); 
        void BuildSampler();
        void BuildLight(BasicSceneBuilder* builder); 
        void BuildHDR();

        std::vector<Rt::Mesh*> GetMeshes() {
            return meshes;
        }

        std::vector<Rt::MeshInstance*> GetMeshInstances() {
            return meshInstances;
        }

        void AddMesh(Mesh* pMesh)
        {
            meshes.push_back(pMesh);
        }

        void AddMeshInstance(int meshId, glm::mat4 matrix, int matId) {
            /*
                与文件顺序保持一致
            */
            MeshInstance* pMeshInstance = new MeshInstance(meshId, matrix, matId);
            meshInstances.push_back(pMeshInstance);
        }

        void AddTexture(RtTexture* pTex) {
            textures.push_back(pTex);
        }

        void Build() {

        }

        std::vector<RtImage*> images;
        std::vector<RtTexture*> textures;
        std::vector<RtMaterial*> materials; 
        /*
            网格
        */
        std::vector<Mesh*> meshes; 
        std::vector<MeshInstance*> meshInstances;

        std::vector<Light*> lights;

        std::vector<Light*> AreaLights;

        std::map<std::string, int> textureMap;
        std::map<std::string, int> materialMap;
        std::map<std::string, int> meshMap;

        Point2i m_Resolution;
        std::string m_OutputImageFilename;
        float m_outputGamma = 2.2;
        float m_exposure = 1.0;
        int m_tonemapType = 0;

        Camera *m_pMainCamera;

        //hdr
        float power = 1.0f;
        std::string hdrFilename;
        // HDR旋转
        glm::mat4    HDRRotateY;
        glm::mat4    HDRRotateYInv;
        //float phi = 0.0f;
        //float theta = 0.0f;

        std::map<int, std::string> ImageTextureIndexToImageTextureName;					// 图片纹理索引 -> 图片纹理名称

        size_t ImageOffset = 0;
        size_t ConstOffset = 0;
        size_t NoiseOffset = 0;
        size_t RandomOffset = 0;

        /*
            实例ID->网格ID
        */
        std::map<std::uint32_t, std::uint32_t> InstanceIndexToMeshIndex;
        /*
            网格ID->面光源ID
        */
        std::map<std::uint32_t, std::uint32_t> MeshIndexToAreaLightIndex;
        /*
            网格ID->网格三角形数量
        */
        std::map<std::uint32_t, std::uint32_t> MeshIndexToMeshTriangleCount;
        /*
            实例ID->面光源ID
        */
        std::vector<InstanceIndexToLightIndex> InstanceIndexToAreaLightIndexMap;
        /*
            面光源ID->网格ID
        */
        //std::vector<InstanceIndexToLightIndex> AreaLightIndexToMeshIndexMap;
    };

}