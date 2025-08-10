#include "OffscreenRender.h"

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

#include <VulkanRHI/Vulkan.h>
#include <VulkanRHI/VulkanRHI.h>
#include <VulkanRHI/VulkanObjects/VulkanBuffer.h>
#include <VulkanRHI/VulkanObjects/Texture/VulkanTexture.h>
#include <RHIPixelFormat.h>
#include <RHIShaderBindings.h>

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h> 

#include <stb_image.h>  
#include <stb_image_write.h>

//#include <Test/Common.h>
#include <Image/Image.h>
#include <Memory/CPU/CpuMemoryResource.h>

#include <Light/HDRLight.h>
#include "GlfwRender.h"

#define TEST_DENOISER 0

#define FB_COLOR_FORMAT RHIPixelFormat::PF_R8G8B8A8_UBYTE

static VkTransformMatrixKHR toVkTransform(glm::mat4 matrix) {
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


OffscreenRender::OffscreenRender(const char* filename, int GpuIndex)
{
    BaseRender::GpuIndex = GpuIndex;

    /*
        解析场景文件
    */
    LoadSceneFile(filename);

    m_width = m_pMainScene->m_Resolution.x;//7680;// 
    m_height = m_pMainScene->m_Resolution.y;// 4320;//
    m_pMainCamera = m_pMainScene->m_pMainCamera;

    /*
        创建RHI
    */
    InitRHI();

    /*
        创建渲染资源
    */
    PrepareForRendering();
} 

OffscreenRender::~OffscreenRender()
{

}

void OffscreenRender::InitRHI()
{
    m_pRHI = new VulkanRHI(GpuIndex);
}

void OffscreenRender::CreateStorageImages()
{
    uint32_t width = m_width;
    uint32_t height = m_height; //PF_B8G8R8A8_SRGB
    StorageImageOut = m_pRHI->RHICreateTexture2D(width, height, RHIPixelFormat::PF_R8G8B8A8_UBYTE, 1, TextureCreateFlags::UAV | TextureCreateFlags::CPUReadback, RHIAccess::Unknown);
    RHITexture2D* StorageImage2D = dynamic_cast<RHITexture2D*>(StorageImageOut);
    RHIUpdateTextureRegion2D region = { 0,0,0,0,width,height }; 
    auto BytesInRow = width * 4 * sizeof(uint8_t);
    m_pRHI->RHIUpdateTexture2D(StorageImage2D, 0, region, BytesInRow, nullptr); 

    StorageImageOutFloat = m_pRHI->RHICreateTexture2D(width, height, RHIPixelFormat::PF_R32G32B32A32_FLOAT, 1, TextureCreateFlags::Presentable, RHIAccess::Unknown);
    RHITexture2D* StorageImage2DFloat = dynamic_cast<RHITexture2D*>(StorageImageOutFloat);
    auto BytesInRowFloat = width * 4 * sizeof(float);
    m_pRHI->RHIUpdateTexture2D(StorageImage2DFloat, 0, region, BytesInRowFloat, nullptr);
}

int OffscreenRender::Run()
{
    int nFrame = 500; //渲染帧数   

   // std::cout << "CommandBuffer " << m_pRHI->CommandBuffer->GetHandle() << std::endl;
    printf("start rendering... total frame num %d\n", nFrame);
    printf("frame 0\n");
    for (int i = 0; i < nFrame; i++) 
    {
        DrawFrame();
        //printf("\033[A");   //光标上移一行 //printf("\033[K");   //删除光标后内容
        if (i % 100 == 0)
        printf("frame %d\n", frame);
    }
    m_pRHI->GetDevice()->DeviceWaitIdle();
    SaveFile(m_pMainScene->m_OutputImageFilename.c_str());
    //exit(0);
    return 0;
}

void OffscreenRender::UpdateCameraUBO(uint32_t currentFrame)
{
    ubo.frame = frame;
    UBO->UpdateData(sizeof(ubo), &ubo); 
}

void OffscreenRender::DrawFrame()
{
    UpdateCameraUBO(m_pRHI->CurrentImageIndex_);

    RHICommandBuffer* pCmdBuffer = m_pRHI->RHIBeginFrame();
    //if (0 == frame)
    {
        pCmdBuffer->SetRaytracingPipeline(pRaytracingPipeline);
        pCmdBuffer->BindDescriptorSets();
        pCmdBuffer->RaytraceDispatch(m_width, m_height, SBTs);
    }
    m_pRHI->RHIEndFrame();
    frame++;
}

void OffscreenRender::SaveFile(const char* filename)
{
    int nChannels = 4;
    auto start = std::chrono::high_resolution_clock::now();
    VulkanTexture2D* VkStorageImageOut = dynamic_cast<VulkanTexture2D*>(StorageImageOut);

    uint8_t* data = VkStorageImageOut->getData();
    Render::Image* pImage = new Render::Image(data, Point2i(m_width, m_height), 0, nChannels);
    //pImage->ScalePixel(1 / 255.f);
    if (false == pImage->Write(filename))
    {
        std::cout << "write error" << std::endl;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << duration.count() / 1000.0 << "ms" << std::endl;
}


int VulkanOffscreenRender(char *scene, int GpuIndex)
{
    std::unique_ptr<OffscreenRender> Render(new OffscreenRender(scene, GpuIndex));
    return Render->Run();
}
 

