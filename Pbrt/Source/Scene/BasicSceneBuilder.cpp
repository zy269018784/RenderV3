#include <Scene/BasicSceneBuilder.h>
#include <Scene/ShapeSceneEntity.h>
#include <Util/String.h>
#include <Container/Span.h>
#include <Math/SquareMatrix.h>
#include <iostream>
using namespace std;
namespace Render {
	BasicSceneBuilder::BasicSceneBuilder() {

	}

	void BasicSceneBuilder::LookAt(Float ex, Float ey, Float ez, Float lx, Float ly, Float lz, Float ux,
		Float uy, Float uz, FileLoc loc) {
		eye = Point3f(ex, ey, ez);
		lookat = Point3f(lx, ly, lz);
		up = Vector3f(ux, uy, uz);

		cameraFromWorld = graphicsState.ctm * Render::LookAt(eye, lookat, up);
		worldFromCamera = Inverse(cameraFromWorld);

		//graphicsState.ctm *= cameraFromWorld;
		//graphicsState.ctm *= cameraFromWorld;
		//graphicsState.ctm *= Render::LookAt(Point3f(ex, ey, ez), Point3f(lx, ly, lz), Vector3f(ux, uy, uz));
	}

	void BasicSceneBuilder::Camera(const std::string& name, ParsedParameterVector params, FileLoc loc) {
		ParameterDictionary dict(std::move(params));
		Float fov = dict.GetOneFloat("fov", 90);
		printf("Camera %s , fov %f\n", name.c_str(), fov);
		camera = CameraSceneEntity(name, std::move(dict), loc, worldFromCamera);
	}

	void BasicSceneBuilder::ObjectBegin(const std::string& origName, FileLoc loc) {

		// 保存当前状态
		pushedGraphicsStates.push_back(graphicsState);
		
		// ObjectBegin不允许嵌套
		if (activeInstanceDefinition != nullptr) {
			printf("ObjectBegin called inside of instance definition, line:column %d:%d", loc.line, loc.column);
			return;
		}

		// 检测对象名是否存在
		std::string name = NormalizeUTF8(origName);
		//printf("object name %s\n", name.c_str());
		if (objectDefintions.find(name) != objectDefintions.end()) {
			printf("\t%s: trying to redefine an object instance.\n", name.c_str());
			return;
		}
		
		// 创建对象定义
		activeInstanceDefinition = new ObjectDefinitionSceneEntity(name, loc);

		// 插入对象
		objectDefintionsNameToIndex[name] = objectDefintions.size();
		objectDefintionsIndexToName[objectDefintions.size()] = name;
		objectDefintions[name] = activeInstanceDefinition;	
	}

	void BasicSceneBuilder::ObjectEnd(FileLoc loc) {
		// 未创建对象
		if (!activeInstanceDefinition) {
			printf("ObjectEnd called outside of instance definition,line:column %d:%d\n", loc.line, loc.column);
			return;
		}

		// 还原状态
		graphicsState = std::move(pushedGraphicsStates.back());
		pushedGraphicsStates.pop_back();

		// 对象定义结束
		activeInstanceDefinition = nullptr;
	}

	void BasicSceneBuilder::UVScaleBegin(const std::string& origName, FileLoc loc) {

		// 保存当前状态
		pushedGraphicsStates.push_back(graphicsState);
		
		// ObjectBegin不允许嵌套
		if (activeUvScaleDefinition != nullptr) {
			printf("UVScaleBegin called inside of instance definition, line:column %d:%d", loc.line, loc.column);
			return;
		}

		// 检测对象名是否存在
		std::string name = NormalizeUTF8(origName);
		//printf("object name %s\n", name.c_str());
		if (objectUvScaleDefintions.find(name) != objectUvScaleDefintions.end()) {
			printf("\t%s: trying to redefine an UVScaleBegin instance.\n", name.c_str());
			return;
		}
		
		// 创建对象定义
		activeUvScaleDefinition = new ObjectUVScaleDefinitionSceneEntity(name, loc);

		// 插入对象
		objectUvScaleDefintionsNameToIndex[name] = objectUvScaleDefintions.size();
		objectUvScaleDefintionsIndexToName[objectUvScaleDefintions.size()] = name;
		objectUvScaleDefintions[name] = activeUvScaleDefinition;
	}

	void BasicSceneBuilder::UVScaleEnd(FileLoc loc) {
		// 未创建对象
		if (!activeUvScaleDefinition) {
			printf("UVScaleEnd called outside of instance definition,line:column %d:%d\n", loc.line, loc.column);
			return;
		}

		// 还原状态
		graphicsState = std::move(pushedGraphicsStates.back());
		pushedGraphicsStates.pop_back();

		// 对象定义结束
		activeUvScaleDefinition = nullptr;
	}


	void BasicSceneBuilder::ObjectInstance(const std::string& origName, FileLoc loc) {
	
		std::string name = NormalizeUTF8(origName);

		if (activeInstanceDefinition != nullptr) {
			printf("ObjectInstance can't be called inside instance definition,line:column %d:%d\n", loc.line, loc.column);
			return;
		}

		Render::Transform worldFromInstance = RenderFromObject();
		InstanceSceneEntity instance(name, loc, worldFromInstance);
		instances.push_back(instance);
	}

	void BasicSceneBuilder::Texture(const std::string& origName, const std::string& type,
		const std::string& texname, ParsedParameterVector params, FileLoc loc) {

		if (type != "float" && type != "spectrum") {
			printf("%s: texture type unknown. Must be \"float\" or \"spectrum\".", type.c_str());
			return;
		}
		
		std::string name = NormalizeUTF8(origName);

		if (spectrumImageTextures.find(name) != spectrumImageTextures.end()) {
			printf("%s: named texture redefined.", name.c_str());
			return;
		}

		if (spectrumConstTextures.find(name) != spectrumConstTextures.end()) {
			printf("%s: named texture redefined.", name.c_str());
			return;
		}

		if (spectrumNoiseTextures.find(name) != spectrumNoiseTextures.end()) {
			printf("%s: named texture redefined.", name.c_str());
			return;
		}

		if (spectrumRandomImageTextures.find(name) != spectrumRandomImageTextures.end()) {
			printf("%s: named texture redefined.", name.c_str());
			return;
		}

		ParameterDictionary dict(std::move(params));
		if ("spectrum" == type) {
			if ("imagemap" == texname) {		
				int index = sceneData.GetImageTextureCount();
				sceneData.MapImageTextureIndexToImageTextureName(index, name);
				sceneData.MapImageTextureNameToImageTextureIndex(name, index);
				spectrumImageTextures[name] = SceneEntity(texname, std::move(dict), loc);
				//cout << "imagemap name " << name << endl;
			}
			else if ("const" == texname) {
				int index = sceneData.GetConstTextureCount();
				sceneData.MapConstTextureIndexToConstTextureName(index, name);
				sceneData.MapConstTextureNameToConstTextureIndex(name, index);
				spectrumConstTextures[name] = SceneEntity(texname, std::move(dict), loc);
			}
			else if ("noise" == texname) {
				int index = sceneData.GetNoiseTextureCount();
				sceneData.MapNoiseTextureIndexToNoiseTextureName(index, name);
				sceneData.MapNoiseTextureNameToNoiseTextureIndex(name, index);
				spectrumNoiseTextures[name] = SceneEntity(texname, std::move(dict), loc);
			}
			else if ("randomimage" == texname) {
				int index = sceneData.GetRandomTextureCount();
				sceneData.MapRandomTextureIndexToRandomTextureName(index, name);
				sceneData.MapRandomTextureNameToRandomTextureIndex(name, index);
				spectrumRandomImageTextures[name] = SceneEntity(texname, std::move(dict), loc);
			}
			else {
				printf("unsupported texname %s\n", texname.c_str());
			}
		}
	}

	void BasicSceneBuilder::MakeNamedMaterial(const std::string& origName, ParsedParameterVector params, FileLoc loc) {

		std::string name = NormalizeUTF8(origName);
		if (namedMaterials.find(name) != namedMaterials.end()) {
			printf("%s: named material redefined.", name.c_str());
			return;
		}
		//materialNames.insert(name);
		
		ParameterDictionary dict(std::move(params));

		int Index = namedMaterials.size();
		std::string MaterialName = name;
		sceneData.MapMaterialIndexToMaterialName(Index, MaterialName);
		sceneData.MapMaterialNameToMaterialIndex(MaterialName, Index);

		namedMaterials[MaterialName] = SceneEntity(MaterialName, std::move(dict), loc);
	}

	void BasicSceneBuilder::NamedMaterial(const std::string& origName, FileLoc loc) {
		std::string name = NormalizeUTF8(origName);
		auto iter = namedMaterials.find(name);
		if (iter != namedMaterials.end()) {
			graphicsState.currentMaterialName = name;
		}
	}

	void BasicSceneBuilder::Transform(Float tr[16], FileLoc loc) {
		graphicsState.ctm = Transpose(Render::Transform(SquareMatrix<4>(MakeSpan(tr, 16))));
		//graphicsState.ctm = Render::Transform(SquareMatrix<4>(MakeSpan(tr, 16)));
	}

	void BasicSceneBuilder::ConcatTransform(Float tr[16], FileLoc loc) {
    	graphicsState.ctm *= Transpose(Render::Transform(SquareMatrix<4>(MakeSpan(tr, 16))));
	}


	void BasicSceneBuilder::Translate(Float dx, Float dy, Float dz, FileLoc loc) {
		graphicsState.ctm *= Render::Translate(Vector3f(dx, dy, dz));
	}

	void BasicSceneBuilder::Scale(Float sx, Float sy, Float sz, FileLoc loc) {
		graphicsState.ctm *= Render::Scale(sx, sy, sz);
	}

	void BasicSceneBuilder::Rotate(Float angle, Float dx, Float dy, Float dz, FileLoc loc) {
		graphicsState.ctm *= Render::Rotate(angle, Vector3f(dx, dy, dz));
	}

	void BasicSceneBuilder::AttributeBegin(FileLoc loc) {
		pushedGraphicsStates.push_back(graphicsState);
	}

	void BasicSceneBuilder::AttributeEnd(FileLoc loc) {
		graphicsState = std::move(pushedGraphicsStates.back());
		pushedGraphicsStates.pop_back();
	}

	void BasicSceneBuilder::Shape(const std::string& origName, const std::string& type, ParsedParameterVector params, FileLoc loc) {
		std::string name = NormalizeUTF8(origName);

		SceneEntity entity(name, std::move(params), loc);
		auto index = shapeParams.size();
		shapeParamsIndexToName[index] = name;
		shapeParamsNameToIndex[name] = index;
		shapeParams[name] = std::move(entity);
		if (type == "trianglemesh")
			shapeType[index] = 0;
		//if (type == "sphere")
		//	shapeType[index] = 2;
		else if (type == "VBO") 
			shapeType[index] = 1;		
	}

	void BasicSceneBuilder::ShapeRef(const std::string& origName, ParsedParameterVector params, FileLoc loc) {
		std::string name = NormalizeUTF8(origName);
		//if (shapeParams.find(name) == shapeParams.end()) {
		//	printf("[%d-%d] %s: named ShapeRef undefined.\n", loc.line, loc.column, name.c_str());
		//	return;
		//}

		graphicsState.currentShapeName = name;

		if (activeInstanceDefinition) {
			activeInstanceDefinition->shapeRefs.push_back(graphicsState.currentShapeName);
			activeInstanceDefinition->materialsNames.push_back(graphicsState.currentMaterialName);
			activeInstanceDefinition->transforms.push_back(renderFromWorld * graphicsState.ctm);
			//printf("currentShapeName %s\n", graphicsState.currentShapeName.c_str());
			//showTransform(renderFromWorld * graphicsState.ctm);
		}
	}

	void BasicSceneBuilder::UVScaleShapeRef(const std::string& origName, ParsedParameterVector params, FileLoc loc) {
		std::string name = NormalizeUTF8(origName);
		graphicsState.currentShapeName = name;
		if (activeUvScaleDefinition) {
			activeUvScaleDefinition->shapeRefs.push_back(graphicsState.currentShapeName);
			ParameterDictionary dict(std::move(params));
			activeUvScaleDefinition->uvScale = dict.GetOnePoint2f("UVScale", Point2f(1, 1));
		}
	}

	void BasicSceneBuilder::UVOffsetShapeRef(const std::string& origName, ParsedParameterVector params, FileLoc loc)
	{
		std::string name = NormalizeUTF8(origName);
		graphicsState.currentShapeName = name;
		if (activeUvScaleDefinition) {
			activeUvScaleDefinition->shapeRefs.push_back(graphicsState.currentShapeName);
			ParameterDictionary dict(std::move(params));
			activeUvScaleDefinition->uvOffset = dict.GetOnePoint2f("UVOffset", Point2f(0, 0)); //uv偏移
		}
	}

	void BasicSceneBuilder::UVRotationShapeRef(const std::string& origName, ParsedParameterVector params, FileLoc loc)
	{
		std::string name = NormalizeUTF8(origName);
		graphicsState.currentShapeName = name;
		if (activeUvScaleDefinition) {
			activeUvScaleDefinition->shapeRefs.push_back(graphicsState.currentShapeName);
			ParameterDictionary dict(std::move(params));
			activeUvScaleDefinition->uvRotation = dict.GetOneFloat("UVRotation", 0.0); //旋转角度
		}
	}

	void BasicSceneBuilder::EndOfFiles() {
	
	}

	void BasicSceneBuilder::LightSource(const std::string& name, ParsedParameterVector params, FileLoc loc) {
		ParameterDictionary dict(std::move(params));
		lights.push_back(LightSceneEntity(name, std::move(dict), loc, graphicsState.ctm));
	}

	void BasicSceneBuilder::Integrator(const std::string& name, ParsedParameterVector params, FileLoc loc) {
		ParameterDictionary dict(std::move(params));
		integrator = SceneEntity(name, std::move(dict), loc);
	}

	void BasicSceneBuilder::Film(const std::string& name, ParsedParameterVector params, FileLoc loc) {
		ParameterDictionary dict(std::move(params));
		film = SceneEntity(name, std::move(dict), loc);
	}
}