#pragma once
#include <Util/Optional.h>
#include <Util/Paramdict.h>
#include <Util/Error.h>
#include <string_view>
#include <functional>
#include <memory>

namespace Render {
    struct Token {
        Token() = default;
        Token(std::string_view token, FileLoc loc) : token(token), loc(loc) {}
        //std::string ToString() const;
        std::string_view token;
        FileLoc loc;
    };


    class Tokenizer {
    public:
        // Tokenizer Public Methods
        Tokenizer(std::string str, std::string filename,
            std::function<void(const char*, const FileLoc*)> errorCallback);

        Tokenizer(void* ptr, size_t len, std::string filename,
            std::function<void(const char*, const FileLoc*)> errorCallback);

        ~Tokenizer();

        static std::unique_ptr<Tokenizer> CreateFromFile(
            const std::string& filename,
            std::function<void(const char*, const FileLoc*)> errorCallback);

        Optional<Token> Next();
    public:
        FileLoc loc;
    private:
        void CheckUTF(const void* ptr, int len) const;

        int getChar() {
            if (pos == end)
                return EOF;
            int ch = *pos++;
            if (ch == '\n') {
                ++loc.line;
                loc.column = 0;
            }
            else
                ++loc.column;
            return ch;
        }
        void ungetChar() {
            --pos;
            if (*pos == '\n')
                // Don't worry about the column; we'll be going to the start of
                // the next line again shortly...
                --loc.line;
        }

    private:
        std::function<void(const char*, const FileLoc*)> errorCallback;
        void* unmapPtr = nullptr;
        size_t unmapLength = 0;
        std::string contents;
        const char* pos, * end;
        std::string sEscaped;
    };

    // ParserTarget
    class ParserTarget {
    public:
        ~ParserTarget() {}
        virtual void LookAt(Float ex, Float ey, Float ez, Float lx, Float ly, Float lz,
            Float ux, Float uy, Float uz, FileLoc loc) = 0; 
        virtual void Camera(const std::string&, ParsedParameterVector params,
            FileLoc loc) = 0;
        virtual void ObjectBegin(const std::string& name, FileLoc loc) = 0;
        virtual void ObjectEnd(FileLoc loc) = 0;
        virtual void ObjectInstance(const std::string& name, FileLoc loc) = 0;
        virtual void UVScaleBegin(const std::string& name, FileLoc loc) = 0;
        virtual void UVScaleEnd(FileLoc loc) = 0;
        //virtual void Image(const std::string& name, ParsedParameterVector params, FileLoc loc) = 0;
        virtual void Texture(const std::string& name, const std::string& type,
            const std::string& texname, ParsedParameterVector params,
            FileLoc loc) = 0; 
        virtual void MakeNamedMaterial(const std::string& name, ParsedParameterVector params, FileLoc loc) = 0;
        virtual void NamedMaterial(const std::string& name, FileLoc loc) = 0;
        virtual void Transform(Float transform[16], FileLoc loc) = 0;
        virtual void ConcatTransform(Float transform[16], FileLoc loc) = 0;
        virtual void Translate(Float dx, Float dy, Float dz, FileLoc loc) = 0;
        virtual void Scale(Float sx, Float sy, Float sz, FileLoc loc) = 0;
        virtual void Rotate(Float angle, Float ax, Float ay, Float az, FileLoc loc) = 0;
        virtual void AttributeBegin(FileLoc loc) = 0;
        virtual void AttributeEnd(FileLoc loc) = 0;
        virtual void Shape(const std::string& name, const std::string& type, ParsedParameterVector params,
            FileLoc loc) = 0;
        virtual void UVScaleShapeRef(const std::string& name, ParsedParameterVector params, FileLoc loc) = 0;
        virtual void UVOffsetShapeRef(const std::string& name, ParsedParameterVector params, FileLoc loc) = 0;
        virtual void UVRotationShapeRef(const std::string& name, ParsedParameterVector params, FileLoc loc) = 0;
        virtual void ShapeRef(const std::string& name,  ParsedParameterVector params, FileLoc loc) = 0;
        virtual void LightSource(const std::string& name, ParsedParameterVector params,
            FileLoc loc) = 0;

        virtual void Integrator(const std::string& name, ParsedParameterVector params, FileLoc loc) = 0;
        virtual void Film(const std::string& name, ParsedParameterVector params, FileLoc loc) = 0;
    };


    int parseInt(const Token& t);
    double parseFloat(const Token& t);
    void parse(ParserTarget* target, std::unique_ptr<Tokenizer> t);
    std::string toString(std::string_view s);
}