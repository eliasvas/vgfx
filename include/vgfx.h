#ifndef VGFX_H
#define VGFX_H
/*  
    This doesnt work because of:
    void vk_set_buff_binding(vgContext *ctx, u32 set, u32 binding, vk_AllocatedBuffer *buf);
    for some reason this is wrong, ref: https://github.com/KhronosGroup/Vulkan-Samples/tree/main/samples/extensions/descriptor_buffer_basic
    until the time of writing, the latest validation layers dont have support for buffer descriptors and throw an exception
    maybe I can look at it in the future when there is better support
*/
//TODO: what abt loonix?!
#define VK_USE_PLATFORM_WIN32_KHR
#include "base.h"
#include "volk.h"
#include "include/vk_mem_alloc.h"
#include <spirv_reflect.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#if OS_WIN
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif OS_LINUX
    #define GLFW_EXPOSE_NATIVE_X11
#else
    #error "Platform not supported!"
#endif
#include <GLFW/glfw3native.h>

//TODO:
//1.) textures / texture samplers
//2.) better handling of uniforms (API + implementation)
//3.) render graph basics
//4.) skybox, we could probably cheat by using this: https://www.shadertoy.com/view/wslyWs
//5.) frames in flight?!

#if OS_WIN
    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif OS_LINUX
    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>
    #define GLFW_EXPOSE_NATIVE_X11
#else
    #error "Platform not supported!"
#endif

#define VK_MAX_OBJECTS 256
#define VK_SWAP_IMAGE_COUNT 2
#define MAX_BINDINGS_PER_DESCRIPTOR_SET 4
#define MAX_DESCRIPTOR_SET 4

//TODO: decouple GLFW code with vulkan layer
#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			LOG_ERR("Detected Vulkan error: %i\n",err);              \
			exit(1);                                                \
		}                                                           \
	} while (0)


//TODO: shaders should compile also from glsl
#define SPV_DIR "./assets/spv_shaders/"

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

typedef struct {
	VkPipeline pipe;
    VkPipelineLayout layout;
    VkDescriptorSetLayout dsls[MAX_DESCRIPTOR_SET];
    VkShaderModule vert;
    SpvReflectShaderModule vert_info;
    VkShaderModule frag;
    SpvReflectShaderModule frag_info;
    //PIPELINE_OPTIONS???
}vk_PipeBundle; //just a type to include all VkObjects needed for a swapchain

typedef struct {
    VkBuffer buf;
    u32 size;
    VmaAllocation allocation;
} vk_AllocatedBuffer;

typedef struct {
    VkDescriptorSetLayout layout;
    VkDeviceSize size; //size of the descriptor set (in bytes)
    VkDeviceSize offset; //the offset from which the first descriptor starts 
    vk_AllocatedBuffer buf; //buffer holding the actual descriptor
}vk_DescriptorBuffer;

typedef struct {
    //(TODO): this should probably be a custom typed enum window, for multiple window abstractions
    GLFWwindow *window;
    //Implementation (Vulkan) specific fields, we should abstract them somehow!! (yep)
    VkInstance instance;
    VkPhysicalDevice pd;
    VkDevice device;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkQueue graphics_queue;
    VkQueue present_queue;
    u32 qgraphics_family_index;
    u32 qpresent_family_index;
    VkSurfaceKHR surface;
    iv2 window_dim;

    vk_SwapBundle swap_bundle;

    vk_PipeBundle pipe;
    vk_PipeBundle tri_pipe;

    VkCommandPool base_command_pool;
    VkCommandBuffer cmdbuf;

    VkSemaphore image_available_sem;
    VkSemaphore render_finished_sem;
    VkFence in_flight_fence;
    u32 image_index;

    vk_AllocatedBuffer quad_vbo;

    vk_AllocatedBuffer color_ubo;
    vk_DescriptorBuffer color_ubo_descriptor; 

    vk_AllocatedBuffer global_ubo;
    vk_AllocatedBuffer color_ubo2;

    VmaAllocator allocator;

    VkPhysicalDeviceDescriptorBufferPropertiesEXT descriptor_buffer_properties;
}vgContext;
void vma_load_functions(VmaVulkanFunctions *fun){
    fun->vkAllocateMemory                    = vkAllocateMemory;
    fun->vkBindBufferMemory                  = vkBindBufferMemory;
    fun->vkBindImageMemory                   = vkBindImageMemory;
    fun->vkCreateBuffer                      = vkCreateBuffer;
    fun->vkCreateImage                       = vkCreateImage;
    fun->vkDestroyBuffer                     = vkDestroyBuffer;
    fun->vkDestroyImage                      = vkDestroyImage;
    fun->vkFlushMappedMemoryRanges           = vkFlushMappedMemoryRanges;
    fun->vkFreeMemory                        = vkFreeMemory;
    fun->vkGetBufferMemoryRequirements       = vkGetBufferMemoryRequirements;
    fun->vkGetImageMemoryRequirements        = vkGetImageMemoryRequirements;
    fun->vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    fun->vkGetPhysicalDeviceProperties       = vkGetPhysicalDeviceProperties;
    fun->vkInvalidateMappedMemoryRanges      = vkInvalidateMappedMemoryRanges;
    fun->vkMapMemory                         = vkMapMemory;
    fun->vkUnmapMemory                       = vkUnmapMemory;
    fun->vkCmdCopyBuffer                     = vkCmdCopyBuffer;
}

M_RESULT vg_init(vgContext *ctx, GLFWwindow *win);


static const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation"
};
static const char *needed_device_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    //VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
    VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME,
    VK_KHR_MAINTENANCE2_EXTENSION_NAME,
    VK_KHR_MULTIVIEW_EXTENSION_NAME,
    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
    VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
    VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
    VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
	VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
	VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
	VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,
    //VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
};

static b32 vl_enable = TRUE;
//static b32 vl_enable = FALSE;

//TODO: the extensions array is never freed, free it!
char** vk_get_required_extensions(){
    u32 glfw_ext_count = 0;
    char **glfw_ext = glfwGetRequiredInstanceExtensions(&glfw_ext_count);
    char **extensions = MALLOC(sizeof(char*) * (glfw_ext_count+1));
    MEMCPY(extensions, glfw_ext, sizeof(char*) * glfw_ext_count);
    extensions[glfw_ext_count] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    //for (u32 i = 0; i < glfw_ext_count+1;++i){
    //    printf("--ext: %s\n", extensions[i]);
    //}
    return extensions;
}
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_cb( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    printf("VL: %s\n", pCallbackData->pMessage);
    return VK_FALSE;
}
static VkDebugUtilsMessengerCreateInfoEXT vk_debug_messenger_ci_create(void){
    VkDebugUtilsMessengerCreateInfoEXT ci = {0};
    ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    ci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    ci.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    ci.pfnUserCallback = debug_cb;
    ci.pUserData = NULL;
    return ci;
}
static VkDebugUtilsMessengerEXT vk_debug_messenger_create(VkInstance instance){
    VkDebugUtilsMessengerEXT msg = {0};
    VkDebugUtilsMessengerCreateInfoEXT ci = vk_debug_messenger_ci_create();
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    VK_CHECK(func(instance, &ci, NULL, &msg));
    return msg;
}

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
    ci.enabledExtensionCount = glfw_ext_count+1; //TODO we need more extensions than what glfw brings!
    ci.ppEnabledExtensionNames = vk_get_required_extensions();
    ci.ppEnabledLayerNames= (vl_enable) ? validation_layers : NULL;
    ci.enabledLayerCount = (vl_enable) ? ARRAY_COUNT(validation_layers) : 0;
    //VkDebugUtilsMessengerCreateInfoEXT mci = vk_debug_messenger_ci_create();
    //ci.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&mci;
    VK_CHECK(vkCreateInstance(&ci, NULL, &instance));

    return instance;
}

b32 vk_check_vl_support(void){
    u32 layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkLayerProperties *available_layers = MALLOC(sizeof(VkLayerProperties) * layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers);
    for (u32 i = 0; i < ARRAY_COUNT(validation_layers); ++i){
        b32 layer_found = FALSE;
        for (u32 j = 0; j < layer_count; ++j){
            if (strcmp(validation_layers[i], available_layers[j].layerName) == 0){
                layer_found = TRUE;
                break;
            }
        }
        if (layer_found == FALSE){
            FREE(available_layers);
            return FALSE;
        }
    }
    FREE(available_layers);
    return TRUE;
}

b32 vk_check_device_extension_support(VkPhysicalDevice pd){
    b32 res = TRUE;
    u32 extension_count = 0;
    vkEnumerateDeviceExtensionProperties(pd, NULL, &extension_count, NULL);
    VkExtensionProperties *available_extensions = MALLOC(sizeof(VkExtensionProperties) * extension_count);
    vkEnumerateDeviceExtensionProperties(pd, NULL, &extension_count, available_extensions);
    for (u32 i = 0; i < ARRAY_COUNT(needed_device_extensions) && res; ++i){
        res = FALSE;
        for (u32 j = 0; j < extension_count; ++j){
            if (strcmp(needed_device_extensions[i], available_extensions[j].extensionName)==0){
                res = TRUE;
                break;
            }
        } 
        if (!res)printf("Extension: [%s], not found!\n", needed_device_extensions[i]);
    }
    FREE(available_extensions);
    return res;
}

static inline VkSurfaceKHR vk_surface_create(VkInstance instance, GLFWwindow *win){
    VkSurfaceKHR surface = {0};
    VK_CHECK(glfwCreateWindowSurface(instance, win, NULL, &surface));
    return surface;
}

static inline b32 vk_find_queue_family(VkPhysicalDevice pd, VkQueueFlagBits queue_family, u32 *family_index){
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(pd, &queue_family_count, NULL);
    VkQueueFamilyProperties *queue_families = MALLOC(sizeof(VkQueueFamilyProperties) * queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(pd, &queue_family_count, queue_families);
    for (u32 i = 0; i < queue_family_count; ++i){
        if (queue_families[i].queueFlags & queue_family){
            *(family_index) = i;
            FREE(queue_families);
            return TRUE;
        }
    }
    FREE(queue_families);
    return FALSE;
}

static inline b32 vk_find_present_queue_family(VkPhysicalDevice pd,VkSurfaceKHR surface, u32 *family_index){
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(pd, &queue_family_count, NULL);
    VkQueueFamilyProperties *queue_families = MALLOC(sizeof(VkQueueFamilyProperties) * queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(pd, &queue_family_count, queue_families);
    for (u32 i = 0; i < queue_family_count; ++i){
        VkBool32 present_support = FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(pd, i, surface, &present_support);
        if (present_support){
            *(family_index) = i;
            FREE(queue_families);
            return TRUE;
        }
    }

    FREE(queue_families);
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
    //printf("Checking Physical Device: %s\n", dp.deviceName);
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
        i32 width = 0;
        i32 height = 0;
        //TODO: this is WRONG, we MUST get the framebuffer size.. i think!
        //glfwGetFramebufferSize(win, &width, &height);
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
    
    VkPhysicalDeviceDescriptorBufferFeaturesEXT descriptor_buffer_features = {0};
    descriptor_buffer_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT;
    descriptor_buffer_features.descriptorBuffer = VK_TRUE;
    descriptor_buffer_features.pNext = NULL;

    VkPhysicalDeviceBufferDeviceAddressFeatures device_address_features = {0};
    device_address_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    device_address_features.bufferDeviceAddress = VK_TRUE;
    device_address_features.pNext = &descriptor_buffer_features;




    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features = {0};
    dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamic_rendering_features.dynamicRendering = VK_TRUE;
    dynamic_rendering_features.pNext = &device_address_features;//&enabled_shader_object_features;


    VkPhysicalDeviceFeatures2 physical_features = {0};
    physical_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    physical_features.pNext = &dynamic_rendering_features; 
    vkGetPhysicalDeviceFeatures2(pd, &physical_features); //optional??
    VkDeviceCreateInfo ci = {0};
    ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    //ci.pEnabledFeatures = &physical_features;
    ci.pNext= &physical_features;
    ci.queueCreateInfoCount = (graphics_family_index == present_family_index) ? 1 : 2;
    ci.pQueueCreateInfos = queue_cis;
    ci.enabledExtensionCount = ARRAY_COUNT(needed_device_extensions);
    ci.ppEnabledExtensionNames = needed_device_extensions;
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
    VkExtent2D extent = vk_pick_swap_extent(NULL, details.cap);
    //VkExtent2D extent = vk_pick_swap_extent(window, details.cap);

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
    return M_OK;
}
M_RESULT vk_swap_bundle_swap_recreate(VkDevice device, VkPhysicalDevice pd, VkSurfaceKHR surface, vk_SwapBundle *prev_swap){
    vkDeviceWaitIdle(device);
    //glfwGetFramebufferSize(window, &ctx->window_dim.x, &ctx->window_dim.y);
    vk_swap_bundle_swap_cleanup(device, prev_swap);
    *prev_swap = vk_swap_bundle_swap_create(device, pd, surface);
    return M_OK;
}


VkShaderModule vk_shader_module_create(VkDevice device, const char *spv_bin, u32 size){
    VkShaderModule m = {0};
    VkShaderModuleCreateInfo ci = {0};
    ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = size;
    ci.pCode = (u32*)(spv_bin);
    VK_CHECK(vkCreateShaderModule(device, &ci, NULL, &m));
    return m;
}
M_RESULT vk_pipe_bundle_cleanup(VkDevice device, vk_PipeBundle *bundle){
    spvReflectDestroyShaderModule(&bundle->vert_info);
    vkDestroyShaderModule(device, bundle->vert, VK_NULL_HANDLE);
    spvReflectDestroyShaderModule(&bundle->frag_info);
    vkDestroyShaderModule(device, bundle->frag, VK_NULL_HANDLE);
    vkDestroyPipeline(device, bundle->pipe, VK_NULL_HANDLE);
    for (u32 i = 0; i < MAX_DESCRIPTOR_SET; ++i) {
        vkDestroyDescriptorSetLayout(device, bundle->dsls[i], VK_NULL_HANDLE);
    }
    vkDestroyPipelineLayout(device, bundle->layout, VK_NULL_HANDLE);
    MEMZERO_STRUCT(bundle);
    return M_OK;
}

VkDescriptorSetLayout vk_dsl_create(VkDevice device, SpvReflectShaderModule *vert_info,SpvReflectShaderModule *frag_info, u32 set){
    VkDescriptorSetLayoutBinding set_layout_bindings[VK_MAX_OBJECTS] = {0};
    u32 set_layout_bindings_count = 0;

    SpvReflectDescriptorSet *vert_set_info = NULL;
    for(u32 i = 0; i < vert_info->descriptor_set_count; ++i) {
        if (vert_info->descriptor_sets[i].set == set){
            vert_set_info = &vert_info->descriptor_sets[i];
            break;
        }
    }
    SpvReflectDescriptorSet *frag_set_info = NULL;
    for(u32 i = 0; i < frag_info->descriptor_set_count; ++i) {
        if (frag_info->descriptor_sets[i].set == set){
            frag_set_info = &frag_info->descriptor_sets[i];
            break;
        }
    }
    if (vert_set_info == NULL && frag_set_info == NULL)return VK_NULL_HANDLE;

    for (u32 binding_num = 0; binding_num < MAX_BINDINGS_PER_DESCRIPTOR_SET; ++binding_num){
        b32 binding_complete = FALSE;
        for (u32 i = 0; vert_set_info != NULL && i < vert_set_info->binding_count; ++i){
            SpvReflectDescriptorBinding *binding = vert_set_info->bindings[i];
            if (binding->binding == binding_num) {
                set_layout_bindings[i].stageFlags = VK_SHADER_STAGE_ALL;
                set_layout_bindings[i].binding = binding->binding;
                set_layout_bindings[i].descriptorType = binding->descriptor_type;
                set_layout_bindings[i].descriptorCount = 1; //TODO: implement array types!
                set_layout_bindings[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
                ++set_layout_bindings_count;
                binding_complete = TRUE;
                break;
            }
        }
        for (u32 i = 0; frag_set_info != NULL && i < frag_set_info->binding_count && !binding_complete; ++i){
            SpvReflectDescriptorBinding *binding = frag_set_info->bindings[i];
            if (binding->binding == binding_num) {
                set_layout_bindings[i].stageFlags = VK_SHADER_STAGE_ALL;
                set_layout_bindings[i].binding = binding->binding;
                set_layout_bindings[i].descriptorType = binding->descriptor_type;
                set_layout_bindings[i].descriptorCount = 1; //TODO: implement array types!
                set_layout_bindings[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
                ++set_layout_bindings_count;
                binding_complete = TRUE;
                break;
            }
        }
    }

    VkDescriptorSetLayoutCreateInfo desc_layoutCI = {0};
    desc_layoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    desc_layoutCI.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;
    desc_layoutCI.bindingCount = set_layout_bindings_count;
    desc_layoutCI.pBindings = set_layout_bindings;
    VkDescriptorSetLayout set_layout = VK_NULL_HANDLE;
    VK_CHECK(vkCreateDescriptorSetLayout(device, &desc_layoutCI, NULL, &set_layout));
    return set_layout;
}
//TODO: vertex and fragment shaders can have different descriptor bindings/sets! We should intersect all their bindings to make the final layout
//TODO: we should switch to VK_EXT_descriptor_buffer for descriptors at some point
//TODO: no path for textures? only uniform buffer supported at this time sadly
VkPipelineLayout vk_pipe_layout_create(VkDevice device, SpvReflectShaderModule *vert_info,SpvReflectShaderModule *frag_info, VkDescriptorSetLayout *dsls) {
    VkPipelineLayout pipe_layout = VK_NULL_HANDLE;

    u32 dsl_count = 0;
    for (u32 desc_set = 0; desc_set < MAX_DESCRIPTOR_SET; ++desc_set){
        VkDescriptorSetLayout layout = vk_dsl_create(device, vert_info, frag_info, desc_set);
        if (layout != VK_NULL_HANDLE){
            ++dsl_count;
            dsls[desc_set] = layout;
        }
    } 

    //*dsl = dsl_arr[0];

    VkPipelineLayoutCreateInfo pipe_layoutCI = {0};
    pipe_layoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipe_layoutCI.setLayoutCount = dsl_count;
    pipe_layoutCI.pSetLayouts = dsls;
    pipe_layoutCI.pushConstantRangeCount = 0;
    pipe_layoutCI.pPushConstantRanges = NULL;

    VK_CHECK(vkCreatePipelineLayout(device, &pipe_layoutCI, NULL, &pipe_layout));
    return pipe_layout;
}

iv2 vk_get_vertex_input_info(SpvReflectShaderModule *vert_info,VkVertexInputBindingDescription  *vi_bindings, VkVertexInputAttributeDescription *vi_attribs)
{
    i32 bindings_count = 0;
    i32 attribs_count = 0;
    u32 vertex_offset = 0;
    for (u32 k = 0; k < vert_info->input_variable_count; ++k){
        for (u32 i = 0; i < vert_info->input_variable_count; ++i){
            SpvReflectInterfaceVariable *input_var = vert_info->input_variables[i];
            if ( (input_var->decoration_flags & SpvDecorationBuiltIn) || input_var->location != k)continue;
            u32 attrib_index = input_var->location;
            vi_attribs[attrib_index].location = input_var->location;
            vi_attribs[attrib_index].binding = 0; //TODO: maybe we need multiple buffers for some attribs?!??!?!
            vi_attribs[attrib_index].format = input_var->format;
            u32 offset = sizeof(f32) * input_var->numeric.vector.component_count;
            if (input_var->numeric.matrix.column_count > 0)
                offset *= input_var->numeric.vector.component_count;
            vi_attribs[attrib_index].offset = vertex_offset;
            vertex_offset += offset;
            ++attribs_count;
        }
    }

    if (attribs_count > 0)
        bindings_count = 1;
    else
        bindings_count = 0;

	vi_bindings[0].binding = 0;
	vi_bindings[0].stride = vertex_offset;
	vi_bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return (iv2){bindings_count, attribs_count};
}

vk_PipeBundle vk_pipe_bundle_create(VkDevice device, char *vs_path, char * fs_path){
    vk_PipeBundle bundle = {0};

    VkPipeline pipe = VK_NULL_HANDLE;
    VkPipelineLayout pipe_layout = VK_NULL_HANDLE;
    SpvReflectShaderModule vert_info = {0};
    u32 vs_size=0;
    u8 *vs = read_whole_file(vs_path, &vs_size);
    VkShaderModule vert = vk_shader_module_create(device, vs, vs_size*sizeof(u8));
    spvReflectCreateShaderModule(vs_size*sizeof(u8), vs, &vert_info);

    
    SpvReflectShaderModule frag_info = {0};
    u32 fs_size=0;
    u8 *fs = read_whole_file(fs_path, &fs_size);
    VkShaderModule frag = vk_shader_module_create(device, fs, fs_size*sizeof(u8));
    spvReflectCreateShaderModule(fs_size*sizeof(u8), fs, &frag_info);

    VkPipelineShaderStageCreateInfo vci = {0};
    vci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vci.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vci.module = vert;
    vci.pName = "main";
    VkPipelineShaderStageCreateInfo fci = {0};
    fci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fci.module = frag;
    fci.pName = "main";
    VkPipelineShaderStageCreateInfo shader_stages[] = {vci, fci};

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo ds_ci = {0};
    ds_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    ds_ci.dynamicStateCount = ARRAY_COUNT(dynamic_states);
    ds_ci.pDynamicStates = dynamic_states;


    VkVertexInputBindingDescription vi_bindings[VK_MAX_OBJECTS];
    VkVertexInputAttributeDescription vi_attribs[VK_MAX_OBJECTS];
    iv2 vertex_input_counts = vk_get_vertex_input_info(&vert_info, vi_bindings, vi_attribs);
    i32 bindings_count = vertex_input_counts.x;
    i32 attribs_count = vertex_input_counts.y;
    //TODO: shader introspection!!!! (we already have mr. spirv_reflect with us!)
    VkPipelineVertexInputStateCreateInfo vert_input_ci = {0};
    vert_input_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vert_input_ci.vertexBindingDescriptionCount = bindings_count;
    vert_input_ci.pVertexBindingDescriptions = vi_bindings;
    vert_input_ci.vertexAttributeDescriptionCount = attribs_count;
    vert_input_ci.pVertexAttributeDescriptions = vi_attribs;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_ci = {0};
    input_assembly_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_ci.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_ci.primitiveRestartEnable = VK_FALSE;


    VkViewport viewport = {0};
    VkRect2D scissor = {0};
    VkPipelineViewportStateCreateInfo viewport_state_ci = {0};
    viewport_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_ci.viewportCount = 1;
    viewport_state_ci.scissorCount = 1;


    VkPipelineRasterizationStateCreateInfo rasterizer_ci = {0};
    rasterizer_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer_ci.depthClampEnable = VK_FALSE;
    rasterizer_ci.rasterizerDiscardEnable = VK_FALSE;
    rasterizer_ci.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer_ci.lineWidth = 1.0f;
    rasterizer_ci.cullMode = VK_CULL_MODE_NONE;
    rasterizer_ci.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer_ci.depthBiasEnable = VK_FALSE;
    rasterizer_ci.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer_ci.depthBiasClamp = 0.0f; // Optional
    rasterizer_ci.depthBiasSlopeFactor = 0.0f; // Optional


    VkPipelineMultisampleStateCreateInfo multisampling_ci = {0};
    multisampling_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_ci.sampleShadingEnable = VK_FALSE;
    multisampling_ci.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling_ci.minSampleShading = 1.0f; // Optional
    multisampling_ci.pSampleMask = NULL; // Optional
    multisampling_ci.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling_ci.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional


    VkPipelineColorBlendStateCreateInfo color_blend_ci = {0};
    color_blend_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_ci.logicOpEnable = VK_FALSE;
    color_blend_ci.logicOp = VK_LOGIC_OP_COPY; // Optional
    color_blend_ci.attachmentCount = 1;
    color_blend_ci.pAttachments = &color_blend_attachment;
    color_blend_ci.blendConstants[0] = 0.0f; // Optional
    color_blend_ci.blendConstants[1] = 0.0f; // Optional
    color_blend_ci.blendConstants[2] = 0.0f; // Optional
    color_blend_ci.blendConstants[3] = 0.0f; // Optional2

    pipe_layout = vk_pipe_layout_create(device, &vert_info, &frag_info, bundle.dsls);


    VkPipelineRenderingCreateInfoKHR pipe_rendering_ci = {0};
    pipe_rendering_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipe_rendering_ci.colorAttachmentCount = 1;
    VkFormat swap_format = VK_FORMAT_B8G8R8A8_SRGB;
    pipe_rendering_ci.pColorAttachmentFormats = &swap_format;

    VkGraphicsPipelineCreateInfo pipeline_ci = {0};
    pipeline_ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_ci.stageCount = 2;
    pipeline_ci.pStages = shader_stages;
    pipeline_ci.pNext = &pipe_rendering_ci;
    pipeline_ci.renderPass = VK_NULL_HANDLE;
    pipeline_ci.pVertexInputState = &vert_input_ci;
    pipeline_ci.pInputAssemblyState = &input_assembly_ci;
    pipeline_ci.pViewportState = &viewport_state_ci;
    pipeline_ci.pRasterizationState = &rasterizer_ci;
    pipeline_ci.pMultisampleState = &multisampling_ci;
    pipeline_ci.pDepthStencilState = NULL; // Optional
    pipeline_ci.pColorBlendState = &color_blend_ci;
    pipeline_ci.pDynamicState = &ds_ci;
    pipeline_ci.layout = pipe_layout;
    pipeline_ci.flags = VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;

    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_ci, NULL, &pipe));
    bundle.pipe = pipe;
    bundle.layout = pipe_layout;
    bundle.vert = vert;
    bundle.vert_info = vert_info;
    bundle.frag = frag;
    bundle.frag_info = frag_info;
    if (fs != NULL)FREE(fs);
    if (vs != NULL)FREE(vs);
    return bundle;
}

M_RESULT vk_allocated_buffer_update(VmaAllocator alloc, vk_AllocatedBuffer *buf, void *data, u32 data_size){
    if (data) {
        void *mapped_data;
        vmaMapMemory(alloc, buf->allocation, &mapped_data);
        MEMCPY(mapped_data, data, data_size);
        vmaUnmapMemory(alloc, buf->allocation);
    }
    return M_OK;
}
vk_AllocatedBuffer vk_allocated_buffer_create(VmaAllocator alloc, VkBufferUsageFlagBits usage, void *data, u32 data_size){
    vk_AllocatedBuffer buf = {0};
    //allocate vertex buffer
	VkBufferCreateInfo bufCI = {0};
	bufCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufCI.size = data_size;
	bufCI.usage = usage;

    VmaAllocationCreateInfo vma_allocCI = {0};
	vma_allocCI.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	VK_CHECK(vmaCreateBuffer(alloc, &bufCI, &vma_allocCI,
		&buf.buf,
		&buf.allocation,
		NULL));
    vk_allocated_buffer_update(alloc, &buf, data, data_size);
    buf.size = data_size;
    
    return buf;
}
u64 vk_allocated_buffer_get_device_address(VkDevice device, vk_AllocatedBuffer *buf)
{
	VkBufferDeviceAddressInfoKHR buffer_device_address_info = {0};
	buffer_device_address_info.sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	buffer_device_address_info.buffer = buf->buf;
	return vkGetBufferDeviceAddress(device, &buffer_device_address_info);
}
M_RESULT vk_allocated_buffer_cleanup(VmaAllocator alloc, vk_AllocatedBuffer *buf){
    vmaDestroyBuffer(alloc, buf->buf, buf->allocation);
    MEMZERO_STRUCT(buf);
    return M_OK;
}

VkCommandPool vk_command_pool_create(VkDevice device, u32 family_index){
    VkCommandPool pool = VK_NULL_HANDLE;
    VkCommandPoolCreateInfo pool_info_ci = {0};
    pool_info_ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info_ci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info_ci.queueFamilyIndex = family_index;

    VK_CHECK(vkCreateCommandPool(device, &pool_info_ci, NULL, &pool));
    return pool;
}
VkCommandBuffer vk_command_buffer_create(VkDevice device, VkCommandPool pool){
    VkCommandBuffer cmd_buf = VK_NULL_HANDLE;
    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    VK_CHECK(vkAllocateCommandBuffers(device, &alloc_info, &cmd_buf));

    return cmd_buf;
}

VkSemaphore vk_semaphore_create(VkDevice device){
    VkSemaphore sem = VK_NULL_HANDLE;
    VkSemaphoreCreateInfo sem_ci = {0};
    sem_ci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VK_CHECK(vkCreateSemaphore(device, &sem_ci, NULL, &sem));
    return sem;
}

VkFence vk_fence_create(VkDevice device){
    VkFence fence = VK_NULL_HANDLE;
    VkFenceCreateInfo fence_ci = {0};
    fence_ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_ci.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    VK_CHECK(vkCreateFence(device, &fence_ci, NULL, &fence));
    return fence;
}

static inline VkViewport vk_viewport_make(f32 x, f32 y, f32 width, f32 height){
    VkViewport viewport = {0};
    viewport.x = x;
    viewport.y = y;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    return viewport;
}

// typedef struct {
//     VkDescriptorSetLayout layout;
//     u32 size; //size of the descriptor set (in bytes)
//     u32 offset; //the offset from which the first descriptor starts 
//     vk_AllocatedBuffer buf; //buffer holding the actual descriptor
// }vk_DescriptorBuffer;
inline VkDeviceSize aligned_size(VkDeviceSize value, VkDeviceSize alignment)
{
	return (value + alignment - 1) & ~(alignment - 1);
}

vk_DescriptorBuffer vk_desc_buffer_create(vgContext *ctx, VkDescriptorSetLayout layout){
    vk_DescriptorBuffer desc_buf = {0};

    // Get set layout descriptor sizes.
    vkGetDescriptorSetLayoutSizeEXT(ctx->device, layout, &desc_buf.size);
    //vkGetDescriptorSetLayoutSizeEXT(device, image_binding_descriptor.layout, &image_binding_descriptor.size);

    // Adjust set layout sizes to satisfy alignment requirements.
    desc_buf.size = aligned_size(desc_buf.size, ctx->descriptor_buffer_properties.descriptorBufferOffsetAlignment);
    //image_binding_descriptor.size = aligned_size(image_binding_descriptor.size, descriptor_buffer_properties.descriptorBufferOffsetAlignment);

    // Get descriptor bindings offsets as descriptors are placed inside set layout by those offsets.
    vkGetDescriptorSetLayoutBindingOffsetEXT(ctx->device, layout, 0, &desc_buf.offset);
    //vkGetDescriptorSetLayoutBindingOffsetEXT(device, image_binding_descriptor.layout, 0u, &image_binding_descriptor.offset);

    desc_buf.buf = vk_allocated_buffer_create(ctx->allocator, VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, NULL, (u32)desc_buf.size);

    return desc_buf;
}

void vk_desc_buffer_cleanup(vgContext *ctx, vk_DescriptorBuffer *buf){
    vk_allocated_buffer_cleanup(ctx->allocator,&buf->buf);
    MEMZERO_STRUCT(buf);
}


//TODO: make vk_set_image_binding
//also check: VK_EXT_inline_uniform_block, should simplify implementation
void vk_set_buff_binding(vgContext *ctx, u32 set, u32 binding, vk_AllocatedBuffer *buf){
    // VkWriteDescriptorSet write_desc_set = {0};
    // write_desc_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    // write_desc_set.dstSet = 0;
    // write_desc_set.dstBinding = binding;
    // write_desc_set.descriptorCount = 1; //TODO: make array descriptors POSSIBLE!
    // write_desc_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // VkDescriptorBufferInfo buf_info = {0};
    // buf_info.buffer = buf->buf;
    // buf_info.range = buf->size;
    // buf_info.offset = 0;
    // write_desc_set.pBufferInfo = &buf_info;
    // vkCmdPushDescriptorSetKHR(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe->layout, set, 1, &write_desc_set);
    char *uniform_descriptor_buf_ptr = (char *) ctx->color_ubo_descriptor.buf.buf;

    VkDescriptorAddressInfoEXT addr_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_ADDRESS_INFO_EXT};
    addr_info.address                    = vk_allocated_buffer_get_device_address(ctx->device, buf); 
    addr_info.range                      = buf->size; 
    addr_info.format                     = VK_FORMAT_UNDEFINED;
    VkDescriptorGetInfoEXT buffer_descriptor_info = {0};
    buffer_descriptor_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT;
    buffer_descriptor_info.type                = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    buffer_descriptor_info.data.pUniformBuffer = &addr_info;
    vkGetDescriptorEXT(ctx->device, &buffer_descriptor_info, ctx->descriptor_buffer_properties.uniformBufferDescriptorSize, uniform_descriptor_buf_ptr);

    // Binding 0 = uniform buffer
    VkDescriptorBufferBindingInfoEXT descriptor_buffer_binding_info = {0};
    descriptor_buffer_binding_info.sType   = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
    descriptor_buffer_binding_info.address = vk_allocated_buffer_get_device_address(ctx->device, &ctx->color_ubo_descriptor.buf);
    descriptor_buffer_binding_info.usage   = VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;
    vkCmdBindDescriptorBuffersEXT(ctx->cmdbuf, 1, &descriptor_buffer_binding_info);
    u32 buffer_index_ubo = 0;
    VkDeviceSize buffer_offset = 0;
    vkCmdSetDescriptorBufferOffsetsEXT(ctx->cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, ctx->tri_pipe.layout, 0, 1, &buffer_index_ubo, &buffer_offset);

}

void record_cmd(vgContext *ctx, u32 image_index){
    VkCommandBufferBeginInfo cmd_begin_info = {0};
    cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_begin_info.flags = 0; // Optional
    cmd_begin_info.pInheritanceInfo = NULL; // Optional

    VK_CHECK(vkBeginCommandBuffer(ctx->cmdbuf, &cmd_begin_info));

    
    VkImageMemoryBarrier image_memory_barrier = {0};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    image_memory_barrier.image = ctx->swap_bundle.images[ctx->image_index];
    image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_memory_barrier.subresourceRange.baseMipLevel = 0;
    image_memory_barrier.subresourceRange.levelCount = 1;
    image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    image_memory_barrier.subresourceRange.layerCount = 1;
    vkCmdPipelineBarrier(
        ctx->cmdbuf,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // srcStageMask
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
        0,
        0,
        NULL,
        0,
        NULL,
        1, // imageMemoryBarrierCount
        &image_memory_barrier // pImageMemoryBarriers
    );


    i32 wwidth = ctx->window_dim.x, wheight = ctx->window_dim.y;
    //glfwGetFramebufferSize(window, &wwidth, &wheight);
    VkRenderingAttachmentInfoKHR color_attachment = {0};
    color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    color_attachment.imageView = ctx->swap_bundle.image_views[ctx->image_index];
    color_attachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.clearValue = (VkClearValue){0,0};
    VkRenderingInfoKHR render_info = {0};
    render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    render_info.renderArea.offset = (VkOffset2D){0, 0};
    render_info.renderArea.extent = (VkExtent2D){wwidth,wheight};
    render_info.layerCount = 1;
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachments = &color_attachment;
    vkCmdBeginRenderingKHR(ctx->cmdbuf, &render_info);


    VkViewport viewport = vk_viewport_make(0,0,(f32)wwidth,(f32)wheight);
    VkRect2D scissor = {(VkOffset2D){0, 0},(VkExtent2D){wwidth,wheight}};

    
    vkCmdBindPipeline(ctx->cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, ctx->pipe.pipe);
    vkCmdSetViewport(ctx->cmdbuf, 0, 1, &viewport);
    vkCmdSetScissor(ctx->cmdbuf, 0, 1, &scissor);
    vkCmdDraw(ctx->cmdbuf, 3, 1, 0, 0);


    vkCmdBindPipeline(ctx->cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, ctx->tri_pipe.pipe);
    vkCmdSetViewport(ctx->cmdbuf, 0, 1, &viewport); 
    vkCmdSetScissor(ctx->cmdbuf, 0, 1, &scissor);
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(ctx->cmdbuf, 0, 1, &ctx->quad_vbo.buf, &offset);
    //vk_set_buff_binding(ctx->cmdbuf,&ctx->tri_pipe,0,0, &ctx->global_ubo);
    vk_set_buff_binding(ctx,0,0,&ctx->color_ubo);
    //vk_set_buff_binding(ctx->cmdbuf,&ctx->tri_pipe,1,0, &ctx->color_ubo2);
    vkCmdDraw(ctx->cmdbuf, 3, 1, 0, 0);

    vkCmdEndRenderingKHR(ctx->cmdbuf);

    VkImageMemoryBarrier image_memory_barrier2 = {0};
    image_memory_barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier2.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    image_memory_barrier2.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    image_memory_barrier2.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    image_memory_barrier2.image = ctx->swap_bundle.images[ctx->image_index];
    image_memory_barrier2.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_memory_barrier2.subresourceRange.baseMipLevel = 0;
    image_memory_barrier2.subresourceRange.levelCount = 1;
    image_memory_barrier2.subresourceRange.baseArrayLayer = 0;
    image_memory_barrier2.subresourceRange.layerCount = 1;
    vkCmdPipelineBarrier(
        ctx->cmdbuf,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // srcStageMask
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, // dstStageMask
        0,
        0,
        NULL,
        0,
        NULL,
        1, // imageMemoryBarrierCount
        &image_memory_barrier2 // pImageMemoryBarriers
    );


    VK_CHECK(vkEndCommandBuffer(ctx->cmdbuf));
}

void draw_sample_frame(vgContext *c){
    vgContext *ctx = c;
    vkWaitForFences(ctx->device, 1, &ctx->in_flight_fence, VK_TRUE, UINT64_MAX);
    VkResult res = vkAcquireNextImageKHR(ctx->device, ctx->swap_bundle.swap, UINT64_MAX, ctx->image_available_sem, VK_NULL_HANDLE, &ctx->image_index);
    if (res == VK_ERROR_OUT_OF_DATE_KHR){ 
        glfwGetFramebufferSize(ctx->window, &ctx->window_dim.x, &ctx->window_dim.y);
        vk_swap_bundle_swap_recreate(ctx->device, ctx->pd, ctx->surface, &ctx->swap_bundle);
        return;
    }
    vkResetFences(ctx->device, 1, &ctx->in_flight_fence);
    
    
    vkResetCommandBuffer(ctx->cmdbuf, 0);
    record_cmd(ctx, ctx->image_index);
    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_sems[] = {ctx->image_available_sem};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_sems;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &ctx->cmdbuf;
    VkSemaphore signal_sems[] = {ctx->render_finished_sem};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_sems;
    VK_CHECK(vkQueueSubmit(ctx->graphics_queue, 1, &submit_info, ctx->in_flight_fence));


    VkPresentInfoKHR present_info = {0};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_sems;
    VkSwapchainKHR swaps[] = {ctx->swap_bundle.swap};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swaps;
    present_info.pImageIndices = &ctx->image_index;
    present_info.pResults = NULL;
    VkResult resq = vkQueuePresentKHR(ctx->present_queue, &present_info);
    if (resq == VK_ERROR_OUT_OF_DATE_KHR || resq == VK_SUBOPTIMAL_KHR) { 
        glfwGetFramebufferSize(ctx->window, &ctx->window_dim.x, &ctx->window_dim.y);
        vk_swap_bundle_swap_recreate(ctx->device, ctx->pd, ctx->surface, &ctx->swap_bundle);
    }
}


M_RESULT vg_init(vgContext *ctx, GLFWwindow *win){
    volkInitialize();
    ctx->window = win;
    ctx->instance = vk_instance_create();
    ASSERT(ctx->instance);
    //ctx->debug_messenger = vk_debug_messenger_create(ctx->instance);
    //ASSERT(ctx->debug_messenger);
    ctx->surface = vk_surface_create(ctx->instance, ctx->window);
    volkLoadInstance(ctx->instance);
    ctx->pd = vk_physical_device_pick(ctx->instance, ctx->surface);
    ASSERT(ctx->pd);
    //TODO: this should be in a function? its the initialization for VK_EXT_descriptor_buffer support
    VkPhysicalDeviceProperties2 device_properties = {0};
    MEMZERO_STRUCT(&ctx->descriptor_buffer_properties);
	ctx->descriptor_buffer_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_PROPERTIES_EXT;
	device_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	device_properties.pNext = &ctx->descriptor_buffer_properties;
	vkGetPhysicalDeviceProperties2(ctx->pd, &device_properties);
    ASSERT(vk_check_vl_support());
    vk_find_queue_family(ctx->pd, VK_QUEUE_GRAPHICS_BIT,&ctx->qgraphics_family_index);
    vk_find_present_queue_family(ctx->pd, ctx->surface,&ctx->qpresent_family_index);
    ctx->device = vk_device_create(ctx->pd, ctx->qgraphics_family_index, &ctx->graphics_queue, ctx->qpresent_family_index, &ctx->present_queue);
    ASSERT(ctx->device);
    glfwGetFramebufferSize(ctx->window, &ctx->window_dim.x, &ctx->window_dim.y);
    ctx->swap_bundle = vk_swap_bundle_swap_create(ctx->device,ctx->pd, ctx->surface);
    ctx->pipe = vk_pipe_bundle_create(ctx->device, SPV_DIR "full_quad.vert.spv", SPV_DIR "full_quad.frag.spv");
    ASSERT(ctx->pipe.pipe);
    ctx->tri_pipe = vk_pipe_bundle_create(ctx->device, SPV_DIR "tri.vert.spv", SPV_DIR "tri.frag.spv");
    ASSERT(ctx->tri_pipe.pipe);
    ctx->base_command_pool = vk_command_pool_create(ctx->device, ctx->qgraphics_family_index);
    ASSERT(ctx->base_command_pool);
    ctx->cmdbuf = vk_command_buffer_create(ctx->device, ctx->base_command_pool);
    ASSERT(ctx->cmdbuf);
    ctx->in_flight_fence = vk_fence_create(ctx->device);
    ASSERT(ctx->in_flight_fence);
    ctx->image_available_sem = vk_semaphore_create(ctx->device);
    ASSERT(ctx->image_available_sem);
    ctx->render_finished_sem = vk_semaphore_create(ctx->device);
    ASSERT(ctx->render_finished_sem);
    VmaVulkanFunctions fun = {0};
    vma_load_functions(&fun);
    VmaAllocatorCreateInfo allocatorCI = {0};
    allocatorCI.physicalDevice = ctx->pd;
    allocatorCI.device = ctx->device;
    allocatorCI.instance = ctx->instance;
    allocatorCI.pVulkanFunctions = &fun;
    allocatorCI.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    VK_CHECK(vmaCreateAllocator(&allocatorCI, &ctx->allocator));
    f32 data[] = {
                    1.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,1.f,
                    -1.f,1.f,0.f,0.f,0.f,0.f,1.f,0.f,0.f,
                    0.f,-1.f,0.f,0.f,0.f,0.f,0.f,1.f,0.f
                };
    ctx->quad_vbo = vk_allocated_buffer_create(ctx->allocator, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, data, ARRAY_COUNT(data)*sizeof(f32));

    f32 data2[] = {1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    ctx->global_ubo = vk_allocated_buffer_create(ctx->allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, data2, ARRAY_COUNT(data2)*sizeof(f32));
    f32 data_color[] = {1.0,0.0,1.0,1.0};
   ctx->color_ubo2 = vk_allocated_buffer_create(ctx->allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT| VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, data_color, ARRAY_COUNT(data_color)*sizeof(f32));

    ctx->color_ubo = vk_allocated_buffer_create(ctx->allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT| VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, data_color, ARRAY_COUNT(data_color)*sizeof(f32));
    ctx->color_ubo_descriptor = vk_desc_buffer_create(ctx, ctx->tri_pipe.dsls[0]);
 

    LOG_DBG("vg_init success!\n");
    return M_OK;
}
M_RESULT vg_cleanup(vgContext *ctx){
    vkQueueWaitIdle(ctx->present_queue);
    vkQueueWaitIdle(ctx->graphics_queue);
    vkDestroyCommandPool(ctx->device, ctx->base_command_pool, NULL);
    vk_pipe_bundle_cleanup(ctx->device, &ctx->pipe);
    vk_pipe_bundle_cleanup(ctx->device, &ctx->tri_pipe);
    vk_swap_bundle_swap_cleanup(ctx->device, &ctx->swap_bundle);
    vk_allocated_buffer_cleanup(ctx->allocator, &ctx->quad_vbo);
    vk_allocated_buffer_cleanup(ctx->allocator, &ctx->global_ubo);
    vk_allocated_buffer_cleanup(ctx->allocator, &ctx->color_ubo);
    vk_allocated_buffer_cleanup(ctx->allocator, &ctx->color_ubo2);
    vmaDestroyAllocator(ctx->allocator);
    vkDestroySemaphore(ctx->device, ctx->image_available_sem, NULL);
    vkDestroySemaphore(ctx->device, ctx->render_finished_sem, NULL);
    vkDestroyFence(ctx->device, ctx->in_flight_fence, NULL);
    vkDestroyDevice(ctx->device, NULL);
    vkDestroySurfaceKHR(ctx->instance, ctx->surface, NULL);
    vkDestroyInstance(ctx->instance, NULL);
    ctx->instance = vk_instance_create(); //TODO: WHY WHY WHY is this fucking thing here????????????????
    return M_OK;
}

#endif