#include <vector>

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

#include "getUVs.hpp"

using namespace glm;

/*
CN:
CT 值：
骨组织:		>400	肝脏:	50-70	脾脏:	35-60
血块:		64-84	胰腺:	30-55	肾脏:	25-50
肌肉:		40-55	胆囊:	10-30	血液:	13-32
水:			0			血浆:	3-14		脂肪:	-100-[-20]
脑白质:		25-34	脑灰质:	28-44	脑脊液:	3-8
甲状腺:		50-90

目前贴图只区分:
1. 脂肪			-100-[-20]	RGB: (255,165,40)
2. 水				0				RGB: (127,255,212)
3. 血浆			3-10			RGB: (135,35,10)
4. 胆囊			11-30		RGB: (0,139,0)
5. 混合部分	31-50		RGB: (235,20,90)
6. 肝脏			51-70		RGB: (0,205,0)
7. 骨				>400		RGB: (190,190,190)
肾脏，胰腺，肌肉，脾脏统合为一种颜色(5)，因为目前难以区分
其他CT值为白色 (255,255,255)

脑部几个加甲状腺没处理，
可能需要dcmtk获取tag：Body Part Examined (0018,0015) --> HEAD
*/

//void getUVs(
//	const std::vector<vec3> & vertices, 
//	const std::vector<int> & colors,
//	std::vector<vec2> & uvs
//) {
//	//	1. 脂肪			-100 - [-20]
//	//	2. 水				0
//	//	3. 血浆			3 - 10
//	//	4. 胆囊			11 - 30
//	//	5. 混合部分	31 - 50
//	//	6. 肝脏			51 - 70
//	//	7. 骨				>400
//
//	int color1 = 0;
//	int color2 = 0;
//	int color3 = 0;
//	int color4 = 0;
//	for (auto color : colors)
//	{
//		vec2 uv;
//		if (color >= 0 && color <= 100)
//		{
//			color1++;
//		}
//		else if (color >= 101 && color <= 180)
//		{
//			color2++;
//		}
//		else if (color >= 181 && color <= 255)
//		{
//			color3++;
//		}
//		else
//		{
//			color4++;
//		}
//	}
//
//	float size1 = 0.5 / (color1+1);
//	float size2 = 0.5 / (color2+1);
//	float size3 = 0.5 / (color3+1);
//	float size4 = 0.5 / (color4+1);
//	int count1 = 1;
//	int count2 = 1;
//	int count3 = 1;
//	int count4 = 1;
//
//	for (auto color : colors)
//	{
//		vec2 uv;
//		if (color >= 0 && color <= 100)
//		{
//			uv = vec2(0.0f + count1 * size1, 1.0f - 0.25f);
//			count1++;
//		}
//		else if (color >= 101 && color <= 180)
//		{
//			uv = vec2(0.5f + count2 * size2, 1.0f - 0.25f);
//			count2++;
//		}
//		else if (color >= 181 && color <= 255)
//		{
//			uv = vec2(0.0f + count3 * size3, 1.0f - 0.75f);
//			count3++;
//		}
//		else
//		{
//			uv = vec2(0.5f + count4 * size4, 1.0f - 0.75f);
//			count4++;
//		}
//		uvs.push_back(uv);
//	}
//}

//void getUVs(
//	const std::vector<vec3> & vertices,
//	const std::vector<int> & colors,
//	std::vector<vec2> & uvs
//) {
//	//	1. 脂肪			-100 - [-20]
//	//	2. 水				0
//	//	3. 血浆			3 - 10
//	//	4. 胆囊			11 - 30
//	//	5. 混合部分	31 - 50
//	//	6. 肝脏			51 - 70
//	//	7. 骨				>400
//
//	for (auto color : colors)
//	{
//		vec2 uv;
//		if (color >=0 && color <= 100)
//		{
//			uv = vec2(0.25f, 1.0f - 0.25f);
//		}
//		else if (color >= 101 && color <= 180)
//		{
//			uv = vec2(0.75f, 1.0f - 0.25f);
//		}
//		else if (color >= 181 && color <= 255)
//		{
//			uv = vec2(0.25f, 1.0f - 0.75f);
//		}
//		else
//		{
//			uv = vec2(0.75f, 1.0f - 0.75f);
//		}
//		uvs.push_back(uv);
//	}
//}


void getUVs(
	const std::vector<vec3> & vertices,
	const std::vector<int> & colors,
	std::vector<vec2> & uvs,
	const uint8_t THRESHOLD
) {
	//	1. 脂肪			-100 - [-20]
	//	2. 水				0
	//	3. 血浆			3 - 10
	//	4. 胆囊			11 - 30
	//	5. 混合部分	31 - 50
	//	6. 肝脏			51 - 70
	//	7. 骨				>400

	for (auto color : colors)
	{
		if (color < THRESHOLD)
		{
			color = THRESHOLD;
		}
		vec2 uv;
		if (color < 0)
		{
			uv = vec2(0.0f+0.01f, 1.0f - 0.25f);
		}
		else if (color == 0)
		{
			uv = vec2(0.125f + 0.01f, 1.0f - 0.25f);
		}
		else if (color >= 1 && color <= 10)
		{
			uv = vec2(0.125f * 2 + 0.01f, 1.0f - 0.25f);
		}
		else if (color >= 11 && color <= 30)
		{
			uv = vec2(0.125f * 3 + 0.01f, 1.0f - 0.25f);
		}
		else if (color >= 31 && color <= 50)
		{
			uv = vec2(0.125f * 4 + 0.01f, 1.0f - 0.25f);
		}
		else if (color >= 51 && color <= 70)
		{
			uv = vec2(0.125f * 5 + 0.01f, 1.0f - 0.25f);
		}
		else if (color >= 70)
		{
			uv = vec2(0.125f * 6 + 0.01f, 1.0f - 0.25f);
		}
		uvs.push_back(uv);
	}
}

