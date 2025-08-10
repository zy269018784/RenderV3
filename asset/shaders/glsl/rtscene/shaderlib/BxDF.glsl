#ifndef BxDF_H
#define BxDF_H

#include "Sampling.glsl"
#include "TrowbridgeReitzDistribution.glsl"
#include "Scattering.glsl"
#include "Complex.glsl"





#define Diffuse                         1
#define Glass                           2
#define Conductor                       3
#define Dielectric                      4
#define Disney                          5
#define ColoredAluminumPlate1           6
#define ColoredAluminumPlate2           7
#define ColoredAluminumPlate3           8
#define ColoredAluminumPlate4           9
#define ColoredAluminumPlate5           10
#define ColoredAluminumPlate6           11
#define ColoredAluminumPlate7           12
#define ColoredAluminumPlate8           13
#define ColoredAluminumPlate9           14
#define ColoredAluminumPlate10          15


struct BSDFSample 
{
    vec3 wi;
    float pdf;
    vec3 f;
};

struct BSDFSampleInput 
{
    Material m;
    vec2 uv;
};

#include "BxDFs\DiffuseBxDF.glsl"
#include "BxDFs\ConductorBxDF.glsl"
#include "BxDFs\GlassBxDF.glsl"
#include "BxDFs\DielectricBxDF.glsl" 
#include "BxDFs\DisneyBxDF.glsl"
#include "BxDFs\ColoredAluminumPlate1BxDF.glsl" 

BSDFSample BXDFSample(vec3 wo, float u, vec2 u2, BSDFSampleInput SampleInput)
{
    switch (GetMaterialType(SampleInput.m)) {
        case Diffuse:
            return DiffuseBxDF_Sample_F(wo, u, u2, SampleInput);
        case Glass:
            return GlassBxDF_Sample_F(wo, u, u2, SampleInput);
        case Conductor:
            return ConductorBxDF_Sample_F(wo, u, u2, SampleInput);
        case Dielectric:
            return DielectricBxDF_Sample_F(wo, u, u2, SampleInput); 
        case Disney:
            return DisneyBxDF_Sample_F(wo, u, u2, SampleInput.m);
        case ColoredAluminumPlate1:
            return ColoredAluminumPlate1BxDF_Sample_F(wo, u, u2, SampleInput);
    }
}

vec3 BXDFSample_F(vec3 wo, vec3 wi, Material m)
{
    switch (GetMaterialType(m)) {
        case Diffuse:
            return DiffuseBxDF_F(wo, wi,m);
        case Glass:
            return GlassBxDF_F(wo, wi, m);
        case Conductor:
            return ConductorBxDF_F(wo, wi,m);
        case Dielectric:
            return DielectricBxDF_F(wo, wi,m);
        case Disney:
            return DisneyBxDF_F(wo, wi, m);
        case ColoredAluminumPlate1:
            return ColoredAluminumPlate1BxDF_F(wo, wi, m);
    }
}

float BXDFSample_PDF(vec3 wo, vec3 wi, Material m)
{
    switch (GetMaterialType(m)) {
        case Diffuse:
            return DiffuseBxDF_PDF(wo, wi, m);
        case Glass:
            return GlassBxDF_PDF(wo, wi, m);
        case Conductor:
            return ConductorBxDF_PDF(wo, wi, m);
        case Dielectric:
            return DielectricBxDF_PDF(wo, wi, m);
        case Disney:
            return DisneyBxDF_PDF(wo, wi, m);
        case ColoredAluminumPlate1:
            return ColoredAluminumPlate1BxDF_PDF(wo, wi, m);
    }
}

#endif