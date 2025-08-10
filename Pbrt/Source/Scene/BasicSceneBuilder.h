#pragma once
#include <Math/Transform.h>
#include <Util/Parse.h>
#include <Util/Paramdict.h>
#include <Scene/GraphicsState.h>
#include <Scene/ShapeSceneEntity.h>
#include <Scene/ObjectDefinitionSceneEntity.h>
#include <Scene/ObjectUVScaleDefinitionSceneEntity.h>
#include <Scene/InstanceSceneEntity.h>
#include <Scene/CameraSceneEntity.h>
#include <Scene/LightSceneEntity.h>
#include <Container/InlinedVector.h>
#include <Scene/SceneData.h>
#include <set>
namespace Render {

    class BasicSceneBuilder : public ParserTarget {
    public:
        // BasicSceneBuilder Public Methods
        BasicSceneBuilder();
        // 相机
        void LookAt(Float ex, Float ey, Float ez, Float lx, Float ly, Float lz, Float ux,
            Float uy, Float uz, FileLoc loc);
        void Camera(const std::string&, ParsedParameterVector params, FileLoc loc);
        // 对象
        void ObjectBegin(const std::string& name, FileLoc loc);
        void ObjectEnd(FileLoc loc);
         // uv缩放
        void UVScaleBegin(const std::string& name, FileLoc loc);
        void UVScaleEnd(FileLoc loc);
        // 实例化
        void ObjectInstance(const std::string& name, FileLoc loc);
        // 纹理
        void Texture(const std::string& name, const std::string& type,
            const std::string& texname, ParsedParameterVector params, FileLoc loc);
        // 材质
        void MakeNamedMaterial(const std::string& name, ParsedParameterVector params, FileLoc loc);
        void NamedMaterial(const std::string& name, FileLoc loc);
        // 矩阵
        void Transform(Float transform[16], FileLoc loc);
        void ConcatTransform(Float transform[16], FileLoc loc);
        void Translate(Float dx, Float dy, Float dz, FileLoc loc);
        void Scale(Float sx, Float sy, Float sz, FileLoc loc);
        void Rotate(Float angle, Float ax, Float ay, Float az, FileLoc loc);
        // 
        void AttributeBegin(FileLoc loc);
        void AttributeEnd(FileLoc loc);
        // 形状
        void Shape(const std::string& name, const std::string& type, ParsedParameterVector params, FileLoc loc);
        // 形状引用
        void ShapeRef(const std::string& name, ParsedParameterVector params, FileLoc loc);
        void UVScaleShapeRef(const std::string& name, ParsedParameterVector params, FileLoc loc);
        void UVOffsetShapeRef(const std::string& name, ParsedParameterVector params, FileLoc loc);
        void UVRotationShapeRef(const std::string& name, ParsedParameterVector params, FileLoc loc);

        // 文件结束
        void EndOfFiles();
        // 光源
        void LightSource(const std::string& name, ParsedParameterVector params, FileLoc loc);

        void Integrator(const std::string& name, ParsedParameterVector params, FileLoc loc);

        void Film(const std::string& name, ParsedParameterVector params, FileLoc loc);
    private:
        class Transform RenderFromObject() const {
            return Render::Transform((renderFromWorld * graphicsState.ctm).GetMatrix());
        }
    public:
        std::map<std::string, SceneEntity> namedMaterials;                          // 材质
        std::map<std::string, SceneEntity> spectrumImageTextures;                   // imagemap
        std::map<std::string, SceneEntity> spectrumConstTextures;                   // const
        std::map<std::string, SceneEntity> spectrumNoiseTextures;                   // noise
        std::map<std::string, SceneEntity> spectrumRandomImageTextures;             // random

        std::map<std::string, SceneEntity> shapeParams;                             // 形状参数
        std::map<int, std::string> shapeParamsIndexToName;
        std::map<std::string, int> shapeParamsNameToIndex;
        std::map<int, int>         shapeType;

        std::map<std::string, ObjectDefinitionSceneEntity* >    objectDefintions;   // 对象定义, 名称都唯一。
        std::map<int, std::string> objectDefintionsIndexToName;
        std::map<std::string, int> objectDefintionsNameToIndex;

        ObjectDefinitionSceneEntity* activeInstanceDefinition = nullptr;            // 当前对象定义

        std::map<std::string, ObjectUVScaleDefinitionSceneEntity* >    objectUvScaleDefintions;   // 对象定义, 名称都唯一。
        std::map<int, std::string> objectUvScaleDefintionsIndexToName;
        std::map<std::string, int> objectUvScaleDefintionsNameToIndex;
        ObjectUVScaleDefinitionSceneEntity* activeUvScaleDefinition = nullptr;            // 当前uv縮放定义

        std::vector<InstanceSceneEntity> instances;                                 // 对象的实例
        std::vector<LightSceneEntity>         lights;                               // 光源

        SceneEntity integrator;                                                     // 积分器
        SceneEntity film;                                                           // film
        CameraSceneEntity camera;

        class Transform renderFromWorld;                                            // world转render矩阵
        class Transform cameraFromWorld;                                            // world转camera矩阵
        class Transform worldFromCamera;    
        Point3f eye;                                                                // 相机位置
        Point3f lookat;                                                             // 相机Lookat
        Vector3f up;                                                                // 相机上方向
        GraphicsState graphicsState;                                                // 当前状态
        std::vector<GraphicsState> pushedGraphicsStates;                            // 状态栈

        SceneData sceneData;
    };
}