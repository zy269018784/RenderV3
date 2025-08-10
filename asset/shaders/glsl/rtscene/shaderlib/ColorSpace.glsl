#ifndef COLORSPACE_H
#define COLORSPACE_H


#define sat(x) clamp(x, 0.0, 1.0)

/// Color conversions ///
// https://gist.github.com/sugi-cho/6a01cae436acddd72bdf
vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0,-1/3.,2/3.,-1);
    vec4 p = mix(vec4(c.bg,K.wz), vec4(c.gb,K.xy), step(c.b,c.g));
    vec4 q = mix(vec4(p.xyw,c.r), vec4(c.r,p.yzx), step(p.x,c.r));
    float d = q.x - min(q.w,q.y);
    float e = 1.0e-10;
    return vec3(
      abs(q.z+(q.w-q.y)/(6*d+e)), 
      d/(q.x+e), 
      q.x
    );
}

vec3 rgb2hsv(float r, float g, float b) {
    return rgb2hsv(vec3(r, g, b));
}

vec3 hsv2rgb(vec3 c) {
    return c.z*mix(vec3(1), sat(abs(fract(vec3(1,2/3.,1/3.)+c.x)*6-3)-1),c.y);
}

vec3 hsv2rgb(float h,float s,float v) {
    return hsv2rgb(vec3(h,s,v));
} 

// from http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
vec3 colorFromKelvin(float temperature) // photographic temperature values are between 15 to 150
{
    float r, g, b;
    if(temperature <= 66.0) {
        r = 1.0;
        g = sat((99.4708025861 * log(temperature) - 161.1195681661) / 255.0);
        if(temperature < 19.0)
            b = 0.0;
        else
            b = sat((138.5177312231 * log(temperature - 10.0) - 305.0447927307) / 255.0);
    }
    else {
        r = sat((329.698727446 / 255.0) * pow(temperature - 60.0, -0.1332047592));
        g = sat((288.1221695283 / 255.0) * pow(temperature - 60.0, -0.0755148492));
        b = 1.0;
    }
    return vec3(r, g, b);
}

//ACEScg  
const mat3 ACEScg_to_Linear_Matrix = mat3(
     1.7050796, -0.6211893, -0.0837696,
    -0.1302562,  1.1408018, -0.0105478,
    -0.0240033, -0.1289687,  1.1530260
);

const mat3 Linear_to_ACEScg_Matrix = mat3(
    0.6130974, 0.3395232, 0.0473793,
    0.0701937, 0.9163539, 0.0134524,
    0.0206156, 0.1095699, 0.8698145
);

// Linear → ACEScg 
vec3 ACEScg_to_Linear(vec3 aces) { 
    return aces * ACEScg_to_Linear_Matrix;  //note ：向量左乘右乘顺序不能反了
} 

// ACEScg → Linear
vec3 Linear_to_ACEScg(vec3 lrgb) { 
    return lrgb * Linear_to_ACEScg_Matrix ;
}


// sRGB → Linear
vec3 sRGB_to_Linear(vec3 srgb) {
  return mix(srgb / 12.92, pow((srgb + 0.055)/1.055, vec3(2.4)), step(0.04045, srgb));
}

// Linear → sRGB 
vec3 Linear_to_sRGB(vec3 lrgb){ 
    return mix( lrgb * 12.92, pow(lrgb, vec3(1.0/2.4)) * 1.055 - 0.055, step(0.0031308, lrgb) ); 
}


#endif