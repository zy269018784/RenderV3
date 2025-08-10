#pragma once
#include <Math/Math.h>
#include <Texture/RGB.h>
#include <vector>
#include <string>
#include <map>
using namespace std;
namespace Render{
	class Model
	{
	public:
		vector<Point3f> vertexs;					// 顶点坐标
		vector<int> indices;				// 顶点索引
		vector<Point2f> uv;							// UV
		vector<Normal3f> normals;					// 法线
		//map<string, int> textures;				// 纹理贴图
		vector<string>		 meshes;				// 网格名称
		vector<unsigned int> meshMaterialIndex;		// 网格的材质索引
		vector<unsigned int> meshIndexCount;		// 网格顶点索引的数量
		vector<unsigned int> meshFirstIndex;		// 网格顶点索引的第一个索引
		vector<string> materials;					// 材质名称
		vector<string> texture_image;				// 材质的纹理贴图
		vector<RGB>    texture_rgb;					// 材质的RGB
		vector<unsigned int> texture_type;			// 0: rgb 1:texture
		//vector<unsigned int> material_texture_index;
		//vector<unsigned int> material_rgb_index;
	};
}

