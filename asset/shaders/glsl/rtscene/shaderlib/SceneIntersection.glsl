#ifndef SceneIntersection_H
#define SceneIntersection_H

/*
    void traceRayEXT(accelerationStructureEXT topLevel,
                   uint rayFlags,
                   uint cullMask,
                   uint sbtRecordOffset,
                   uint sbtRecordStride,
                   uint missIndex,
                   vec3 origin,
                   float Tmin,
                   vec3 direction,
                   float Tmax,
                   int payload
                   );
*/
void IntersectScene(vec3 o, vec3 d, float min, float max, accelerationStructureEXT topLevelAS)
{
    uint rayFlags = gl_RayFlagsNoneEXT;
    uint cullMask = 0xff;
    uint sbtRecordOffset = 0;
    uint sbtRecordStride = 0;
    uint missIndex = 0;
    vec3 origin = o;
    float Tmin = min;
    vec3 direction = d;
    float Tmax = max;
    traceRayEXT(topLevelAS, rayFlags, cullMask, sbtRecordOffset, sbtRecordStride, missIndex, origin, Tmin, direction, Tmax, 0);   
}

/*
    true:  没有被遮挡
	false: 被遮挡
    两点之间是否有遮挡
*/
bool Unoccluded(vec3 o, vec3 p, accelerationStructureEXT AS)
{
	vec3 d = normalize(p - o);
    uint cullMask = 0xff;
    uint sbtRecordOffset = 0;
    uint sbtRecordStride = 0;
    uint missIndex = 1;
    float tMin = 0.0002;
	float tMax = length(p - o);
	uint flags = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT;
	shadowed = true;
	traceRayEXT(AS, flags, cullMask, sbtRecordOffset, sbtRecordStride, missIndex, o, tMin, d, tMax, 2); 
	return !shadowed;
}

bool Unoccluded(vec3 o, vec3 p)
{
    return Unoccluded(o, p, topLevelAS);
}

#endif