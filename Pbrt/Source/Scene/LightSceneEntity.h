#pragma once
#include <Scene/SceneEntity.h>
#include <Math/Transform.h>
namespace Render {
	struct LightSceneEntity : SceneEntity {
        LightSceneEntity() = default;
        LightSceneEntity(const std::string& name, ParameterDictionary parameters, FileLoc loc, const Transform& renderFromLight) 
            : SceneEntity(name, parameters, loc), renderFromLight(renderFromLight) {}
        Transform renderFromLight;
	};
}