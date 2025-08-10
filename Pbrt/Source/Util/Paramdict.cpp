#include <Util/Paramdict.h>

namespace Render {
    void ParsedParameter::AddFloat(Float v) {
        floats.push_back(v);
    }

    void ParsedParameter::AddInt(int i) {
        ints.push_back(i);
    }

    void ParsedParameter::AddString(std::string_view str) {
        strings.push_back({ str.begin(), str.end() });
    }

    void ParsedParameter::AddBool(bool v) {
        bools.push_back(v);
    }

    std::string ParsedParameter::ToString() const {
        std::string str;
       // str += std::string("\"") + type + " " + name + std::string("\" [ ");
       // if (!floats.empty())
       //     for (Float d : floats)
       //         str += StringPrintf("%f ", d);
       // else if (!ints.empty())
       //     for (int i : ints)
       //         str += StringPrintf("%d ", i);
       // else if (!strings.empty())
       //     for (const auto& s : strings)
       //         str += '\"' + s + "\" ";
       // else if (!bools.empty())
       //     for (bool b : bools)
       //         str += b ? "true " : "false ";
       // str += "] ";

        return str;
    }

    template <>
    struct ParameterTypeTraits<ParameterType::Boolean> {
        static constexpr char typeName[] = "bool";
        static constexpr int nPerItem = 1;
        using ReturnType = uint8_t;
        static bool Convert(const uint8_t* v, const FileLoc* loc) { return *v; }
        static const auto& GetValues(const ParsedParameter& param) { return param.bools; }
    };

    constexpr char ParameterTypeTraits<ParameterType::Boolean>::typeName[];

    template <>
    struct ParameterTypeTraits<ParameterType::Float> {
        static constexpr char typeName[] = "float";
        static constexpr int nPerItem = 1;
        using ReturnType = Float;
        static Float Convert(const Float* v, const FileLoc* loc) { return *v; }
        static const auto& GetValues(const ParsedParameter& param) { return param.floats; }
    };

    constexpr char ParameterTypeTraits<ParameterType::Float>::typeName[];

    template <>
    struct ParameterTypeTraits<ParameterType::Integer> {
        static constexpr char typeName[] = "integer";
        static constexpr int nPerItem = 1;
        using ReturnType = int;
        static int Convert(const int* i, const FileLoc* loc) { return *i; }
        static const auto& GetValues(const ParsedParameter& param) { return param.ints; }
    };

    constexpr char ParameterTypeTraits<ParameterType::Integer>::typeName[];

    template <>
    struct ParameterTypeTraits<ParameterType::Point2f> {
        static constexpr char typeName[] = "point2";
        static constexpr int nPerItem = 2;
        using ReturnType = Point2f;
        static Point2f Convert(const Float* v, const FileLoc* loc) {
            return Point2f(v[0], v[1]);
        }
        static const auto& GetValues(const ParsedParameter& param) { return param.floats; }
    };

    constexpr char ParameterTypeTraits<ParameterType::Point2f>::typeName[];

    template <>
    struct ParameterTypeTraits<ParameterType::Vector2f> {
        static constexpr char typeName[] = "vector2";
        static constexpr int nPerItem = 2;
        using ReturnType = Vector2f;
        static Vector2f Convert(const Float* v, const FileLoc* loc) {
            return Vector2f(v[0], v[1]);
        }
        static const auto& GetValues(const ParsedParameter& param) { return param.floats; }
    };

    constexpr char ParameterTypeTraits<ParameterType::Vector2f>::typeName[];

    // Point3f ParameterTypeTraits Definition
    template <>
    struct ParameterTypeTraits<ParameterType::Point3f> {
        // ParameterType::Point3f Type Traits
        using ReturnType = Point3f;

        static constexpr char typeName[] = "point3";

        static const auto& GetValues(const ParsedParameter& param) { return param.floats; }

        static constexpr int nPerItem = 3;

        static Point3f Convert(const Float* f, const FileLoc* loc) {
            return Point3f(f[0], f[1], f[2]);
        }
    };

    constexpr char ParameterTypeTraits<ParameterType::Point3f>::typeName[];

    template <>
    struct ParameterTypeTraits<ParameterType::Vector3f> {
        static constexpr char typeName[] = "vector3";
        static constexpr int nPerItem = 3;
        using ReturnType = Vector3f;
        static Vector3f Convert(const Float* v, const FileLoc* loc) {
            return Vector3f(v[0], v[1], v[2]);
        }
        static const auto& GetValues(const ParsedParameter& param) { return param.floats; }
    };

    constexpr char ParameterTypeTraits<ParameterType::Vector3f>::typeName[];

    template <>
    struct ParameterTypeTraits<ParameterType::Normal3f> {
        static constexpr char typeName[] = "normal";
        static constexpr int nPerItem = 3;
        using ReturnType = Normal3f;
        static Normal3f Convert(const Float* v, const FileLoc* loc) {
            return Normal3f(v[0], v[1], v[2]);
        }
        static const auto& GetValues(const ParsedParameter& param) { return param.floats; }
    };

    constexpr char ParameterTypeTraits<ParameterType::Normal3f>::typeName[];

    template <>
    struct ParameterTypeTraits<ParameterType::String> {
        static constexpr char typeName[] = "string";
        static constexpr int nPerItem = 1;
        using ReturnType = std::string;
        static std::string Convert(const std::string* s, const FileLoc* loc) { return *s; }
        static const auto& GetValues(const ParsedParameter& param) { return param.strings; }
    };

    constexpr char ParameterTypeTraits<ParameterType::String>::typeName[];

    ParameterDictionary::ParameterDictionary(ParsedParameterVector p)
        : params(std::move(p)){
        std::reverse(params.begin(), params.end());
    }

    template <ParameterType PT>
    typename ParameterTypeTraits<PT>::ReturnType ParameterDictionary::lookupSingle(
        const std::string& name,
        typename ParameterTypeTraits<PT>::ReturnType defaultValue) const {
        // Search _params_ for parameter _name_
        using traits = ParameterTypeTraits<PT>;
        for (const ParsedParameter* p : params) {
            //printf("p->name %s\n", p->name.c_str());
            if (p->name != name || p->type != traits::typeName)
                continue;
            // Extract parameter values from _p_
            const auto& values = traits::GetValues(*p);

            // Issue error if an incorrect number of parameter values were provided
            if (values.empty())
                printf("No values provided for parameter \"%s\" \"%s\".\n", name.c_str(), p->name.c_str());
            if (values.size() > traits::nPerItem)
                printf("Expected %d values for parameter \"%s\".\n", traits::nPerItem, name.c_str());

            // Return parameter values as _ReturnType_
            p->lookedUp = true;
            return traits::Convert(values.data(), &p->loc);
        }

        return defaultValue;
    }

    Float ParameterDictionary::GetOneFloat(const std::string& name, Float def) const {
        return lookupSingle<ParameterType::Float>(name, def);
    }

    int ParameterDictionary::GetOneInt(const std::string& name, int def) const {
        return lookupSingle<ParameterType::Integer>(name, def);
    }

    bool ParameterDictionary::GetOneBool(const std::string& name, bool def) const {
        return lookupSingle<ParameterType::Boolean>(name, def);
    }

    std::string ParameterDictionary::GetOneString(const std::string& name,
        const std::string& def) const {
        return lookupSingle<ParameterType::String>(name, def);
    }

    Point2f ParameterDictionary::GetOnePoint2f(const std::string& name, Point2f def) const {
        return lookupSingle<ParameterType::Point2f>(name, def);
    }

    Point3f ParameterDictionary::GetOnePoint3f(const std::string& name, Point3f def) const {
        return lookupSingle<ParameterType::Point3f>(name, def);
    }

    Vector2f ParameterDictionary::GetOneVector2f(const std::string& name,
        Vector2f def) const {
        return lookupSingle<ParameterType::Vector2f>(name, def);
    }

    Vector3f ParameterDictionary::GetOneVector3f(const std::string& name,
        Vector3f def) const {
        return lookupSingle<ParameterType::Vector3f>(name, def);
    }

    Normal3f ParameterDictionary::GetOneNormal3f(const std::string& name,
        Normal3f def) const {
        return lookupSingle<ParameterType::Normal3f>(name, def);
    }

    //--------------------------------------------------------------------------------
    template <typename ReturnType, typename ValuesType, typename C>
    static std::vector<ReturnType> returnArray(const ValuesType& values,
        const ParsedParameter& param, int nPerItem,
        C convert) {
        if (values.empty())
            printf("No values provided for \"%s\".", param.name.c_str());
        if (values.size() % nPerItem)
            printf( "Number of values provided for \"%s\" not a multiple of %d",
                param.name.c_str(), nPerItem);

        param.lookedUp = true;
        size_t n = values.size() / nPerItem;
        std::vector<ReturnType> v(n);
        for (size_t i = 0; i < n; ++i)
            v[i] = convert(&values[nPerItem * i], &param.loc);
        return v;
    }

    template <typename ReturnType, typename G, typename C>
    std::vector<ReturnType> ParameterDictionary::lookupArray(const std::string& name,
        ParameterType type,
        const char* typeName,
        int nPerItem, G getValues,
        C convert) const {
        for (const ParsedParameter* p : params) 
            if (p->name == name && p->type == typeName)
                return returnArray<ReturnType>(getValues(*p), *p, nPerItem, convert);
        return {};
    }

    template <ParameterType PT>
    std::vector<typename ParameterTypeTraits<PT>::ReturnType>
        ParameterDictionary::lookupArray(const std::string& name) const {
        using traits = ParameterTypeTraits<PT>;
        return lookupArray<typename traits::ReturnType>(
            name, PT, traits::typeName, traits::nPerItem, traits::GetValues, traits::Convert);
    }

    std::vector<Float> ParameterDictionary::GetFloatArray(const std::string& name) const {
        return lookupArray<ParameterType::Float>(name);
    }

    std::vector<int> ParameterDictionary::GetIntArray(const std::string& name) const {
        return lookupArray<ParameterType::Integer>(name);
    }

    std::vector<uint8_t> ParameterDictionary::GetBoolArray(const std::string& name) const {
        return lookupArray<ParameterType::Boolean>(name);
    }

    std::vector<std::string> ParameterDictionary::GetStringArray(
        const std::string& name) const {
        return lookupArray<ParameterType::String>(name);
    }

    std::vector<Point2f> ParameterDictionary::GetPoint2fArray(const std::string& name) const {
        return lookupArray<ParameterType::Point2f>(name);
    }

    std::vector<Vector2f> ParameterDictionary::GetVector2fArray(
        const std::string& name) const {
        return lookupArray<ParameterType::Vector2f>(name);
    }

    std::vector<Point3f> ParameterDictionary::GetPoint3fArray(const std::string& name) const {
        return lookupArray<ParameterType::Point3f>(name);
    }

    std::vector<Vector3f> ParameterDictionary::GetVector3fArray(
        const std::string& name) const {
        return lookupArray<ParameterType::Vector3f>(name);
    }

    std::vector<Normal3f> ParameterDictionary::GetNormal3fArray(
        const std::string& name) const {
        return lookupArray<ParameterType::Normal3f>(name);
    }


}