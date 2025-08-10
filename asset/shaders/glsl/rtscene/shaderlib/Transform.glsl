#ifndef TRANSFORM_H
#define TRANSFORM_H

mat4 RotateX(float theta) 
{
    float sinTheta = sin(radians(theta));
    float cosTheta = cos(radians(theta));
    mat4 m = mat4(1,            0,          0,          0,
                 0,             cosTheta,   -sinTheta,  0,
                 0,             sinTheta,   cosTheta,   0,
                 0,             0,          0,          1);
    return transpose(m);
}

mat4 RotateY(float theta) 
{
    float sinTheta = sin(radians(theta));
    float cosTheta = cos(radians(theta));
    mat4 m = mat4(cosTheta,     0,              sinTheta,    0,
                  0,            1,              0,           0,
                  -sinTheta,    0,              cosTheta,    0,
                  0,            0,              0,           1);
    return transpose(m);
}

mat4 RotateZ(float theta) 
{
    float sinTheta = sin(radians(theta));
    float cosTheta = cos(radians(theta));
    mat4 m = mat4(cosTheta,     -sinTheta,      0,      0,
                  sinTheta,     cosTheta,       0,      0,
                  0,            0,              1,      0,
                  0,            0,              0,      1);
    return transpose(m);
}

vec3 Mul(mat4 m, vec3 d)
{
    vec3 d2;
    d2.x = m[0][0] * d.x + m[1][0] * d.y + m[2][0] * d.z;
	d2.y = m[0][1] * d.x + m[1][1] * d.y + m[2][1] * d.z;
	d2.z = m[0][2] * d.x + m[1][2] * d.y + m[2][2] * d.z;
    return d2;
}

#endif



 





