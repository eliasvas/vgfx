#ifndef VGFX_H
#define VGFX_H
#include "base.h"
#include <volk/volk.h>
#include <vma/vk_mem_alloc.h>
#include <spirv_reflect.h>
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


//all shaders can be converted to binary, first glslc the shader file, then go to https://notisrac.github.io/FileToCArray/ to get the c-style array.
char vs[] = "#version 450\nlayout (location = 0) out vec2 outUV;void main(){outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);gl_Position = vec4(outUV * 2.0f + -1.0f, 0.0f, 1.0f);}\n";
static const u8 vs_spv[] = {
  0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x0b, 0x00, 0x0d, 0x00, 0x2c, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00, 
  0x01, 0x00, 0x00, 0x00, 0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, 0x30, 
  0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
  0x0f, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 
  0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 
  0x03, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0xc2, 0x01, 0x00, 0x00, 0x04, 0x00, 0x0a, 0x00, 
  0x47, 0x4c, 0x5f, 0x47, 0x4f, 0x4f, 0x47, 0x4c, 0x45, 0x5f, 0x63, 0x70, 0x70, 0x5f, 0x73, 0x74, 
  0x79, 0x6c, 0x65, 0x5f, 0x6c, 0x69, 0x6e, 0x65, 0x5f, 0x64, 0x69, 0x72, 0x65, 0x63, 0x74, 0x69, 
  0x76, 0x65, 0x00, 0x00, 0x04, 0x00, 0x08, 0x00, 0x47, 0x4c, 0x5f, 0x47, 0x4f, 0x4f, 0x47, 0x4c, 
  0x45, 0x5f, 0x69, 0x6e, 0x63, 0x6c, 0x75, 0x64, 0x65, 0x5f, 0x64, 0x69, 0x72, 0x65, 0x63, 0x74, 
  0x69, 0x76, 0x65, 0x00, 0x05, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 
  0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00, 0x6f, 0x75, 0x74, 0x55, 
  0x56, 0x00, 0x00, 0x00, 0x05, 0x00, 0x06, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x67, 0x6c, 0x5f, 0x56, 
  0x65, 0x72, 0x74, 0x65, 0x78, 0x49, 0x6e, 0x64, 0x65, 0x78, 0x00, 0x00, 0x05, 0x00, 0x06, 0x00, 
  0x1b, 0x00, 0x00, 0x00, 0x67, 0x6c, 0x5f, 0x50, 0x65, 0x72, 0x56, 0x65, 0x72, 0x74, 0x65, 0x78, 
  0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x06, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x67, 0x6c, 0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x06, 0x00, 0x07, 0x00, 
  0x1b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x67, 0x6c, 0x5f, 0x50, 0x6f, 0x69, 0x6e, 0x74, 
  0x53, 0x69, 0x7a, 0x65, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x07, 0x00, 0x1b, 0x00, 0x00, 0x00, 
  0x02, 0x00, 0x00, 0x00, 0x67, 0x6c, 0x5f, 0x43, 0x6c, 0x69, 0x70, 0x44, 0x69, 0x73, 0x74, 0x61, 
  0x6e, 0x63, 0x65, 0x00, 0x06, 0x00, 0x07, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
  0x67, 0x6c, 0x5f, 0x43, 0x75, 0x6c, 0x6c, 0x44, 0x69, 0x73, 0x74, 0x61, 0x6e, 0x63, 0x65, 0x00, 
  0x05, 0x00, 0x03, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 
  0x09, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 
  0x0c, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 
  0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x48, 0x00, 0x05, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 
  0x01, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
  0x0b, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x1b, 0x00, 0x00, 0x00, 
  0x03, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x47, 0x00, 0x03, 0x00, 
  0x1b, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x13, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 
  0x21, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x16, 0x00, 0x03, 0x00, 
  0x06, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 
  0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 
  0x03, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 
  0x09, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x15, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 
  0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00, 
  0x01, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00, 
  0x0c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 
  0x0e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 
  0x10, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x17, 0x00, 0x00, 0x00, 
  0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x15, 0x00, 0x04, 0x00, 0x18, 0x00, 0x00, 0x00, 
  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x18, 0x00, 0x00, 0x00, 
  0x19, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x04, 0x00, 0x1a, 0x00, 0x00, 0x00, 
  0x06, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x06, 0x00, 0x1b, 0x00, 0x00, 0x00, 
  0x17, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 
  0x20, 0x00, 0x04, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 
  0x3b, 0x00, 0x04, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
  0x2b, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 
  0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xbf, 
  0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 
  0x20, 0x00, 0x04, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 
  0x36, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 
  0x0a, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0xc4, 0x00, 0x05, 0x00, 
  0x0a, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 
  0xc7, 0x00, 0x05, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 
  0x10, 0x00, 0x00, 0x00, 0x6f, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 
  0x11, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 
  0x0c, 0x00, 0x00, 0x00, 0xc7, 0x00, 0x05, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 
  0x13, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x6f, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 
  0x15, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x50, 0x00, 0x05, 0x00, 0x07, 0x00, 0x00, 0x00, 
  0x16, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 
  0x09, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 
  0x1f, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x8e, 0x00, 0x05, 0x00, 0x07, 0x00, 0x00, 0x00, 
  0x21, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x50, 0x00, 0x05, 0x00, 
  0x07, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 
  0x81, 0x00, 0x05, 0x00, 0x07, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 
  0x23, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 
  0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 
  0x28, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x50, 0x00, 0x07, 0x00, 
  0x17, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 
  0x25, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x2a, 0x00, 0x00, 0x00, 
  0x2b, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 
  0x2b, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x01, 0x00, 0x38, 0x00, 0x01, 0x00
};
char fs[] = "#version 450\nlayout (location = 0) out vec4 FragColor;layout (location = 0) in vec2 inUV;void main(){FragColor = vec4(1.0,0.0,0.0,1.0);}";
static const u8 fs_spv[] = {
  0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x0b, 0x00, 0x0d, 0x00, 0x10, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00, 
  0x01, 0x00, 0x00, 0x00, 0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, 0x30, 
  0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
  0x0f, 0x00, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 
  0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00, 
  0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 
  0xc2, 0x01, 0x00, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x47, 0x4c, 0x5f, 0x47, 0x4f, 0x4f, 0x47, 0x4c, 
  0x45, 0x5f, 0x63, 0x70, 0x70, 0x5f, 0x73, 0x74, 0x79, 0x6c, 0x65, 0x5f, 0x6c, 0x69, 0x6e, 0x65, 
  0x5f, 0x64, 0x69, 0x72, 0x65, 0x63, 0x74, 0x69, 0x76, 0x65, 0x00, 0x00, 0x04, 0x00, 0x08, 0x00, 
  0x47, 0x4c, 0x5f, 0x47, 0x4f, 0x4f, 0x47, 0x4c, 0x45, 0x5f, 0x69, 0x6e, 0x63, 0x6c, 0x75, 0x64, 
  0x65, 0x5f, 0x64, 0x69, 0x72, 0x65, 0x63, 0x74, 0x69, 0x76, 0x65, 0x00, 0x05, 0x00, 0x04, 0x00, 
  0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 
  0x09, 0x00, 0x00, 0x00, 0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x00, 0x00, 0x00, 
  0x05, 0x00, 0x04, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x69, 0x6e, 0x55, 0x56, 0x00, 0x00, 0x00, 0x00, 
  0x47, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x47, 0x00, 0x04, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x13, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x21, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 
  0x02, 0x00, 0x00, 0x00, 0x16, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 
  0x17, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 
  0x20, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 
  0x3b, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
  0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 
  0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x2c, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 
  0x0b, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 
  0x0d, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 
  0x0e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 
  0x0e, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x36, 0x00, 0x05, 0x00, 
  0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
  0xf8, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x09, 0x00, 0x00, 0x00, 
  0x0c, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x01, 0x00, 0x38, 0x00, 0x01, 0x00
};
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

typedef struct {
	VkPipeline pipe;
    VkPipelineLayout layout;
    VkShaderModule vert;
    SpvReflectShaderModule vert_info;
    VkShaderModule frag;
    SpvReflectShaderModule frag_info;
    //PIPELINE_OPTIONS???
}vk_PipeBundle; //just a type to include all VkObjects needed for a swapchain
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

    vk_PipeBundle pipe;

    VkCommandPool base_command_pool;
    VkCommandBuffer cmdbuf;

    VkSemaphore image_available_sem;
    VkSemaphore render_finished_sem;
    VkFence in_flight_fence;
    u32 image_index;
}vgContext;


M_RESULT vg_init(vgContext *ctx);

#if defined(VGFX_IMPLEMENATION) || 1

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
    //VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
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
/*
    VkPhysicalDeviceShaderObjectFeaturesEXT enabled_shader_object_features = {0};
    enabled_shader_object_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT;
    enabled_shader_object_features.shaderObject = VK_TRUE;
*/
    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features = {0};
    dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamic_rendering_features.dynamicRendering = VK_TRUE;
    dynamic_rendering_features.pNext = NULL;//&enabled_shader_object_features;


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
    vkDestroyPipelineLayout(device, bundle->layout, VK_NULL_HANDLE);
    MEMZERO_STRUCT(bundle);
    return M_OK;
}
vk_PipeBundle vk_pipe_bundle_create(VkDevice device){
    VkPipeline pipe = VK_NULL_HANDLE;
    VkPipelineLayout pipe_layout = VK_NULL_HANDLE;
    SpvReflectShaderModule vert_info = {0};
    VkShaderModule vert = vk_shader_module_create(device, vs_spv, ARRAY_COUNT(vs_spv)*sizeof(u8));
    spvReflectCreateShaderModule(ARRAY_COUNT(vs_spv)*sizeof(u8), vs_spv, &vert_info);

    
    SpvReflectShaderModule frag_info = {0};
    VkShaderModule frag = vk_shader_module_create(device, fs_spv, ARRAY_COUNT(fs_spv)*sizeof(u8));
    spvReflectCreateShaderModule(ARRAY_COUNT(fs_spv)*sizeof(u8), fs_spv, &frag_info);

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

    //TODO: shader introspection!!!! (we already have mr. spirv_reflect with us!)
    VkPipelineVertexInputStateCreateInfo vert_input_ci = {0};
    vert_input_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vert_input_ci.vertexBindingDescriptionCount = 0;
    vert_input_ci.pVertexBindingDescriptions = NULL; // Optional
    vert_input_ci.vertexAttributeDescriptionCount = 0;
    vert_input_ci.pVertexAttributeDescriptions = NULL; // Optional

    VkPipelineInputAssemblyStateCreateInfo input_assembly_ci = {0};
    input_assembly_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_ci.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_ci.primitiveRestartEnable = VK_FALSE;


    VkViewport viewport = {0};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    //TODO: maybe we need to pass the swapchain in the pipe_create function? we need it kinda a lot
    viewport.width = (float) 800; 
    viewport.height = (float) 600;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor = {0};
    scissor.offset = (VkOffset2D){0, 0};
    scissor.extent = (VkExtent2D){800,600};

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



    VkPipelineLayoutCreateInfo pipe_layout_ci = {0};
    pipe_layout_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipe_layout_ci.setLayoutCount = 0; // Optional
    pipe_layout_ci.pSetLayouts = NULL; // Optional
    pipe_layout_ci.pushConstantRangeCount = 0; // Optional
    pipe_layout_ci.pPushConstantRanges = NULL; // Optional
    VK_CHECK(vkCreatePipelineLayout(device, &pipe_layout_ci, NULL, &pipe_layout));


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
    pipeline_ci.renderPass = NULL;
    pipeline_ci.pVertexInputState = &vert_input_ci;
    pipeline_ci.pInputAssemblyState = &input_assembly_ci;
    pipeline_ci.pViewportState = &viewport_state_ci;
    pipeline_ci.pRasterizationState = &rasterizer_ci;
    pipeline_ci.pMultisampleState = &multisampling_ci;
    pipeline_ci.pDepthStencilState = NULL; // Optional
    pipeline_ci.pColorBlendState = &color_blend_ci;
    pipeline_ci.pDynamicState = &ds_ci;
    pipeline_ci.layout = pipe_layout;

    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_ci, NULL, &pipe));
    vk_PipeBundle bundle = {0};
    bundle.pipe = pipe;
    bundle.layout = pipe_layout;
    bundle.vert = vert;
    bundle.vert_info = vert_info;
    bundle.frag = frag;
    bundle.frag_info = frag_info;
    return bundle;
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
    render_info.renderArea.extent = (VkExtent2D){800,600};
    render_info.layerCount = 1;
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachments = &color_attachment;
    vkCmdBeginRenderingKHR(ctx->cmdbuf, &render_info);


    vkCmdBindPipeline(ctx->cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, ctx->pipe.pipe);

    VkViewport viewport = {0};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = 800;
    viewport.height = 600;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(ctx->cmdbuf, 0, 1, &viewport);

    VkRect2D scissor = {0};
    scissor.offset = (VkOffset2D){0, 0};
    scissor.extent = (VkExtent2D){800,600};
    vkCmdSetScissor(ctx->cmdbuf, 0, 1, &scissor);


    /*
    VkClearAttachment ca = {0};
    ca.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    ca.colorAttachment = VK_ATTACHMENT_UNUSED;
    VkClearColorValue ccv = (VkClearColorValue){1.f,1.f,1.f,1.f};
    ca.clearValue.color = ccv;
    VkRect2D rectx = (VkRect2D){(VkOffset2D){0,0},(VkExtent2D){800,600}};
    VkClearRect rect = (VkClearRect){rectx, 0,1};
    vkCmdClearAttachments(ctx->cmdbuf, 1, &ca, 1, &rect);
    */
    vkCmdDraw(ctx->cmdbuf, 3, 1, 0, 0);
    vkCmdDraw(ctx->cmdbuf, 3, 1, 0, 0);
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

extern vgContext c;
void draw_frame(){
    vgContext *ctx = &c;
    vkWaitForFences(ctx->device, 1, &ctx->in_flight_fence, VK_TRUE, UINT64_MAX);
    vkResetFences(ctx->device, 1, &ctx->in_flight_fence);
    vkAcquireNextImageKHR(ctx->device, ctx->swap_bundle.swap, UINT64_MAX, ctx->image_available_sem, VK_NULL_HANDLE, &ctx->image_index);
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
    vkQueuePresentKHR(ctx->present_queue, &present_info);
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
    ctx->pipe = vk_pipe_bundle_create(ctx->device);
    ASSERT(ctx->pipe.pipe);
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
    printf("vg_init success!\n");
    return M_OK;
}
M_RESULT vg_cleanup(vgContext *ctx){
    vkQueueWaitIdle(ctx->present_queue);
    vkQueueWaitIdle(ctx->graphics_queue);
    vkDestroyCommandPool(ctx->device, ctx->base_command_pool, NULL);
    vk_pipe_bundle_cleanup(ctx->device, &ctx->pipe);
    vk_swap_bundle_swap_cleanup(ctx->device, &ctx->swap_bundle);
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
#endif