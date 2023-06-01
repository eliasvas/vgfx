#include <stdio.h>
#include <stdlib.h>
#include "base.h"
#define VOLK_IMPLEMENTATION
#include <volk/volk.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main(){
    volkInitialize();
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan Window", NULL, NULL);

    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);

    printf("extension count: [%i]\n", extension_count);
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}