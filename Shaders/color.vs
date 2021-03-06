#version 450 core
layout (location = 0) in vec3 inputVertexPosition;
layout (location = 1) in vec3 inputVertexNormal;
layout (location = 2) in vec3 inputVertexColor;


out vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	gl_Position = projection * view  * model * vec4(inputVertexPosition, 1.f);
	vertexColor = inputVertexColor;
}
