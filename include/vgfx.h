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

#define VK_MAX_OBJECTS 256
#define VK_SWAP_IMAGE_COUNT 2

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

char vs[] = "#version 450\nlayout (location = 0) out vec2 outUV;void main(){outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);gl_Position = vec4(outUV * 2.0f + -1.0f, 0.0f, 1.0f);}\n";
char fs[] = "#version 450\nlayout (location = 0) out vec4 FragColor;layout (location = 0) in vec2 inUV;void main(){FragColor = vec4(1 * inUV.x,0,0,1);}";

extern GLFWwindow *window;

typedef struct {
    VkSurfaceCapabilitiesKHR cap;
    VkSurfaceFormatKHR formats[VK_MAX_OBJECTS];
    u32 format_count;
    VkPresentModeKHR present_modes[VK_MAX_OBJECTS];
    u32 present_mode_count;
}vk_SwapSupportDetails;
typedef struct {
	VkSwapchainKHR swap;
	VkFormat format;
    VkExtent2D extent;
	VkImage images[VK_SWAP_IMAGE_COUNT];
	VkImageView image_views[VK_SWAP_IMAGE_COUNT];
    u32 image_count;
}vk_SwapBundle; //just a type to include all VkObjects needed for a swapchain

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

    vk_SwapBundle swap_bundle;
}vgContext;


M_RESULT vg_init(vgContext *ctx);

#if defined(VGFX_IMPLEMENATION) || 1

static const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation"
};
static const char *needed_device_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
    VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME,
    VK_KHR_MAINTENANCE2_EXTENSION_NAME,
    VK_KHR_MULTIVIEW_EXTENSION_NAME,
    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
    VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,

};

#ifdef NDEBUG
    const b32 enable_vl = FALSE;
#else
    const b32 enable_vl = FALSE;
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
    const char** glfw_ext = NULL;
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

b32 vk_check_device_extension_support(VkPhysicalDevice pd){
    b32 res = TRUE;
    u32 extension_count = 0;
    vkEnumerateDeviceExtensionProperties(pd, NULL, &extension_count, NULL);
    extension_count = MIN(VK_MAX_OBJECTS, extension_count);
    VkExtensionProperties available_extensions[VK_MAX_OBJECTS];
    vkEnumerateDeviceExtensionProperties(pd, NULL, &extension_count, available_extensions);
    for (u32 i = 0; i < ARRAY_COUNT(needed_device_extensions) && res; ++i){
        res = FALSE;
        for (u32 j = 0; j < extension_count; ++j){
            if (strcmp(needed_device_extensions[i], available_extensions[j].extensionName)==0){
                res = TRUE;
                break;
            }
        } 
    }

    return res;
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


vk_SwapSupportDetails vk_query_swap_support(VkPhysicalDevice pd, VkSurfaceKHR surface){
    vk_SwapSupportDetails details = {0};
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pd, surface, &details.cap));

    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &details.format_count, NULL));
    details.format_count = MIN(details.format_count, VK_MAX_OBJECTS);
    if (details.format_count != 0) {
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &details.format_count, details.formats));
    }


    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(pd, surface, &details.present_mode_count, NULL));
    details.present_mode_count = MIN(details.present_mode_count, VK_MAX_OBJECTS);
    if (details.present_mode_count != 0) {
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(pd, surface, &details.present_mode_count, details.present_modes));
    }
    return details;
}

static inline b32 vk_physical_device_gud(VkPhysicalDevice pd, VkSurfaceKHR surface){
    VkPhysicalDeviceProperties dp;
    VkPhysicalDeviceFeatures df;
    vkGetPhysicalDeviceProperties(pd, &dp);
    vkGetPhysicalDeviceFeatures(pd, &df);
    printf("Checking Physical Device: %s\n", dp.deviceName);
    u32 graphics_family_index=0;
    b32 pd_supports_graphics = vk_find_queue_family(pd, VK_QUEUE_GRAPHICS_BIT,&graphics_family_index);
    u32 present_family_index=0;
    b32 pd_supports_present = vk_find_present_queue_family(pd, surface,&present_family_index);
    b32 extensions_supported = vk_check_device_extension_support(pd);

    
    vk_SwapSupportDetails swap_details = vk_query_swap_support(pd,surface);
    b32 swapchain_gud = (swap_details.format_count > 0) && (swap_details.present_mode_count > 0);
    return (swapchain_gud && extensions_supported && pd_supports_present && pd_supports_graphics && VK_API_VERSION_MAJOR(dp.apiVersion) >= 1 && VK_API_VERSION_MINOR(dp.apiVersion) >= 3);
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

VkSurfaceFormatKHR vk_pick_swap_format(VkSurfaceFormatKHR *available_formats, u32 format_count) {
    for (u32 i = 0; i < format_count; ++i){
        if (available_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && available_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return available_formats[i];
        }
    }
    LOG_DBG("Didn't find a good swap chain format, returning the first one!\n");
    return available_formats[0];
}

VkPresentModeKHR vk_pick_swap_present_mode(VkPresentModeKHR* available_present_modes, u32 present_mode_count) {
    return VK_PRESENT_MODE_FIFO_KHR; //TODO: Fix???? we want a lot of FPS!
}

VkExtent2D vk_pick_swap_extent(GLFWwindow *win, VkSurfaceCapabilitiesKHR cap) {
    if (cap.currentExtent.width != UINT32_MAX) {
        return cap.currentExtent;
    } else {
        i32 width, height;
        glfwGetFramebufferSize(win, &width, &height);

        VkExtent2D extent = {
            .width = width,
            .height = height,
        };

        extent.width = CLAMP(cap.minImageExtent.width,extent.width, cap.maxImageExtent.width);
        extent.height = CLAMP(cap.minImageExtent.height,extent.height, cap.maxImageExtent.height);

        return extent;
    }
}

//maybe we can dynamically calculate the graphics/present family index and pass only queues as arguments
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

    VkPhysicalDeviceShaderObjectFeaturesEXT enabled_shader_object_features = {0};
    enabled_shader_object_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT;
    enabled_shader_object_features.shaderObject = VK_TRUE;
    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features = {0};
    dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamic_rendering_features.dynamicRendering = VK_TRUE;
    dynamic_rendering_features.pNext = &enabled_shader_object_features;

    VkPhysicalDeviceFeatures device_features = {0};
    VkDeviceCreateInfo ci = {0};
    ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    ci.pEnabledFeatures = &device_features;
    ci.queueCreateInfoCount = (graphics_family_index == present_family_index) ? 1 : 2;
    ci.pQueueCreateInfos = queue_cis;
    ci.enabledExtensionCount = ARRAY_COUNT(needed_device_extensions);
    ci.ppEnabledExtensionNames = needed_device_extensions;
    ci.pNext = &dynamic_rendering_features;
    VK_CHECK(vkCreateDevice(pd, &ci, NULL,&device));
    

    
    vkGetDeviceQueue(device, graphics_family_index, 0, graphics_queue);
    vkGetDeviceQueue(device, present_family_index, 0, present_queue);
    return device;
}

M_RESULT vk_swap_bundle_swap_images_create(VkDevice device, vk_SwapBundle *bundle){
    M_RESULT res = TRUE;
    for (u32 i = 0; i < bundle->image_count; ++i){
        VkImageViewCreateInfo ci = {0};
        ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ci.image = bundle->images[i];
        ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ci.format = bundle->format;
        ci.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ci.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ci.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ci.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ci.subresourceRange.baseMipLevel = 0;
        ci.subresourceRange.levelCount = 1;
        ci.subresourceRange.baseArrayLayer = 0;
        ci.subresourceRange.layerCount = 1;
        VK_CHECK(vkCreateImageView(device, &ci, NULL, &bundle->image_views[i]));
    }

    return res;
}
vk_SwapBundle vk_swap_bundle_swap_create(VkDevice device, VkPhysicalDevice pd, VkSurfaceKHR surface){
    vk_SwapBundle bundle = {0};

    vk_SwapSupportDetails details = vk_query_swap_support(pd, surface);

    VkSurfaceFormatKHR surface_format = vk_pick_swap_format(details.formats, details.format_count);
    VkPresentModeKHR present_mode = vk_pick_swap_present_mode(details.present_modes, details.present_mode_count);
    VkExtent2D extent = vk_pick_swap_extent(window, details.cap);

    u32 image_count = details.cap.minImageCount + 1;
    image_count = 2; //Whoops! TODO: fix, dis thing is hardcoded

    u32 graphics_family_index, present_family_index;
    vk_find_queue_family(pd,VK_QUEUE_GRAPHICS_BIT,&graphics_family_index);
    vk_find_present_queue_family(pd,surface,&present_family_index);

    VkSwapchainCreateInfoKHR ci = {0};
    ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    ci.surface = surface;
    ci.minImageCount = image_count;
    ci.imageFormat = surface_format.format;
    ci.imageColorSpace = surface_format.colorSpace;
    ci.imageExtent = extent;
    ci.imageArrayLayers = 1;
    ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (graphics_family_index != present_family_index){
        u32 family_indices_array[] = {graphics_family_index, present_family_index};
        ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        ci.queueFamilyIndexCount = 2;
        ci.pQueueFamilyIndices = family_indices_array;
    }else {
        ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        ci.queueFamilyIndexCount = 0;
        ci.pQueueFamilyIndices = NULL;
    }
    ci.preTransform = details.cap.currentTransform;
    ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    ci.presentMode = present_mode;
    ci.clipped = VK_TRUE;
    ci.oldSwapchain = VK_NULL_HANDLE;
    VK_CHECK(vkCreateSwapchainKHR(device, &ci, NULL, &bundle.swap));
    VK_CHECK(vkGetSwapchainImagesKHR(device, bundle.swap, &image_count, NULL));
    image_count = 2; //WHY IS DIS GUY HERE?!?!
    VK_CHECK(vkGetSwapchainImagesKHR(device, bundle.swap, &image_count, bundle.images));
    bundle.format = surface_format.format;
    bundle.extent = extent;
    bundle.image_count = image_count;
    ASSERT(vk_swap_bundle_swap_images_create(device, &bundle));
    return bundle;
}

M_RESULT vk_swap_bundle_swap_cleanup(VkDevice device, vk_SwapBundle *bundle){
    for (u32 i = 0; i < bundle->image_count; ++i){
        vkDestroyImageView(device, bundle->image_views[i], NULL);
    }
    vkDestroySwapchainKHR(device, bundle->swap, NULL);
    MEMZERO_STRUCT(bundle);
    return TRUE;
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
    ASSERT(ctx->device);
    ctx->swap_bundle = vk_swap_bundle_swap_create(ctx->device,ctx->pd, ctx->surface);
    printf("vg_init success!\n");
    return M_OK;
}
M_RESULT vg_cleanup(vgContext *ctx){
    vk_swap_bundle_swap_cleanup(ctx->device, &ctx->swap_bundle);
    vkDestroyDevice(ctx->device, NULL);
    vkDestroySurfaceKHR(ctx->instance, ctx->surface, NULL);
    vkDestroyInstance(ctx->instance, NULL);
    ctx->instance = vk_instance_create(); //TODO: WHY WHY WHY is this fucking thing here????????????????
    return M_OK;
}

#endif
#endif