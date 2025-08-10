#include <Camera/CameraTransform.h>

namespace Render {
    enum class RenderingCoordinateSystem { Camera, CameraWorld, World };


    CameraTransform::CameraTransform(const AnimatedTransform& worldFromCamera) {
        //printf("CameraTransform::CameraTransform\n");
        //showTransform((Transform&)worldFromCamera.startTransform);
        //showTransform((Transform&)worldFromCamera.endTransform);
        //printf("---------------------\n");

        switch (RenderingCoordinateSystem::CameraWorld) {
        case RenderingCoordinateSystem::Camera: {
            // Compute _worldFromRender_ for camera-space rendering
            Float tMid = (worldFromCamera.startTime + worldFromCamera.endTime) / 2;
            worldFromRender = worldFromCamera.Interpolate(tMid);
            cout << "RenderingCoordinateSystem::Camera\n";
            showTransform(worldFromRender);
            break;
        }
        case RenderingCoordinateSystem::CameraWorld: {
            // Compute _worldFromRender_ for camera-world space rendering
            Float tMid = (worldFromCamera.startTime + worldFromCamera.endTime) / 2;
            Point3f pCamera = worldFromCamera(Point3f(0, 0, 0), tMid);
            Point3f pCamera2 = worldFromCamera.startTransform(Point3f(0, 0, 0));
            Point3f pCamera3 = worldFromCamera.endTransform(Point3f(0, 0, 0));
            showTransform((Transform &)worldFromCamera.startTransform);
           //cout << "tMid " << tMid << endl;
           //cout << "pCamera "  << pCamera.x << " " << pCamera.y << " " << pCamera.z << endl;
           //cout << "pCamera2 " << pCamera2.x << " " << pCamera2.y << " " << pCamera2.z << endl;
           //cout << "pCamera3 " << pCamera3.x << " " << pCamera3.y << " " << pCamera3.z << endl;
            cout << "RenderingCoordinateSystem::CameraWorld\n";
            worldFromRender = Translate(Vector3f(pCamera));
            showTransform(worldFromRender);
            break;
        }
        case RenderingCoordinateSystem::World: {
            // Compute _worldFromRender_ for world-space rendering
            worldFromRender = Transform();
            break;
        }
        //default:
        //    LOG_FATAL("Unhandled rendering coordinate space");
        }
        

        Transform renderFromWorld = Inverse(worldFromRender);
        Transform rfc[2] = { renderFromWorld * worldFromCamera.startTransform,
                            renderFromWorld * worldFromCamera.endTransform };
        renderFromCamera = AnimatedTransform(rfc[0], worldFromCamera.startTime, rfc[1],
            worldFromCamera.endTime);

        //showTransform(renderFromWorld);
        //showTransform((Transform&)worldFromCamera.startTransform);
        //showTransform((Transform&)worldFromCamera.endTransform);
        //printf("CameraTransform::CameraTransform2222\n");
        //showTransform((Transform&)renderFromCamera.startTransform);
        //showTransform((Transform&)renderFromCamera.endTransform);
        //printf("---------------------\n");

    }

}