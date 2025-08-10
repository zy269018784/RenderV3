#pragma once
#include <Math/Math.h>
#include <Util/Error.h>
#include <Init.h>
#include <Container/InlinedVector.h>
#include <Texture/RGB.h>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Render {

    class ParsedParameter {
      public:
        ParsedParameter(FileLoc loc) : loc(loc) {}

        void AddFloat(Float v);
        void AddInt(int i);
        void AddString(std::string_view str);
        void AddBool(bool v);

        std::string ToString() const;

        std::string type, name;
        FileLoc loc;
        std::vector<Float> floats;
        std::vector<int> ints;
        std::vector<std::string> strings;
        std::vector<uint8_t> bools;
        mutable bool lookedUp = false;
        bool mayBeUnused = false;
    };

    using ParsedParameterVector = InlinedVector<ParsedParameter *, 8>;
    
    enum class ParameterType {
        Boolean,
        Float,
        Integer,
        Point2f,
        Vector2f,
        Point3f,
        Vector3f,
        Normal3f,
        Spectrum,
        String,
        Texture
    };

    template <ParameterType PT>
    struct ParameterTypeTraits {};


    class ParameterDictionary {
    public:
        // ParameterDictionary Public Methods
        ParameterDictionary() = default;
        ParameterDictionary(ParsedParameterVector params);

        Float GetOneFloat(const std::string& name, Float def) const;
        int GetOneInt(const std::string& name, int def) const;
        bool GetOneBool(const std::string& name, bool def) const;
        std::string GetOneString(const std::string& name, const std::string& def) const;

        Point2f GetOnePoint2f(const std::string& name, Point2f def) const;
        Vector2f GetOneVector2f(const std::string& name, Vector2f def) const;
        Point3f GetOnePoint3f(const std::string& name, Point3f def) const;
        Vector3f GetOneVector3f(const std::string& name, Vector3f def) const;
        Normal3f GetOneNormal3f(const std::string& name, Normal3f def) const;

        std::vector<Float> GetFloatArray(const std::string& name) const;
        std::vector<int> GetIntArray(const std::string& name) const;
        std::vector<uint8_t> GetBoolArray(const std::string& name) const;
        std::vector<std::string> GetStringArray(const std::string& name) const;

        std::vector<Point2f> GetPoint2fArray(const std::string &name) const;
        std::vector<Vector2f> GetVector2fArray(const std::string &name) const;
        std::vector<Point3f> GetPoint3fArray(const std::string &name) const;
        std::vector<Vector3f> GetVector3fArray(const std::string &name) const;
        std::vector<Normal3f> GetNormal3fArray(const std::string &name) const;

    private:
        template <ParameterType PT>
        typename ParameterTypeTraits<PT>::ReturnType lookupSingle(
            const std::string& name,
            typename ParameterTypeTraits<PT>::ReturnType defaultValue) const;
#if 1

        template <ParameterType PT>
        std::vector<typename ParameterTypeTraits<PT>::ReturnType> lookupArray(
            const std::string& name) const;

        template <typename ReturnType, typename G, typename C>
        std::vector<ReturnType> lookupArray(const std::string& name, ParameterType type,
            const char* typeName, int nPerItem, G getValues,
            C convert) const;
#endif
    private:
    public:
        ParsedParameterVector params;
    };
} 

