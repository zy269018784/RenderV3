#pragma once
#include <string>
#include <Scene/SceneEntity.h>
#include <Math/Transform.h>
namespace Render {
    struct InstanceSceneEntity {
        InstanceSceneEntity() = default;

        InstanceSceneEntity(const std::string& name, FileLoc loc,
            Transform renderFromInstance)
            : name(name),
            loc(loc),
            renderFromInstance(renderFromInstance) {}


        std::string name;
        FileLoc loc;
        Transform renderFromInstance;
    };

}