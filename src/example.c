
//SAMPLE project (to test the engine):
//Pokemon type advantage game! (OSU style graphics, meaning GOOD responsive, animated UI code)
//pokemon mystery dungeon style background shader + two types + how much advantage + win animation
//MUST be playable in the browser
//MUST have normal mode AND memorization mode AND specific type mode
//Maybe even some DYNAMIC and kinda procedural? (based on 1,1/2,2 OR type) physics based animations
//REQ: ZERO assets, the Ball vs Ball of types will be generated of course,
//the sounds can be generated (most anyway others can be .ini'ed), animations too, UI too (ini'ed) 

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