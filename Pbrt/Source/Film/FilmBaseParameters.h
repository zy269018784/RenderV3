#pragma once
#include <Util/Paramdict.h>
#include <iostream>
using namespace std;
namespace Render {
    struct FilmBaseParameters {
        FilmBaseParameters(const ParameterDictionary& parameters) {
            fullResolution = Point2i(parameters.GetOneInt("xresolution", 1280), parameters.GetOneInt("yresolution", 720));
            filename = parameters.GetOneString("filename", "");
            if (filename.empty()) {
                filename = "default.png";
            }
        }

        FilmBaseParameters(Point2i fullResolution, std::string filename) :  
            fullResolution(fullResolution),
            filename(filename) {}
        Point2i fullResolution;
        std::string filename;
    };
}