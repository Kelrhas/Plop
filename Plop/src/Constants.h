#pragma once


#ifdef _DEBUG
#pragma optimize( "gt", on )
#endif
//#define GLM_FORCE_MESSAGES
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#ifdef _DEBUG
#pragma optimize( "", off )
#endif

constexpr glm::vec2 VEC2_0(0, 0);
constexpr glm::vec2 VEC2_1(1, 1);
constexpr glm::vec3 VEC3_0(0, 0, 0);
constexpr glm::vec3 VEC3_1(1, 1, 1);
constexpr glm::vec3 VEC3_X(1, 0, 0);
constexpr glm::vec3 VEC3_Y(0, 1, 0);
constexpr glm::vec3 VEC3_Z(0, 0, 1);

#define VEC3_RIGHT VEC3_X
#define VEC3_UP VEC3_Y
#define VEC3_FORWARD -VEC3_Z


constexpr glm::vec4 COLOR_BLACK	( 0, 0, 0, 1 );
constexpr glm::vec4 COLOR_RED	( 1, 0, 0, 1 );
constexpr glm::vec4 COLOR_GREEN	( 0, 1, 0, 1 );
constexpr glm::vec4 COLOR_BLUE	( 0, 0, 1, 1 );
constexpr glm::vec4 COLOR_WHITE	( 1, 1, 1, 1 );