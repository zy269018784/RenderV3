#include <Scene/SceneEntity.h>
#include <Math/Transform.h>
namespace Render {
    // CameraSceneEntity Definition
    struct CameraSceneEntity : public SceneEntity {
        // CameraSceneEntity Public Methods
        CameraSceneEntity() = default;
        CameraSceneEntity(const std::string& name, ParameterDictionary parameters,
            FileLoc loc, const Transform& cameraTransform)
            : SceneEntity(name, parameters, loc),
            cameraTransform(cameraTransform){}


        Transform cameraTransform;
    };
}