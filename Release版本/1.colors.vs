#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
//out float Cut;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	//根据 aPos.z的高度来判断是否切割——传出一个bool出去
	/*if(aPos.z<0.5)
	{
		Cut=1.0;
	}
	else
	{
		Cut=0.0;
	}*/
	FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoords;

	//gl_Position = projection * view * model * vec4(FragPos, 1.0);

	gl_Position = projection * view  * vec4(FragPos, 1.0);
}