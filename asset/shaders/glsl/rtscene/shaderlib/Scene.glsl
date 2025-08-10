#ifndef SCENE_H
#define SCENE_H
struct Scene
{
    accelerationStructureEXT    SceneAS;
};


void IntersectScene(Scene sc, Ray ray, float min, float max)
{
    traceRayEXT(sc.SceneAS, gl_RayFlagsOpaqueEXT, 0xff, 0, 0, 0, ray.o, min, ray.d, max, 0);
	//traceRayEXT(sc.SceneAS,             // 场景加速结构
    //            gl_RayFlagsOpaqueEXT,   //
    //            0xff,                   //
    //            0,                      //
    //            0,                      //
    //            0,                      //  
    //            ray.o,                  // 射线起点
    //            min,                    // 最小距离
    //            ray.d,                  // 射线方向
    //            max,                    // 最大距离
    //            0);
}

#endif