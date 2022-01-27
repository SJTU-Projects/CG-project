#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out float Cut;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float fixxx;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
    vec3 n=aNormal;
	//根据 aPos.z的高度来判断是否切割——传出一个bool出去
	if(aPos.z<0.5)
	{
		Cut=1.0;
		//if(aPos.x!=0.5 && aPos.x!=-0.5)
			//n=normalize(n+(0.0,0.0,-0.5));
			//n=vec3(0.0,-1.0,0.0);
			//n=vec3(0.0,0.0,-1.0);
	}
	else
	{
		Cut=0.0;
	};
	//if(aPos.x!=0.5 && aPos.x!=-0.5)
	//{	
		//float x=n.x;
		//float z=n.z;
		//n.z=x;
		//n.x=-z;
		
		//n=rotate(n,radians(90.0),vec3(1.0,0.0,0.0));
		//n=normalize(n+(0.0,1.0,1.0));
		//n=vec3(0.0,1.0,0.0);
	//};
	Normal = mat3(transpose(inverse(model))) * n;

	//gl_Position = projection * view * model * vec4(FragPos, 1.0);
	gl_Position = projection * view  * vec4(FragPos, 1.0);
	//正交视角
	/*if(fixxx==1.0)
	{
		//gl_Position =  view  * vec4(FragPos, 1.0);
		gl_Position = vec4(1.0,1.0,1.0, 1.0);
	}
	else{*/
		gl_Position = projection * view  * vec4(FragPos, 1.0);
	//}
}