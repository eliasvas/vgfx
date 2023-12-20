//This example is a simple deferred renderer, it shows the basic usage of the library.
//Not many assets where used, I don't want to deal with assets inside a source repo. 

#include <stdio.h>
#include <stdlib.h>
#include "base.h"
#define VOLK_IMPLEMENTATION
#include "vgfx.h"
vgContext c;
GLFWwindow *window;
int main(int argc, char *argv[]){
    printf("Game started!\n");
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, "example", NULL, NULL);

    vg_init(&c, window);

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        draw_sample_frame(&c);
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    vg_cleanup(&c);

    return 0;
}