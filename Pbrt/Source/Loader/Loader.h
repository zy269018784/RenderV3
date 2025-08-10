#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Util/TriangleMesh.h>
#include <Scene/Model.h>
#include <vector>
using namespace std;
namespace Render {
	class Loader
	{
	public:
		Loader();
		Model* loadFile(std::string name);
		void processNode(aiNode* node, aiMatrix4x4& m, const aiScene* scene);
		void processMesh(aiNode* node, aiMatrix4x4 &m, aiMesh* mesh, const aiScene* scene);
	//	void processTexture(const aiScene* scene);
		void processMaterial(const aiScene* scene);

		void output(std::string filename, const aiScene* scene);
		Model* model;
	};
}