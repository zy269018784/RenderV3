#ifndef GammaCorrection_H
#define GammaCorrection_H

vec3 GammaCorrection(vec3 Color, float Gamma)
{
   return pow(Color, vec3(Gamma));
}

#endif