#version 330 core
out vec4 FragColor;
//"in vec4 vertexColor;\n"//从顶点着色器来的变量——名称和类型相同所以
uniform vec4 ourColor; // 在OpenGL程序代码中设定这个变量
void main()
{
   FragColor = ourColor;
}