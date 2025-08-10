#include <Weather/Weather.h>
#include <Type/TypeDeclaration.h>
#include <Type/Types.h>
#include <Texture/RGB.h>
#include <Math/Scattering.h>
namespace Render {

    Weather::Weather(enum WeatherType Type)
        :  WeatherType(Type){
    
    }

    void Weather::SetSkyBaseColor(RGB c) {
        SkyBaseColor = c;
    }

    RGB Weather::SkyColor(Vector3f d) {
       // RGB SkyColor(0.3333f, 0.5961f, 0.9725f);
        // RGB SkyColor(0.9176f, 0.3922f, 0.2784f);
        RGB SunColor(1.f, 1.f, 1.f);
        RGB SColor(0.9804, 0.7686, 0.4549);
        //RGB SColor2(0.9255f, 0.3961f, 0.f);
        RGB SColor2(1.f, 1.f, 1.f);
        RGB Color;

        Float g = 0.999;
        Float cosTheta = 0.f;
        Float hg = 0.f;

        Float t1 = SmoothStep(0.06f, 0.0f, d.y);

        //cosTheta = Dot(d, Normalize(Vector3f(0, 0.1, -1)));
        cosTheta = Dot(d, Normalize(Vector3f(1, 0.5, 1)));

        hg = HenyeyGreenstein(g, cosTheta);
        hg = Clamp(hg, 0.f, 1.f);

        Float t2 = SmoothStep(cosTheta, 0.85f, 1.f);

        Float t3 = SmoothStep(cosTheta, 0.995f, 1.f);

        // Ìì¿ÕÑÕÉ«
        Color = this->SkyBaseColor;

       
        Color = Mix(Color, SColor2, t2);
 

        //Color = Mix(Color, SColor2, t1);
        
        // Ì«Ñô
        Color = Mix(Color, SunColor, hg);
        //Color = Mix(Color, SunColor, t3);

        Color = Clamp(Color, 0.f, 1.f);
        return Color;
    }
}