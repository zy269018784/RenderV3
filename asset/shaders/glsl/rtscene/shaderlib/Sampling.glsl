#ifndef SAMPLING_H
#define SAMPLING_H

#include "Math.glsl"
/*
	1维采样
*/
uint UniformSample(uint size, float u)
{
    uint LightIndex = uint(u * float(size));
    LightIndex = clamp(LightIndex, 0, size - 1);
    return LightIndex;
}

float UniformPMF(uint size)
{
    if (size == 0)
        return 0;
    return 1.f / float(size);
}


/*
	2 维采样
*/
// PDF = 1 / (2 * Pi)
vec4 UniformSampleHemisphere( vec2 E )
{
	float Phi = 2 * Pi * E.x;
	float CosTheta = E.y;
	float SinTheta = sqrt( 1 - CosTheta * CosTheta );

	vec3 H;
	H.x = SinTheta * cos( Phi );
	H.y = SinTheta * sin( Phi );
	H.z = CosTheta;

	float PDF = Inv2Pi;

	return vec4( H, PDF );
}

// PDF = NoL / Pi
vec4 CosineSampleHemisphere( vec2 E )
{
	float Phi = 2 * Pi * E.x;
	float CosTheta = sqrt(E.y);
	float SinTheta = sqrt(1 - CosTheta * CosTheta);

	vec3 H;
	H.x = SinTheta * cos(Phi);
	H.y = SinTheta * sin(Phi);
	H.z = CosTheta;

	float PDF = CosTheta * InvPi;

	return vec4(H, PDF);
}



// PDF = 1 / (4 * Pi)
vec4 UniformSampleSphere( vec2 E )
{
	float Phi = 2 * Pi * E.x;
	float CosTheta = 1 - 2 * E.y;
	float SinTheta = sqrt( 1 - CosTheta * CosTheta );

	vec3 H;
	H.x = SinTheta * cos( Phi );
	H.y = SinTheta * sin( Phi );
	H.z = CosTheta;

	float PDF = Inv4Pi;

	return vec4( H, PDF );
}
//----------------------------------------------------------
vec2 SampleUniformDiskConcentric(vec2 u)
{
	// Map _u_ to $[-1,1]^2$ and handle degeneracy at the origin
	vec2 uOffset = 2 * u - vec2(1, 1);
	if (uOffset.x == 0 && uOffset.y == 0)
		return vec2 (0, 0);

	// Apply concentric mapping to point
	float theta, r;
	if (abs(uOffset.x) > abs(uOffset.y)) {
		r = uOffset.x;
		theta = PiOver4 * (uOffset.y / uOffset.x);
	}
	else {
		r = uOffset.y;
		theta = PiOver2 - PiOver4 * (uOffset.x / uOffset.y);
	}
	return r * vec2(cos(theta), sin(theta));
}

vec3 SampleCosineHemisphere(vec2 u) 
{
	vec2 d = SampleUniformDiskConcentric(u);
	float z = sqrt(max(0.0, 1.0 - dot(d,d)));
	return vec3(d.x, d.y, z);
}

float CosineHemispherePDF(float cosTheta)
{
    return cosTheta * InvPi;
}

vec2 SampleUniformDiskPolar(vec2 u)
{
	float r = sqrt(u.x);
	float theta = 2 * Pi * u.y;
	return vec2( r * cos(theta), r * sin(theta) );
}

vec3 SampleUniformTriangle(vec2 u) 
{
    float b0, b1;
    if (u.x < u.y) 
	{
        b0 = u.x / 2;
        b1 = u.y - b0;
    } 
	else
	{
        b1 = u.y / 2;
        b0 = u.x - b1;
    }
    return vec3(b0, b1, 1 - b0 - b1);
}

float PowerHeuristic(int nf, float fPdf, int ng, float gPdf) 
{
    float f = nf * fPdf, g = ng * gPdf;
    if (isinf(Sqr(f)))
        return 1;
    return Sqr(f) / (Sqr(f) + Sqr(g));
}

#endif