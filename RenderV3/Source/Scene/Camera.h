#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/hash.hpp>
namespace Rt
{
    enum ECameraType {
        FIRST_PERSON,  //第一人称 固定高度
        TWO_POINT_PERSPECTIVE,      //二点透视
        AIRCRAFT        //飞行
    };

    enum EMoveType {
        LEFT = 0,
        RIGHT = 1,
        FRONT = 2,
        BACK = 3,
        UP = 4,
        DOWN = 5
    };

    class Camera {
    public:
        Camera() = default;
        Camera(glm::mat4 transform, glm::vec2 resolution, glm::vec3 pos, glm::vec3 center, glm::vec3 up, float fov = 90.0f, float near = 1e-2f, float far = 1000.0f)
        {
            transform_ = transform;
            resolution_ = resolution;
            aspect_ = resolution.x / resolution.y;
            fov_ = fov;
            zNear_ = near;
            zFar_ = far;
            set(pos, center, up);
        }

        inline glm::mat4 ScreenFromCamera(float fov, float n, float f) {
            float invTanAng = 1 / tan(glm::radians(fov) * 0.5);
            glm::mat4 m(0);
            m[0][0] = m[1][1] = m[3][2] = 1.0f;
            m[2][2] = f / (f - n);
            m[2][3] = -f * n / (f - n);
            m = glm::scale(glm::mat4(1.0), glm::vec3(invTanAng, invTanAng, 1)) * m;
            return m;
        }

        Camera(glm::vec3 pos, glm::vec3 center, glm::vec3 up)
        {
            set(pos, center, up);
        }

        glm::vec3 getPos() const {
            return pos_;
        }

        void set(glm::vec3 pos, glm::vec3 center, glm::vec3 up) {
            pos_ = pos;
            center_ = center;
            up_ = glm::normalize(up);
            FrontDir_ = glm::normalize(center_ - pos_);
            RightDir_ = glm::normalize(glm::cross(FrontDir_, up_));
            UpDir_ = glm::normalize(glm::cross(RightDir_, FrontDir_));
            needUpdate = true;
            update();
        }

        void setPerspective(float fovDegree, int32_t width, int32_t height, float near, float far)
        {
            setPerspective(fovDegree, width / (float)height, near, far);
        }

        void setPerspective(float fovDegree, float aspect, float near, float far)
        {
            fov_ = fovDegree;
            aspect_ = aspect;
            zNear_ = near;
            zFar_ = far;
        }

        void update(float dtime = 1.0) {
            if (needUpdate) {
                //std::cout << "camera pos " << pos_.x << " " << pos_.y << " " << pos_.z << " "
                //    << "front dir " << (pos_ + FrontDir_).x << " " << (pos_ + FrontDir_).y << " " << (pos_ + FrontDir_).z << " "
                //     << std::endl;
                glm::vec4 screenWindow = glm::vec4(-aspect_, -1, aspect_, 1);
                cameraFromWorld = transform_ * glm::lookAtLH(pos_, pos_ + FrontDir_, UpDir_);
                worldFromCamera = glm::inverse(cameraFromWorld);
                NDCFromScreen = glm::scale(glm::mat4(1.0f),
                    glm::vec3(1 / (screenWindow.z - screenWindow.x), 1 / (screenWindow.w - screenWindow.y), 1)) * glm::translate(glm::mat4(1.0f), glm::vec3(-screenWindow.x, -screenWindow.w, 0));
                rasterFromNDC = glm::scale(glm::mat4(1.0f), glm::vec3(resolution_.x, -resolution_.y, 1));
                rasterFromScreen = rasterFromNDC * NDCFromScreen;
                screenFromCamera = glm::transpose(ScreenFromCamera(fov_, zNear_, zFar_));// 需要转置  
                screenFromRaster = glm::inverse(rasterFromScreen);
                screenFromNDC = glm::inverse(NDCFromScreen);
                NDCFromRaster = glm::inverse(rasterFromNDC);
                NDCFromCamera = NDCFromScreen * screenFromCamera;
                rasterFromCamera = rasterFromNDC * NDCFromCamera;
                cameraFromRaster = glm::inverse(screenFromCamera) * screenFromRaster;
                cameraFromNDC = cameraFromRaster * rasterFromNDC;
                cameraFromScreen = cameraFromRaster * rasterFromScreen;
                needUpdate = false;
            }
        }

        void setMoveSpeed(float speed) {
            speed_ = speed;
        }

        void move(EMoveType eType, float deltaTime) {
            float v = speed_;// *deltaTime;
            v = 10;
            switch (eType) {
            case LEFT:
                pos_ -= RightDir_ * v;
                break;
            case RIGHT:
                pos_ += RightDir_ * v;
                break;
            case FRONT:
                pos_ += FrontDir_ * v;
                break;
            case BACK:
                pos_ -= FrontDir_ * v;
                break;
            case UP:
                pos_ += UpDir_ * v;
                break;
            case DOWN:
                pos_ -= UpDir_ * v;
                break;
            }
            needUpdate = true;
        }
          
        void Yaw(float angle) { 
            glm::mat4 m(1.0);
            switch (type_) {
            case FIRST_PERSON:
                m = glm::mat4(1.0);
                m = glm::rotate(m, angle, UpDir_);
                FrontDir_ = m * glm::vec4(FrontDir_, 0.0);
                FrontDir_ = glm::normalize(FrontDir_);
                RightDir_ = glm::normalize(glm::cross(FrontDir_, UpDir_));
                UpDir_ = glm::normalize(glm::cross(RightDir_, FrontDir_));
                needUpdate = true;
                update();
                break;
            case TWO_POINT_PERSPECTIVE:
                break;
            case AIRCRAFT:
                break;
            }
        }

        void Roll(float angle) {
            glm::mat4 m(1.0);
            switch (type_) {
            case FIRST_PERSON:
                m = glm::mat4(1.0);
                m = glm::rotate(m, angle, RightDir_);
                FrontDir_ = m * glm::vec4(FrontDir_, 0.0);
                FrontDir_ = glm::normalize(FrontDir_);
                RightDir_ = glm::normalize(glm::cross(FrontDir_, UpDir_));
                UpDir_ = glm::normalize(glm::cross(RightDir_, FrontDir_));
                needUpdate = true;
                update();
                break;
            case TWO_POINT_PERSPECTIVE:
                break;
            case AIRCRAFT:
                break;
            }
        }

        void Pitch(float angle) {
            switch (type_) {
            case FIRST_PERSON:
                break;
            case TWO_POINT_PERSPECTIVE:
                break;
            case AIRCRAFT:
                break;
            }
        }
        void zoom(float delta) {
            fov_ -= (float)delta * 2.0;
            if (fov_ < 1.0f)
                fov_ = 1.0f;
            if (fov_ > 90.0f)
                fov_ = 90.0f;
            needUpdate = true;
        }

        void printMatrix(glm::mat4 matrix) {
            printf("%f,%f,%f,%f\n", matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]);
            printf("%f,%f,%f,%f\n", matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]);
            printf("%f,%f,%f,%f\n", matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
            printf("%f,%f,%f,%f\n", matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
        }

    public:
        bool needUpdate = false;

        ECameraType type_ = FIRST_PERSON;

        glm::mat4 transform_;
        glm::vec2 resolution_;
        float aspect_ = 1.0f;           // 窗口宽高比 Bounds2f ScreenWindow;
        float fov_ = 90.0f;             // 相机 field of view 		
        float zNear_ = 0.01f;
        float zFar_ = 1000.0f;

        float speed_ = 1.0;	            // 相机 移动速度 													
        glm::vec3 pos_;					// 相机 位置
        glm::vec3 center_;				// 相机 Center
        glm::vec3 up_;					// 相机 上向量

        glm::vec3 FrontDir_;
        glm::vec3 RightDir_;
        glm::vec3 UpDir_;

        glm::mat4 cameraFromWorld;
        glm::mat4 worldFromCamera;
        glm::mat4 NDCFromScreen;
        glm::mat4 rasterFromNDC;
        glm::mat4 rasterFromScreen;
        glm::mat4 screenFromCamera;
        glm::mat4 screenFromRaster;
        glm::mat4 screenFromNDC;
        glm::mat4 NDCFromRaster;
        glm::mat4 NDCFromCamera;
        glm::mat4 rasterFromCamera;
        glm::mat4 cameraFromRaster;
        glm::mat4 cameraFromNDC;
        glm::mat4 cameraFromScreen;
    };
}
