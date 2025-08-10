#include <Util/File.h>
#include <Util/String.h>
#include <iostream>
#include <fstream>

#ifndef  RENDER_IS_WINDOWS
#include <dirent.h>
#include <fcntl.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

using namespace Render;
using namespace std;
namespace Render
{
    bool FileExists(std::string filename) {
#ifdef RENDER_IS_WINDOWS
        std::ifstream ifs(WStringFromUTF8(filename).c_str());
#else
        std::ifstream ifs(filename);
#endif
        return (bool)ifs;
    }

    std::string ReadFileContents(std::string filename) {
#ifdef RENDER_IS_WINDOWS
        std::ifstream ifs(WStringFromUTF8(filename).c_str(), std::ios::binary);
        if (!ifs)
            cout << "file " << filename << " read error" << endl;
           // ErrorExit("%s: %s", filename, ErrorString());
        return std::string((std::istreambuf_iterator<char>(ifs)),
            (std::istreambuf_iterator<char>()));
#else
        int fd = open(filename.c_str(), O_RDONLY);
        if (fd == -1) {
            cout << filename << endl;
            return "open error";
        }

        struct stat stat;
        if (fstat(fd, &stat) != 0) {
            cout << filename << endl;
            return "fstat error";
        }

        std::string contents(stat.st_size, '\0');
        if (read(fd, contents.data(), stat.st_size) == -1) {
            cout << filename << endl;
            return "read error";
        }
        close(fd);
        return contents;
#endif
    }

    bool WriteFileContents(std::string filename, const std::string& contents) {
#ifdef RENDER_IS_WINDOWS
        std::ofstream out(WStringFromUTF8(filename).c_str(), std::ios::binary);
#else
        std::ofstream out(filename, std::ios::binary);
#endif
        out << contents;
        out.close();
        if (!out.good()) {
            cout << filename << ": write error" << endl;
            return false;
        }
        return true;
    }

    bool HasExtension(std::string filename, std::string e) {
        std::string ext = e;
        if (!ext.empty() && ext[0] == '.')
            ext.erase(0, 1);

        size_t pos = filename.find('.');
        std::string filenameExtension;
        if (pos != string::npos)
        {
            filenameExtension = filename.substr(pos + 1);
        }

        if (ext.size() > filenameExtension.size())
            return false;
        return std::equal(ext.rbegin(), ext.rend(), filenameExtension.rbegin(),
            [](char a, char b) { return std::tolower(a) == std::tolower(b); });
    }
}