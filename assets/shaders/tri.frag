#version 450 

layout (location = 0) in vec4 inColor;
layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform UBOScene {
	mat4 projection;
	mat4 view;
} uboCamera;

layout (set = 1, binding = 0) uniform UBOTestFrag {
	vec4 color_mod;
} uboTestFrag;

void main() 
{
    outColor = inColor + uboTestFrag.color_mod;
}