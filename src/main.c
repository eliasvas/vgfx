#include <stdio.h>
#include <stdlib.h>
#include "base.h"
#define VOLK_IMPLEMENTATION
#include <volk/volk.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vgfx.h"
vgContext c;

int main(){
    printf("Game started!\n");
    volkInitialize();
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan Window", NULL, NULL);

    vg_init(&c);

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    vg_cleanup(&c);

    return 0;
}