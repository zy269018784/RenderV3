#pragma once
#include <string>
#include <Util/Error.h>
#include <Util/Paramdict.h>
namespace Render {
    struct SceneEntity {
    public:
        SceneEntity() = default;
        SceneEntity(const std::string& name, ParameterDictionary parameters, FileLoc loc)
            : name(name), parameters(parameters), loc(loc) {}
    public:
        std::string name;                   // 实体名称
        FileLoc loc;                        // 文件位置
        ParameterDictionary parameters;     // 参数
    };
}