#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform UBOScene {
	mat4 projection;
	mat4 view;
} uboCamera;

void main()
{
	gl_Position = vec4(vPosition, 1.0f);
	outColor = vec4(vColor,1.0);
}
