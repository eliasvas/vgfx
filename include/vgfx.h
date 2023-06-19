#ifndef VGFX_H
#define VGFX_H
#include "base.h"
#include <volk/volk.h>
#include <vma/vk_mem_alloc.h>
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			printf("Detected Vulkan error: %i\n",err);              \
			exit(1);                                                \
		}                                                           \
	} while (0)

extern GLFWwindow *window;

typedef struct vgContext {
    //Implementation (vulkan) specific fields, we should abstract them somehow!!
    VkInstance instance;
    VkPhysicalDevice pd;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    u32 qgraphics_family_index;
    u32 qpresent_family_index;
    VkSurfaceKHR surface;
}vgContext;

M_RESULT vg_init(vgContext *ctx);

#if defined(VGFX_IMPLEMENATION) || 1

static const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation"
};
#ifdef NDEBUG
    const b32 enable_vl = FALSE;
#else
    const b32 enable_vl = TRUE;
#endif
//static char vk_error_log[256];

static inline VkInstance vk_instance_create(void){
    VkInstance instance = VK_NULL_HANDLE;

    VkApplicationInfo app_info = {0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Null";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Null";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;


    u32 glfw_ext_count = 0;
    const char** glfw_ext;
    glfw_ext = glfwGetRequiredInstanceExtensions(&glfw_ext_count); //TODO should this be freed somehow??

    VkInstanceCreateInfo ci = {0};
    ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ci.pApplicationInfo = &app_info;
    ci.enabledExtensionCount = glfw_ext_count; //TODO we need more extensions than what glfw brings!
    ci.ppEnabledExtensionNames = glfw_ext;
    ci.enabledLayerCount = 0;
    VK_CHECK(vkCreateInstance(&ci, NULL, &instance));

    return instance;
}

#define VK_MAX_OBJECTS 64
b32 vk_check_vl_support(void){
    u32 layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkLayerProperties available_layers[VK_MAX_OBJECTS]; //TODO this should be a dynamic allocation!
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers);
    layer_count = MIN(VK_MAX_OBJECTS, layer_count);
    for (u32 i = 0; i < ARRAY_COUNT(validation_layers); ++i){
        b32 layer_found = FALSE;
        for (u32 j = 0; j < layer_count; ++j){
            if (strcmp(validation_layers[i], available_layers[j].layerName) == 0){
                layer_found = TRUE;
                break;
            }
        }
        if (layer_found == FALSE){
            return FALSE;
        }
    }
    return TRUE;
}

static inline VkSurfaceKHR vk_surface_create(VkInstance instance, GLFWwindow *win){
    VkSurfaceKHR surface = {0};
    VK_CHECK(glfwCreateWindowSurface(instance, win, NULL, &surface));
    return surface;
}

static inline b32 vk_find_queue_family(VkPhysicalDevice pd, VkQueueFlagBits queue_family, u32 *family_index){
    VkQueueFamilyProperties queue_families[VK_MAX_OBJECTS];
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(pd, &queue_family_count, NULL);
    queue_family_count = MIN(VK_MAX_OBJECTS, queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(pd, &queue_family_count, queue_families);
    for (u32 i = 0; i < queue_family_count; ++i){
        if (queue_families[i].queueFlags & queue_family){
            *(family_index) = i;
            return TRUE;
        }
    }
    return FALSE;
}

static inline b32 vk_find_present_queue_family(VkPhysicalDevice pd,VkSurfaceKHR surface, u32 *family_index){
    VkQueueFamilyProperties queue_families[VK_MAX_OBJECTS];
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(pd, &queue_family_count, NULL);
    queue_family_count = MIN(VK_MAX_OBJECTS, queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(pd, &queue_family_count, queue_families);
    for (u32 i = 0; i < queue_family_count; ++i){
        VkBool32 present_support = FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(pd, i, surface, &present_support);
        if (present_support){
            *(family_index) = i;
            return TRUE;
        }
    }
    return FALSE;
}
static inline b32 vk_physical_device_gud(VkPhysicalDevice pd, VkSurfaceKHR surface){
    VkPhysicalDeviceProperties dp;
    VkPhysicalDeviceFeatures df;
    vkGetPhysicalDeviceProperties(pd, &dp);
    vkGetPhysicalDeviceFeatures(pd, &df);
    u32 graphics_family_index=0;
    b32 pd_supports_graphics = vk_find_queue_family(pd, VK_QUEUE_GRAPHICS_BIT,&graphics_family_index);
    u32 present_family_index=0;
    b32 pd_supports_present = vk_find_present_queue_family(pd, surface,&present_family_index);
    return (pd_supports_present && pd_supports_graphics && VK_API_VERSION_MAJOR(dp.apiVersion) >= 1 && VK_API_VERSION_MINOR(dp.apiVersion) >= 3);
}

static inline VkPhysicalDevice vk_physical_device_pick(VkInstance instance, VkSurfaceKHR surface){
    VkPhysicalDevice pd = VK_NULL_HANDLE;
    u32 device_count;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);
    if (device_count == 0){
        LOG_ERR("No GPUs with vulkan support found!\n");
        return pd;
    }
    device_count = MIN(VK_MAX_OBJECTS, device_count);
    VkPhysicalDevice devices[VK_MAX_OBJECTS];
    vkEnumeratePhysicalDevices(instance, &device_count, devices);
    for (u32 i = 0; i < device_count; ++i){
        if (vk_physical_device_gud(devices[i], surface)){
            pd = devices[i];
            break;
        }
    }
    if (pd == VK_NULL_HANDLE){
        LOG_ERR("No Suitable GPUs found!\n");
    }
    return pd;
}

static inline VkDevice vk_device_create(VkPhysicalDevice pd, u32 graphics_family_index, VkQueue *graphics_queue, u32 present_family_index, VkQueue *present_queue){
    VkDevice device;
    VkDeviceQueueCreateInfo queue_cis[2] = {0};
    if (graphics_family_index == present_family_index){
        float queue_priority = 1.0f;
        VkDeviceQueueCreateInfo queue_ci = {0};
        queue_ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_ci.queueFamilyIndex = graphics_family_index;
        queue_ci.queueCount = 1;
        queue_ci.pQueuePriorities = &queue_priority;
        queue_cis[0] = queue_ci;
    }else {
        float queue_priority = 1.0f;
        VkDeviceQueueCreateInfo queue_ci = {0};
        queue_ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_ci.queueFamilyIndex = graphics_family_index;
        queue_ci.queueCount = 1;
        queue_ci.pQueuePriorities = &queue_priority;
        queue_cis[0] = queue_ci;
        queue_ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_ci.queueFamilyIndex = present_family_index;
        queue_ci.queueCount = 1;
        queue_ci.pQueuePriorities = &queue_priority;
        queue_cis[1] = queue_ci;   
    }

    VkPhysicalDeviceFeatures device_features = {0};
    VkDeviceCreateInfo ci = {0};
    ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    ci.pEnabledFeatures = &device_features;
    ci.queueCreateInfoCount = (graphics_family_index == present_family_index) ? 1 : 2;
    ci.pQueueCreateInfos = queue_cis;
    VK_CHECK(vkCreateDevice(pd, &ci, NULL,&device));
    

    
    vkGetDeviceQueue(device, graphics_family_index, 0, graphics_queue);
    vkGetDeviceQueue(device, present_family_index, 0, present_queue);
    return device;
}

M_RESULT vg_init(vgContext *ctx){
    ctx->instance = vk_instance_create();
    ASSERT(ctx->instance);
    ctx->surface = vk_surface_create(ctx->instance, window);
    volkLoadInstance(ctx->instance);
    ctx->pd = vk_physical_device_pick(ctx->instance, ctx->surface);
    ASSERT(ctx->pd);
    ASSERT(vk_check_vl_support());
    vk_find_queue_family(ctx->pd, VK_QUEUE_GRAPHICS_BIT,&ctx->qgraphics_family_index);
    vk_find_present_queue_family(ctx->pd, ctx->surface,&ctx->qpresent_family_index);
    ctx->device = vk_device_create(ctx->pd, ctx->qgraphics_family_index, &ctx->graphics_queue, ctx->qpresent_family_index, &ctx->present_queue);
    printf("vg_init success!\n");
    return M_OK;
}
M_RESULT vg_cleanup(vgContext *ctx){
    vkDestroyDevice(ctx->device, NULL);
    vkDestroySurfaceKHR(ctx->instance, ctx->surface, NULL);
    vkDestroyInstance(ctx->instance, NULL);
    ctx->instance = vk_instance_create();
    return M_OK;
}

#endif
#endif