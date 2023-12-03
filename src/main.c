#include <stdio.h>
#include <stdlib.h>
#include "base.h"
#if OS_WIN
    #define VK_USE_PLATFORM_WIN32_KHR
    #define VOLK_IMPLEMENTATION
    #include "volk.h"
    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif OS_LINUX
    #define VK_USE_PLATFORM_X11_KHR
    #define VOLK_IMPLEMENTATION
    #include "volk.h"
    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>
    #define GLFW_EXPOSE_NATIVE_X11
#else
    #error "Platform not supported!"
#endif
#include <GLFW/glfw3native.h>
#include "vgfx.h"
vgContext c;
GLFWwindow *window;
int main(int argc, char *argv[]){
    printf("Game started!\n");
    volkInitialize();
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, "Vulkan Window", NULL, NULL);

    vg_init(&c);

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        draw_frame();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    vg_cleanup(&c);

    return 0;
}