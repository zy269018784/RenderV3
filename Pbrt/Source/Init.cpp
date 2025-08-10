#include <Init.h>
#include <Memory/GPU/CUDAMemoryResource.h>
#include <Memory/CPU/CpuMemoryResource.h>
#include <Scene/Scene.h>
#include <Scene/BasicSceneBuilder.h>
#if RENDER_IS_GPU_CODE
    #include "cuda.h" 
    #include "cuda_runtime.h"
    #include "device_launch_parameters.h"
#endif
#include <iostream>
#include <chrono>
using namespace std;

namespace Render {

    Allocator* alloc;
    Allocator* cpu_alloc;
    Scene* scene = nullptr;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::chrono::time_point<std::chrono::high_resolution_clock> last;

    void Start()
    {
        start = std::chrono::high_resolution_clock::now();
    }

    long long Elapsed()
    {
        last = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(last - start).count();
    }

    void InitRender()
    {
        MemoryResource* GpuMemoryResource = &CUDAMemoryResource::singleton;
        MemoryResource* CpuemoryResource  = &CpuMemoryResource::singleton;

        alloc = new Allocator(GpuMemoryResource);
        cpu_alloc = new Allocator(CpuemoryResource);
#if RENDER_IS_GPU_CODE
        // 设置栈大小
        cudaDeviceSetLimit(cudaLimitStackSize, 8192 * 4);
        size_t stacksize;
        cudaDeviceGetLimit(&stacksize, cudaLimitStackSize);

        cout << "stacksize " << stacksize << endl;

        int GridDimX = 0;
        int GridDimY = 0;
        int GridDimZ = 0;
        int BlockDimX = 0;
        int BlockDimY = 0;
        int BlockDimZ = 0;
        cudaDeviceGetAttribute(&GridDimX, cudaDevAttrMaxGridDimX, 0);
        cudaDeviceGetAttribute(&GridDimY, cudaDevAttrMaxGridDimY, 0);
        cudaDeviceGetAttribute(&GridDimZ, cudaDevAttrMaxGridDimZ, 0);
        cudaDeviceGetAttribute(&BlockDimX, cudaDevAttrMaxBlockDimX, 0);
        cudaDeviceGetAttribute(&BlockDimY, cudaDevAttrMaxBlockDimY, 0);
        cudaDeviceGetAttribute(&BlockDimZ, cudaDevAttrMaxBlockDimZ, 0);

        cout << "GridDimX " << GridDimX << endl;
        cout << "GridDimY " << GridDimY << endl;
        cout << "GridDimZ " << GridDimZ << endl;
        cout << "BlockDimX " << BlockDimX << endl;
        cout << "BlockDimY " << BlockDimY << endl;
        cout << "BlockDimZ " << BlockDimZ << endl;
#endif

    }

    void CleanupRender()
    {
        delete alloc;
    }

    void BuildScene(string filename, BasicSceneBuilder &builder)
    {
        Start();

        scene = alloc->allocate_object<Scene>(1);
        // 场景数据
        scene->SetSceneData(&builder.sceneData);

        auto tokError = [](const char* msg, const FileLoc* loc) {
            printf("%s", msg);
        };
        std::unique_ptr<Tokenizer> t = Tokenizer::CreateFromFile(filename, tokError);
        
        parse(&builder, std::move(t));

        scene->BuildScene(&builder);

        std::cout << "build scene: " << Elapsed() << " ms" << std::endl;
    }

    void CleanUpScene()
    {
        alloc->delete_object(scene);
    }
}