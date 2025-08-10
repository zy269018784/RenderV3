#include <Loader/Loader.h>
#include <iostream>
#include <Math/Math.h>
#include <Shape/Triangle.h>
#include <Util/TriangleMesh.h>
#include <Scene/Scene.h>
#include <assimp/texture.h>
#include <Init.h>
#include <chrono>

using namespace std;
using namespace Render;

namespace Render {
	Loader::Loader()
	{
		model = nullptr;
	}

	Model * Loader::loadFile(std::string path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate
			//aiProcess_ConvertToLeftHanded
			// aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
		);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString()  << endl;
			return nullptr;
		}

		auto start = std::chrono::high_resolution_clock::now();

		model = new Model;
		aiMatrix4x4 mat;
		if (model) 
			processNode(scene->mRootNode, mat, scene);
		
		//processTexture(scene);
		//processMaterial(scene);
		

		auto end = std::chrono::high_resolution_clock::now();
		auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		std::cout << "processNode cost:" << delta.count() << std::endl;
		return model;
	}

	aiMatrix4x4 getNodeTransform(aiNode* node) {
		std::vector<aiMatrix4x4> mats;
		while (node->mParent != nullptr) {
			mats.push_back(node->mTransformation);
		}

		aiMatrix4x4 m;
		for (auto m1 = mats.rbegin(); m1 != mats.rend(); m1++) {
			m = m1[0] * m;
		}
		return m;
	}

	void Loader::processNode(aiNode* node, aiMatrix4x4& m, const aiScene* scene)
	{
		aiMatrix4x4 mat = m * node->mTransformation;
		//aiMatrix4x4 mat = node->mTransformation * m;
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(node, mat, mesh, scene);
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], mat, scene);
		}
	}



	void Loader::processMesh(aiNode * node, aiMatrix4x4& m, aiMesh* mesh, const aiScene* scene)
	{		
		// 顶点索引偏移
		unsigned int offset = model->vertexs.size();
		model->meshes.push_back(mesh->mName.C_Str());
		model->meshMaterialIndex.push_back(mesh->mMaterialIndex);
		if (mesh->HasPositions()) {

			// 解析顶点坐标, 法线.
			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
				aiVector3D aa, bb;
				aa.x = mesh->mVertices[i].x;
				aa.y = mesh->mVertices[i].y;
				aa.z = mesh->mVertices[i].z;
			//	bb = m * aa;
				bb = aa;
				model->vertexs.push_back({ bb.x, bb.y, bb.z });
				if (mesh->HasNormals()) {

					model->normals.push_back({ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z});
					//model->normals.push_back({ mesh->mNormals[i].x, mesh->mNormals[i].z, - mesh->mNormals[i].y });
					//cout << bb.x << " " << bb.y << " " << bb.z << endl;
					//model->normals.push_back({ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z });
				}
			}	

			// 纹理坐标 
			for (unsigned int j = 0; j < mesh->mNumVertices; j++)
			{
				if (mesh->mTextureCoords[0])
				{
					Float u = mesh->mTextureCoords[0][j].x;
					Float v = mesh->mTextureCoords[0][j].y;
					model->uv.push_back({ u, v });
				}
			}

			// 解析顶点索引
			if (mesh->HasFaces())
			{
				// 保存网格索引的起始位置
				model->meshFirstIndex.push_back(model->indices.size());
				// 统计网格索引数量
				unsigned int indicesCount = 0;
				// 遍历网格的面
				for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
					aiFace* face = mesh->mFaces + i;
					// 统计网格索引数量
					indicesCount += face->mNumIndices;
					// 保存索引
					for (unsigned int k = 0; k < face->mNumIndices; k++) {
						model->indices.push_back(face->mIndices[k] + offset);
					}
				}
				if (indicesCount % 3 != 0) {
					cout << "non 3 mesh "  << indicesCount << endl;
					for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
						aiFace* face = mesh->mFaces + i;
						cout << face->mNumIndices << " ";

					}
					cout << endl;
				}
				// 保存网格索引数量
				model->meshIndexCount.push_back(indicesCount);
			}
			else {
				cout << "mesh has no face" << endl;
			}
		}
	}

	//void Loader::processTexture(const aiScene* scene)
	//{
	//	//for (int i = 0; i < scene->mNumTextures; i++) {
	//	//	model->textures[scene->mTextures[i]->mFilename.C_Str()] = i;
	//	//	cout << scene->mTextures[i]->mFilename.C_Str() << endl;
	//	//}
	//	//cout << scene->mNumTextures << "----------------------------------------------\n\n\n\n\n";
	//}

	void Loader::processMaterial(const aiScene* scene)
	{

		cout << "scene->mNumMaterials " << scene->mNumMaterials << endl;
		for (int i = 0; i < scene->mNumMaterials; i++)
		{
			model->materials.push_back(scene->mMaterials[i]->GetName().C_Str());
			unsigned int textureCount = scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE);
			cout << "material name " << scene->mMaterials[i]->GetName().C_Str() 
				<< "   textureCount " << textureCount
				<< endl;
			
			if (textureCount <= 0)
			{
				aiColor3D color;
				scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, color);
				RGB rgb(color.r * 255, color.g * 255, color.b * 255);
				model->texture_rgb.push_back(rgb);
				model->texture_image.push_back("");
				model->texture_type.push_back(0);
				printf("rgb texture\n");
			}
			else {
				//if (scene->mNumTextures <= 0)
				//{
				//	aiColor3D color;
				//	//scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, color);
				//	//RGB rgb(255, 255, 255);
				//	RGB rgb(color.r * 255, color.g * 255, color.b * 255);
				//	model->texture_rgb.push_back(rgb);
				//	model->texture_image.push_back("");
				//	model->texture_type.push_back(0);
				//	continue;
				//}
			
				RGB rgb(0, 0, 0);
				model->texture_rgb.push_back(rgb);
				model->texture_type.push_back(1);
				for (unsigned int j = 0; j < 1; j++)
				{
					aiString str;
					scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, j, &str);
					string ss = str.C_Str();
					cout << "ss " << ss << " " << atoi(ss.data() + 1) << " "
						<< scene->mNumTextures << " "
						//<< scene->mTextures[0]->mFilename.C_Str()
						<< endl;
					if (str.data[0] == '*')
					{
						ss = string() + scene->mTextures[atoi(ss.data() + 1)]->mFilename.C_Str() + ".jpg";
					}
					//cout << str.C_Str() << "\t" << ss  << endl;
					model->texture_image.push_back(ss);
				}
			}
		}
	}

	void Loader::output(std::string filename, const aiScene* scene) {
	
	}
}