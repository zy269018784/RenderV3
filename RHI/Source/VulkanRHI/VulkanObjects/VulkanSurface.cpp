#include <VulkanRHI/VulkanObjects/VulkanSurface.h>
#include <VulkanRHI/VulkanObjects/VulkanInstance.h>
#include <stdexcept>
#include <iostream>



//#define USE_PLATFORM_GLFW3 
//static void CreateWindowSurface(VkInstance& instance, GLFWwindow* window,VkSurfaceKHR& surface)
//{
//#ifdef USE_PLATFORM_GLFW3
//    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create window surface!");
//    }
//#else
//
//#endif
//}

VulkanSurface::VulkanSurface(VulkanInstance& instance, GLFWwindow* window)
{ 
    //CreateWindowSurface(instance.Handle, window, Handle); 
    VkResult ret = glfwCreateWindowSurface(instance.GetHandle(), window, nullptr, &Handle);
    std::cout << "glfwCreateWindowSurface ret = " << ret << std::endl;
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}