#pragma once
#include <RHI.h>


#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/hash.hpp>

#include <iostream>
#include <string>
#include <vector>

#include <RHI.h>

#include "Scene.h"
#include "Material.h"
#include "RtTexture.h"
#include "RtInstance.h"
#include "Instance.h"
#include "Scene.h"
#include "Camera.h"
#include "HDRParams.h"
#include "GeometryNode.h"
#include "UniformBufferObject.h"
#include "Light.h"
#include "BaseRender.h"


class VulkanRHI;
class VulkanBuffer;
using DeviceAddress = uint64_t;



#define USE_HDR 0
#define USE_EXR 1

#define HDR_FORMAT USE_EXR
 
class OffscreenRender : public BaseRender
{
public:
    OffscreenRender(const char* filename, int GpuIndex = 0);
    ~OffscreenRender();
public:
    virtual int Run() final override;
protected:
    virtual void InitRHI() final override;
    virtual void CreateStorageImages() final override;
    virtual void UpdateCameraUBO(uint32_t currentFrame) final override;
    virtual void DrawFrame() final override;
    virtual void SaveFile(const char* filename) final override;
};