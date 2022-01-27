//#pragma once
#ifndef TRI_H
#define TRI_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>

using namespace std;
#define NUM 400
#define THICK 0.01f
#define Map(x) 0.5f;

class Tri
{
public:
	glm::vec3 Vertex[2 + 2 * NUM];
	float half_length;
	/*float tri[4 * NUM * 3 * 6] = {0.0};*/
	float tri[2 * NUM * 3 * 6] = { 0.0 };
	Tri(float length)
	{
		half_length = length/2.0;
		Vertex[0] = glm::vec3(-half_length, 0.0f, 0.0f);
		for (int i = 0;i < NUM;i++)
		{
			float x = itox(i);
			float z = Map(x);//默认的这么大
			Vertex[1 + i] = glm::vec3(x, THICK, z);
			Vertex[1 + i + NUM] = glm::vec3(x, 0.0f, z);
		}
		Vertex[2 * NUM + 1] = glm::vec3(half_length, 0.0f, 0.0f);
		draw();
	}
	void change_at_pos(float pos, float h)//修改i这个地方的高度
	{
		int i = xtoi(pos);//注意，这里并没有检查上界——应该交给应用层去检查
		float x = itox(i);
		float z = h;
		float thick = THICK;
		if (z == 0.0f)
		{
			thick = 0.0f;
		}
		else {
			//
		}
		Vertex[1 + i] = glm::vec3(x, thick, z);
		Vertex[1 + i + NUM] = glm::vec3(x, 0.0f, z);
		//两边的也要变化——注意边界条件！！不要越界
		if (i > 0)
		{
			if (Vertex[i].z >= 0.5)
			{
				//cout << Vertex[i].z << endl;
				float z1 = max(Vertex[i].z - 0.00001, 0.0);
				float z2 = max(Vertex[i + NUM].z - 0.00001, 0.0);
				if (z1 == 0.0)
				{
					Vertex[i] = glm::vec3(Vertex[i].x, thick, z1);
				}
				else
				{
					Vertex[i] = glm::vec3(Vertex[i].x, THICK, z1);
				}
				/*cout << max(Vertex[i].z - 0.00000001, 0.0) << endl;*/
				Vertex[i + NUM] = glm::vec3(Vertex[i + NUM].x, 0.0f, z2);
			}
		}
		if (i < NUM-1)
		{
			if (Vertex[i + 2].z >= 0.5)
			{
				//cout <<"right:"<< Vertex[i + NUM + 2].z << endl;
				float z1 = max(Vertex[i + 2].z - 0.00001, 0.0);
				float z2 = max(Vertex[i + NUM + 2].z - 0.00001, 0.0);
				if (z1 == 0.0)
				{
					Vertex[i + 2] = glm::vec3(Vertex[i + 2].x, thick, z1);
				}
				else
				{
					Vertex[i + 2] = glm::vec3(Vertex[i + 2].x, THICK, z1);
				}
				Vertex[i + NUM + 2] = glm::vec3(Vertex[i + NUM + 2].x, 0.0f, z2);
			}
		}
		draw();
	}
	float get_height(float xpos)
	{
		int i = xtoi(xpos);
		return Vertex[1 + i].z;

	}
private:
	float itox(int i) {//注意，这里并没有检查上界——应该交给应用层去检查
		return 2.0*half_length / (NUM - 1.0) * i - half_length;
	};
	int xtoi(float x)
	{
		float piece = 2 * half_length / (NUM - 1);
		int pos = (x + half_length) / piece;
		float x1 = itox(pos);
		float x2 = itox(pos + 1);
		float d1 = x - x1;
		float d2 = x2 - x;
		if (d1 < d2)
		{
			return pos;
		}
		else
		{
			return pos + 1;
		}
	}
	void draw_triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, int i)
	{
		float x0 = v1.x;
		float x1 = v2.x;
		float x2 = v3.x;
		float y0 = v1.y;
		float y1 = v2.y;
		float y2 = v3.y;
		float z0 = v1.z;
		float z1 = v2.z;
		float z2 = v3.z;

		//顶点的位置
		tri[18 * i] = x0;
		tri[18 * i + 1] = y0;
		tri[18 * i + 2] = z0;
		tri[18 * i + 6] = x1;
		tri[18 * i + 6 + 1] = y1;
		tri[18 * i + 6 + 2] = z1;
		tri[18 * i + 12] = x2;
		tri[18 * i + 12 + 1] = y2;
		tri[18 * i + 12 + 2] = z2;
		//法向量
		v1 = glm::vec3(x1 - x0, y1 - y0, z1 - z0);
		v2 = glm::vec3(x2 - x0, y2 - y0, z2 - z0);
		glm::vec3 norm = glm::normalize(glm::cross(v2, v1));

		//glm::vec4 vec(norm.x, norm.y, norm.z, 1.0f);
		//// 译注：下面就是矩阵初始化的一个例子，如果使用的是0.9.9及以上版本
		//// 下面这行代码就需要改为:
		//// glm::mat4 trans = glm::mat4(1.0f)
		//// 之后将不再进行提示
		//glm::mat4 trans = glm::mat4(1.0f);
		////trans = glm::rotate(trans, glm::radians(90.0), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
		//trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//vec = trans * vec;
		//norm.x = vec.a;
		//norm.y = vec.b;
		//norm.z = vec.g;

		
		//norm = glm::rotate(norm, glm::radians(90.0), glm::vec3(1.0, 0.0, 0.0));
		//cout << "norm:" << norm.x << "," << norm.y << "," << norm.z << endl;
		for (int j = 0;j < 3;j++)
		{
			tri[18 * i + 6 * j + 3] = norm.x;
			tri[18 * i + 6 * j + 4] = norm.y;
			tri[18 * i + 6 * j + 5] = norm.z;
		}
	}
	//void upper_layer()
	//{
	//	for (int i = 0;i < NUM - 1;i++)
	//	{
	//		glm::vec3 v1 = Vertex[0];
	//		glm::vec3 v2 = Vertex[1 + i];
	//		glm::vec3 v3 = Vertex[2 + i];
	//		draw_triangle(v1, v2, v3, i);
	//	}
	//	//单独的最后一个点——
	//	glm::vec3 v1 = Vertex[0];
	//	glm::vec3 v2 = Vertex[NUM];
	//	glm::vec3 v3 = Vertex[2 * NUM + 1];
	//	draw_triangle(v1, v2, v3, NUM-1);
	//}
	//void lower_layer()
	//{
	//	for (int i = NUM;i < 2*NUM - 1;i++)
	//	{
	//		glm::vec3 v1 = Vertex[0];
	//		glm::vec3 v2 = Vertex[1 + i];
	//		glm::vec3 v3 = Vertex[2 + i];
	//		draw_triangle(v1, v2, v3, i);
	//	}
	//	//单独的最后一个点——
	//	glm::vec3 v1 = Vertex[0];
	//	glm::vec3 v2 = Vertex[2*NUM];
	//	glm::vec3 v3 = Vertex[2 * NUM + 1];
	//	draw_triangle(v1, v2, v3, 2*NUM - 1);
	//}
	void back()
	{
		//float x0 = Vertex[0].x;
		//float x1 = Vertex[1 + NUM].x;
		//float x2 = Vertex[1].x;
		//float y0 = Vertex[0].y;
		//float y1 = Vertex[1 + NUM].y;
		//float y2 = Vertex[1].y;
		//float z0 = Vertex[0].z;
		//float z1 = Vertex[1 + NUM].z;
		//float z2 = Vertex[1].z;
		//tri[18 * (2 * NUM)] = x0;
		//tri[18 * (2 * NUM) + 1] = y0;
		//tri[18 * (2 * NUM) + 2] = z0;
		//tri[18 * (2 * NUM) + 6] = x1;
		//tri[18 * (2 * NUM) + 6 + 1] = y1;
		//tri[18 * (2 * NUM) + 6 + 2] = z1;
		//tri[18 * (2 * NUM) + 12] = x2;
		//tri[18 * (2 * NUM) + 12 + 1] = y2;
		//tri[18 * (2 * NUM) + 12 + 2] = z2;
		//glm::vec3 v1 = glm::vec3(x1 - x0, y1 - y0, z1 - z0);
		//glm::vec3 v2 = glm::vec3(x2 - x0, y2 - y0, z2 - z0);
		////norm = glm::normalize(glm::cross(v1, v2));
		//glm::vec3 norm = glm::vec3(0.0, 1.0, 0.0);
		//cout << "norm:" << norm.x << "," << norm.y << "," << norm.z << endl;
		//for (int j = 0;j < 3;j++)
		//{
		//	tri[18 * (2 * NUM) + 6 * j + 3] = norm.x;
		//	tri[18 * (2 * NUM) + 6 * j + 4] = norm.y;
		//	tri[18 * (2 * NUM) + 6 * j + 5] = norm.z;
		//}
		float x0 = Vertex[0].x;
		float x1 = Vertex[1 + NUM].x;
		float x2 = Vertex[1].x;
		float y0 = Vertex[0].y;
		float y1 = Vertex[1 + NUM].y;
		float y2 = Vertex[1].y;
		float z0 = Vertex[0].z;
		float z1 = Vertex[1 + NUM].z;
		float z2 = Vertex[1].z;
		tri[0] = x0;
		tri[1] = y0;
		tri[2] = z0;
		tri[6] = x1;
		tri[6 + 1] = y1;
		tri[6 + 2] = z1;
		tri[12] = x2;
		tri[12 + 1] = y2;
		tri[12 + 2] = z2;
		glm::vec3 v1 = glm::vec3(x1 - x0, y1 - y0, z1 - z0);
		glm::vec3 v2 = glm::vec3(x2 - x0, y2 - y0, z2 - z0);
		glm::vec3 norm = glm::normalize(glm::cross(v2, v1));
		//glm::vec3 norm = glm::vec3(0.0, 1.0, 0.0);//————这里因为法向量是x方向的，所以看起来会是黑色？
		//norm = normalize(glm::vec3(1.0, 0.0, 0.0));
		//cout << "norm:" << norm.x << "," << norm.y << "," << norm.z << endl;
		for (int j = 0;j < 3;j++)
		{
			tri[ 6 * j + 3] = norm.x;
			tri[ 6 * j + 4] = norm.y;
			tri[ 6 * j + 5] = norm.z;
		}
	}
	void front()
	{
		//float x0 = Vertex[2 * NUM + 1].x;
		//float x1 = Vertex[2 * NUM].x;
		//float x2 = Vertex[NUM].x;
		//float y0 = Vertex[2 * NUM + 1].y;
		//float y1 = Vertex[2 * NUM].y;
		//float y2 = Vertex[NUM].y;
		//float z0 = Vertex[2 * NUM + 1].z;
		//float z1 = Vertex[2 * NUM].z;
		//float z2 = Vertex[NUM].z;
		//tri[18 * (2 * NUM + 1)] = x0;
		//tri[18 * (2 * NUM + 1) + 1] = y0;
		//tri[18 * (2 * NUM + 1) + 2] = z0;
		//tri[18 * (2 * NUM + 1) + 6] = x1;
		//tri[18 * (2 * NUM + 1) + 6 + 1] = y1;
		//tri[18 * (2 * NUM + 1) + 6 + 2] = z1;
		//tri[18 * (2 * NUM + 1) + 12] = x2;
		//tri[18 * (2 * NUM + 1) + 12 + 1] = y2;
		//tri[18 * (2 * NUM + 1) + 12 + 2] = z2;
		//glm::vec3 v1 = glm::vec3(x1 - x0, y1 - y0, z1 - z0);
		//glm::vec3 v2 = glm::vec3(x2 - x0, y2 - y0, z2 - z0);
		////norm = glm::normalize(glm::cross(v1, v2));
		//glm::vec3 norm = glm::vec3(0.0, 1.0, 0.0);
		//cout << "norm:" << norm.x << "," << norm.y << "," << norm.z << endl;
		//for (int j = 0;j < 3;j++)
		//{
		//	tri[18 * (2 * NUM + 1) + 6 * j + 3] = norm.x;
		//	tri[18 * (2 * NUM + 1) + 6 * j + 4] = norm.y;
		//	tri[18 * (2 * NUM + 1) + 6 * j + 5] = norm.z;
		//}
		float x0 = Vertex[2 * NUM + 1].x;
		float x1 = Vertex[2 * NUM].x;
		float x2 = Vertex[NUM].x;
		float y0 = Vertex[2 * NUM + 1].y;
		float y1 = Vertex[2 * NUM].y;
		float y2 = Vertex[NUM].y;
		float z0 = Vertex[2 * NUM + 1].z;
		float z1 = Vertex[2 * NUM].z;
		float z2 = Vertex[NUM].z;
		tri[18 * (1)] = x0;
		tri[18 * (1) + 1] = y0;
		tri[18 * ( 1) + 2] = z0;
		tri[18 * (1) + 6] = x1;
		tri[18 * ( 1) + 6 + 1] = y1;
		tri[18 * ( 1) + 6 + 2] = z1;
		tri[18 * (1) + 12] = x2;
		tri[18 * (1) + 12 + 1] = y2;
		tri[18 * (1) + 12 + 2] = z2;
		glm::vec3 v1 = glm::vec3(x1 - x0, y1 - y0, z1 - z0);
		glm::vec3 v2 = glm::vec3(x2 - x0, y2 - y0, z2 - z0);
		//norm = glm::normalize(glm::cross(v1, v2));
		glm::vec3 norm = glm::normalize(glm::cross(v2, v1));
		//glm::vec3 norm = glm::vec3(0.0, 1.0, 0.0);
		//norm = glm::normalize(glm::vec3(0.0, 1.0, 0.0));
		//cout << "norm:" << norm.x << "," << norm.y << "," << norm.z << endl;
		for (int j = 0;j < 3;j++)
		{
			tri[18 * (1) + 6 * j + 3] = norm.x;
			tri[18 * (1) + 6 * j + 4] = norm.y;
			tri[18 * (1) + 6 * j + 5] = norm.z;
		}
	}
	void side()
	{
		//for (int i = 1;i < NUM;i++)
		//{
		//	glm::vec3 p1 = Vertex[i];
		//	glm::vec3 p2 = Vertex[i + NUM];
		//	glm::vec3 p3 = Vertex[i + 1];
		//	glm::vec3 p4 = Vertex[i + 1 + NUM];

		//	//四个点画两个三角形 ①p1,p2,p4 ②p4,p3,p1
		//	draw_triangle(p1, p2, p4, 2 * NUM + 2 * i);
		//
		//	//第二个三角形
		//	draw_triangle(p4, p3, p1, 2 * NUM + 2 * i+1);
		//}
		for (int i = 1;i < NUM;i++)
		{
			glm::vec3 p1 = Vertex[i];
			glm::vec3 p2 = Vertex[i + NUM];
			glm::vec3 p3 = Vertex[i + 1];
			glm::vec3 p4 = Vertex[i + 1 + NUM];

			//四个点画两个三角形 ①p1,p2,p4 ②p4,p3,p1
			draw_triangle(p1, p2, p4,2 * i);

			//第二个三角形
			draw_triangle(p4, p3, p1, 2 * i + 1);
		}
	}
	void draw()//初始化以及每次发生变化的时候就要调用一次
	{
		//upper_layer();
		//lower_layer();
		back();
		front();
		side();
	}
};

#endif