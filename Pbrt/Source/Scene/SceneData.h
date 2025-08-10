#pragma once
#include <vector>
#include <map>
#include <string>

namespace Render {

	class Scene;
	class DiffuseMaterialParameters;
	class DielectricMaterialParameters;
	class GlassMaterialParameters;
	class ConductorMaterialParameters;
	class GraniteMaterialParameters;
	class SceneData {
	public:
		// All Material
		int GetMaterialNameCount();
		void MapMaterialNameToMaterialIndex(std::string name, int index);
		void MapMaterialIndexToMaterialName(int index, std::string name);
		void MapMaterialIndexToMaterialType(int index, std::string name);
		void MapMaterialIndexToMaterialNormalMapTextureName(int index, std::string name);
		std::string GetMaterialNameFromMaterialIndex(int index);
		std::string GetMaterialTypeFromMaterialIndex(int index);
		std::string GetMaterialNormalMapTextureNameFromMaterialIndex(int index);
		std::string GetMaterialTypeFromMaterialName(std::string  name);
		int GetMaterialIndexFromMaterialName(std::string name);
		// All Texture
		int GetTextureNameCount();
		void MapTextureNameToTextureIndex(std::string name, int index);
		void MapTextureIndexToTextureName(int index, std::string name);
		std::string GetTextureNameFromTextureIndex(int index);
		int GetTextureIndexFromTextureName(std::string name);
		int GetTextureTypeFromTextureIndex(int index);
		// Offset
		void SetImageTextureOffset(int offset);
		void SetConstTextureOffset(int offset);
		void SetNoiseTextureOffset(int offset);
		void SetRandomTextureOffset(int offset);
		int GetImageTextureOffset();
		int GetConstTextureOffset();
		int GetNoiseTextureOffset();
		int GetRandomTextureOffset();
		// Image Texture
		int GetImageTextureCount();
		void MapImageTextureIndexToImageTextureName(int index, std::string name);
		void MapImageTextureNameToImageTextureIndex(std::string name, int index);
		std::string GetImageTextureNameFromImageTextureIndex(int index);
		int GetImageTextureIndexFromImageTextureName(std::string name);
		bool HasImageTextureName(std::string name);
		// Const Texture
		int GetConstTextureCount();
		void MapConstTextureIndexToConstTextureName(int index, std::string name);
		void MapConstTextureNameToConstTextureIndex(std::string name, int index);
		std::string GetConstTextureNameFromConstTextureIndex(int index);
		int GetConstTextureIndexFromConstTextureName(std::string name);
		bool HasConstTextureName(std::string name);
		// Noise Texture
		int GetNoiseTextureCount();
		void MapNoiseTextureIndexToNoiseTextureName(int index, std::string name);
		void MapNoiseTextureNameToNoiseTextureIndex(std::string name, int index);
		std::string GetNoiseTextureNameFromNoiseTextureIndex(int index);
		int GetNoiseTextureIndexFromNoiseTextureName(std::string name);
		bool HasNoiseTextureName(std::string name);
		// Random Texture
		int GetRandomTextureCount();
		void MapRandomTextureIndexToRandomTextureName(int index, std::string name);
		void MapRandomTextureNameToRandomTextureIndex(std::string name, int index);
		std::string GetRandomTextureNameFromRandomTextureIndex(int index);
		int GetRandomTextureIndexFromRandomTextureName(std::string name);
		bool HasRandomTextureName(std::string name);
		// all material <-> all texture
		void MapMaterialIndexToTextureName(int index, std::string name);
		std::string GetTextureNameFromMaterialIndex(int index);
		std::string GetTextureNameFromMaterialName(std::string name);
		int GetTextureIndexFromMaterialIndex(int index);
		int GetTextureIndexFromMaterialName(std::string name);
		// Texture Index <-> Image Texture File Name
		void MapTextureIndexToImageTextureFileName(int index, std::string name);
		std::string GetImageTextureFileNameFromTextureIndex(int index);
		//
		void AddImageTextureFileName(std::string name);

		void SetTextureColor(int index, float& r, float& g, float& b);

		// 材质参数
		void SetDiffuseMaterialParameters(int index, DiffuseMaterialParameters p);
		DiffuseMaterialParameters GetDiffuseMaterialParameters(int index);

		void SetDielectricMaterialParameters(int index, DielectricMaterialParameters p);
		DielectricMaterialParameters GetDielectricMaterialParameters(int index);

		void SetGlassMaterialParameters(int index, GlassMaterialParameters p);
		GlassMaterialParameters GetGlassMaterialParameters(int index);

		void SetGraniteMaterialParameters(int index, GraniteMaterialParameters p);
		GraniteMaterialParameters GetGraniteMaterialParameters(int index);

		void SetConductorMaterialParameters(int index, ConductorMaterialParameters p);
		ConductorMaterialParameters GetConductorMaterialParameters(int index);


	private:
		std::map<std::string, int> MaterialNameToMaterialIndex;							// 材质名称 ->  材质索引
		std::map<int, std::string> MaterialIndexToMaterialName;							// 材质索引 ->  材质名称  
		std::map<int, std::string> MaterialIndexToMaterialType;							// 材质索引 ->  材质名称  
		std::map<int, std::string> MaterialIndexToMaterialNormalMapTextureName;			// 材质索引 ->  法线贴图引用的纹理名称
		std::map<int, DiffuseMaterialParameters> DiffuseMaterialParameterMap;			// 材质索引 ->  Diffuse材质参数
		std::map<int, GlassMaterialParameters> GlassMaterialParameterMap;				// 材质索引 ->  玻璃材质参数
		std::map<int, GraniteMaterialParameters> GraniteMaterialParameterMap;			// 材质索引 ->  花岗岩材质参数
		std::map<int, DielectricMaterialParameters> DielectricMaterialParameterMap;		// 材质索引 ->  电解质材质参数
		std::map<int, ConductorMaterialParameters>  ConductorMaterialParameterMap;		// 材质索引 ->  导体材质参数
		

		std::map<std::string, int> TextureNameToTextureIndex;							// 纹理名称  -> 纹理索引
		std::map<int, std::string> TextureIndexToTextureName;							// 纹理索引  -> 纹理名称 

		std::map<int, std::string> ImageTextureIndexToImageTextureName;					// 图片纹理索引 -> 图片纹理名称
		std::map<std::string, int> ImageTextureNameToImageTextureIndex;					// 图片纹理名称 -> 图片纹理索引

		std::map<int, std::string> ConstTextureIndexToConstTextureName;					// 常量纹理索引 -> 常量纹理名称
		std::map<std::string, int> ConstTextureNameToConstTextureIndex;					// 常量纹理名称 -> 常量纹理索引

		std::map<int, std::string> NoiseTextureIndexToNoiseTextureName;					// 噪声纹理索引 -> 噪声纹理名称
		std::map<std::string, int> NoiseTextureNameToNoiseTextureIndex;					// 噪声纹理名称 -> 噪声纹理索引

		std::map<int, std::string> RandomTextureIndexToRandomTextureName;				// 随机纹理索引 -> 随机纹理名称
		std::map<std::string, int> RandomTextureNameToRandomTextureIndex;				// 随机纹理名称 -> 随机纹理索引		
		
		std::map<int, std::string> MaterialIndexToTextureName;							// 材质索引 ->  纹理名称

		std::vector<std::string>   ImageTextureFileNames;								// 图片名称

		size_t ImageOffset;
		size_t ConstOffset;
		size_t NoiseOffset;
		size_t RandomOffset;
		Scene* scene;
	};

	class DiffuseMaterialParameters {
	public:
		DiffuseMaterialParameters() = default;
		DiffuseMaterialParameters(float r, float g, float b, bool has = false);
		float GetR();
		float GetG();
		float GetB();
		bool  GetHasFile();
	private:
		float r;
		float g;
		float b;
		bool hasFile;
	};

	class  DielectricMaterialParameters {
	public:
		 DielectricMaterialParameters() = default;
		 DielectricMaterialParameters(float r, float g, float b, float eta,  bool has = false);
		float GetEta();
		float GetR();
		float GetG();
		float GetB();
		bool  GetHasFile();

	private:
		float eta;
		float r;
		float g;
		float b;
		bool hasFile;
	};

	class  ConductorMaterialParameters {
	public:
		ConductorMaterialParameters() = default;
		ConductorMaterialParameters(float uRoughness, float vRoughness, bool remapRoughness = true);
		float GetURoughness();
		float GetVRoughness();
		bool GetRemapRoughness();

	private:
		float uRoughness;
		float vRoughness;
		bool remapRoughness;
	};



	class GlassMaterialParameters {
	public:
		GlassMaterialParameters() = default;
		GlassMaterialParameters(float r, float g, float b, float R, float T, bool has = false);
		float GetReflectivity();
		float GetTransmissivity();
		float GetR();
		float GetG();
		float GetB();
		bool  GetHasFile();

	private:
		float Reflectivity;
		float Transmissivity;
		float r;
		float g;
		float b;
		bool hasFile;
	};

	class GraniteMaterialParameters {
	public:
		GraniteMaterialParameters() = default;
		GraniteMaterialParameters(float r, float g, float b, float R, float T, bool has = false);
		float GetReflectivity();
		float GetTransmissivity();
		float GetR();
		float GetG();
		float GetB();
		bool  GetHasFile();

	private:
		float Reflectivity;
		float Transmissivity;
		float r;
		float g;
		float b;
		bool hasFile;
	};


}