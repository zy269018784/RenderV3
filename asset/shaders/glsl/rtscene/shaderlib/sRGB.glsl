#ifndef SRGB_H
#define SRGB_H

float to_sRGB(float rgb_color)
{
	const float a = 0.055f;
	if (rgb_color < 0.0031308)
		return 12.92 * rgb_color;
	else
		return (1.0 + a) * pow(rgb_color, 1.0 / 2.4) - a;
}

#endif