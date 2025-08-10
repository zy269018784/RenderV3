#pragma once
#include <iostream>
#include <string>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/hash.hpp>

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

//#include <Test/HDRLoader.h>

#include <RHI.h>
#include <VulkanRHI/VulkanObjects/VulkanBuffer.h>
#include <Scene/Material.h>
#include <Scene/RtTexture.h>
#include <Scene/RtInstance.h>
#include <Scene/Instance.h>
#include <Scene/Scene.h>
#include <Scene/Camera.h>
#include <Scene/HDRParams.h>
#include <Scene/GeometryNode.h>
#include <Scene/UniformBufferObject.h>
#include <Scene/Light.h>
#include <Scene/Material.h>

#include <Render/BaseRender.h>


#define USE_HDR 0
#define USE_EXR 1

#define HDR_FORMAT USE_EXR

 
class VulkanRHI;

using DeviceAddress = uint64_t;
 

#define WIN_WIDTH  1920 //2200
#define WIN_HEIGHT 919//1220 

struct GlobalContext 
{  
    Rt::Camera* ActiveCamera = nullptr;
    bool zoom = false;  
    bool bMouseDown = false;
    float lastX = 800.0f / 2.0;
    float lastY = 600.0 / 2.0;


    bool g_rotate = false;
    float g_yawAngle = 0.0;
    float g_rollAngle = 0.0;

};

class GlfwRender : public BaseRender
{
public:
    GlfwRender(std::string scene);
    ~GlfwRender();
public:
    virtual int Run() final override;
protected:
    virtual void InitRHI() final override;
    virtual void CreateStorageImages() final override;
    virtual void UpdateCameraUBO(uint32_t currentFrame) final override;
    virtual void DrawFrame() final override;
    virtual void SaveFile(const char* filename) final override;
protected:
    void InitWindow();

    void recreateSwapChain();

    void processInput();

    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xpos, double ypos);
public:
    const std::vector<Rt::RtMaterial*>& GetMaterials();
    const std::vector<Rt::Light*>& GetLights();
private:
    void ResetAccumulation() 
    {
        frame = 1;
    }
public:
    glm::vec3 GetMaterialAlbedo(uint32_t matId);
    const Rt::Material &GetMaterial(uint32_t matId) const;
    Rt::Light* GetLight(uint32_t lightId) const;
public:
    
    /*
        更新材质类型
    */
    void UpdateMaterialType(uint32_t matId, Rt::MaterialType Type);
    /*
        更新纹理
    */
    void UpdateMaterialTexture(uint32_t matId, int index);
    /*
        更新法线贴图
    */
    void UpdateMaterialNormalMap(uint32_t matId, int index);
    /*
        更新材质RGB
    */
    void UpdateMaterialAlbedo(uint32_t matId, glm::vec3 color);
    /*
        更新材质反射率
    */
    void UpdateGlassMaterialReflectivity(uint32_t matId, float a);

    void UpdateDielectricMaterialReflectivity(uint32_t matId, float a);
    /*
        更新材质透过率
    */
    void UpdateMaterialTransmissivity(uint32_t matId, float a);
    /*
        更新材质URoughness
    */
    void UpdateMaterialURoughness(uint32_t matId, float a);
    /*
        更新材质URoughness
    */
    void UpdateMaterialVRoughness(uint32_t matId, float a);

    /*
        更新迪士尼材质Metallic
    */
    void UpdateDisneyMaterialMetallic(uint32_t matId, float a);

    /*
        更新迪士尼材质Roughness
    */
    void UpdateDisneyMaterialRoughness(uint32_t matId, float a);

    /*
        更新迪士尼材质Subsurface
    */
    void UpdateDisneyMaterialSubsurface(uint32_t matId, float a);

    /*
        更新迪士尼材质Specular
    */
    void UpdateDisneyMaterialSpecular(uint32_t matId, float a);

    /*
        更新迪士尼材质SpecularTint
    */
    void UpdateDisneyMaterialSpecularTint(uint32_t matId, float a);

    /*
        更新迪士尼材质Anisotropy
    */
    void UpdateDisneyMaterialAnisotropy(uint32_t matId, float a);

    /*
        更新迪士尼材质Sheen
    */
    void UpdateDisneyMaterialSheen(uint32_t matId, float a);

    /*
        更新迪士尼材质SheenTint
    */
    void UpdateDisneyMaterialSheenTint(uint32_t matId, float a);

    /*
        更新迪士尼材质Clearcoat
    */
    void UpdateDisneyMaterialClearcoat(uint32_t matId, float a);

    /*
        更新迪士尼材质ClearcoatGloss
    */
    void UpdateDisneyMaterialClearcoatGloss(uint32_t matId, float a);

    /*
        更新迪士尼材质IOR
    */
    void UpdateDisneyMaterialIOR(uint32_t matId, float a);

    /*
        更新迪士尼材质Transmission
    */
    void UpdateDisneyMaterialTransmission(uint32_t matId, float a);


    /*
        更新HDR 旋转
    */
    void UpdateHDRRotate(float theta);
    /*
        更新材质SSBO
    */
    void UpdateMaterialSSBO();
    /*
        更新HDR
    */
    void UpdateHDR(std::string hdrFilePath);
    /*
        更新HDR UBO
    */
    void UpdateHDRUBO();
    /*
        更新光源RGB
    */
    void UpdateLightAlbedo(uint32_t LightId, glm::vec3 color);

    void ClearTexture(uint32_t matId);

    void updateTexture(uint32_t matId, const char* filename);

    void ClearNormalMap(uint32_t matId);

    void updateNormalMapTexture(uint32_t matId, const char* filename);

    /*
        更新光源颜色
    */
    void UpdateLightColor(uint32_t lightId, glm::vec3 color);

    std::string GetHDRFileName() { return m_pMainScene->hdrFilename; }
private:
    bool screenshotSaved = false;
public:
    GLFWwindow* window = nullptr;
};