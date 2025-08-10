#ifndef LIGHT_SAMPLER_H
#define LIGHT_SAMPLER_H

int UniformLightSampler_Sample(int size, float u)
{
    int LightIndex = int(u * float(size));
    LightIndex = Clamp(LightIndex, 0, size - 1);
    return LightIndex;
}

float UniformLightSampler_PMF(int size)
{
    if (size == 0)
        return 0;
    return 1.f / float(size);
}

#endif