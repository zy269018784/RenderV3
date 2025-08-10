#include <Scene/SceneData.h>
#include <Scene/Scene.h>
namespace Render {
	// All Material
	int SceneData::GetMaterialNameCount() {
		return MaterialIndexToMaterialName.size();
	}

	void SceneData::MapMaterialNameToMaterialIndex(std::string name, int index) {
		MaterialNameToMaterialIndex[name] = index;
	}

	void SceneData::MapMaterialIndexToMaterialName(int index, std::string name) {
		MaterialIndexToMaterialName[index] = name;
	}

	void SceneData::MapMaterialIndexToMaterialType(int index, std::string name) {
		MaterialIndexToMaterialType[index] = name;
	}

	void SceneData::MapMaterialIndexToMaterialNormalMapTextureName(int index, std::string name) {
		MaterialIndexToMaterialNormalMapTextureName[index] = name;
	}

	int SceneData::GetMaterialIndexFromMaterialName(std::string name) {
		return MaterialNameToMaterialIndex[name];
	}

	std::string SceneData::GetMaterialNameFromMaterialIndex(int index) {
		return MaterialIndexToMaterialName[index];
	}

	std::string SceneData::GetMaterialTypeFromMaterialIndex(int index) {
		return MaterialIndexToMaterialType[index];
	}

	std::string SceneData::GetMaterialNormalMapTextureNameFromMaterialIndex(int index) {
		return MaterialIndexToMaterialNormalMapTextureName[index];
	}

	std::string  SceneData::GetMaterialTypeFromMaterialName(std::string  name) {
		int index = GetMaterialIndexFromMaterialName(name);
		return GetMaterialTypeFromMaterialIndex(index);
	}

	// All Texture
	int SceneData::GetTextureNameCount() {
		return TextureIndexToTextureName.size();
	}

	void SceneData::MapTextureNameToTextureIndex(std::string name, int index) {
		TextureNameToTextureIndex[name] = index;
	}

	void SceneData::MapTextureIndexToTextureName(int index, std::string name) {
		TextureIndexToTextureName[index] = name;
	}

	std::string SceneData::GetTextureNameFromTextureIndex(int index) {
		return TextureIndexToTextureName[index];
	}

	int SceneData::GetTextureIndexFromTextureName(std::string name) {
		return TextureNameToTextureIndex[name];
	}

	int SceneData::GetTextureTypeFromTextureIndex(int index) {
		if (0 <= index && index < ConstOffset) {
			return 0;
		}
		else if (ConstOffset <= index && index < NoiseOffset) {
			return 1;
		}
		else if (NoiseOffset <= index && index < RandomOffset) {
			return 2;
		}
		else if (RandomOffset <= index) {
			return 3;
		}

		return -1;
	}

	// Offset
	void  SceneData::SetImageTextureOffset(int offset) {
		ImageOffset = offset;
	}

	void  SceneData::SetConstTextureOffset(int offset) {
		ConstOffset = offset;
	}

	void  SceneData::SetNoiseTextureOffset(int offset) {
		NoiseOffset = offset;
	}

	void  SceneData::SetRandomTextureOffset(int offset) {
		RandomOffset = offset;
	}

	int SceneData::GetImageTextureOffset() {
		return ImageOffset;
	}

	int SceneData::GetConstTextureOffset() {
		return ConstOffset;
	}

	int SceneData::GetNoiseTextureOffset() {
		return NoiseOffset;
	}

	int SceneData::GetRandomTextureOffset() {
		return RandomOffset;
	}


	//  Image Texture
	int  SceneData::GetImageTextureCount() {
		return ImageTextureIndexToImageTextureName.size();
	}

	void  SceneData::MapImageTextureIndexToImageTextureName(int index, std::string name) {
		ImageTextureIndexToImageTextureName[index] = name;
	}

	void  SceneData::MapImageTextureNameToImageTextureIndex(std::string name, int index) {
		ImageTextureNameToImageTextureIndex[name] = index;
	}

	std::string SceneData::GetImageTextureNameFromImageTextureIndex(int index) {
		return ImageTextureIndexToImageTextureName[index];
	}

	int SceneData::GetImageTextureIndexFromImageTextureName(std::string name) {
		return ImageTextureNameToImageTextureIndex[name];
	}

	bool SceneData::HasImageTextureName(std::string name) {
		if (ImageTextureNameToImageTextureIndex.find(name) != ImageTextureNameToImageTextureIndex.end())
			return true;
		return false;
	}

	// Const Texture
	int  SceneData::GetConstTextureCount() {
		return ConstTextureIndexToConstTextureName.size();
	}

	void  SceneData::MapConstTextureIndexToConstTextureName(int index, std::string name) {
		ConstTextureIndexToConstTextureName[index] = name;
	}

	void  SceneData::MapConstTextureNameToConstTextureIndex(std::string name, int index) {
		ConstTextureNameToConstTextureIndex[name] = index;
	}

	std::string SceneData::GetConstTextureNameFromConstTextureIndex(int index) {
		return ConstTextureIndexToConstTextureName[index];
	}

	int SceneData::GetConstTextureIndexFromConstTextureName(std::string name) {
		return ConstTextureNameToConstTextureIndex[name];
	}

	bool SceneData::HasConstTextureName(std::string name) {
		if (ConstTextureNameToConstTextureIndex.find(name) != ConstTextureNameToConstTextureIndex.end())
			return true;
		return false;
	}

	// Noise Texture
	int  SceneData::GetNoiseTextureCount() {
		return NoiseTextureIndexToNoiseTextureName.size();
	}

	void  SceneData::MapNoiseTextureIndexToNoiseTextureName(int index, std::string name) {
		NoiseTextureIndexToNoiseTextureName[index] = name;
	}

	void  SceneData::MapNoiseTextureNameToNoiseTextureIndex(std::string name, int index) {
		NoiseTextureNameToNoiseTextureIndex[name] = index;
	}

	std::string SceneData::GetNoiseTextureNameFromNoiseTextureIndex(int index) {
		return NoiseTextureIndexToNoiseTextureName[index];
	}

	int SceneData::GetNoiseTextureIndexFromNoiseTextureName(std::string name) {
		return NoiseTextureNameToNoiseTextureIndex[name];
	}

	bool SceneData::HasNoiseTextureName(std::string name) {
		if (NoiseTextureNameToNoiseTextureIndex.find(name) != NoiseTextureNameToNoiseTextureIndex.end())
			return true;
		return false;
	}

	// Random Texture
	int  SceneData::GetRandomTextureCount() {
		return RandomTextureIndexToRandomTextureName.size();
	}

	void  SceneData::MapRandomTextureIndexToRandomTextureName(int index, std::string name) {
		RandomTextureIndexToRandomTextureName[index] = name;
	}

	void  SceneData::MapRandomTextureNameToRandomTextureIndex(std::string name, int index) {
		RandomTextureNameToRandomTextureIndex[name] = index;
	}

	std::string SceneData::GetRandomTextureNameFromRandomTextureIndex(int index) {
		return RandomTextureIndexToRandomTextureName[index];
	}

	int SceneData::GetRandomTextureIndexFromRandomTextureName(std::string name) {
		return RandomTextureNameToRandomTextureIndex[name];
	}

	bool SceneData::HasRandomTextureName(std::string name) {
		if (RandomTextureNameToRandomTextureIndex.find(name) != RandomTextureNameToRandomTextureIndex.end())
			return true;
		return false;
	}


	// all material <-> all texture
	void SceneData::MapMaterialIndexToTextureName(int index, std::string name) {
		MaterialIndexToTextureName[index] = name;
	}

	std::string SceneData::GetTextureNameFromMaterialIndex(int index) {
		return MaterialIndexToTextureName[index];
	}

	std::string SceneData::GetTextureNameFromMaterialName(std::string name) {
		int MaterialIndex = MaterialNameToMaterialIndex[name];
		return GetTextureNameFromMaterialIndex(MaterialIndex);
	}

	int SceneData::GetTextureIndexFromMaterialIndex(int index) {
		std::string TextureName = GetTextureNameFromMaterialIndex(index);
		return GetTextureIndexFromTextureName(TextureName);
	}
	int SceneData::GetTextureIndexFromMaterialName(std::string name) {
		int MaterialIndex = GetMaterialIndexFromMaterialName(name);
		return GetTextureIndexFromMaterialIndex(MaterialIndex);
	}

	// Texture Index <-> Image Texture File Name
	void SceneData::MapTextureIndexToImageTextureFileName(int index, std::string name) {
		if (index < ImageTextureFileNames.size())
			ImageTextureFileNames[index] = name;
	}

	std::string  SceneData::GetImageTextureFileNameFromTextureIndex(int index) {
		std::string filename;
		if (ImageOffset <= index && index < ConstOffset) {
			filename = ImageTextureFileNames[index];
		}
		return filename;
	}
	//
	//void SceneData::SetTextures(SpectrumTexture* t) {
	//	Textures = t;
	//}

	void  SceneData::AddImageTextureFileName(std::string name) {
		ImageTextureFileNames.push_back(name);
	}

	void SceneData::SetTextureColor(int index, float& r, float& g, float& b) {
		if (scene->GetConstTextureOffset() <= index && index < scene->GetNoiseTextureOffset()) {
			index -= scene->GetConstTextureOffset();
			scene->SetConstTexture(index, r, g, b);
		}
	}

	void SceneData::SetDiffuseMaterialParameters(int index, DiffuseMaterialParameters p) {
		DiffuseMaterialParameterMap[index] = p;
	}

	DiffuseMaterialParameters SceneData::GetDiffuseMaterialParameters(int index) {
		return DiffuseMaterialParameterMap[index];
	}

	void SceneData::SetDielectricMaterialParameters(int index, DielectricMaterialParameters p) {
		DielectricMaterialParameterMap[index] = p;
	}

	DielectricMaterialParameters SceneData::GetDielectricMaterialParameters(int index) {
		return DielectricMaterialParameterMap[index];
	}

	void SceneData::SetGlassMaterialParameters(int index, GlassMaterialParameters p) {
		GlassMaterialParameterMap[index] = p;
	}

	GlassMaterialParameters SceneData::GetGlassMaterialParameters(int index) {
		return GlassMaterialParameterMap[index];
	}

	void SceneData::SetGraniteMaterialParameters(int index, GraniteMaterialParameters p) {
		GraniteMaterialParameterMap[index] = p;
	}


	GraniteMaterialParameters SceneData::GetGraniteMaterialParameters(int index) {
		return GraniteMaterialParameterMap[index];
	}

	void SceneData::SetConductorMaterialParameters(int index, ConductorMaterialParameters p) {
		ConductorMaterialParameterMap[index] = p;
	}

	ConductorMaterialParameters SceneData::GetConductorMaterialParameters(int index) {
		return ConductorMaterialParameterMap[index];
	}

	DiffuseMaterialParameters::DiffuseMaterialParameters(float r, float g, float b, bool has)
		: r(r), g(g), b(b), hasFile(has)
	{

	}
	float DiffuseMaterialParameters::GetR() {
		return r;
	}

	float DiffuseMaterialParameters::GetG() {
		return g;
	}

	float DiffuseMaterialParameters::GetB() {
		return b;
	}

	bool  DiffuseMaterialParameters::GetHasFile() {
		return hasFile;
	}


	DielectricMaterialParameters::DielectricMaterialParameters(float r, float g, float b, float eta, bool has)
		: r(r), g(g), b(b), eta(eta), hasFile(has)
	{

	}

	float DielectricMaterialParameters::GetEta() {
		return eta;
	}

	float DielectricMaterialParameters::GetR() {
		return r;
	}

	float DielectricMaterialParameters::GetG() {
		return g;
	}

	float DielectricMaterialParameters::GetB() {
		return b;
	}

	bool  DielectricMaterialParameters::GetHasFile() {
		return hasFile;
	}

	ConductorMaterialParameters::ConductorMaterialParameters(float uRoughness, float vRoughness, bool remapRoughness) 
	: uRoughness(uRoughness), vRoughness(vRoughness), remapRoughness(remapRoughness) {
	
	}

	float ConductorMaterialParameters::GetURoughness() {
		return uRoughness;
	}

	float ConductorMaterialParameters::GetVRoughness() {
		return vRoughness;
	}

	bool ConductorMaterialParameters::GetRemapRoughness() {
		return remapRoughness;
	}

	GlassMaterialParameters::GlassMaterialParameters(float r, float g, float b, float R, float T, bool has)
		: r(r), g(g), b(b), Reflectivity(R), Transmissivity(T), hasFile(has)
	
	{
	
	}

	float GlassMaterialParameters::GetReflectivity() {
		return Reflectivity;
	}

	float GlassMaterialParameters::GetTransmissivity() {
		return Transmissivity;
	}

	float GlassMaterialParameters::GetR() {
		return r;
	}

	float GlassMaterialParameters::GetG() {
		return g;
	}

	float GlassMaterialParameters::GetB() {
		return b;
	}

	bool  GlassMaterialParameters::GetHasFile() {
		return hasFile;
	}


	GraniteMaterialParameters::GraniteMaterialParameters(float r, float g, float b, float R, float T, bool has)
		: r(r), g(g), b(b), Reflectivity(R), Transmissivity(T), hasFile(has)

	{

	}

	float GraniteMaterialParameters::GetReflectivity() {
		return Reflectivity;
	}

	float GraniteMaterialParameters::GetTransmissivity() {
		return Transmissivity;
	}

	float GraniteMaterialParameters::GetR() {
		return r;
	}

	float GraniteMaterialParameters::GetG() {
		return g;
	}

	float GraniteMaterialParameters::GetB() {
		return b;
	}

	bool  GraniteMaterialParameters::GetHasFile() {
		return hasFile;
	}
}