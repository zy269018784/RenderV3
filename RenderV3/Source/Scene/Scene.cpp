#include "Scene.h"
#include "Math/Transform.h"
#include "Utils/File.h"
#include "Utils/Filesystem.h"
#include "SceneFileFormat/SceneFileFormat.h"

#include <Film/FilmBaseParameters.h>

#include <chrono>
#include <boost/json.hpp>
#include <boost/exception/exception.hpp>
#include <boost/json/stream_parser.hpp>
#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

static glm::mat4 toGLMat4(Render::Transform& t) {
	SquareMatrix<4> mat = t.GetMatrix();
	return glm::mat4(
		mat[0][0], mat[0][1], mat[0][2], mat[0][3],
		mat[1][0], mat[1][1], mat[1][2], mat[1][3],
		mat[2][0], mat[2][1], mat[2][2], mat[2][3],
		mat[3][0], mat[3][1], mat[3][2], mat[3][3]
	);
}

Rt::Scene::Scene(const char* filename) 
{
    Render::BasicSceneBuilder builder;

    auto tokError = [](const char* msg, const FileLoc* loc) {
        printf("%s", msg);
    };
    std::unique_ptr<Tokenizer> t = Tokenizer::CreateFromFile(filename, tokError);
	
	auto start = std::chrono::high_resolution_clock::now();

    parse(&builder, std::move(t)); 
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "parse cost============ " << duration.count() / 1000 << " ms " << std::endl;

    Build(&builder);

	std::cout << "scene build ook--------------------" << std::endl;
#if 0
	std::string SceneFile;
	std::string SceneJson = "{ \"VBO\": { \"filename\": \"1.vbo\" } }";
	//SceneJson = "{  \"Buffers\": [ \"1\", \"2\", \"3\", { \"name\": \"hello\" } ] }";
	boost::json::value Value = boost::json::parse(SceneJson);
	if (Value.is_array())
	{
		std::cout << "is array =============\n";
	//	boost::json::array Array = Value.as_array();
	//	for (int i = 0; i < Array.size(); i++)
	//	{
	//		std::string Key = "VBO";
	//		if (Array[i].is_object())
	//		{
	//			auto Object = Array[i].as_object();
	//			
	//		}
	//	}
	}
	else if (Value.is_object())
	{
		std::cout << "is object =============\n";
	}
	boost::property_tree::ptree ParseRoot;
	//boost::property_tree::ptree PositionsArray;
	std::stringstream StrStream(SceneJson);
	boost::property_tree::read_json(StrStream, ParseRoot);
	auto VBOfilename = ParseRoot.get<std::string>("VBO.filename");
	std::cout << "VBOfilename " << VBOfilename << std::endl;
	//std::string VBOJSON = ParseRoot.get<std::string>("VBO");
	//std::cout << "VBOJSON " << VBOJSON << std::endl;

#endif
}

Rt::Scene::~Scene() {
	//cout << "~Scene" << endl; 
	//cout << "~RtImage" << endl;
	{
		std::vector<RtImage*>::iterator it, et;
		it = images.begin();
		et = images.end();
		for (; it != et; ++it) {
			if (*it) {
				delete (*it);
			}
		}
	}
	//cout << "~RtTexture" << endl;
	{
		std::vector<RtTexture*>::iterator it, et;
		it = textures.begin();
		et = textures.end();
		for (; it != et; ++it) {
			if (*it) {
				delete (*it);
			}
		}
	}
	//cout << "~RtMaterial" << endl;
	{
		std::vector<RtMaterial*>::iterator it, et;
		it = materials.begin();
		et = materials.end();
		for (; it != et; ++it) {
			if (*it) {
				delete (*it);
			}
		}
	}
	//cout << "~Mesh" << endl;
	{
		std::vector<Mesh*>::iterator it, et;
		it = meshes.begin();
		et = meshes.end();
		for (; it != et; ++it) {
			if (*it) {
				delete (*it);
			}
		}
	}

	//cout << "~MeshInstance" << endl;
	{
		std::vector<MeshInstance*>::iterator it, et;
		it = meshInstances.begin();
		et = meshInstances.end();
		for (; it != et; ++it) {
			if (*it) {
				delete (*it);
			}
		}
	}

	//cout << "~Light" << endl; 
	std::vector<Light*>::iterator it, et;
	it = lights.begin();
	et = lights.end();
	for (; it != et; ++it) {
		if (*it) {
			delete (*it);
		}
	}

}

void Rt::Scene::Build(BasicSceneBuilder* builder)
{
    BuildFilm(builder);
    BuildCamera(builder);			// 相机  
    BuildImage(builder);			// 图片  
    BuildTexture(builder);			// 纹理
    BuildMaterial(builder);			// 材质
    BuildShape(builder);			// 形状
	std::cout << "build shape end;" << std::endl;
	BuildLight(builder);			// 光源
}

void Rt::Scene::BuildFilm(BasicSceneBuilder* builder)
{
    auto parameters = builder->film.parameters;
    m_Resolution = Point2i(parameters.GetOneInt("xresolution", 1280), parameters.GetOneInt("yresolution", 720));

    printf("Resolution w:%d h:%d\n", m_Resolution.x, m_Resolution.y);
	m_OutputImageFilename = parameters.GetOneString("filename", "");
	if (m_OutputImageFilename.empty()) {
		m_OutputImageFilename = "default.png";
	}  	
	m_exposure = parameters.GetOneFloat("exposure", 1.0); 
	m_tonemapType = parameters.GetOneInt("tonemapType", 0);
	m_outputGamma = parameters.GetOneFloat("gamma", 2.2);  

}

void Rt::Scene::BuildCamera(BasicSceneBuilder* builder)
{
	glm::vec3 pos = glm::vec3(builder->eye.x, builder->eye.y, builder->eye.z);
	glm::vec3 center = glm::vec3(builder->lookat.x, builder->lookat.y, builder->lookat.z);
	glm::vec3 up = glm::vec3(builder->up.x, builder->up.y, builder->up.z);

	float fov = builder->camera.parameters.GetOneFloat("fov", 90);
	float aspect = m_Resolution.x / (float)m_Resolution.y;

	glm::mat4 transform;
	transform = glm::scale(glm::mat4(1), glm::vec3(-1.0f, 1.0f, 1.0f));
	glm::vec2 resolution = glm::vec2(m_Resolution.x, m_Resolution.y);
	/*
		近平面改为0.5f 与前端对齐
	*/
	m_pMainCamera = new Camera(transform, resolution,pos, center, up ,fov, 0.5f, 1000.f);
}

void Rt::Scene::BuildImage(BasicSceneBuilder* builder)
{  
	SceneData* sceneData = &builder->sceneData;  
	for (int i = 0, n = builder->spectrumImageTextures.size(); i < n; i++) {
		std::string name = sceneData->GetImageTextureNameFromImageTextureIndex(i); 
		std::string filename = builder->spectrumImageTextures[name].parameters.GetOneString("filename", "");
		//printf("Image name:%s filename %s\n", name.c_str(), filename.c_str());  
		RtImage* pImage = new RtImage(name, filename);
		images.push_back(pImage);
	}
}

void Rt::Scene::BuildTexture(BasicSceneBuilder* builder)
{  
	size_t n1 = builder->spectrumImageTextures.size();	// imagemap
	size_t n2 = builder->spectrumConstTextures.size();	// const 即RGB
	size_t n = n1 + n2; 
	ImageOffset = 0;
	ConstOffset = n1; 

	SceneData* sceneData = &builder->sceneData;  
	for (int i = 0; i < n1; i++) {
		int TextureIndex = i;
		std::string name = sceneData->GetImageTextureNameFromImageTextureIndex(i);
		std::string filename = images[i]->filename_;  
		Rt::RtTexture* pTexture = new Rt::RtTexture(name, filename); 
		textures.push_back(pTexture);
		textureMap[name] = textures.size() - 1;
	}

	for (int i = 0; i < n2; i++) {
		int TextureIndex = ConstOffset + i;
		std::string name = sceneData->GetConstTextureNameFromConstTextureIndex(i);  
		auto rgb = builder->spectrumConstTextures[name].parameters.GetFloatArray("rgb"); 
		if (rgb.size() > 0) { 	
			Rt::RtTexture* pTexture = new Rt::RtTexture(name, glm::vec3(rgb[0], rgb[1], rgb[2])); //RGB color(rgb[0] * 255, rgb[1] * 255, rgb[2] * 255);
			textures.push_back(pTexture);
			textureMap[name] = textures.size() - 1;
		}
		else {
			cout << "name " << name << endl;
		}
	}
}

void Rt::Scene::BuildMaterial(BasicSceneBuilder* builder)
{
    //cout << __FUNCTION__ << "--------------------------------------------" <<endl;
	SceneData* sceneData = &builder->sceneData;
	int MaterialCount = sceneData->GetMaterialNameCount(); 
	for (int MaterialIndex = 0; MaterialIndex < MaterialCount; MaterialIndex++) {
		std::string MaterialName = sceneData->GetMaterialNameFromMaterialIndex(MaterialIndex); 	 
		ParameterDictionary MaterialParameters = builder->namedMaterials[MaterialName].parameters;

		std::string MaterialType = MaterialParameters.GetOneString("type", "diffuse");
		std::string TextureName = MaterialParameters.GetOneString("reflectance", "");
		std::string MaterialNormalMap = MaterialParameters.GetOneString("normalmap", "");
		std::string MaterialRoughnessMap = MaterialParameters.GetOneString("roughnessmap", "");
		std::string MaterialMatllicMap = MaterialParameters.GetOneString("metallicmap", "");
		std::string MaterialHeightMap = MaterialParameters.GetOneString("heightmap", "");
		//printf("MaterialName: %s  MaterialType: %s  TextureName: %s  MaterialNormalMap: %s\n", MaterialName.c_str(), MaterialType.c_str(), TextureName.c_str(), MaterialNormalMap.c_str());
  
		RtMaterial* pMaterial = new RtMaterial();
		pMaterial->name_ = MaterialName;
		pMaterial->type_ = MaterialType; 
		pMaterial->textureName_ = TextureName;
		pMaterial->normalMap_ = MaterialNormalMap;  
		pMaterial->roughnessMap_ = MaterialRoughnessMap;
		pMaterial->metallicMap_ = MaterialMatllicMap;
		//pMaterial->heightMap_ = MaterialHeightMap;
		//std::cout << "pMaterial->normalMap_  " << pMaterial->normalMap_ << std::endl;

		// 材质的texture
		int index = -1;
		if (textureMap.find(TextureName) != textureMap.end()) {
			index = textureMap[TextureName];
		} 

		if (index >= 0) {
			RtTexture* pTexture = textures[index];
			if (pTexture->type == 0) {

			} else if(pTexture->type == 1) {
				pMaterial->SetAlbedo(pTexture->rgb_);
			}

			if ("diffuse" == MaterialType) { 
				pMaterial->SetMaterialType(static_cast<int>(MaterialType::Diffuse));
			}
			else if ("glass" == MaterialType) { 
				Float Reflectivity = builder->namedMaterials[MaterialName].parameters.GetOneFloat("reflectivity", 1.0f);
				Float Transmissivity = builder->namedMaterials[MaterialName].parameters.GetOneFloat("transmissivity", 1.0f); 
				pMaterial->SetMaterialType(static_cast<int>(MaterialType::Glass));
				pMaterial->SetGlassReflectivity(Reflectivity);
				pMaterial->SetGlassTransmissivity(Transmissivity);
			}	
			else if ("conductor" == MaterialType) {
				Float uRoughness = builder->namedMaterials[MaterialName].parameters.GetOneFloat("uroughness", 0.0f);
				Float vRoughness = builder->namedMaterials[MaterialName].parameters.GetOneFloat("vroughness", 0.0f);
				bool remapRoughness = builder->namedMaterials[MaterialName].parameters.GetOneBool("remaproughness", true); 
				pMaterial->SetMaterialType(static_cast<int>(MaterialType::Conductor));
				pMaterial->SetConductorURoughness(uRoughness);
				pMaterial->SetConductorVRoughness(vRoughness);
			}
			else if ("dielectric" == MaterialType) {
				Float uRoughness = builder->namedMaterials[MaterialName].parameters.GetOneFloat("uroughness", 0.0f);
				Float vRoughness = builder->namedMaterials[MaterialName].parameters.GetOneFloat("vroughness", 0.0f);
				bool remapRoughness = builder->namedMaterials[MaterialName].parameters.GetOneBool("remaproughness", true); 
				Float reflectivity = builder->namedMaterials[MaterialName].parameters.GetOneFloat("reflectivity", 0.0f); 
				pMaterial->SetMaterialType(static_cast<int>(MaterialType::Dielectric));
				pMaterial->SetConductorURoughness(uRoughness);
				pMaterial->SetConductorVRoughness(vRoughness);
				pMaterial->SetDielectricReflectivity(reflectivity);
			}
			else if ("disney" == MaterialType) {  
				pMaterial->SetMaterialType(static_cast<int>(MaterialType::Disney));
				//p1
				float roughness = MaterialParameters.GetOneFloat("roughness", 0.5f);
				float metallic = MaterialParameters.GetOneFloat("metallic", 0.0f); 
				float specular = MaterialParameters.GetOneFloat("specular", 0.0f);
				float specularTint = MaterialParameters.GetOneFloat("specularTint", 0.0f);
				pMaterial->SetDisneyRoughness(roughness);
				pMaterial->SetDisneyMetallic(metallic);
				pMaterial->SetDisneySpecular(specular);
				pMaterial->SetDisneySpecularTint(specularTint);

				//p2

				//p3
				float transmission = MaterialParameters.GetOneFloat("transmission", 0.0f);
				pMaterial->SetDisneyTransmission(transmission);

				//p4 eta(ior), k(extinction) 
				Point3f k = MaterialParameters.GetOnePoint3f("k", Render::Point3f(1.0, 1.0, 1.0));
				float ior = MaterialParameters.GetOneFloat("ior", 1.0f);
				pMaterial->SetDisneyK( glm::vec3(k.x,k.y,k.z) );
				pMaterial->SetDisneyIOR( ior );

				//p5
				float sheen = MaterialParameters.GetOneFloat("sheen", 0.0f);
				float sheenTint = MaterialParameters.GetOneFloat("sheenTint", 0.5f);
				float clearcoat = MaterialParameters.GetOneFloat("clearcoat", 0.0f);
				float clearcoatGloss = MaterialParameters.GetOneFloat("clearcoatGloss", 1.0f);
				pMaterial->SetDisneySheen(sheen);
				pMaterial->SetDisneySheenTint(sheenTint);
				pMaterial->SetDisneyClearcoat(clearcoat);
				pMaterial->SetDisneyClearcoatGloss(clearcoatGloss);

				//p6
				float subsurface = MaterialParameters.GetOneFloat("subsurface", 0.0f);
				float flatness = MaterialParameters.GetOneFloat("flatness", 0.0f);
				float anisotropy = MaterialParameters.GetOneFloat("anisotropy", 0.0f);
				float atDistance = MaterialParameters.GetOneFloat("atDistance", 0.0f);
				//pMaterial->SetDisneySubsurface(subsurface);
				pMaterial->SetDisneyFlatness(flatness);
				pMaterial->SetDisneyAnisotropy(anisotropy);
				//pMaterial->SetDisneyClearcoatGloss(atDistance);

				//p7
				Point3f emission = MaterialParameters.GetOnePoint3f("emission", Render::Point3f(0, 0, 0)); 
				float emissionTint = MaterialParameters.GetOneFloat("emissionTint", 1.0f);
				pMaterial->SetDisneyEmission(glm::vec3(emission.x, emission.y, emission.z));
				pMaterial->SetDisneyEmissionTint(emissionTint);

				//p8
				int thin = MaterialParameters.GetOneInt("thin", 0);
				pMaterial->SetDisneyThin(thin); 
				printf("9999999999999999999999999999999999999999999------------%f\n", roughness);

				//p2
				int bsdf_props = 0;

				if (roughness < 1.0f) {
					bsdf_props |= BSDF_FLAG_REFLECTION;
				}
				if (transmission > 0.0f) {
					bsdf_props |= BSDF_FLAG_TRANSMISSION;
				}
				if (roughness > 0.08) {
					bsdf_props |= BSDF_FLAG_GLOSSY;
				} else {
					bsdf_props |= BSDF_FLAG_SPECULAR;
				}	
				pMaterial->SetDisneyBSDFProperties(bsdf_props);//pMaterial->SetDisneyBSDFProperties(BSDF_FLAG_ALL);
			}
			else if ("granite" == MaterialType) 
			{
				Float Reflectivity = builder->namedMaterials[MaterialName].parameters.GetOneFloat("reflectivity", 1.0f);
				Float Transmissivity = builder->namedMaterials[MaterialName].parameters.GetOneFloat("transmissivity", 1.0f); 
			}
			else if ("ColoredAluminumPlate1" == MaterialType)
			{
				std::cout << "huan cai lv ban" << std::endl;
				Float uRoughness = builder->namedMaterials[MaterialName].parameters.GetOneFloat("uroughness", 0.0f);
				Float vRoughness = builder->namedMaterials[MaterialName].parameters.GetOneFloat("vroughness", 0.0f);
				//bool remapRoughness = builder->namedMaterials[MaterialName].parameters.GetOneBool("remaproughness", true);
				pMaterial->SetMaterialType(static_cast<int>(MaterialType::ColoredAluminumPlate1));
				pMaterial->SetConductorURoughness(uRoughness);
				pMaterial->SetConductorVRoughness(vRoughness);
			}
		} 
		materials.push_back(pMaterial); 
		materialMap[MaterialName] = materials.size() - 1;
	}
}

void Rt::Scene::BuildShape(BasicSceneBuilder* builder)
{
	SceneData* sceneData = &builder->sceneData;

	// triangle mesh
	std::vector<int>		cpuIndices;
	std::vector<Point3f>	cpuVertexs;
	std::vector<Point2f>	cpuUVs;
	std::vector<Normal3f> 	cpuNormals;
	std::vector<Vector3f>   cpuTangents;
	vector<unsigned int>	cpuMeshIndexCount;		// 网格顶点索引的数量
	vector<unsigned int>	cpuMeshFirstIndex;		// 网格顶点索引的第一个索引
 

	// 网格在全局内存中的偏移量和数量
	std::vector<size_t> vertexCount;
	std::vector<size_t> indicesCount;
	std::vector<size_t> uvCount;
	std::vector<size_t> normalCount;
	std::vector<size_t> tangentCount;

	std::vector<size_t> vertexOffset;
	std::vector<size_t> indicesOffset;
	std::vector<size_t> uvOffset;
	std::vector<size_t> normalOffset;
	std::vector<size_t> tangentOffset;

	void* dst = nullptr;
	void* src = nullptr;
	size_t count = 0;
	size_t total = 0;
	Point2f uv0 = Point2f(0, 0), uv1 = Point2f(1, 0), uv2 = Point2f(1, 1);



	auto start = std::chrono::high_resolution_clock::now();


	// 按文件定义顺序
	for (int i = 0; i < builder->shapeParamsIndexToName.size(); i++) 
	{ 
		std::vector<Point3f> vertexs; 
		std::vector<Normal3f> normals;
		std::vector<Point2f> uvs;
		//std::vector<Vector3f> tangents;

		std::vector<int> vi;

		
		auto& name = builder->shapeParamsIndexToName[i];
		auto& shapeParams = builder->shapeParams[name];
		auto& parameters = shapeParams.parameters;
		//cout << name << builder->shapeType[i] << endl;

		std::string meshName = name;
		size_t pos = meshName.find_last_of('_');
		if (pos != std::string::npos) {
			// meshes[0]-1_1 to meshes[0]-1
			meshName = meshName.substr(0, pos);
		}
		 
		auto& uvScaleDef = builder->objectUvScaleDefintions[meshName]; 
		if (0 == builder->shapeType[i]) 
		{// trianglemesh TriangleType 0 
			vertexs = parameters.GetPoint3fArray("P");
			normals = parameters.GetNormal3fArray("N");		// optional
			uvs = parameters.GetPoint2fArray("uv");			// optional
			//tangents = parameters.GetVector3fArray("S");	// optional 
			vi = parameters.GetIntArray("indices");  
			if (vi.size() % 3 != 0) {
				cout << "vi.size() % 3 != 0 " << name << endl;
				continue;
			}

			if ( (uvs.size() > 0 && vertexs.size() != uvs.size()) 
				|| (normals.size() > 0 && vertexs.size() != normals.size()) 
				//|| (tangents.size() > 0 && vertexs.size() != tangents.size())
			) {
				cout << "vi.size()  != uvs.size, normals.size(), tangents.size()" << name << endl;
				continue;
			}

			if (uvScaleDef) {
				auto uvScale = uvScaleDef->uvScale;
				auto uvOffset = uvScaleDef->uvOffset;
				auto uvRotation = uvScaleDef->uvRotation;
				auto a = uvRotation / 180.0 * Pi;

				Point2f DefaultCenter = Point2f(0.0, 0.0); //Point2f(0.5, 0.5);

				for (auto& uv : uvs) {
					uv *= uvScale;
					uv.x += uvOffset.x;
					uv.y += uvOffset.y;

					uv -= DefaultCenter; 
					uv = Point2f(uv.x * cos(a) - uv.y * sin(a), uv.x * sin(a) + uv.y * cos(a));
					uv += DefaultCenter;
				}
			}
			 
			Rt::Mesh* pMesh = new Rt::Mesh(name, vertexs, normals, uvs, vi);
			meshes.push_back(pMesh); 
			
			meshMap[name] = meshes.size() - 1;

			/*
				统计网格三角形数量
			*/
			MeshIndexToMeshTriangleCount[i] = vi.size() / 3;
		}  
		else if (1 == builder->shapeType[i])
		{
			std::string VBOFileName = parameters.GetOneString("VBOFileName", "");
			std::string EBOFileName = parameters.GetOneString("EBOFileName", "");
			int IndicesCount = parameters.GetOneInt("IndicesCount", 0);
			int EBOOffset = parameters.GetOneInt("EBOOffset", 0);

			std::vector<IndexType> vi1;
			if (!boost::filesystem::exists(EBOFileName))
			{
				std::cout << "EBO not exists" << std::endl;
				continue;
			}
			else
			{
				auto EboSize = boost::filesystem::file_size(EBOFileName);
				vi1.resize(EboSize / sizeof(IndexType));
				ReadEBOFromFile(EBOFileName, vi1);
				//std::cout << "Read EBO " << std::endl;
			}
			
			std::vector<Rt::Vertex> Vertices;
			if (!boost::filesystem::exists(VBOFileName))
			{
				std::cout << "VBO not exists" << std::endl;

				continue;
			}
			else
			{
				auto vbosize = boost::filesystem::file_size(VBOFileName);
				Vertices.resize(vbosize / sizeof(Rt::Vertex));
				ReadVBOFromFile(VBOFileName, Vertices);
			}


			Rt::Mesh* pMesh = new Rt::Mesh(name, Vertices, vi1);
			//Rt::Mesh* pMesh = new Rt::Mesh(name, vertexs, normals, uvs, vi);
			meshes.push_back(pMesh);

			meshMap[name] = meshes.size() - 1;

			/*
				统计网格三角形数量
			*/
			MeshIndexToMeshTriangleCount[i] = vi.size() / 3;
		}
	} 	



	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "load vbo ebo cost " << duration.count() / 1000 << " ms " << std::endl;

	cout << "nTriangleMeshes " << meshes.size() << endl;
  
	uint32_t nObjectDefintions = builder->objectDefintions.size(); 
	cout << "nObjectDefintions : " << nObjectDefintions << endl;
	for (int i = 0; i < nObjectDefintions; i++) {
		std::string name = builder->objectDefintionsIndexToName[i]; 
		auto object = builder->objectDefintions[name];
		std::string shapeName = object->shapeRefs[0];
		std::string MaterialName = object->materialsNames[0]; 
		/*
			设置网格材质名称
		*/
		meshes[meshMap[shapeName]]->MaterialName = MaterialName;
		Render::Transform transform = object->transforms[0]; 
		//printf("name: %s,shapeName: %s,MaterialName:%s\n", name.c_str(), shapeName.c_str(), MaterialName.c_str());
	} 


	// 实例数量
	uint32_t nInstances = builder->instances.size();
	cout << "nInstance : " << nInstances << endl;
	for (int i = 0; i < nInstances; i++) 
	{
		//std::cout << "instance index " << i << std::endl;
		std::string name = builder->instances[i].name;	
		//std::cout << "name " << name << std::endl;
		if (builder->objectDefintions.find(name) == builder->objectDefintions.end())
		{
			std::cout << "object not found " << std::endl;
			continue;
		}
		auto object = builder->objectDefintions[name];
		std::string shapeName = object->shapeRefs[0];
		auto objectIndex = builder->objectDefintionsNameToIndex[name];
		Render::Transform Matrix = builder->instances[i].renderFromInstance;		//printTransform(matrix);   	
		int MaterialIndex = sceneData->GetMaterialIndexFromMaterialName(object->materialsNames[0]);
		int MeshIndex = meshMap[shapeName];
		/*
			增加网格实例
		*/
		AddMeshInstance(MeshIndex, toGLMat4(Matrix), MaterialIndex);
		InstanceIndexToMeshIndex[i] = MeshIndex;
	}
}


void Rt::Scene::BuildSampler()
{

}

void Rt::Scene::BuildLight(BasicSceneBuilder* builder)
{
	auto n = builder->lights.size(); 
	for (int LightIndex = 0; LightIndex < n; LightIndex++) {
		auto& name = builder->lights[LightIndex].name;
		auto& params = builder->lights[LightIndex].parameters;
		auto& renderFromLight = builder->lights[LightIndex].renderFromLight; 
		if (name == "hdr") {
			power = params.GetOneFloat("power", 1);
			auto textureName = params.GetOneString("reflectance", "");

			std::string filename = images[textureMap[textureName]]->filename_; 

			printf("%s\n", filename.c_str());
			auto phi = params.GetOneFloat("phi", 0);
			auto theta = params.GetOneFloat("theta", 0);
			hdrFilename = filename;

			HDRRotateY = glm::mat4(1);
			HDRRotateY = glm::rotate(HDRRotateY, glm::radians(360.f - theta), glm::vec3(0, 1, 0));

			HDRRotateYInv = glm::mat4(1);
			HDRRotateYInv = glm::rotate(HDRRotateYInv, glm::radians(theta - 360.f), glm::vec3(0, 1, 0));
		}
		else if (name == "spot")
		{
			auto from = params.GetOnePoint3f("from", Point3f(0, 0, 0));
			auto to = params.GetOnePoint3f("to", Point3f(0, 0, 1));
			auto coneangle = params.GetOneFloat("coneangle", 30);
			auto conedeltaangle = params.GetOneFloat("conedeltaangle", 5);
			auto power = params.GetOneFloat("power", 1);
			auto range = params.GetOneFloat("range", 200);
			auto L = params.GetFloatArray("rgb");
			Light *pLight = new Light();
			pLight->CreateSpotLight(glm::vec3(L[0], L[1], L[2]), power, range, glm::vec3(from.x, from.y, from.z), glm::vec3(to.x, to.y, to.z), coneangle, coneangle - conedeltaangle);
			lights.push_back(pLight);
		}
		else if (name == "point")
		{
			auto from = params.GetOnePoint3f("from", Point3f(0, 0, 0));
			auto power = params.GetOneFloat("power", 1);
			auto range = params.GetOneFloat("range", 200);
			auto L = params.GetFloatArray("rgb");
			Light* pLight = new Light();
			pLight->CreatePointLight(glm::vec3(L[0], L[1], L[2]), power, range, glm::vec3(from.x, from.y, from.z));
			lights.push_back(pLight);
		}
		else if (name == "distant")
		{
			auto from = params.GetOnePoint3f("from", Point3f(0, 0, 0));
			auto to = params.GetOnePoint3f("to", Point3f(0, 0, 1));
			auto power = params.GetOneFloat("power", 1);
			auto L = params.GetFloatArray("rgb");
			Light* pLight = new Light();
			pLight->CreateDistantLight(glm::vec3(L[0], L[1], L[2]), power, glm::vec3(from.x, from.y, from.z), glm::vec3(to.x, to.y, to.z));
			lights.push_back(pLight);
		}
		else if (name == "area") {
			auto L = params.GetFloatArray("rgb");
			auto power = params.GetOneFloat("power", 1);
			auto textureName = params.GetOneString("reflectance", "");
			auto objectName = params.GetOneString("object", "");
			auto visible = params.GetOneBool("visible", true);
			Light* pLight = new Light();


			std::uint32_t MeshIndex = builder->shapeParamsNameToIndex[objectName];
			pLight->CreateAreaLight(glm::vec3(L[0], L[1], L[2]), power, builder->shapeParamsNameToIndex[objectName]);
			pLight->SetLightType(Light::Type::Area);
			pLight->SetLightVisible(visible);

			std::uint32_t AreaLightIndex = AreaLights.size();
			AreaLights.push_back(pLight);
			//std::cout << "area MeshIndex " << MeshIndex << " LightIndex " << AreaLights.size() - 1 << std::endl;

			if (MeshIndexToAreaLightIndex.find(MeshIndex) == MeshIndexToAreaLightIndex.end())
				MeshIndexToAreaLightIndex[MeshIndex] = AreaLightIndex;
	
		}
	}

	for (auto iter = InstanceIndexToMeshIndex.begin(); iter != InstanceIndexToMeshIndex.end(); iter++)
	{
		std::uint32_t InstanceIndex = iter->first;
		std::uint32_t MeshIndex		= iter->second;
		//std::cout << "InstanceIndex " << InstanceIndex << " MeshIndex " << MeshIndex <<  " ";
		// Mesh有关联光源, 才把实例映射到光源
		if (MeshIndexToAreaLightIndex.find(MeshIndex) != MeshIndexToAreaLightIndex.end())
		{
			std::uint32_t LightIndex = MeshIndexToAreaLightIndex[MeshIndex];
			std::uint32_t MeshTriangleCount = MeshIndexToMeshTriangleCount[MeshIndex];
			InstanceIndexToAreaLightIndexMap.push_back({ InstanceIndex, LightIndex, MeshTriangleCount });
		}
		else
		{
			//std::cout  << "no mesh index";	
		}
		
	}
}

void Rt::Scene::BuildHDR()
{

}
 