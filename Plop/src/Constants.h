#pragma once


#ifdef _DEBUG
#pragma optimize( "gt", on )
//#define GLM_FORCE_MESSAGES
#endif
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#ifdef _DEBUG
#pragma optimize( "", off )
#endif

const glm::vec3 VEC3_0(0, 0, 0);
const glm::vec3 VEC3_1(1, 1, 1);
const glm::vec3 VEC3_X(1, 0, 0);
const glm::vec3 VEC3_Y(0, 1, 0);
const glm::vec3 VEC3_Z(0, 0, 1);

#define VEC3_RIGHT VEC3_X
#define VEC3_UP VEC3_Y
#define VEC3_FORWARD -VEC3_Z