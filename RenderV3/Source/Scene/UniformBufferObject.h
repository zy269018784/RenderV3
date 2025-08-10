#pragma once
#include <glm/glm.hpp>

struct UniformBufferObject 
{
    glm::mat4 cameraFromWorld;
    glm::mat4 worldFromCamera;
    glm::mat4 screenFromCamera;     // 已知: Perspective(fov, 1e-2f, 1000.f);
    glm::mat4 screenFromRaster;     // Inverse(rasterFromScreen) 
    glm::mat4 screenFromNDC;        // Inverse(NDCFromScreen)
    glm::mat4 rasterFromScreen;     // rasterFromNDC * NDCFromScreen;
    glm::mat4 rasterFromNDC;        // 已知: Scale(film.FullResolution().x, -film.FullResolution().y, 1);
    glm::mat4 rasterFromCamera;     // Inverse(cameraFromRaster)
    glm::mat4 cameraFromRaster;     // Inverse(screenFromCamera) * screenFromRaster;
    glm::mat4 cameraFromNDC;        // cameraFromRaster * rasterFromNDC; 
    glm::mat4 cameraFromScreen;     // cameraFromRaster * rasterFromScreen;
    glm::mat4 NDCFromScreen;        // 已知:
    glm::mat4 NDCFromRaster;        // Inverse(rasterFromNDC) = NDCFromScreen * screenFromRaster
    glm::mat4 NDCFromCamera;        // Inverse(cameraFromNDC) = NDCFromScreen * screenFromCamera
    uint32_t RenderWidth;
    uint32_t RenderHeight;
    uint32_t frame;
    uint32_t spp;
    uint32_t maxDepth;
    uint32_t lightCount;
    uint32_t arealightCount;
    bool useHDR;
    uint32_t hdrResolution;
    float hdrMultiplier; 

    //postprocess
    float exposure;
    uint32_t tonemapType;
    float outputGamma;


};