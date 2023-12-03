#version 450 

layout (location = 0) in vec4 inColor;
layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform UBOScene {
	mat4 projection;
	mat4 view;
} uboCamera;

void main() 
{
    outColor = inColor;
}