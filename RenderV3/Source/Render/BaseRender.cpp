#include "BaseRender.h"
#include <stb_image.h>  
#include <stb_image_write.h>

#include <Light/HDRLight.h>
#include <Image/Image.h>
#include <Memory/CPU/CpuMemoryResource.h>

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

#include <chrono>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/filesystem.hpp>

#include <Utils/File.h>
/*

*/
BaseRender::BaseRender()
{
    BLASs.clear();
}

BaseRender::~BaseRender()
{
   // std::cout << "~BaseRender()" << std::endl;
    if (m_pMainScene) 
    {
        delete m_pMainScene;
    }

    {
        //auto Device = ((VulkanTexture2D*)ConditionalDataRGBA)->m_VulkanImage->Device;

        //std::cout << "delete TextureMaps start" << std::endl;
        for (int i = 0; i < TextureMaps.size(); i++)
        {
            delete TextureMaps[i];
        }

        for (int i = 0; i < HDRs.size(); i++)
        {
            delete HDRs[i];
        }

        /*
            HDR算法参数
        */
        //std::cout << "delete ConditionalRG" << std::endl;
        delete ConditionalRG;

        //std::cout << "delete ConditionalDataRGBA" << std::endl;
        delete ConditionalDataRGBA;

        //std::cout << "delete MarginalRG" << std::endl;
        delete MarginalRG;

        //std::cout << "delete StorageImageOut" << std::endl;
        delete StorageImageOut;

        // std::cout << "delete StorageImageOutFloat" << std::endl;
        delete StorageImageOutFloat;
    }

    delete DefaultSampler;
    delete HDRSampler;
    delete VBO;
    delete EBO;
    delete OffsetBuffer;
    delete UBO;
    delete SSBO;
    delete SSBOInstance;
    delete SSBOLight;
    delete AreaLightInstanceSSBO;
    delete SSBOMaterial;
    /*
        面光源参数
    */
    delete SSBOAreaLight;

    /*
        后处理
    */
    delete PostProcessingParamsUBO;
    /*
        Hdr 参数
    */
    delete HDRParamsUBO;

    /*
       顶层加速结构
   */
    //std::cout << "delete TLAS" << std::endl;
    delete TLAS;

    for (int i = 0; i < BLASs.size(); i++)
    {
        delete BLASs[i];
    }

    /*
        释放shaders
    */
    delete RGShader;
    delete RMShader;
    delete RMShaderShadow;
    delete RHShader;

    delete ShaderBindings;


    /*
        释放光追管线
    */
    delete pRaytracingPipeline;
    /*
        释放着色器绑定表
    */
    //std::cout << "delete SBTs" << std::endl;
    delete SBTs;

    if (m_pRHI)
    {
        m_pRHI->Release();
        delete m_pRHI;
    }
    //printf("~BaseRender() finished\n");
}


int BaseRender::LoadSceneFile(std::string File)
{
    auto start = std::chrono::high_resolution_clock::now();

    m_pMainScene = new Rt::Scene(File.c_str());

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << __FUNCTION__ << " cost " << duration.count() / 1000.0 << "ms" << std::endl;

    if (!m_pMainScene)
    {
        std::cout << "Load Scene Failed" << std::endl;
        return -1;
    }
    return 0;
}

int BaseRender::PrepareForRendering()
{
    std::cout << __FUNCTION__ << std::endl;
    CreateSampler();
    InitHDR();
    InitRaytracing();
    CreateStorageImages();
    CreateRaytracingPipeline();
    CreateShaderBindingTables();
    return 0;
}

//void BaseRender::DrawFrame()
//{
//    UpdateCameraUBO(m_pRHI->CurrentImageIndex_);
//
//    RHICommandBuffer* pCmdBuffer = m_pRHI->RHIBeginFrame();
//    pCmdBuffer->SetRaytracingPipeline(pRaytracingPipeline);
//    pCmdBuffer->BindDescriptorSets();
//    pCmdBuffer->RaytraceDispatch(m_width, m_height, SBTs);
//    m_pRHI->SwapChain->CopyImage(pCmdBuffer, StorageImageOut, m_pRHI->CurrentImageIndex_);
//    m_pRHI->RHIEndFrame();
//
//    frame++;
//}

void BaseRender::CreateSampler()
{
    /*
        普通纹理采样器
    */
    DefaultSampler = m_pRHI->RHINewSampler();
    DefaultSampler->setAddressMode(EAddressMode::REPEAT);
    DefaultSampler->create();

    /*
        HDR采样器
    */
    HDRSampler = m_pRHI->RHINewSampler();
    HDRSampler->setAddressMode(EAddressMode::CLAMP_TO_EDGE);
    HDRSampler->create();
}

void BaseRender::InitTexture()
{
    CreateSampler();
}


void BaseRender::InitMaterials()
{
    printf("InitMaterials\n");
    //textures todo: 16位浮点型、单通道灰度图、跳过hdr
    RHITexture* Texture_ = nullptr;
    uint64_t totalTextureMemSize = 0;
    for (int i = 0; i < m_pMainScene->textures.size(); i++) {
        Rt::RtTexture* pTexture = m_pMainScene->textures[i];
        if (pTexture->type != 0) //只获取贴图
            continue;
        std::string name = pTexture->name_;
        std::string filename = pTexture->GetFilename(); //"E:\\Work\\project\\global-illumination-dev\\pbrt_optical_build_qt\\" + pTexture->GetFilename();

        int width, height, nrChannels;
        //stbi_uc* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
        stbi_uc* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
        //printf("*******filename %s,width:%d,height:%d,channels:%d\n", filename.c_str(), width, height, nrChannels);
 
        totalTextureMemSize += width * height * nrChannels;
        if (data) {
            if(nrChannels == 4 || nrChannels == 3)
                Texture_ = m_pRHI->RHICreateTexture2D(width, height, RHIPixelFormat::PF_R8G8B8A8_UBYTE, 1, TextureCreateFlags::ShaderResource, RHIAccess::Unknown); 
            else if (nrChannels == 2)
                Texture_ = m_pRHI->RHICreateTexture2D(width, height, RHIPixelFormat::PF_R8G8_UBYTE, 1, TextureCreateFlags::ShaderResource, RHIAccess::Unknown);
            else if (nrChannels == 1)
                Texture_ = m_pRHI->RHICreateTexture2D(width, height, RHIPixelFormat::PF_R8_UBYTE, 1, TextureCreateFlags::ShaderResource, RHIAccess::Unknown);

            RHITexture2D* Texture2D = dynamic_cast<RHITexture2D*>(Texture_);
            RHIUpdateTextureRegion2D region(0, 0, 0, 0, width, height);
            auto BytesInRow = width * nrChannels * sizeof(uint8_t);    
            
            if (nrChannels == 3) { //vulkan对R8G8B8  24位有限制，一般要对齐改成4通道
                std::vector<uint8_t> rgbaData(width * height * 4);
                for (int i = 0; i < width * height; i++) {
                    rgbaData[i * 4 + 0] = data[i * 3 + 0]; // R
                    rgbaData[i * 4 + 1] = data[i * 3 + 1]; // G
                    rgbaData[i * 4 + 2] = data[i * 3 + 2]; // B
                    rgbaData[i * 4 + 3] = 255;             // A
                } 
                m_pRHI->RHIUpdateTexture2D(Texture2D, 0, region, BytesInRow, rgbaData.data());
            }
            else {
                m_pRHI->RHIUpdateTexture2D(Texture2D, 0, region, BytesInRow, data);
            } 

            stbi_image_free(data);
            TextureMaps.push_back(Texture_);
            textureMap[name] = TextureMaps.size() - 1;
        }
        else {
            printf("error=====================================stbi_load texture error %s\n", filename.c_str());
        }
    }
    std::cout << "[SCENE] Texture buffer size = " << static_cast<double>(totalTextureMemSize) / 1024 / 1024 << " MB" << std::endl;

    if (TextureMaps.size() < 1) {//默认纹理，防止纹理数组长度为0，shader映射报错
        uint8_t* data = new uint8_t[2 * 2 * 4];
        Texture_ = m_pRHI->RHICreateTexture2D(2, 2, RHIPixelFormat::PF_R8G8B8A8_UBYTE, 1, TextureCreateFlags::ShaderResource, RHIAccess::Unknown);
        RHITexture2D* Texture2D = dynamic_cast<RHITexture2D*>(Texture_);
        RHIUpdateTextureRegion2D region(0, 0, 0, 0, 2, 2);
        auto BytesInRow = 2 * 4 * sizeof(uint8_t);
        m_pRHI->RHIUpdateTexture2D(Texture2D, 0, region, BytesInRow, data); 
        TextureMaps.push_back(Texture_);
        delete[] data;
    }
    //materials
    for (const auto& pMaterial : m_pMainScene->materials) {
        Rt::Material mat0 = pMaterial;
        int index = -1;
        if (textureMap.find(pMaterial->textureName_) != textureMap.end()) {
            index = textureMap[pMaterial->textureName_];
        }

        mat0.SetTextureID(index);//mat0.p2.x = index;      // texture id
        
        index = -1;
        if (textureMap.find(pMaterial->normalMap_) != textureMap.end()) {
            index = textureMap[pMaterial->normalMap_];
        }
        mat0.SetNormalMapID(index);//mat0.p2.y = index;
        
        index = -1;
        if (textureMap.find(pMaterial->roughnessMap_) != textureMap.end()) {
            index = textureMap[pMaterial->roughnessMap_];
        }
        mat0.SetRoughnessMapID(index); 

        index = -1;
        if (textureMap.find(pMaterial->metallicMap_) != textureMap.end()) {
            index = textureMap[pMaterial->metallicMap_];
        }
        mat0.SetMetallicMapID(index);

        Materials.push_back(mat0);
        materialMap[pMaterial->name_] = Materials.size() - 1;
    }
    SSBOMaterial = dynamic_cast<VulkanBuffer*>(m_pRHI->RHICreateBuffer(sizeof(Rt::Material) * Materials.size(), BUF_StorageBuffer, 0, Materials.data()));
}

void BaseRender::InitHDR()
{
    MemoryResource* CpuemoryResource = &Render::CpuMemoryResource::singleton;
    Allocator* alloc = nullptr;
    alloc = new Allocator(CpuemoryResource);
    std::cout 
        << "hdr exists " << boost::filesystem::exists(m_pMainScene->hdrFilename) << "\n"
       << " hdr file " << m_pMainScene->hdrFilename << std::endl;
    Render::Image* pImageHDR = Render::Image::Read(m_pMainScene->hdrFilename, alloc);
    uint32_t hdrWidth = pImageHDR->Resolution().x;
    uint32_t hdrHeight = pImageHDR->Resolution().y;
    RHITexture* hdrTexture = m_pRHI->RHICreateTexture2D(hdrWidth, hdrHeight, RHIPixelFormat::PF_R32G32B32A32_FLOAT, 1, TextureCreateFlags::ShaderResource | TextureCreateFlags::CPUReadback, RHIAccess::Unknown);
    RHITexture2D* Texture2D = dynamic_cast<RHITexture2D*>(hdrTexture);
    RHIUpdateTextureRegion2D region(0, 0, 0, 0, hdrWidth, hdrHeight);
    auto BytesInRow = hdrWidth * 3 * sizeof(float);
    m_pRHI->RHIUpdateTexture2D(Texture2D, 0, region, BytesInRow, (uint8_t*)pImageHDR->RawPointer(Point2i(0, 0)));
    HDRs.push_back(hdrTexture); 

    Transform renderFromLight;
    auto hdrLight = HDRLight::Create(renderFromLight, 0, 0, nullptr, pImageHDR, 1, alloc[0]);
    Render::Array2D<Point2f>* Conditional;
    PiecewiseConstant2D distribution = hdrLight->distribution;
    Conditional = alloc->new_object<Array2D<Point2f>>(distribution.pConditionalV.size(), distribution.pConditionalV[0].cdf.size(), alloc[0]);

    for (int i = 0; i < distribution.pConditionalV.size(); i++)
    {
        for (int j = 0; j < distribution.pConditionalV[i].func.size(); j++)
        {
            (*Conditional)[Point2i(i, j)].x = distribution.pConditionalV[i].func[j];
            (*Conditional)[Point2i(i, j)].y = distribution.pConditionalV[i].cdf[j];
        }
        (*Conditional)[Point2i(i, distribution.pConditionalV[i].func.size())].y = distribution.pConditionalV[i].cdf[distribution.pConditionalV[i].func.size()];
    }

    ConditionalRG = m_pRHI->RHICreateTexture2D(hdrWidth, hdrHeight, RHIPixelFormat::PF_R32G32_FLOAT, 1, TextureCreateFlags::ShaderResource | TextureCreateFlags::UAV, RHIAccess::Unknown);
    {
        RHITexture2D* Texture2D = dynamic_cast<RHITexture2D*>(ConditionalRG);
        RHIUpdateTextureRegion2D region(0, 0, 0, 0, hdrWidth, hdrHeight);
        auto BytesInRow = hdrWidth * 3 * sizeof(float);
        m_pRHI->RHIUpdateTexture2D(Texture2D, 0, region, BytesInRow, (uint8_t*)Conditional->begin()); //todo...
    }

    ConditionalData.resize(distribution.pConditionalV.size());
    for (int i = 0; i < ConditionalData.size(); i++) {
        ConditionalData[i].r = 0;
        ConditionalData[i].g = 1;
        ConditionalData[i].b = distribution.pConditionalV[i].funcInt;
        ConditionalData[i].a = distribution.pConditionalV[i].func.size();
    }

    ConditionalDataRGBA = m_pRHI->RHICreateTexture1D(ConditionalData.size(), RHIPixelFormat::PF_R32G32B32A32_FLOAT, 1, TextureCreateFlags::ShaderResource | TextureCreateFlags::UAV, RHIAccess::Unknown);
    {
        RHITexture1D* Texture1D = dynamic_cast<RHITexture1D*>(ConditionalDataRGBA);
        RHIUpdateTextureRegion1D region(0, 0, ConditionalData.size());
        m_pRHI->RHIUpdateTexture1D(Texture1D, 0, region, (uint8_t*)ConditionalData.data()); //todo...
    }

    std::vector<Point2f>  Marginal;
    Marginal.resize(distribution.pMarginal.size() + 1);
    for (int i = 0; i < distribution.pMarginal.size(); i++)
    {
        Marginal[i].x = distribution.pMarginal.func[i];
        Marginal[i].y = distribution.pMarginal.cdf[i];
    }
    Marginal[distribution.pMarginal.func.size()].y = distribution.pMarginal.cdf[distribution.pMarginal.func.size()];
 
    MarginalRG = m_pRHI->RHICreateTexture1D(hdrWidth, RHIPixelFormat::PF_R32G32_FLOAT, 1, TextureCreateFlags::ShaderResource | TextureCreateFlags::UAV, RHIAccess::Unknown);
    {
        RHITexture1D* Texture1D = dynamic_cast<RHITexture1D*>(MarginalRG);
        RHIUpdateTextureRegion1D region(0, 0, hdrWidth);
        m_pRHI->RHIUpdateTexture1D(Texture1D, 0, region, (uint8_t*)Marginal.data()); //todo...
    }

    glm::vec4 Marginaldata;
    Marginaldata.r = 0;
    Marginaldata.g = 1;
    Marginaldata.b = distribution.pMarginal.funcInt;
    Marginaldata.a = distribution.pMarginal.func.size();

    HDRParamsUBO = dynamic_cast<VulkanBuffer*>(m_pRHI->RHICreateBuffer(sizeof(HDRParams), BUF_UniformBuffer, 0, nullptr));
    hdrUBO.Bound = glm::vec4(hdrWidth, hdrHeight, 0, 1);
    hdrUBO.MarginalData = Marginaldata;// glm::vec4(hdrWidth, hdrHeight, 0, 1); //todo...
    hdrUBO.RotateY      = m_pMainScene->HDRRotateY;
    hdrUBO.RotateYInv   = m_pMainScene->HDRRotateYInv;
    void* data;

    HDRParamsUBO->MapMemory(0, sizeof(hdrUBO), 0, &data);
    memcpy(data, &hdrUBO, sizeof(hdrUBO));
    HDRParamsUBO->UnmapMemory();

    //m_pRHI->GetDevice()->MapMemory(HDRParamsUBO->DeviceMemoryHandle.Handle, 0, sizeof(hdrUBO), 0, &data);
    //memcpy(data, &hdrUBO, sizeof(hdrUBO));
    //m_pRHI->GetDevice()->UnmapMemory(HDRParamsUBO->DeviceMemoryHandle.Handle);
}

void BaseRender::InitLights()
{
    printf("InitLights\n");
    for (int i = 0; i < m_pMainScene->lights.size(); i++) {
        m_pMainScene->lights[i]->Print();
        Rt::RenderLight pRenderLight;
        pRenderLight.p1 = m_pMainScene->lights[i]->p1;
        pRenderLight.p2 = m_pMainScene->lights[i]->p2;
        pRenderLight.p3 = m_pMainScene->lights[i]->p3;
        std::cout << "light pos " << pRenderLight.p3.x << " " << pRenderLight.p3.x << " " << pRenderLight.p3.x << std::endl;
        pRenderLight.p4 = m_pMainScene->lights[i]->p4;
        pRenderLight.p5 = m_pMainScene->lights[i]->p5;
        pRenderLight.WorldFromLight = m_pMainScene->lights[i]->WorldFromLight;
        Lights.push_back(pRenderLight);
    }
    nLightCount = Lights.size();
    if (nLightCount < 1) {
        Rt::RenderLight pRenderLight;
        Lights.push_back(pRenderLight);
    }
    SSBOLight = dynamic_cast<VulkanBuffer*>(m_pRHI->RHICreateBuffer(sizeof(Rt::RenderLight) * Lights.size(), BUF_StorageBuffer, 0, Lights.data()));


    /*
        Area Lights
    */
    for (int i = 0; i < m_pMainScene->AreaLights.size(); i++) {
        m_pMainScene->AreaLights[i]->Print();
        Rt::RenderLight pRenderLight;
        pRenderLight.p1 = m_pMainScene->AreaLights[i]->p1;
        pRenderLight.p2 = m_pMainScene->AreaLights[i]->p2;
        pRenderLight.p3 = m_pMainScene->AreaLights[i]->p3;
        pRenderLight.p4 = m_pMainScene->AreaLights[i]->p4;
        pRenderLight.p5 = m_pMainScene->AreaLights[i]->p5;
        printf("light visible : %d \n", pRenderLight.p2.g);
        pRenderLight.WorldFromLight = m_pMainScene->AreaLights[i]->WorldFromLight;
        AreaLights.push_back(pRenderLight);
    }
    nAreaLightCount = AreaLights.size();
    if (nAreaLightCount < 1) {
        Rt::RenderLight pRenderLight;
        AreaLights.push_back(pRenderLight);
    }
    SSBOAreaLight = dynamic_cast<VulkanBuffer*>(m_pRHI->RHICreateBuffer(sizeof(Rt::RenderLight) * AreaLights.size(), BUF_StorageBuffer, 0, AreaLights.data()));

    //光源实例化缓存数据
    nAreaLightInstanceCount = m_pMainScene->InstanceIndexToAreaLightIndexMap.size();
    for (int i = 0; i < nAreaLightInstanceCount; ++i) {
        Rt::InstanceIndexToLightIndex ins = m_pMainScene->InstanceIndexToAreaLightIndexMap[i];
        AreaLightInstance areaLightInstance;
        areaLightInstance.ins = glm::uvec4(ins.InstanceIndex, ins.LightIndex, ins.MeshTriangleCount, 0);
        printf("InstanceIndex : %d, LightIndex : %d, MeshTriangleCount %d\n", ins.InstanceIndex, ins.LightIndex, ins.MeshTriangleCount);
        areaLightInstances.push_back(areaLightInstance);
    }

    if (nAreaLightInstanceCount < 1) {
        AreaLightInstance areaLightInstance;
        areaLightInstances.push_back(areaLightInstance);
    }
    AreaLightInstanceSSBO = dynamic_cast<VulkanBuffer*>(m_pRHI->RHICreateBuffer(sizeof(AreaLightInstance) * areaLightInstances.size(), BUF_StorageBuffer, 0, areaLightInstances.data()));
}

void BaseRender::InitRaytracing()
{
    printf("initRaytracing*******\n");
    RGShader = m_pRHI->RHILoadRayGenShader("asset/shaders/rtscene/rtscene_rgs.spv");
    RMShader = m_pRHI->RHILoadRayMissShader("asset/shaders/rtscene/rtscene_rms.spv");
    RMShaderShadow = m_pRHI->RHILoadRayMissShader("asset/shaders/rtscene/shadow_rms.spv");
    RHShader = m_pRHI->RHILoadRayHitGroupShader("asset/shaders/rtscene/rtscene_rhs.spv", "asset/shaders/rtscene/rtscene_ras.spv", 0); //closesthit 
    //RHShaderShadow = m_pRHI->RHILoadRayHitGroupShader(0, "asset/shaders/rtscene/shadow_ras.spv", 0); //closesthit 
    InitMaterials();
    InitLights();

#ifdef FOR_MESHINSTANCE_
    CreateBLASs();
#else
    createBLAS();//需要先创建 
#endif 
    CreateTLAS();

    CreateUniformBuffer();
    CreatePostProcessingParamsUBO();
    printf("initRaytracing end*********\n");
}

void BaseRender::CreateTLAS()
{
#ifdef FOR_MESHINSTANCE_
    TLAS = m_pRHI->RHINewRaytracingTLAS(BLASs, transforms);
#else
    TLAS = m_pRHI->RHINewRaytracingTLAS(BLAS->GetDeviceAddress());
#endif
}

void BaseRender::CreateMergedVertexBuffer()
{
#ifdef USE_MESH_CACHE
    meshCaches.clear();
#endif

    std::vector<uint32_t> indices;
    std::vector<Rt::Vertex> vertices;
    std::vector<glm::uvec2> offsets;
    std::cout << "instance count : " << m_pMainScene->meshInstances.size() << std::endl;
    /*
         合并实例化的网格的VBO
    */
    for (const auto& meshInstance : m_pMainScene->meshInstances) {
        auto& mesh = m_pMainScene->meshes[meshInstance->meshId];
        glm::mat4 modelMatrix = meshInstance->modelTransform;
        glm::mat4 modelTransInvMatrix = transpose(inverse(modelMatrix)); //需要转秩 
        transforms.push_back(modelMatrix);  //

#if 0 //预处理世界矩阵变换  

        for (int i = 0; i < mesh->vertices.size(); i++) {
            PBRT::Vertex& v = mesh->vertices[i];
            v.pos = modelMatrix * glm::vec4(v.pos, 1.0f);
            v.normal = normalize(modelTransInvMatrix * glm::vec4(v.normal, 1.f));
        }
#endif  

#ifndef USE_MESH_CACHE
        const auto vertexOffset = static_cast<uint32_t>(vertices.size());
        const auto indexOffset = static_cast<uint32_t>(indices.size());
        offsets.emplace_back(vertexOffset, indexOffset);

        vertices.insert(vertices.end(), mesh->vertices.begin(), mesh->vertices.end());
        indices.insert(indices.end(), mesh->indices.begin(), mesh->indices.end());
#else
        bool hasMesh = false;
        for (int i = 0; i < meshCaches.size(); i++) {
            if (meshInstance->meshId == meshCaches[i].meshId) {
                hasMesh = true;
                const auto vertexOffset = meshCaches[i].vertexOffset;
                const auto indexOffset = meshCaches[i].indexOffset;
                offsets.emplace_back(vertexOffset, indexOffset);
                //printf("has Mesh %d,%d,%d\n", meshCaches[i].meshId, vertexOffset, indexOffset);
                break;
            }
        }
        if (!hasMesh) {
            const auto vertexOffset = static_cast<uint32_t>(vertices.size());
            const auto indexOffset = static_cast<uint32_t>(indices.size());
            offsets.emplace_back(vertexOffset, indexOffset);
            MeshCache meshCache;
            meshCache.meshId = meshInstance->meshId;
            meshCache.vertexOffset = vertexOffset;
            meshCache.indexOffset = indexOffset;
            meshCaches.push_back(meshCache);
            //printf("%d, %d,%d,\n", meshInstance->meshId, vertexOffset, indexOffset);
            vertices.insert(vertices.end(), mesh->vertices.begin(), mesh->vertices.end());
            indices.insert(indices.end(), mesh->indices.begin(), mesh->indices.end());
        }
        //_sleep(100); 
#endif
    }

   
    std::cout << "[SCENE] Vertex buffer size = " << vertices.size() << std::endl;
    // =============== VERTEX BUFFER =============== 
    auto size = sizeof(Rt::Vertex) * vertices.size(); 
    //VBO.reset(dynamic_cast<VulkanBuffer*>(m_pRHI->RHICreateBuffer(size, BUF_AccelerationStructure, sizeof(Rt::Vertex), vertices.data())));
    VBO = dynamic_cast<VulkanBuffer*>(m_pRHI->RHICreateBuffer(size, BUF_AccelerationStructure, sizeof(Rt::Vertex), vertices.data()));
    std::cout << "[SCENE] Vertex buffer size = " << static_cast<double>(size) / 1024 / 1024 << " MB" << std::endl;

    // =============== INDEX BUFFER =============== 
    size = sizeof(indices[0]) * indices.size();
    EBO = dynamic_cast<VulkanBuffer*>(m_pRHI->RHICreateBuffer(size, BUF_AccelerationStructure, 0, indices.data()));
    std::cout << "[SCENE] Index buffer size = " << static_cast<double>(size) / 1024 / 1024 << " MB" << std::endl;

    // =============== OFFSET BUFFER =============== 
    size = sizeof(offsets[0]) * offsets.size();
#if 0
    ReadOffsetFile("OffsetBuffer.bin", offsets);
#endif
    OffsetBuffer = dynamic_cast<VulkanBuffer*>(m_pRHI->RHICreateBuffer(size, BUF_StorageBuffer, 0, offsets.data()));
    std::cout << "[SCENE] Offset buffer size = " << static_cast<double>(size) / 1024 / 1024 << " MB" << std::endl;
#if 0
    std::cout << "Offset Test========================================" << std::endl;
    for (int i = 0; i < 10; i++)
    {
        std::cout << offsets[i].x << " " << offsets[i].y << std::endl;
    }
    auto start = std::chrono::high_resolution_clock::now();

    WriteFile("VBO.bin", (const char*)vertices.data(), vertices.size() * sizeof(Rt::Vertex));
    WriteFile("EBO.bin", (const char*)indices.data(), indices.size() * sizeof(std::uint32_t));
    WriteFile("OffsetBuffer.bin", (const char *)offsets.data(), offsets.size() * sizeof (glm::uvec2));

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "load vbo ebo cost 2============ " << duration.count() / 1000 << " ms " << std::endl;
    //ReadOffsetFile("OffsetBuffer.bin", offsets);
#endif
}

void BaseRender::CreateBLASs()
{
    CreateMergedVertexBuffer();
    std::vector< RHIRaytracingGeometryDataSimple* > geoDatas;

    uint32_t vertexOffset = 0;
    uint32_t indexOffset = 0;

    std::cout << "m_pMainScene->GetMeshInstances() " << m_pMainScene->GetMeshInstances().size() << std::endl;
    for (const auto& instance : m_pMainScene->GetMeshInstances()) {
        Rt::Mesh* pMesh = m_pMainScene->GetMeshes()[instance->meshId];
        if (pMesh == nullptr) {
            printf("mesh %d not found\n", instance->meshId);
            exit(-1);
        }

        RHIRaytracingASGeometry* pGeo = new RHIRaytracingASGeometry();
        pGeo->vertexAddress = VBO->DeviceAddress;
        pGeo->indexAddress = EBO->DeviceAddress;
        pGeo->MaxVertices = pMesh->vertices.size();
        pGeo->indexCount = pMesh->indices.size();
        pGeo->VertexBufferStride = pMesh->GetStride();
#ifndef USE_MESH_CACHE
        pGeo->VertexBufferOffset = vertexOffset;
        pGeo->IndexBufferOffset = indexOffset;
#else
        for (int i = 0; i < meshCaches.size(); i++) {
            if (instance->meshId == meshCaches[i].meshId) {
                pGeo->VertexBufferOffset = meshCaches[i].vertexOffset * pGeo->VertexBufferStride; 
                pGeo->IndexBufferOffset = meshCaches[i].indexOffset * sizeof(Rt::IndexType);
                break;
            }
        }
#endif 

        RHIRaytracingGeometryDataSimple* geoData = new RHIRaytracingGeometryDataSimple();
        geoData->pASGeometry = pGeo;

       // auto start1 = std::chrono::high_resolution_clock::now();
        RHIRaytracingBLAS* BLAS = m_pRHI->RHINewRaytracingBLAS(geoData);
       // auto end1 = std::chrono::high_resolution_clock::now();
       // auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
       // std::cout << "RHINewRaytracingBLAS cost " << duration1.count() / 1000.0 << "ms" << std::endl;

        //printf("BLAS->GetSizeInfo().ResultSize %u\n", BLAS->GetSizeInfo().ResultSize);
        BLASs.push_back(BLAS);

        vertexOffset += pMesh->GetVertexSize();
        indexOffset += pMesh->GetIndexSize();
        geoDatas.push_back(geoData);

#if 1
        GeometryNode geometryNode{};
        geometryNode.vertexBufferDeviceAddress = VBO->DeviceAddress;
        geometryNode.indexBufferDeviceAddress = EBO->DeviceAddress;
        int matId = instance->materialId;// materialMap[pMesh->MaterialName];   
        geometryNode.uMatId = static_cast<uint32_t>(matId);// 0; 
        geometryNodes.push_back(geometryNode);

        glm::mat4 modelMatrix = glm::transpose(instance->modelTransform);//这里要transpose， 跟加速结构不一致没搞明白
        glm::vec3 scale;
        glm::quat quaternion;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose(modelMatrix, scale, quaternion, translation, skew, perspective);
        glm::mat4 rotationMatrix = glm::toMat4(quaternion);
        glm::mat4 normalTransfrom = rotationMatrix;
        normalTransfrom = glm::scale(normalTransfrom, { 1.0f / scale[0], 1.0f / scale[1], 1.0f / scale[2] });

        Instance instance1{};
        instance1.matrix = modelMatrix;
        instance1.normalMatrix = normalTransfrom;
        instances.push_back(instance1);
#endif 

    }
    //boost::interprocess::named_mutex blas_lock(boost::interprocess::open_or_create_t(), "blas build");
    //blas_lock.unlock();
    // 锁文件夹))
    // blas_lock.lock();
    printf("RHIBuildBLASs START\n");
    auto start = std::chrono::high_resolution_clock::now();
    m_pRHI->RHIBuildBLASs(BLASs);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "RHIBuildBLASs cost " << duration.count() / 1000.0 << "ms" << std::endl;
    printf("RHIBuildBLASs END\n");
    //blas_lock.unlock();
    printf("build BLASs ok %d\n", (int)BLASs.size());
 
    SSBO = dynamic_cast<VulkanBuffer*>(m_pRHI->RHICreateBuffer(sizeof(GeometryNode) * geometryNodes.size(), BUF_StorageBuffer, 0, geometryNodes.data()));
    SSBOInstance = dynamic_cast<VulkanBuffer*>(m_pRHI->RHICreateBuffer(sizeof(Instance) * instances.size(), BUF_StorageBuffer, 0, instances.data()));
}


void BaseRender::CreateUniformBuffer()
{
    float ratio = m_width / (float)m_height;
    Bounds2f screenWindow = Bounds2f({ -ratio, -1 }, { ratio, 1 });
    ubo.cameraFromWorld = m_pMainCamera->cameraFromWorld; //glm::lookAtLH( glm::vec3(0.0f, 2.6f, 20.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); 
    ubo.worldFromCamera = m_pMainCamera->worldFromCamera; //glm::inverse(ubo.cameraFromWorld);
    ubo.NDCFromScreen = m_pMainCamera->NDCFromScreen; //glm::scale(glm::mat4(1.0f), glm::vec3(1 / (screenWindow.pMax.x - screenWindow.pMin.x), 1 / (screenWindow.pMax.y - screenWindow.pMin.y), 1)) * glm::translate(glm::mat4(1.0f), glm::vec3(-screenWindow.pMin.x, -screenWindow.pMax.y, 0));

    //Point2f Resolution(m_width, m_height);
    ubo.rasterFromNDC = m_pMainCamera->rasterFromNDC; //glm::scale(glm::mat4(1.0f), glm::vec3(Resolution.x, -Resolution.y, 1)); 
    ubo.rasterFromScreen = m_pMainCamera->rasterFromScreen;// ubo.rasterFromNDC* ubo.NDCFromScreen;
    ubo.screenFromCamera = m_pMainCamera->screenFromCamera;//
    // OK
    ubo.screenFromRaster = m_pMainCamera->screenFromRaster;     //glm::inverse(ubo.rasterFromScreen);
    ubo.screenFromNDC = m_pMainCamera->screenFromNDC;           //glm::inverse(ubo.NDCFromScreen);
    ubo.NDCFromRaster = m_pMainCamera->NDCFromRaster;           //glm::inverse(ubo.rasterFromNDC);
    ubo.NDCFromCamera = m_pMainCamera->NDCFromCamera;           //ubo.NDCFromScreen * ubo.screenFromCamera;
    ubo.rasterFromCamera = m_pMainCamera->rasterFromCamera;     //ubo.rasterFromNDC * ubo.NDCFromCamera;
    ubo.cameraFromRaster = m_pMainCamera->cameraFromRaster;     //glm::inverse(ubo.screenFromCamera) * ubo.screenFromRaster;
    ubo.cameraFromNDC = m_pMainCamera->cameraFromNDC;           //ubo.cameraFromRaster * ubo.rasterFromNDC;
    ubo.cameraFromScreen = m_pMainCamera->cameraFromScreen;     //ubo.cameraFromRaster * ubo.rasterFromScreen; 
    ubo.RenderWidth = m_width;
    ubo.RenderHeight = m_height;
    ubo.frame = frame;
    ubo.spp = 1;
    ubo.maxDepth = 5;
    ubo.lightCount = nLightCount;
    ubo.arealightCount = nAreaLightInstanceCount;
    ubo.useHDR = true;
    ubo.hdrResolution = hdrResolution;
    ubo.hdrMultiplier = m_pMainScene->power;
    ubo.exposure = m_pMainScene->m_exposure;
    ubo.tonemapType = m_pMainScene->m_tonemapType;
    ubo.outputGamma = m_pMainScene->m_outputGamma;


    UBO = dynamic_cast<VulkanBuffer*>(m_pRHI->RHICreateBuffer(sizeof(UniformBufferObject), BUF_UniformBuffer, 0, &ubo));
    if (!UBO)
    {
        printf("ubo failed\n");
    }
}


void BaseRender::CreateRaytracingPipeline()
{
    ShaderBindings = m_pRHI->RHINewShaderBindings();
    ShaderBindings->SetBindings(
        {
            RHIShaderBinding::AccelStructure(0,EStageFlags::Raygen | EStageFlags::CHit,TLAS),
            RHIShaderBinding::StorageImage(1,EStageFlags::Raygen,StorageImageOut,DefaultSampler),
            RHIShaderBinding::UniformBuffer(2,EStageFlags::Raygen | EStageFlags::CHit | EStageFlags::Miss | EStageFlags::AHit,UBO),
            RHIShaderBinding::SampledTextures(3,EStageFlags::CHit | EStageFlags::AHit,TextureMaps, DefaultSampler),  //需要变成数组，数据支持多少长度要考虑下，是否无限制
            RHIShaderBinding::StorageBuffer(4,EStageFlags::CHit | EStageFlags::AHit,SSBO),
            RHIShaderBinding::StorageBuffer(5,EStageFlags::Raygen | EStageFlags::CHit | EStageFlags::Miss | EStageFlags::AHit,SSBOLight),
            RHIShaderBinding::StorageBuffer(17,EStageFlags::Raygen | EStageFlags::CHit | EStageFlags::Miss | EStageFlags::AHit, SSBOAreaLight),
            RHIShaderBinding::SampledTextures(6,EStageFlags::Raygen | EStageFlags::CHit | EStageFlags::Miss,HDRs,HDRSampler),
            RHIShaderBinding::StorageBuffer(7,EStageFlags::Raygen | EStageFlags::CHit | EStageFlags::AHit, SSBOMaterial),
            RHIShaderBinding::StorageBuffer(8,EStageFlags::CHit | EStageFlags::AHit,SSBOInstance), 
            RHIShaderBinding::StorageImage(9,EStageFlags::Raygen | EStageFlags::CHit, ConditionalRG,DefaultSampler),
            RHIShaderBinding::StorageImage(10,EStageFlags::Raygen | EStageFlags::CHit, ConditionalDataRGBA,DefaultSampler),
            RHIShaderBinding::StorageImage(11,EStageFlags::Raygen | EStageFlags::CHit, MarginalRG,DefaultSampler),
            RHIShaderBinding::UniformBuffer(12,EStageFlags::Raygen | EStageFlags::CHit | EStageFlags::Miss, HDRParamsUBO), 
            RHIShaderBinding::StorageImage(13,EStageFlags::Raygen,StorageImageOutFloat,DefaultSampler), 
            RHIShaderBinding::StorageBuffer(14,EStageFlags::Raygen | EStageFlags::CHit | EStageFlags::AHit, OffsetBuffer), 
            RHIShaderBinding::UniformBuffer(15,EStageFlags::Raygen, PostProcessingParamsUBO),
            RHIShaderBinding::StorageBuffer(16,EStageFlags::Raygen | EStageFlags::CHit | EStageFlags::AHit, AreaLightInstanceSSBO),
        }
    );
    ShaderBindings->Create();

    pRaytracingPipeline = static_cast<RHIRaytracingPipeline*>(m_pRHI->RHINewRaytracingPipeline());
    pRaytracingPipeline->attachShaderModule(RGShader);
    pRaytracingPipeline->attachShaderModule(RMShader);
    pRaytracingPipeline->attachShaderModule(RMShaderShadow);
    pRaytracingPipeline->attachShaderModule(RHShader);
    // pRaytracingPipeline->attachShaderModule(RHShaderShadow);
    pRaytracingPipeline->setShaderBindings(ShaderBindings);
    pRaytracingPipeline->create();

}

void BaseRender::CreateShaderBindingTables()
{
    auto p = SBTs;
    SBTs = m_pRHI->RHINewShaderBindingTables(pRaytracingPipeline);
    delete p;
}

void BaseRender::CreatePostProcessingParamsUBO()
{ 
    postprocessingData.params = glm::vec4(1.0,0.0,0.0,0.0);
    PostProcessingParamsUBO = m_pRHI->RHICreateBuffer(sizeof(postprocessingData), BUF_UniformBuffer, 0, nullptr);
    PostProcessingParamsUBO->UpdateData( sizeof(postprocessingData), &postprocessingData);
}
