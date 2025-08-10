#ifndef MATH_H
#define MATH_H

#define Pi  		3.14159265358979323846
#define TwoPi	    6.28318530717958647692 
#define InvPi  		0.31830988618379067154
#define Inv2Pi  	0.15915494309189533577
#define Inv4Pi 		0.07957747154594766788
#define PiOver2  	1.57079632679489661923
#define PiOver4  	0.78539816339744830961
#define Sqrt2  		1.41421356237309504880

float Sqr(float v) 
{
	return v * v;
}

float SafeSqrt(float x)
{
    return sqrt(max(0.f, x));
}

float Clamp(float v, float min, float max)
{
	return clamp(v, min, max);
}

float LengthSquared(vec2 n) 
{
	return Sqr(n.x) + Sqr(n.y);
}

float LengthSquared(vec3 n) 
{
	return Sqr(n.x) + Sqr(n.y) + Sqr(n.z);
}

bool SameHemisphere(vec3 w, vec3 wp)
{
    return w.z * wp.z > 0;
}

float Dot(vec3 a, vec3 b)
{
	return dot(a, b);
}

float AbsDot(vec3 a, vec3 b)
{
	return abs(dot(a, b));
}

float CosTheta(vec3 w)
 {
	return w.z;
}

float Cos2Theta(vec3 w) 
{
	return Sqr(w.z);
}

float AbsCosTheta(vec3 w) 
{
	return abs(w.z);
}

float Sin2Theta(vec3 w) 
{
	return max(0, 1 - Cos2Theta(w));
}

float SinTheta(vec3 w) 
{
	return sqrt(Sin2Theta(w));
}

float TanTheta(vec3 w)
{
	return SinTheta(w) / CosTheta(w);
}

float Tan2Theta(vec3 w) 
{
	return Sin2Theta(w) / Cos2Theta(w);
}

float CosPhi(vec3 w) 
{
	float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 1 : Clamp(w.x / sinTheta, -1, 1);
}

float SinPhi(vec3 w) 
{
	float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 0 : Clamp(w.y / sinTheta, -1, 1);
}

/*
	向量归一化
*/
vec3 Normalize(vec3 w)
{
	return normalize(w);
}

/*
	FMA
*/
float FMA(float a, float b, float c)
{
	return fma(a, b, c);
}

vec3 FMA(vec3 a, vec3 b, vec3 c)
{
	return fma(a, b, c);
}


float DifferenceOfProducts(float a, float b, float c, float d)
{
	float cd = c * d;
	float differenceOfProducts = FMA(a, b, -cd);
	float error = FMA(-c, d, cd);
	return differenceOfProducts + error;
}

vec3 DifferenceOfProducts(vec3 a, vec3 b, vec3 c, vec3 d)
{
	vec3 cd = c * d;
	vec3 differenceOfProducts = FMA(a, b, -cd);
	vec3 error = FMA(-c, d, cd);
	return differenceOfProducts + error;
}

vec3 Cross(vec3 v, vec3 w)
{
	return vec3(DifferenceOfProducts(v.y, w.z, v.z, w.y), DifferenceOfProducts(v.z, w.x, v.x, w.z), DifferenceOfProducts(v.x, w.y, v.y, w.x));
}


float Lerp(float x, float a, float b)
{
	return (1 - x) * a + x * b;
}

float Copysign(float a, float b)
{
	if (b > 0)
		return a;
	return -a;
}

/*
	矩阵列主序
*/
vec3 MulPoint(mat4 m, vec3 p)
{
	float xp = m[0][0] * p.x + m[1][0] * p.y + m[2][0] * p.z + m[3][0];
	float yp = m[0][1] * p.x + m[1][1] * p.y + m[2][1] * p.z + m[3][1];
	float zp = m[0][2] * p.x + m[1][2] * p.y + m[2][2] * p.z + m[3][2];
	float wp = m[0][3] * p.x + m[1][3] * p.y + m[2][3] * p.z + m[3][3];
	if (wp == 1)
		return vec3(xp, yp, zp);
	else
		return vec3(xp, yp, zp) / wp;
}

vec3 MulVector(mat4 m, vec3 v)
{
	float xp = m[0][0] * v.x + m[1][0]* v.y + m[2][0] * v.z;
	float yp = m[0][1] * v.x + m[1][1]* v.y + m[2][1] * v.z;
	float zp = m[0][2] * v.x + m[1][2]* v.y + m[2][2] * v.z;
	return vec3(xp, yp, zp);
}

float SmoothStep(float x, float a, float b)
{
	if (a == b)
		return (x < a) ? 0.f : 1.f;
	float t = clamp((x - a) / (b - a), 0, 1);
	return t * t * (3 - 2 * t);
}


#endif