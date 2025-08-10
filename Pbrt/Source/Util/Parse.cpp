#include <Math/Math.h>
#include <Util/Parse.h>
#include <Util/File.h>
#include <Util/String.h>
#include <Util/Paramdict.h>
#include <double-conversion/double-conversion.h>
#ifdef RENDER_IS_WINDOWS
 #include <windows.h>  // Windows file mapping API
#endif
#include <limits.h>
#include <algorithm>
namespace Render {
    static double_conversion::StringToDoubleConverter floatParser(
        double_conversion::StringToDoubleConverter::ALLOW_HEX, 0. /* empty string value */,
        0. /* junk string value */, nullptr /* infinity symbol */, nullptr /* NaN symbol */);


    char decodeEscaped(int ch, const FileLoc& loc) {
        switch (ch) {
        case EOF:
            printf("premature EOF after character escape '\\'");
        case 'b':
            return '\b';
        case 'f':
            return '\f';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        case '\\':
            return '\\';
        case '\'':
            return '\'';
        case '\"':
            return '\"';
        default:
            return ch;
            //printf("unexpected escaped character \"%c\"", ch);
        }
        return 0;  // NOTREACHED
    }

    inline bool isQuotedString(std::string_view str) {
        return str.size() >= 2 && str[0] == '"' && str.back() == '"';
    }

    std::string toString(std::string_view s) {
        return std::string(s.data(), s.size());
    }

    static std::string_view dequoteString(const Token& t) {
        if (!isQuotedString(t.token))
            printf("\"%s\": expected quoted string", toString(t.token).c_str());

        std::string_view str = t.token;
        str.remove_prefix(1);
        str.remove_suffix(1);
        return str;
    }


    int parseInt(const Token& t) {
        bool negate = t.token[0] == '-';

        int index = 0;
        if (t.token[0] == '+' || t.token[0] == '-')
            ++index;

        int64_t value = 0;
        while (index < t.token.size()) {
            if (!(t.token[index] >= '0' && t.token[index] <= '9'))
                printf("\"%c\": expected a number", t.token[index]);
            value = 10 * value + (t.token[index] - '0');
            ++index;

            if (value > std::numeric_limits<int>::max())
                printf(
                    "Numeric value too large to represent as a 32-bit integer.");
            else if (value < std::numeric_limits<int>::lowest())
                printf("Numeric value too low to represent as a 32 - bit integer.");
        }

        return negate ? -value : value;
    }

    double parseFloat(const Token& t) {
        if (t.token.size() == 1) {
            if (!(t.token[0] >= '0' && t.token[0] <= '9'))
                printf("\"%c\": expected a number", t.token[0]);
            return t.token[0] - '0';
        }

        // Copy to a buffer so we can NUL-terminate it, as strto[idf]() expect.
        char buf[64];
        char* bufp = buf;
        std::unique_ptr<char[]> allocBuf;
        if (t.token.size() + 1 >= sizeof(buf)) {
            // This should be very unusual, but is necessary in case we get a
            // goofball number with lots of leading zeros, for example.
            allocBuf = std::make_unique<char[]>(t.token.size() + 1);
            bufp = allocBuf.get();
        }

        std::copy(t.token.begin(), t.token.end(), bufp);
        bufp[t.token.size()] = '\0';

        // Can we just use strtol?
        auto isInteger = [](std::string_view str) {
            for (char ch : str)
                if (!(ch >= '0' && ch <= '9'))
                    return false;
            return true;
        };

        int length = 0;
        double val;
        if (isInteger(t.token)) {
            char* endptr;
            val = double(strtol(bufp, &endptr, 10));
            length = endptr - bufp;
        }
        else if (sizeof(Float) == sizeof(float))
            val = floatParser.StringToFloat(bufp, t.token.size(), &length);
        else
            val = floatParser.StringToDouble(bufp, t.token.size(), &length);

        if (length == 0)
            printf("%s: expected a number", toString(t.token).c_str());

        return val;
    }

    //---------------------------------------------------------------------------
    Tokenizer::Tokenizer(std::string str, std::string filename,
        std::function<void(const char*, const FileLoc*)> errorCallback)
        : errorCallback(std::move(errorCallback)), contents(std::move(str)) {
        loc = FileLoc(*new std::string(filename));
        pos = contents.data();
        end = pos + contents.size();
    }

    Tokenizer::Tokenizer(void* ptr, size_t len, std::string filename,
        std::function<void(const char*, const FileLoc*)> errorCallback)
        : errorCallback(std::move(errorCallback)), unmapPtr(ptr), unmapLength(len) {
        loc = FileLoc(*new std::string(filename));
        pos = (const char*)ptr;
        end = pos + len;
    }

    Tokenizer::~Tokenizer() {
#ifdef RENDER_HAVE_MMAP
        if (unmapPtr && unmapLength > 0)
            if (munmap(unmapPtr, unmapLength) != 0)
                errorCallback(StringPrintf("munmap: %s", ErrorString()).c_str(), nullptr);
#elif defined(RENDER_IS_WINDOWS)
        if (unmapPtr && UnmapViewOfFile(unmapPtr) == 0)
            errorCallback("", nullptr);
#endif
    }

    std::unique_ptr<Tokenizer> Tokenizer::CreateFromFile(
        const std::string& filename,
        std::function<void(const char*, const FileLoc*)> errorCallback) {
#ifdef RENDER_HAVE_MMAP
        int fd = open(filename.c_str(), O_RDONLY);
        if (fd == -1) {
            errorCallback(StringPrintf("%s: %s", filename, ErrorString()).c_str(), nullptr);
            return nullptr;
        }

        struct stat stat;
        if (fstat(fd, &stat) != 0) {
            errorCallback(StringPrintf("%s: %s", filename, ErrorString()).c_str(), nullptr);
            return nullptr;
        }

        size_t len = stat.st_size;
        if (len < 16 * 1024 * 1024) {
            close(fd);

            std::string str = ReadFileContents(filename);
            return std::make_unique<Tokenizer>(std::move(str), filename,
                std::move(errorCallback));
        }

        void* ptr = mmap(nullptr, len, PROT_READ, MAP_PRIVATE | MAP_NORESERVE, fd, 0);
        if (ptr == MAP_FAILED)
            errorCallback(StringPrintf("%s: %s", filename, ErrorString()).c_str(), nullptr);

        if (close(fd) != 0) {
            errorCallback(StringPrintf("%s: %s", filename, ErrorString()).c_str(), nullptr);
            return nullptr;
        }

        return std::make_unique<Tokenizer>(ptr, len, filename, std::move(errorCallback));
#elif defined(RENDER_IS_WINDOWS)
        auto errorReportLambda = [&errorCallback, &filename]() -> std::unique_ptr<Tokenizer> {
            LPSTR messageBuffer = nullptr;
            FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, ::GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&messageBuffer, 0, NULL);

            //errorCallback(StringPrintf("%s: %s", filename, messageBuffer).c_str(), nullptr);

            LocalFree(messageBuffer);
            return nullptr;
        };

        HANDLE fileHandle =
            CreateFileW(WStringFromUTF8(filename).c_str(), GENERIC_READ, FILE_SHARE_READ, 0,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (fileHandle == INVALID_HANDLE_VALUE)
            return errorReportLambda();

#if 1
        LARGE_INTEGER fileSize;
        if (!GetFileSizeEx(fileHandle, &fileSize)) {
            std::cerr << "Failed to get file size: " << GetLastError() << std::endl;
            CloseHandle(fileHandle);
            return nullptr;
        }
        size_t len = fileSize.QuadPart;
#else
        size_t len = GetFileSize(fileHandle, 0);
#endif
        HANDLE mapping = CreateFileMapping(fileHandle, 0, PAGE_READONLY, 0, 0, 0);
        CloseHandle(fileHandle);
        if (mapping == 0)
            return errorReportLambda();

        LPVOID ptr = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
        CloseHandle(mapping);
        if (!ptr)
            return errorReportLambda();

        return std::make_unique<Tokenizer>(ptr, len, filename, std::move(errorCallback));
#else
        std::string str = ReadFileContents(filename);
        return std::make_unique<Tokenizer>(std::move(str), filename,
            std::move(errorCallback));
#endif
    }

    Optional<Token> Tokenizer::Next() {
        while (true) {
            const char* tokenStart = pos;
            FileLoc startLoc = loc;

            int ch = getChar();
            

            if (ch == EOF)
                return {};
            else if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r') {
                // nothing
            }
            else if (ch == '"') {
                // scan to closing quote
                bool haveEscaped = false;
                while ((ch = getChar()) != '"') {
                    if (ch == EOF) {
                        errorCallback("premature EOF", &startLoc);
                        return {};
                    }
                    else if (ch == '\n') {
                        errorCallback("unterminated string", &startLoc);
                        return {};
                    }
                    else if (ch == '\\') {
                        haveEscaped = true;
                        // Grab the next character
                        if ((ch = getChar()) == EOF) {
                            errorCallback("premature EOF", &startLoc);
                            return {};
                        }
                    }
                }

                if (!haveEscaped)
                    return Token({ tokenStart, size_t(pos - tokenStart) }, startLoc);
                else {
                    sEscaped.clear();
                    for (const char* p = tokenStart; p < pos; ++p) {
                        if (*p != '\\')
                            sEscaped.push_back(*p);
                        else {
                            ++p;
                            sEscaped.push_back(decodeEscaped(*p, startLoc));
                        }
                    }
                    return Token({ sEscaped.data(), sEscaped.size() }, startLoc);
                }
            }
            else if (ch == '[' || ch == ']') {
                return Token({ tokenStart, size_t(1) }, startLoc);
            }
            else if (ch == '#') {
                // comment: scan to EOL (or EOF)
                while ((ch = getChar()) != EOF) {
                    if (ch == '\n' || ch == '\r') {
                        ungetChar();
                        break;
                    }
                }
                return Token({ tokenStart, size_t(pos - tokenStart) }, startLoc);
            }
            else {
                // Regular statement or numeric token; scan until we hit a
                // space, opening quote, or bracket.
                while ((ch = getChar()) != EOF) {
                    if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r' || ch == '"' ||
                        ch == '[' || ch == ']') {
                        ungetChar();
                        break;
                    }
                }
                return Token({ tokenStart, size_t(pos - tokenStart) }, startLoc);
            }
        }
    }

    void Tokenizer::CheckUTF(const void* ptr, int len) const {
        const unsigned char* c = (const unsigned char*)ptr;
        // https://en.wikipedia.org/wiki/Byte_order_mark
        if (len >= 2 && ((c[0] == 0xfe && c[1] == 0xff) || (c[0] == 0xff && c[1] == 0xfe)))
            errorCallback("File is encoded with UTF-16, which is not currently "
                "supported by pbrt (https://github.com/mmp/pbrt-v4/issues/136).",
                &loc);
    }

    //--------------------------------------------------------------------------------------------------------------
    template <typename Next, typename Unget>
    static ParsedParameterVector parseParameters(
        Next nextToken, Unget ungetToken, bool formatting,
        const std::function<void(const Token& token, const char*)>& errorCallback) {
        ParsedParameterVector parameterVector;

        while (true) {
            Optional<Token> t = nextToken(0);
            if (!t.has_value())
                return parameterVector;
            //printf("t %s\n", t->token.data());
            if (!isQuotedString(t->token)) {
                ungetToken(*t);
                return parameterVector;
            }

            ParsedParameter* param = new ParsedParameter(t->loc);

            std::string_view decl = dequoteString(*t);
            //printf("decl %s\n", decl.data());

            auto skipSpace = [&decl](std::string_view::const_iterator iter) {
                while (iter != decl.end() && (*iter == ' ' || *iter == '\t'))
                    ++iter;
                return iter;
            };
            // Skip to the next whitespace character (or the end of the string).
            auto skipToSpace = [&decl](std::string_view::const_iterator iter) {
                while (iter != decl.end() && *iter != ' ' && *iter != '\t')
                    ++iter;
                return iter;
            };

            auto typeBegin = skipSpace(decl.begin());
            if (typeBegin == decl.end())
                printf("Parameter \"%s\" doesn't have a type declaration?!",
                    std::string(decl.begin(), decl.end()).c_str());

            // Find end of type declaration
            auto typeEnd = skipToSpace(typeBegin);
            param->type.assign(typeBegin, typeEnd);

            if (formatting) {  // close enough: upgrade...
                if (param->type == "point")
                    param->type = "point3";
                if (param->type == "vector")
                    param->type = "vector3";
                if (param->type == "color")
                    param->type = "rgb";
            }

          
            auto nameBegin = skipSpace(typeEnd);
            if (nameBegin == decl.end())
                printf("Unable to find parameter name from \"%s\"",
                    std::string(decl.begin(), decl.end()).c_str());

            auto nameEnd = skipToSpace(nameBegin);
            param->name.assign(nameBegin, nameEnd);

            enum ValType { Unknown, String, Bool, Float, Int } valType = Unknown;

            if (param->type == "integer")
                valType = Int;

            auto addVal = [&](const Token& t) {
                if (isQuotedString(t.token)) {
                    switch (valType) {
                    case Unknown:
                        valType = String;
                        break;
                    case String:
                        break;
                    case Float:
                        errorCallback(t, "expected floating-point value");
                    case Int:
                        errorCallback(t, "expected integer value");
                    case Bool:
                        errorCallback(t, "expected Boolean value");
                    }

                    param->AddString(dequoteString(t));
                }
                else if (t.token[0] == 't' && t.token == "true") {
                    switch (valType) {
                    case Unknown:
                        valType = Bool;
                        break;
                    case String:
                        errorCallback(t, "expected string value");
                    case Float:
                        errorCallback(t, "expected floating-point value");
                    case Int:
                        errorCallback(t, "expected integer value");
                    case Bool:
                        break;
                    }

                    param->AddBool(true);
                }
                else if (t.token[0] == 'f' && t.token == "false") {
                    switch (valType) {
                    case Unknown:
                        valType = Bool;
                        break;
                    case String:
                        errorCallback(t, "expected string value");
                    case Float:
                        errorCallback(t, "expected floating-point value");
                    case Int:
                        errorCallback(t, "expected integer value");
                    case Bool:
                        break;
                    }

                    param->AddBool(false);
                }
                else {
                    switch (valType) {
                    case Unknown:
                        valType = Float;
                        break;
                    case String:
                        errorCallback(t, "expected string value");
                    case Float:
                        break;
                    case Int:
                        break;
                    case Bool:
                        errorCallback(t, "expected Boolean value");
                    }

                    if (valType == Int)
                        param->AddInt(parseInt(t));
                    else
                        param->AddFloat(parseFloat(t));
                }
            };

            Token val = *nextToken(0);

            if (val.token == "[") {
                while (true) {
                    val = *nextToken(0);
                    if (val.token == "]")
                        break;
                    addVal(val);
                }
            }
            else {
                addVal(val);
            }

            if (formatting && param->type == "bool") {
                for (const auto& b : param->strings) {
                    if (b == "true")
                        param->bools.push_back(true);
                    else if (b == "false")
                        param->bools.push_back(false);
                    else
                        printf(
                            "%s: neither \"true\" nor \"false\" in bool "
                            "parameter list.",
                            b);
                }
                param->strings.clear();
            }

            parameterVector.push_back(param);
        }

        return parameterVector;
    }


    void parse(ParserTarget* target, std::unique_ptr<Tokenizer> t) {

        std::vector<std::unique_ptr<Tokenizer>> fileStack;
        fileStack.push_back(std::move(t));
        
        Optional<Token> ungetToken;

        auto parseError = [&](const char* msg, const FileLoc* loc) {
            printf("%s", msg);
        };

        std::function<Optional<Token>(int)> nextToken;
        nextToken = [&](int flags) -> Optional<Token> {
            if (ungetToken.has_value())
                return std::exchange(ungetToken, {});

            if (fileStack.empty()) {
                return {};
            }

            Optional<Token> tok = fileStack.back()->Next();

            if (!tok) {
                fileStack.pop_back();
                return nextToken(flags);
            }
            else if (tok->token[0] == '#') {
                return nextToken(flags);    
            }

            return tok;
        };

        auto syntaxError = [&](const Token& t) {
            printf("Unknown directive: %s", toString(t.token).c_str());
        };

        auto unget = [&](Token t) {
            ungetToken = t;
        };

        auto basicParamListEntrypoint =
            [&](void (ParserTarget::* apiFunc)(const std::string&,
                ParsedParameterVector, FileLoc),
                FileLoc loc) {
                    Token t = *nextToken(0);
                    std::string_view dequoted = dequoteString(t);
                    std::string n = toString(dequoted);
                    ParsedParameterVector parameterVector = parseParameters(
                        nextToken, unget, false, [&](const Token& t, const char* msg) {
                            std::string token = toString(t.token);
                            //std::string str = StringPrintf("%s: %s", token, msg);
                            //parseError(str.c_str(), &t.loc);
                        });
                    (target->*apiFunc)(n, std::move(parameterVector), loc);
        };


        Optional<Token> tok;

        while (true) {
            tok = nextToken(0);
            if (!tok.has_value())
                break;
            //printf("[top] %s\n", toString(tok->token).c_str());
            switch (tok->token[0]) {
            case 'A':
                if (tok->token == "AttributeBegin")
                    target->AttributeBegin(tok->loc);
                else if (tok->token == "AttributeEnd")
                    target->AttributeEnd(tok->loc);
                break;
            case 'C':
                if (tok->token == "Camera") {
                    basicParamListEntrypoint(&ParserTarget::Camera, tok->loc);
                }
                else  if (tok->token == "ConcatTransform") {
                    if (nextToken(0)->token != "[")
                        syntaxError(*tok);
                    Float m[16];
                    for (int i = 0; i < 16; ++i)
                        m[i] = parseFloat(*nextToken(0));
                    if (nextToken(0)->token != "]")
                        syntaxError(*tok);
                    target->ConcatTransform(m, tok->loc);
                }
                break;
            case 'F':
                if (tok->token == "Film") {
                    basicParamListEntrypoint(&ParserTarget::Film, tok->loc);
                }
                break;
            case 'I':
                if (tok->token == "Include") {    
                    Token filenameToken = *nextToken(0);
                    std::string filename = toString(dequoteString(filenameToken));
                    std::unique_ptr<Tokenizer> tinc =
                        Tokenizer::CreateFromFile(filename, parseError);
                    if (tinc) {
                        fileStack.push_back(std::move(tinc));
                    }
                }
                else if (tok->token == "Integrator") {
                    basicParamListEntrypoint(&ParserTarget::Integrator, tok->loc);
                }
                break;
            case 'L':
                if (tok->token == "LookAt") {
                    Float v[9];
                    for (int i = 0; i < 9; ++i)
                        v[i] = parseFloat(*nextToken(0));
                    target->LookAt(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8],
                        tok->loc);
                }  else if (tok->token == "LightSource")
                    basicParamListEntrypoint(&ParserTarget::LightSource, tok->loc);
                break;
            case 'M':
                if (tok->token == "MakeNamedMaterial") {
                    basicParamListEntrypoint(&ParserTarget::MakeNamedMaterial, tok->loc);
                }
                else if (tok->token == "Material");
                break;
            case 'N':
                if (tok->token == "NamedMaterial") {
                    std::string_view n = dequoteString(*nextToken(0));
                    target->NamedMaterial(toString(n), tok->loc);
                }
                break;
            case 'O':
                if (tok->token == "ObjectBegin") {
                    std::string_view n = dequoteString(*nextToken(0));
                    target->ObjectBegin(toString(n), tok->loc);
                } 
                else if (tok->token == "ObjectEnd")
                    target->ObjectEnd(tok->loc);
                else if (tok->token == "ObjectInstance") {
                    std::string_view n = dequoteString(*nextToken(0));
                    target->ObjectInstance(toString(n), tok->loc);
                }
                break;
            case 'R':
                if (tok->token == "Rotate") {
                    Float v[4];
                    for (int i = 0; i < 4; ++i)
                        v[i] = parseFloat(*nextToken(0));
                    target->Rotate(v[0], v[1], v[2], v[3], tok->loc);
                }
                break;
            case 'S':
                if (tok->token == "Shape") {
                    std::string_view n;
                    n = dequoteString(*nextToken(0));
                    std::string name = toString(n);
                    n = dequoteString(*nextToken(0));
                    std::string type = toString(n);


                    ParsedParameterVector params = parseParameters(
                        nextToken, unget, false, [&](const Token& t, const char* msg) {});


                    target->Shape(name, type, params, tok->loc);
                }  
                else if (tok->token == "ShapeRef") {
                    std::string_view n;
                    n = dequoteString(*nextToken(0));
                    std::string name = toString(n);

                    ParsedParameterVector params = parseParameters(
                        nextToken, unget, false, [&](const Token& t, const char* msg) {});


                    target->ShapeRef(name, params, tok->loc);
                }
                else if (tok->token == "Scale") {
                    Float v[3];
                    for (int i = 0; i < 3; ++i)
                        v[i] = parseFloat(*nextToken(0));
                    target->Scale(v[0], v[1], v[2], tok->loc);
                }
                break;
            case 'T':
                if (tok->token == "Transform") {
                    if (nextToken(0)->token != "[")
                        syntaxError(*tok);
                    Float m[16];
                    for (int i = 0; i < 16; ++i) {
                        m[i] = parseFloat(*nextToken(0));
                    }
                    if (nextToken(0)->token != "]")
                        syntaxError(*tok);
                    target->Transform(m, tok->loc);
                }
                else if (tok->token == "Translate") {
                    Float v[3];
                    for (int i = 0; i < 3; ++i)
                        v[i] = parseFloat(*nextToken(0));
                    target->Translate(v[0], v[1], v[2], tok->loc);
                }
                else if (tok->token == "Texture") {

                    std::string_view n;
                    n = dequoteString(*nextToken(0));
                    std::string name = toString(n);
                    n = dequoteString(*nextToken(0));
                    std::string type = toString(n);
                    
                    Token t = *nextToken(0);
                    std::string_view dequoted = dequoteString(t);
                    std::string texName = toString(dequoted);

 
                    ParsedParameterVector params = parseParameters(
                        nextToken, unget, false, [&](const Token& t, const char* msg) {});

                    target->Texture(name, type, texName, std::move(params), tok->loc);
                }
                break;
            case 'U':
                if (tok->token == "UVScaleBegin") {
                    std::string_view n = dequoteString(*nextToken(0));
                    target->UVScaleBegin(toString(n), tok->loc);
                } 
                else if (tok->token == "UVScaleEnd")
                    target->UVScaleEnd(tok->loc);
                else if (tok->token == "UVScaleShapeRef") {
                    std::string_view n;
                    n = dequoteString(*nextToken(0));
                    std::string name = toString(n);

                    ParsedParameterVector params = parseParameters(
                        nextToken, unget, false, [&](const Token& t, const char* msg) {});
                    target->UVScaleShapeRef(name, params, tok->loc);
                    target->UVOffsetShapeRef(name, params, tok->loc);
                    target->UVRotationShapeRef(name, params, tok->loc);
                }
                break;
            case 'W':
                if (tok->token == "WorldBegin");
                break;
            }
        }
    }

}