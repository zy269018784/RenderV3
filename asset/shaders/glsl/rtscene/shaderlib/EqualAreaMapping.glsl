#ifndef EqualAreaMapping_H
#define EqualAreaMapping_H

#include "math.glsl"

vec2 EqualAreaSphereToSquare(vec3 d)
{
    float x = abs(d.x), y = abs(d.y), z = abs(d.z);

    // Compute the radius r
    float r = SafeSqrt(1 - z);  // r = sqrt(1-|z|)

    // Compute the argument to atan (detect a=0 to avoid div-by-zero)
    float a = max(x, y), b = min(x, y);
    b = a == 0 ? 0 : b / a;

    // Polynomial approximation of atan(x)*2/pi, x=b
    // Coefficients for 6th degree minimax approximation of atan(x)*2/pi,
    // x=[0,1].
    const float t1 = 0.406758566246788489601959989e-5;
    const float t2 = 0.636226545274016134946890922156;
    const float t3 = 0.61572017898280213493197203466e-2;
    const float t4 = -0.247333733281268944196501420480;
    const float t5 = 0.881770664775316294736387951347e-1;
    const float t6 = 0.419038818029165735901852432784e-1;
    const float t7 = -0.251390972343483509333252996350e-1;
    //float phi = EvaluatePolynomial(b, t1, t2, t3, t4, t5, t6, t7);
    float phi = b * (b * (b * (b * (b * (b * t7 + t6) + t5) + t4) + t3) + t2) + t1;

    // Extend phi if the input is in the range 45-90 degrees (u<v)
    if (x < y)
        phi = 1 - phi;

    // Find (u,v) based on (r,phi)
    float v = phi * r;
    float u = r - v;

    if (d.z < 0) {
        // southern hemisphere -> mirror u,v
        //Swap(u, v);
        float c;
        c = u;
        u = v;
        v = c;
        u = 1 - u;
        v = 1 - v;
    }

    // Move (u,v) to the correct quadrant based on the signs of (x,y)
    //u = std::copysign(u, d.x);
    //v = std::copysign(v, d.y);  
    u = d.x > 0 ? u : -u;
    v = d.y > 0 ? v : -v;


    // Transform (u,v) from [-1,1] to [0,1]
    return vec2(0.5f * (u + 1), 0.5f * (v + 1));
}

vec3 EqualAreaSquareToSphere(vec2 p) {
    // Transform _p_ to $[-1,1]^2$ and compute absolute values
    float u = 2 * p.x - 1, v = 2 * p.y - 1;
    float up = abs(u), vp = abs(v);

    // Compute radius _r_ as signed distance from diagonal
    float signedDistance = 1 - (up + vp);
    float d = abs(signedDistance);
    float r = 1 - d;

    // Compute angle $\phi$ for square to sphere mapping
    float phi = (r == 0 ? 1 : (vp - up) / r + 1) * Pi / 4;

    // Find $z$ coordinate for spherical direction
    float z = Copysign(1 - Sqr(r), signedDistance);
	

    // Compute $\cos \phi$ and $\sin \phi$ for original quadrant and return vector
    float cosPhi = Copysign(cos(phi), u);
    float sinPhi = Copysign(sin(phi), v);
    return vec3(cosPhi * r * SafeSqrt(2 - Sqr(r)), sinPhi * r * SafeSqrt(2 - Sqr(r)), z);
}

#endif