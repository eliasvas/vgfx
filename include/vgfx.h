#ifndef VGFX_H
#define VGFX_H
#include "base.h"
#include <volk/volk.h>
//#include <vma/vk_mem_alloc.h>
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
    VkInstance instance = {0};

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

#define VK_MAX_LAYERS 64
b32 vk_check_vl_support(void){
    u32 layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkLayerProperties available_layers[VK_MAX_LAYERS]; //TODO this should be a dynamic allocation!
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
            return FALSE;
        }
    }
    return TRUE;
}

M_RESULT vg_init(vgContext *ctx){
    ctx->instance = vk_instance_create();
    ASSERT(vk_check_vl_support());
    printf("vg_init success!\n");
    return M_OK;
}
M_RESULT vg_cleanup(vgContext *ctx){
    vkDestroyInstance(ctx->instance, NULL);
    ctx->instance = vk_instance_create();
    return M_OK;
}
#endif

#endif