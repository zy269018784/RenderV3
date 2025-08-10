#pragma once
#include <glm/glm.hpp>


//struct HDRParams {
//    glm::vec4 Bound;           //(w, h, min, max)
//    glm::vec4 MarginalData;    // R: min   G: max B: funcInt  A: func_size = h
//    glm::mat4 RotateY;
//};

struct HDRParams {
	glm::vec4 Bound;
	glm::vec4 MarginalData;
	glm::mat4 RotateY;
	glm::mat4 RotateYInv;
};