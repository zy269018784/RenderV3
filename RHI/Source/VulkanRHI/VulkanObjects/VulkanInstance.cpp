#include <VulkanRHI/VulkanObjects//VulkanInstance.h>
#include <VulkanRHI/VulkanRHI.h>


#include <iostream>
#if 1

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl; 
    return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

VulkanInstance::VulkanInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    //appInfo.apiVersion = VK_API_VERSION_1_0;
    appInfo.apiVersion = VK_API_VERSION_1_2; //for raytracing

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
 
    bool enableValidationLayers = true;
    const std::vector<const char*> validationLayers
    {
        "VK_LAYER_KHRONOS_validation",
    };
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()); 
        createInfo.ppEnabledLayerNames = validationLayers.data(); 
        //populateDebugMessengerCreateInfo(debugCreateInfo);
        //createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0; 
        createInfo.pNext = nullptr;
    }

    std::cout << "vkCreateInstance begin" << std::endl;
    VkResult r = vkCreateInstance(&createInfo, nullptr, &Handle);
    if (r != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
    std::cout << "vkCreateInstance end " << std::endl;
}

VulkanInstance::~VulkanInstance()
{
    vkDestroyInstance(Handle, nullptr);
}

VkInstance VulkanInstance::GetHandle() const
{
    return Handle;
}

VkResult VulkanInstance::EnumeratePhysicalDevices(std::uint32_t* PhysicalDeviceCount, VkPhysicalDevice* PhysicalDevices)
{
	return vkEnumeratePhysicalDevices(Handle, PhysicalDeviceCount, PhysicalDevices);
}

VkResult VulkanInstance::EnumeratePhysicalDeviceGroups(std::uint32_t* PhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* PhysicalDeviceGroupProperties)
{
	return vkEnumeratePhysicalDeviceGroups(Handle, PhysicalDeviceGroupCount, PhysicalDeviceGroupProperties);
}

PFN_vkVoidFunction VulkanInstance::GetInstanceProcAddr(const char* Name)
{
    return vkGetInstanceProcAddr(Handle, Name);
}

#else
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
VulkanInstance::VulkanInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    /*if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else */
    {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &Handle) != VK_SUCCESS) {
        //throw std::runtime_error("failed to create instance!");
    }
}

VulkanInstance::~VulkanInstance()
{

}

std::vector<const char*> VulkanInstance::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    //if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}
#endif