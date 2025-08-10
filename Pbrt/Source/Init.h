#pragma once
#include <cpu_gpu.h>
#include <Memory/PolymorphicAllocator.h>
//#include <Scene/BasicSceneBuilder.h>
#include <string>
namespace Render {
    class Scene;
    extern Allocator *alloc;
    extern Allocator *cpu_alloc;
    extern Scene* scene;
    class BasicSceneBuilder;
    void InitRender();
    void CleanupRender();
    void BuildScene(std::string filename, BasicSceneBuilder& builder);
    void CleanUpScene();
    void Start();
    long long Elapsed();
}