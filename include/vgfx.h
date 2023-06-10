#ifndef VGFX_H
#define VGFX_H
#include "base.h"
#include <volk/volk.h>
#include <vma/vk_mem_alloc.h>
#include <glfw/glfw3.h>


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

typedef struct vgContext {


    //Implementation (vulkan) specific fields, we should abstract them somehow!!
    VkInstance instance;
    VkPhysicalDevice pd;
    VkDevice device;
    u32 qgraphics_family_index;
    u32 qpresent_family_index;
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
static inline b32 vk_physical_device_gud(VkPhysicalDevice pd){
    VkPhysicalDeviceProperties dp;
    VkPhysicalDeviceFeatures df;
    vkGetPhysicalDeviceProperties(pd, &dp);
    vkGetPhysicalDeviceFeatures(pd, &df);
    u32 graphics_family_index=0;
    b32 pd_supports_graphics = vk_find_queue_family(pd, VK_QUEUE_GRAPHICS_BIT,&graphics_family_index);
    return (pd_supports_graphics && VK_API_VERSION_MAJOR(dp.apiVersion) >= 1 && VK_API_VERSION_MINOR(dp.apiVersion) >= 3);
}

static inline VkPhysicalDevice vk_physical_device_pick(VkInstance instance){
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
        if (vk_physical_device_gud(devices[i])){
            pd = devices[i];
            break;
        }
    }
    if (pd == VK_NULL_HANDLE){
        LOG_ERR("No Suitable GPUs found!\n");
    }
    return pd;
}

static inline VkDevice vk_device_create(VkPhysicalDevice pd){
    VkDevice device;
    u32 graphics_family_index=0;
    vk_find_queue_family(pd, VK_QUEUE_GRAPHICS_BIT,&graphics_family_index);
    VkDeviceQueueCreateInfo queue_create_info = {0};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = graphics_family_index;
    queue_create_info.queueCount = 1;
    f32 queue_priority = 1.0f;
    queue_create_info.pQueuePriorities = &queue_priority;
    VkPhysicalDeviceFeatures device_features = {0};

    VkDeviceCreateInfo ci = {0};
    ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    ci.pQueueCreateInfos = &queue_create_info;
    ci.queueCreateInfoCount = 1;
    ci.pEnabledFeatures = &device_features;
    VK_CHECK(vkCreateDevice(pd, &ci, NULL,&device));
    
    return device;
}

M_RESULT vg_init(vgContext *ctx){
    ctx->instance = vk_instance_create();
    ASSERT(ctx->instance);
    volkLoadInstance(ctx->instance);
    ctx->pd = vk_physical_device_pick(ctx->instance);
    ASSERT(ctx->pd);
    ASSERT(vk_check_vl_support());
    ASSERT(vk_find_queue_family(ctx->pd, VK_QUEUE_GRAPHICS_BIT,&ctx->qgraphics_family_index));
    ctx->device = vk_device_create(ctx->pd);
    printf("vg_init success!\n");
    return M_OK;
}
M_RESULT vg_cleanup(vgContext *ctx){
    vkDestroyDevice(device, NULL);
    vkDestroyInstance(ctx->instance, NULL);
    ctx->instance = vk_instance_create();
    return M_OK;
}
#endif

#endif