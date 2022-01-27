#version 330 core
layout (location = 0) in vec3 aPos; //in声明输入变量
out vec4 vertexColor; //out输出变量
uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	gl_Position = projection * view * model *transform * vec4 (aPos, 1.0f);
	vertexColor = vec4(0.5, 0.0, 0.0, 1.0); //但是暂时还没有输入输出交互
};