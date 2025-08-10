#include <RHI.h>
#include <RHIPixelFormat.h>
#include <cstdint>
#include <VulkanRHI/VulkanRHI.h>
#ifdef USE_QT
#include <QVulkanWindow>

RHI* RHI::create(Implementation impl, QVulkanWindow* window)
{ 
    switch (impl) { 
    case Vulkan: 
        return new VulkanRHI(window); 
    } 

    return nullptr;
}
#endif