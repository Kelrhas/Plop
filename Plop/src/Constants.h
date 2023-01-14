#pragma once


#ifdef _DEBUG
	#pragma optimize("gt", on)
#endif
//#define GLM_FORCE_MESSAGES
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#ifdef _DEBUG
	#pragma optimize("", off)
#endif

constexpr glm::vec2 VEC2_0(0, 0);
constexpr glm::vec2 VEC2_1(1, 1);
constexpr glm::vec3 VEC3_0(0, 0, 0);
constexpr glm::vec3 VEC3_1(1, 1, 1);
constexpr glm::vec3 VEC3_X(1, 0, 0);
constexpr glm::vec3 VEC3_Y(0, 1, 0);
constexpr glm::vec3 VEC3_Z(0, 0, 1);

#define VEC3_RIGHT	 VEC3_X
#define VEC3_UP		 VEC3_Y
#define VEC3_FORWARD -VEC3_Z


constexpr glm::vec4 COLOR_BLACK(0, 0, 0, 1);
constexpr glm::vec4 COLOR_RED(1, 0, 0, 1);
constexpr glm::vec4 COLOR_GREEN(0, 1, 0, 1);
constexpr glm::vec4 COLOR_BLUE(0, 0, 1, 1);
constexpr glm::vec4 COLOR_WHITE(1, 1, 1, 1);
constexpr glm::vec4 COLOR_GREY64(0.25f, 0.25f, 0.25f, 1);
constexpr glm::vec4 COLOR_GREY128(0.5f, 0.5f, 0.5f, 1);
constexpr glm::vec4 COLOR_GREY192(0.75f, 0.75f, 0.75f, 1);


constexpr glm::vec4 ColorFromU32(const uint8_t _uR, const uint8_t _uG, const uint8_t _uB, const uint8_t _uA = 255)
{
	return glm::vec4(_uR / 255.f, _uG / 255.f, _uB / 255.f, _uA / 255.f);
}

constexpr glm::vec4 ColorFromU32ARGB(const uint32_t _uColor)
{
	return glm::vec4(((_uColor & 0x00FF0000) >> 16) / 255.f,
					 ((_uColor & 0x0000FF00) >> 8) / 255.f,
					 ((_uColor & 0x000000FF) >> 0) / 255.f,
					 ((_uColor & 0xFF000000) >> 24) / 255.f);
}

constexpr glm::vec4 ColorFromU32RGB(const uint32_t _uColor, const uint8_t _uA = 255)
{
	if ((_uColor & 0xFF000000) != 0)
		printf("Alpha present, use ColorFromU32ARGB instead");

	return glm::vec4(((_uColor & 0xFF0000) >> 16) / 255.f,
					 ((_uColor & 0x00FF00) >> 8) / 255.f,
					 ((_uColor & 0x0000FF) >> 0) / 255.f,
					 _uA / 255.f);
}
