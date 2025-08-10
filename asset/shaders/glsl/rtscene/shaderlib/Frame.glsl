#ifndef FRAME_H
#define FRAME_H
struct Frame
{
	vec3 x;
	vec3 y;
	vec3 z;
};


vec3 ToLocal(Frame frame, vec3 n)
{
	return vec3(dot(frame.x, n), dot(frame.y, n), dot(frame.z, n));
}

vec3 FromLocal(Frame frame, vec3 n)
{
	return frame.x * n.x + frame.y * n.y + frame.z * n.z;
}

void CoordinateSystem(vec3 v1, inout vec3 v2, inout vec3 v3) 
{
	float sign1 = sign(v1.z) > 0.f ? 1.f : -1;
	float a = -1 / (sign1 + v1.z);
	float b = v1.x * v1.y * a;
	v2 = vec3(1 + sign1 * v1.x * v1.x * a, sign1 * b, -sign1 * v1.x);
	v3 = vec3(b, sign1 + v1.y * v1.y * a, -v1.y);
}

Frame FromX(vec3 x)
{
	vec3 y, z;
	CoordinateSystem(x, y, z);
	
	Frame frame;
	frame.x = x;
	frame.y = y;
	frame.z = z;
	return frame;
}

Frame FromY(vec3 y)
{
	vec3 x, z;
	CoordinateSystem(y, z, x);
	
	Frame frame;
	frame.x = x;
	frame.y = y;
	frame.z = z;
	return frame;
}

Frame FromZ(vec3 z)
{
	vec3 x, y;
	CoordinateSystem(z, x, y);
	
	Frame frame;
	frame.x = x;
	frame.y = y;
	frame.z = z;
	return frame;
}

Frame FromXZ(vec3 x, vec3 z)
{
	Frame frame;
	frame.x = x;
	frame.y = cross(z, x);
	frame.z = z;
	return frame;
}

Frame FromXY(vec3 x, vec3 y)
{
	Frame frame;
	frame.x = x;
	frame.y = y;
	frame.z = cross(x, y);
	return frame;
}
#endif