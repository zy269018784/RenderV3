#pragma once
#include <string>
#include <glm/glm.hpp>
namespace Rt
{

    class RtImage 
    {
    public:
        RtImage() = default;
        RtImage(const std::string& name, const std::string& filename) {
            name_ = name;
            filename_ = filename;
        }
        std::string name_;
        std::string filename_;
    };

    class RtTexture
    {
    public:
        RtTexture(const std::string name, glm::vec3 rgb) {
            name_ = name;
            rgb_ = rgb;
            type = 1;
        }

        RtTexture(const std::string name, const std::string& filename) {
            name_ = name;
            filename_ = filename;
            type = 0;
        }

        const std::string& GetFilename() {
            return filename_;
        }

        std::string name_;
        int type = 0; //imagemap 1:rgb
        std::string filename_;
        glm::vec3 rgb_;
    };
}