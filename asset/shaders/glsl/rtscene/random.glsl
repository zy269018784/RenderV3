#ifndef RANDOM_H
#define RANDOM_H

/* Copyright (c) 2023, Sascha Willems
 *
 * SPDX-License-Identifier: MIT
 *
 */

// Tiny Encryption Algorithm
// By Fahad Zafar, Marc Olano and Aaron Curtis, see https://www.highperformancegraphics.org/previous/www_2010/media/GPUAlgorithms/HPG2010_GPUAlgorithms_Zafar.pdf
uint tea(uint val0, uint val1)
{
    uint sum = 0;
    uint v0 = val0;
    uint v1 = val1;
    for (uint n = 0; n < 16; n++)
    {
        sum += 0x9E3779B9;
        v0 += ((v1 << 4) + 0xA341316C) ^ (v1 + sum) ^ ((v1 >> 5) + 0xC8013EA4);
        v1 += ((v0 << 4) + 0xAD90777D) ^ (v0 + sum) ^ ((v0 >> 5) + 0x7E95761E);
    }
    return v0;
}

// Linear congruential generator based on the previous RNG state
// See https://en.wikipedia.org/wiki/Linear_congruential_generator
uint lcg(inout uint previous)
{
    const uint multiplier = 1664525u;
    const uint increment = 1013904223u;
    previous   = (multiplier * previous + increment);
    return previous & 0x00FFFFFF;
} 

uint xor_shift_rand(inout uint previous)
{
    previous ^= (previous << 13);
    previous ^= (previous >> 17);
    previous ^= (previous << 5);
    return previous;
}

// Generate a random float in [0, 1) given the previous RNG state
float rnd(inout uint previous)
{
    //return float(xor_shift_rand(previous)) * (1.0 / 4294967296.0);
    return (float(lcg(previous)) / float(0x01000000));
}


// PCG random numbers generator
// Source: "Hash Functions for GPU Rendering" by Jarzynski & Olano
uvec4 pcg4d(uvec4 v) {
	v = v * 1664525u + 1013904223u;
	v.x += v.y * v.w;
	v.y += v.z * v.x;
	v.z += v.x * v.y;
	v.w += v.y * v.z;
	v = v ^ (v >> 16u);
	v.x += v.y * v.w;
	v.y += v.z * v.x;
	v.z += v.x * v.y;
	v.w += v.y * v.z;
	return v;
}

 

// 位反转函数（反转32位整数的二进制位）
uint reverseBits(uint n) {
    n = (n << 16) | (n >> 16);
    n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
    n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
    n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
    n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
    return n;
}

// Hammersley 序列（生成第i个样本的2D坐标）
vec2 hammersley(uint i, uint N) {
    float u = float(i) / float(N);                  // 均匀分布的x
    float v = float(reverseBits(i)) / float(0xFFFFFFFFu); // 位反转的y
    return vec2(u, v);
}

// 生成Halton序列的第i个数（基数为base）
float halton(uint i, uint base) {
    float f = 1.0;
    float r = 0.0;
    while (i > 0) {
        f /= float(base);
        r += f * float(i % base);
        i = uint(float(i) / float(base));
    }
    return r;
}

// 生成2D Halton序列（基数2和3）
vec2 halton23(uint i) {
    return vec2(halton(i, 2), halton(i, 3));
}
 
 

#endif