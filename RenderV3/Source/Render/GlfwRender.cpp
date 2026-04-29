#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <optional>
#include <set>
#include <chrono>


#define GLFW_INCLUDE_VULKAN
#include <glfw3.h> 

#include <stb_image.h>  
#include <stb_image_write.h>

#include <VulkanRHI1/Vulkan.h>
#include <VulkanRHI1/VulkanRHI.h>
#include <VulkanRHI1/VulkanObjects/VulkanBuffer.h>
#include <VulkanRHI1/VulkanObjects/Texture/VulkanTexture.h>
#include <RHIPixelFormat.h>
#include <RHIShaderBindings.h>

//#include <Test/Common.h>
#include <Image/Image.h>
#include <Memory/CPU/CpuMemoryResource.h>

#include "GlfwRender.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>


#define FB_COLOR_FORMAT RHIPixelFormat::PF_R8G8B8A8_UBYTE

GlobalContext g_context;

GlfwRender::GlfwRender(std::string scene)
{
    /*
        解析场景文件
    */
    LoadSceneFile(scene);
    m_width = m_pMainScene->m_Resolution.x;
    m_height = m_pMainScene->m_Resolution.y;

    m_pMainScene->m_outputGamma;
    m_pMainScene->m_exposure;
    m_pMainScene->m_tonemapType;

    m_pMainCamera = m_pMainScene->m_pMainCamera;
    g_context.ActiveCamera = m_pMainCamera;

    /*
        创建窗口
    */
    InitWindow();
    /*
        创建RHI
    */
    InitRHI();
    /*
        创建渲染资源
    */
    PrepareForRendering();
}

GlfwRender::~GlfwRender()
{
    std::cout << "~GlfwRender()" << std::endl;
    glfwDestroyWindow(window);
    glfwTerminate();
}
 
void GlfwRender::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    float sensitivity = 0.1; // change this value to your liking

    float xp = static_cast<float>(xpos);
    float yp = static_cast<float>(ypos);
    if (g_context.bMouseDown) {
        float xoffset = xp - g_context.lastX;
        float yoffset = g_context.lastY - yp; // reversed since y-coordinates go from bottom to top
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        g_context.g_yawAngle = xoffset;
        g_context.g_rollAngle = yoffset;
        //if (g_rollAngle > 89.0f)
        //    g_rollAngle = 89.0f;
        //if (g_rollAngle < -89.0f)
        //    g_rollAngle = -89.0f; 
        g_context.lastX = xp;
        g_context.lastY = yp;
        g_context.g_rotate = true;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        g_context.bMouseDown = true;
        g_context.lastX = xp;
        g_context.lastY = yp;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        g_context.bMouseDown = false;
    }
}
 
void GlfwRender::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    g_context.ActiveCamera->zoom((float)yoffset);
    g_context.zoom = true;
}

void GlfwRender::InitWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); 
    window = glfwCreateWindow(m_width, m_height, "Vulkan", nullptr, nullptr); 

    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
}

void GlfwRender::recreateSwapChain()
{
    int width = 0;
    int height = 0;
    while (width == 0 || height == 0) 
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }
}


void GlfwRender::InitRHI()
{
    m_pRHI = new VulkanRHI1(window);
    m_pRHI->GetDevice()->SetupFormats();//预设定像素格式映射
}

void GlfwRender::CreateStorageImages()
{
    uint32_t width = m_width;
    uint32_t height = m_height; //PF_B8G8R8A8_SRGB
    printf("createStorageImages  %d,%d,%d\n ", m_pRHI->GetSwapChain()->swapChainImageFormat, m_pRHI->GetSwapChain()->swapChainExtent.width, m_pRHI->GetSwapChain()->swapChainExtent.height);

    /*
        Validation Warning: [ Undefined-Value-StorageImage-FormatMismatch-ImageView ] | MessageID = 0x13365b2
        vkCmdTraceRaysKHR(): the storage image descriptor [VkDescriptorSet 0xeed1b40000000bdd, Set 0, Binding 1, Index 0, variable "image"] is accessed
        by a OpTypeImage that has a Format operand Rgba8 (equivalent to VK_FORMAT_R8G8B8A8_UNORM) whichdoesn't match the VkImageView 0xfec0e20000000bd3 format (VK_FORMAT_B8G8R8A8_UNORM). 
        Any loads or stores with the variable will produce undefined values to the whole imamge (not just the texel being accessed). While the formats are compatible, Storage Images must exactly match. 
        Few ways to resolve this are
        1. Set your ImageView to VK_FORMAT_R8G8B8A8_UNORM and swizzle the values in the shader to match the desired results.
        2. Use the Unknown format in your shader (will need the widely supported shaderStorageImageWriteWithoutFormat feature)
        Spec information at https://docs.vulkan.org/spec/latest/chapters/textures.html#textures-format-validation
        Objects: 2
            [0] VkDescriptorSet 0xeed1b40000000bdd
            [1] VkImageView 0xfec0e20000000bd3
    */
    //StorageImageOut = m_pRHI->RHICreateTexture2D(width, height, RHIPixelFormat::PF_B8G8R8A8_SRGB, 1, TextureCreateFlags::Presentable | TextureCreateFlags::CPUReadback, RHIAccess::Unknown);
    //StorageImageOut = m_pRHI->RHICreateTexture2D(width, height, RHIPixelFormat::PF_B8G8R8A8_UBYTE, 1, TextureCreateFlags::Presentable | TextureCreateFlags::CPUReadback, RHIAccess::Unknown);
    StorageImageOut = m_pRHI->RHICreateTexture2D(width, height, RHIPixelFormat::PF_R8G8B8A8_UBYTE, 1, TextureCreateFlags::Presentable | TextureCreateFlags::CPUReadback, RHIAccess::Unknown);
    RHITexture2D* StorageImage2D = dynamic_cast<RHITexture2D*>(StorageImageOut);
    RHIUpdateTextureRegion2D region = { 0,0,0,0,width,height };
    auto BytesInRow = width * 4 * sizeof(uint8_t);
    m_pRHI->RHIUpdateTexture2D(StorageImage2D, 0, region, BytesInRow, nullptr); 

    StorageImageOutFloat = m_pRHI->RHICreateTexture2D(width, height, RHIPixelFormat::PF_R32G32B32A32_FLOAT, 1, TextureCreateFlags::Presentable | TextureCreateFlags::CPUReadback, RHIAccess::Unknown);
    RHITexture2D* StorageImage2DFloat = dynamic_cast<RHITexture2D*>(StorageImageOutFloat);
    auto BytesInRowFloat = width * 4 * sizeof(float);
    m_pRHI->RHIUpdateTexture2D(StorageImage2DFloat, 0, region, BytesInRowFloat, nullptr);
}

int GlfwRender::Run()
{
    auto start_time = std::chrono::steady_clock::now();
    const std::chrono::milliseconds duration(50);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        auto current_time = std::chrono::steady_clock::now();
        const auto elapsed = current_time - start_time;
        if (elapsed > duration) {
            processInput();
            start_time = current_time;
        }

        DrawFrame();
        if (screenshotSaved)
        {
            SaveFile("shot.png");
            screenshotSaved = false;
        }
        std::string title = std::to_string(frame);
        glfwSetWindowTitle(window, title.c_str());
    }
    m_pRHI->GetDevice()->DeviceWaitIdle();
    return 0;
}

glm::vec3 GlfwRender::GetMaterialAlbedo(uint32_t matId)
{
    return Materials[matId].GetAlbedo();
}

const Rt::Material& GlfwRender::GetMaterial(uint32_t matId) const
{
    return Materials[matId];
}

Rt::Light* GlfwRender::GetLight(uint32_t lightId) const
{
    return m_pMainScene->lights[lightId];
}

void GlfwRender::UpdateCameraUBO(uint32_t currentFrame)
{
    if (m_pMainCamera->needUpdate) {
        m_pMainCamera->update();
        ResetAccumulation();
    }
    float ratio = m_width / (float)m_height;
    Bounds2f screenWindow = Bounds2f({ -ratio, -1 }, { ratio, 1 });
    ubo.cameraFromWorld = m_pMainCamera->cameraFromWorld; //glm::lookAtLH( glm::vec3(0.0f, 2.6f, 20.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); 
    ubo.worldFromCamera = m_pMainCamera->worldFromCamera; //glm::inverse(ubo.cameraFromWorld);
    ubo.NDCFromScreen = m_pMainCamera->NDCFromScreen;
    ubo.rasterFromNDC = m_pMainCamera->rasterFromNDC; //glm::scale(glm::mat4(1.0f), glm::vec3(Resolution.x, -Resolution.y, 1)); 
    ubo.rasterFromScreen = m_pMainCamera->rasterFromScreen;// ubo.rasterFromNDC* ubo.NDCFromScreen;
    ubo.screenFromCamera = m_pMainCamera->screenFromCamera;// 
    ubo.screenFromRaster = m_pMainCamera->screenFromRaster;//glm::inverse(ubo.rasterFromScreen);
    ubo.screenFromNDC = m_pMainCamera->screenFromNDC;//glm::inverse(ubo.NDCFromScreen);
    ubo.NDCFromRaster = m_pMainCamera->NDCFromRaster;//glm::inverse(ubo.rasterFromNDC);
    ubo.NDCFromCamera = m_pMainCamera->NDCFromCamera;//ubo.NDCFromScreen * ubo.screenFromCamera;
    ubo.rasterFromCamera = m_pMainCamera->rasterFromCamera;//ubo.rasterFromNDC * ubo.NDCFromCamera;
    ubo.cameraFromRaster = m_pMainCamera->cameraFromRaster;//glm::inverse(ubo.screenFromCamera) * ubo.screenFromRaster;
    ubo.cameraFromNDC = m_pMainCamera->cameraFromNDC;//ubo.cameraFromRaster * ubo.rasterFromNDC;
    ubo.cameraFromScreen = m_pMainCamera->cameraFromScreen;//ubo.cameraFromRaster * ubo.rasterFromScreen; 

    ubo.frame = frame;
    // ubo.lightCount = Lights.size();

    UBO->UpdateData(sizeof(ubo), &ubo);

}

void GlfwRender::DrawFrame()
{
    UpdateCameraUBO(m_pRHI->CurrentImageIndex_);

    RHICommandBuffer* pCmdBuffer = m_pRHI->RHIBeginFrame();
    pCmdBuffer->SetRaytracingPipeline(pRaytracingPipeline);
    pCmdBuffer->BindDescriptorSets();
    pCmdBuffer->RaytraceDispatch(m_width, m_height, SBTs);
    m_pRHI->GetSwapChain()->CopyImage(pCmdBuffer, StorageImageOut, m_pRHI->CurrentImageIndex_);
    m_pRHI->RHIEndFrame();

    frame++;
}

void GlfwRender::SaveFile(const char* filename)
{
#if 0
    int nChannels = 4;
    auto start = std::chrono::high_resolution_clock::now();
    VulkanTexture2D* VkStorageImageOut = dynamic_cast<VulkanTexture2D*>(StorageImageOut);
    uint8_t* data = VkStorageImageOut->getData();
    Render::Image* pImage = new Render::Image(data, Point2i(m_width, m_height), 0, nChannels);
    pImage->Write(filename);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << duration.count() / 1000.0 << "ms" << std::endl;
#else
    std::cout << "saveScreenshot ..." << std::endl;
    bool supportsBlit = m_pRHI->GetDevice()->GetSupportedBlitting(&m_pRHI->GetSwapChain()->swapChainImageFormat);
    if (supportsBlit) {
        int nChannels = 4;
        std::cout << "saveScreenshot ..." << std::endl;
        uint8_t* data = m_pRHI->GetSwapChain()->BlitImage(m_pRHI->CurrentImageIndex_);
        std::cout << "saveScreenshot ..." << std::endl;
        Render::Image* pImage = new Render::Image(data, Point2i(m_width, m_height), 0, nChannels);
        pImage->Write(filename);
        m_pRHI->GetSwapChain()->ReleaseBlitImageData(data);
    }
    std::cout << "Screenshot saved to disk" << std::endl;
#endif
}


void GlfwRender::UpdateMaterialType(uint32_t matId, Rt::MaterialType Type)
{
    Materials[matId].SetMaterialType((int)Type);
    UpdateMaterialSSBO();
    ResetAccumulation();
}

void GlfwRender::UpdateMaterialAlbedo(uint32_t matId,glm::vec3 color)
{
    Materials[matId].SetAlbedo(color);
    UpdateMaterialSSBO();
    ResetAccumulation();
}

void GlfwRender::UpdateMaterialTexture(uint32_t matId, int index)
{
    int ID = Materials[matId].GetTextureID();
    /*
        保存上一个有效的纹理索引
    */
    if (ID >= 0)
        Materials[matId].SetOldTextureID(ID);

    Materials[matId].SetTextureID(index);
    std::cout << "UpdateMaterialTexture " << Materials[matId].GetOldTextureID() << " " << Materials[matId].GetTextureID() << std::endl;
    UpdateMaterialSSBO();
    ResetAccumulation();
}

void GlfwRender::UpdateMaterialNormalMap(uint32_t matId, int index)
{
    int ID = Materials[matId].GetNormalMapID();
    /*
        保存上一个有效的法线贴图索引
    */
    if (ID >= 0)
        Materials[matId].SetOldNormalMapID(ID);
    Materials[matId].SetNormalMapID(index);
    UpdateMaterialSSBO();
    ResetAccumulation();
}

void GlfwRender::UpdateGlassMaterialReflectivity(uint32_t matId, float a)
{
    Materials[matId].SetGlassReflectivity(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}


void GlfwRender::UpdateDielectricMaterialReflectivity(uint32_t matId, float a)
{
    Materials[matId].SetDielectricReflectivity(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}

void GlfwRender::UpdateMaterialTransmissivity(uint32_t matId, float a)
{
    Materials[matId].SetGlassTransmissivity(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}

/*
    更新材质URoughness
*/
void GlfwRender::UpdateMaterialURoughness(uint32_t matId, float a)
{
    Materials[matId].SetConductorURoughness(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}

/*
    更新材质URoughness
*/
void GlfwRender::UpdateMaterialVRoughness(uint32_t matId, float a)
{
    Materials[matId].SetConductorVRoughness(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}

/*
    更新迪士尼材质Metallic
*/
void GlfwRender::UpdateDisneyMaterialMetallic(uint32_t matId, float a)
{
    Materials[matId].SetDisneyMetallic(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}


/*
    更新迪士尼材质Roughness
*/
void GlfwRender::UpdateDisneyMaterialRoughness(uint32_t matId, float a)
{
    Materials[matId].SetDisneyRoughness(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}


/*
    更新迪士尼材质Subsurface
*/
void GlfwRender::UpdateDisneyMaterialSubsurface(uint32_t matId, float a)
{
    Materials[matId].SetDisneySubsurface(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}


/*
    更新迪士尼材质Specular
*/
void GlfwRender::UpdateDisneyMaterialSpecular(uint32_t matId, float a)
{
    Materials[matId].SetDisneySpecular(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}


/*
    更新迪士尼材质SpecularTint
*/
void GlfwRender::UpdateDisneyMaterialSpecularTint(uint32_t matId, float a)
{
    Materials[matId].SetDisneySpecularTint(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}


/*
    更新迪士尼材质Anisotropy
*/
void GlfwRender::UpdateDisneyMaterialAnisotropy(uint32_t matId, float a)
{
    Materials[matId].SetDisneyAnisotropy(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}


/*
    更新迪士尼材质Sheen
*/
void GlfwRender::UpdateDisneyMaterialSheen(uint32_t matId, float a)
{
    Materials[matId].SetDisneySheen(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}


/*
    更新迪士尼材质SheenTint
*/
void GlfwRender::UpdateDisneyMaterialSheenTint(uint32_t matId, float a)
{
    Materials[matId].SetDisneySheenTint(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}


/*
    更新迪士尼材质Clearcoat
*/
void GlfwRender::UpdateDisneyMaterialClearcoat(uint32_t matId, float a)
{
    Materials[matId].SetDisneyClearcoat(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}


/*
    更新迪士尼材质ClearcoatGloss
*/
void GlfwRender::UpdateDisneyMaterialClearcoatGloss(uint32_t matId, float a)
{
    Materials[matId].SetDisneyClearcoatGloss(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}


/*
    更新迪士尼材质IOR
*/
void GlfwRender::UpdateDisneyMaterialIOR(uint32_t matId, float a)
{
    Materials[matId].SetDisneyIOR(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}

/*
    更新迪士尼材质Transmission
*/
void GlfwRender::UpdateDisneyMaterialTransmission(uint32_t matId, float a)
{
    Materials[matId].SetDisneyTransmission(a);
    UpdateMaterialSSBO();
    ResetAccumulation();
}

void GlfwRender::UpdateMaterialSSBO()
{
    SSBOMaterial->UpdateData(sizeof(Rt::Material) * Materials.size(), Materials.data());
}

void GlfwRender::UpdateHDR(std::string hdrFilePath) {
    m_pMainScene->hdrFilename = hdrFilePath;
    
    {
        // clear hdr resource 
        if(ConditionalRG){
            delete ConditionalRG;
            ConditionalRG = nullptr;
        }
        if(ConditionalDataRGBA){
            delete ConditionalDataRGBA;
            ConditionalDataRGBA = nullptr;
        }
        if( MarginalRG){
            delete  MarginalRG;
             MarginalRG = nullptr;
        }
        for (auto ptr : HDRs) {
            if (ptr) {
                delete ptr;
                ptr = nullptr;
            }
        }
        HDRs.clear();
    }

    InitHDR();
    RHIShaderBinding hdrssb = RHIShaderBinding::SampledTextures(6, EStageFlags::Raygen | EStageFlags::CHit | EStageFlags::Miss, HDRs, HDRSampler);
    RHIShaderBinding conditionalRGsb = RHIShaderBinding::StorageImage(9,EStageFlags::Raygen | EStageFlags::CHit, ConditionalRG,DefaultSampler);
    RHIShaderBinding conditionalDataRGBAsb = RHIShaderBinding::StorageImage(10,EStageFlags::Raygen | EStageFlags::CHit, ConditionalDataRGBA,DefaultSampler);
    RHIShaderBinding marginalRGsb = RHIShaderBinding::StorageImage(11,EStageFlags::Raygen | EStageFlags::CHit, MarginalRG,DefaultSampler);
    RHIShaderBinding hdrParamsb = RHIShaderBinding::UniformBuffer(12, EStageFlags::Raygen | EStageFlags::CHit | EStageFlags::Miss, HDRParamsUBO);
    pRaytracingPipeline->getShaderBindings()->UpdateBinding(6, hdrssb);
    pRaytracingPipeline->getShaderBindings()->UpdateBinding(9, conditionalRGsb);
    pRaytracingPipeline->getShaderBindings()->UpdateBinding(10, conditionalDataRGBAsb);
    pRaytracingPipeline->getShaderBindings()->UpdateBinding(11, marginalRGsb);
    pRaytracingPipeline->getShaderBindings()->UpdateBinding(12, hdrParamsb);
    pRaytracingPipeline->getShaderBindings()->Update();
    pRaytracingPipeline->create();
    ResetAccumulation();
}

void GlfwRender::UpdateHDRUBO()
{
    HDRParamsUBO->UpdateData(sizeof(hdrUBO), &hdrUBO);
}

void GlfwRender::UpdateHDRRotate(float theta)
{
    glm::mat4 HDRRotateY = glm::mat4(1);
    HDRRotateY = glm::rotate(HDRRotateY, glm::radians(360.f - theta), glm::vec3(0, 1, 0));

    glm::mat4 HDRRotateYInv = glm::mat4(1);
    HDRRotateYInv = glm::rotate(HDRRotateYInv, glm::radians(theta - 360.f), glm::vec3(0, 1, 0));

    hdrUBO.RotateY      = HDRRotateY;
    hdrUBO.RotateYInv   = HDRRotateYInv;
    UpdateHDRUBO();
    ResetAccumulation();
}

void GlfwRender::UpdateLightAlbedo(uint32_t LightId, glm::vec3 color)
{
    Lights[LightId].p1.r = color.r;
    Lights[LightId].p1.g = color.g;
    Lights[LightId].p1.b = color.b;
    SSBOLight->UpdateData(sizeof(Rt::RenderLight) * Lights.size(), Lights.data());
    std::cout << "LightId " << LightId << " " << color.r << " " << color.g << " " << color.b << " " << std::endl;
    ResetAccumulation();
}

void GlfwRender::ClearTexture(uint32_t matId)
{
    Materials[matId].SetTextureID(-1);
    SSBOMaterial->UpdateData(sizeof(Rt::Material) * Materials.size(), Materials.data());
    ResetAccumulation();
}

void GlfwRender::updateTexture(uint32_t matId, const char* filename)
{   
    int index = Materials[matId].GetTextureID();
    //if (index < 0)
    //{
    //    /*
    //        清空按钮会保存纹理ID, update的时候恢复这个ID。
    //    */
    //    index = Materials[matId].GetOldTextureID();
    //    if (index >= 0)
    //    {
    //        Materials[matId].SetTextureID(index);
    //    }
    //}
    //if (index < 0)
    //{
    //    std::cout << "no update" << std::endl;
    //    return;
    //}

    int width, height, nrChannels;
    stbi_uc* data = stbi_load(filename, &width, &height, &nrChannels, STBI_rgb_alpha);
    if (data) {
        RHITexture* Texture_ = m_pRHI->RHICreateTexture2D(width, height, RHIPixelFormat::PF_R8G8B8A8_UBYTE, 1, TextureCreateFlags::ShaderResource, RHIAccess::Unknown);
        RHITexture2D* Texture2D = dynamic_cast<RHITexture2D*>(Texture_);
        RHIUpdateTextureRegion2D region(0, 0, 0, 0, width, height);
        auto BytesInRow = width * nrChannels * sizeof(uint8_t);
        m_pRHI->RHIUpdateTexture2D(Texture2D, 0, region, BytesInRow, data);
        stbi_image_free(data);

        if (index == -1) { //add
            TextureMaps.push_back(Texture_);
            std::string name = filename;
            textureMap[name] = TextureMaps.size() - 1;   
            Materials[matId].p2.x = TextureMaps.size() - 1;

            printf("updateTextue: add\n");
        }
        else {//update  
            //Rt::RtTexture* pTexture = m_pMainScene->textures[index];
            //std::string name = pTexture->name_;
            TextureMaps[index] = Texture_; 
            printf("updateTextue: update\n");
        }  
        RHIShaderBinding sb = RHIShaderBinding::SampledTextures(3, EStageFlags::CHit | EStageFlags::AHit, TextureMaps, DefaultSampler);
        pRaytracingPipeline->getShaderBindings()->UpdateBinding(3, sb);
        pRaytracingPipeline->getShaderBindings()->Update();
        pRaytracingPipeline->create(); 
        CreateShaderBindingTables();
        SSBOMaterial->UpdateData(sizeof(Rt::Material) * Materials.size(), Materials.data());
    }
    else {
        printf("updateTextue: load texture file failed\n");
    }  

    ResetAccumulation();
}

void GlfwRender::ClearNormalMap(uint32_t matId)
{
    Materials[matId].SetNormalMapID(-1);
    ResetAccumulation();
}

void GlfwRender::updateNormalMapTexture(uint32_t matId, const char* filename)
{
    int index = Materials[matId].GetNormalMapID();
    //if (index < 0)
    //{
    //    /*
    //        清空按钮会保存纹理ID, update的时候恢复这个ID。
    //    */
    //    index = Materials[matId].GetOldNormalMapID();
    //    if (index >= 0)
    //    {
    //        Materials[matId].SetNormalMapID(index);
    //    }
    //}
    //if (index < 0)
    //{
    //    std::cout << "no update" << std::endl;
    //    return;
    //}
    int width, height, nrChannels;
    stbi_uc* data = stbi_load(filename, &width, &height, &nrChannels, STBI_rgb_alpha);
    //printf("*******filename %s,width:%d,height:%d\n", filename, width, height);
    if (data) {
        RHITexture* Texture_ = m_pRHI->RHICreateTexture2D(width, height, RHIPixelFormat::PF_R8G8B8A8_UBYTE, 1, TextureCreateFlags::ShaderResource, RHIAccess::Unknown);
        RHITexture2D* Texture2D = dynamic_cast<RHITexture2D*>(Texture_);
        RHIUpdateTextureRegion2D region(0, 0, 0, 0, width, height);
        auto BytesInRow = width * nrChannels * sizeof(uint8_t);
        m_pRHI->RHIUpdateTexture2D(Texture2D, 0, region, BytesInRow, data);
        stbi_image_free(data);

        if (index == -1) { //add
            TextureMaps.push_back(Texture_);
            std::string name = filename;
            textureMap[name] = TextureMaps.size() - 1;
           // Materials[matId].p2.x = TextureMaps.size() - 1;
            Materials[matId].SetNormalMapID(TextureMaps.size() - 1);
            printf("updateTextue: add\n");
        }
        else {//update  
            //Rt::RtTexture* pTexture = m_pMainScene->textures[index];
            //std::string name = pTexture->name_;
            TextureMaps[index] = Texture_;
            printf("updateTextue: update\n");
        }
        RHIShaderBinding sb = RHIShaderBinding::SampledTextures(3, EStageFlags::CHit | EStageFlags::AHit, TextureMaps, DefaultSampler);
        pRaytracingPipeline->getShaderBindings()->UpdateBinding(3, sb);
        pRaytracingPipeline->getShaderBindings()->Update();
        pRaytracingPipeline->create();
        CreateShaderBindingTables();
       
        SSBOMaterial->UpdateData(sizeof(Rt::Material) * Materials.size(), Materials.data());
    }
    else {
        printf("updateTextue: load texture file failed\n");
    }
    ResetAccumulation();
}

void GlfwRender::UpdateLightColor(uint32_t lightId, glm::vec3 color)
{
    UpdateLightAlbedo(lightId, color);
}

VkTransformMatrixKHR toVkTransform(glm::mat4 matrix) {
    VkTransformMatrixKHR transformMatrix;
    transformMatrix.matrix[0][0] = matrix[0][0];
    transformMatrix.matrix[0][1] = matrix[0][1];
    transformMatrix.matrix[0][2] = matrix[0][2];
    transformMatrix.matrix[0][3] = matrix[0][3];

    transformMatrix.matrix[1][0] = matrix[1][0];
    transformMatrix.matrix[1][1] = matrix[1][1];
    transformMatrix.matrix[1][2] = matrix[1][2];
    transformMatrix.matrix[1][3] = matrix[1][3];

    transformMatrix.matrix[2][0] = matrix[2][0];
    transformMatrix.matrix[2][1] = matrix[2][1];
    transformMatrix.matrix[2][2] = matrix[2][2];
    transformMatrix.matrix[2][3] = matrix[2][3];
    return transformMatrix;
}


const std::vector<Rt::RtMaterial*>& GlfwRender::GetMaterials()
{
    return m_pMainScene->materials;
}

const std::vector<Rt::Light*>& GlfwRender::GetLights()
{
    return m_pMainScene->lights;
}


void GlfwRender::processInput()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    bool needToResetAccumulation = false;
    float deltaTime = 1000;
    float rotateSpeed = 10;
    if (m_pMainCamera) {
        if (g_context.zoom) { 
            m_pMainCamera->update();
            needToResetAccumulation = true;
            g_context.zoom = false;
        }

        if (g_context.g_rotate) {
            m_pMainCamera->Yaw(glm::radians(g_context.g_yawAngle * rotateSpeed));
            m_pMainCamera->Roll(glm::radians(g_context.g_rollAngle * rotateSpeed));
            m_pMainCamera->update();
            needToResetAccumulation = true;
            g_context.g_rotate = false;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            m_pMainCamera->move(Rt::FRONT, deltaTime);
            m_pMainCamera->update();
            needToResetAccumulation = true;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            m_pMainCamera->move(Rt::BACK, deltaTime);
            m_pMainCamera->update();
            needToResetAccumulation = true;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            m_pMainCamera->move(Rt::LEFT, deltaTime);
            m_pMainCamera->update();
            needToResetAccumulation = true;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            m_pMainCamera->move(Rt::RIGHT, deltaTime);
            m_pMainCamera->update();
            needToResetAccumulation = true;
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            m_pMainCamera->move(Rt::UP, deltaTime);
            m_pMainCamera->update();
            needToResetAccumulation = true;
        }

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            m_pMainCamera->move(Rt::DOWN, deltaTime);
            m_pMainCamera->update();
            needToResetAccumulation = true;
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) //加速
            m_pMainCamera->setMoveSpeed(2.0);

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) //减速
            m_pMainCamera->setMoveSpeed(0.5);
    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        screenshotSaved = true;
    }

    if (needToResetAccumulation)
        ResetAccumulation();
}
