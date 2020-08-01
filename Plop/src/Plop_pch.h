#pragma once

#include <memory>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <iostream>

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <Windows.h>

#include <Types.h>
#include <Debug/Log.h>

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

#define COMPILE_ASSERT(b, m) static_assert(b, m);
#define ASSERT_COMPILE_TIME(b, m) COMPILE_ASSERT(b, m);

#define BREAK() __debugbreak()
#define ASSERT(action, ...) {bool b = (action); if(!b){ Log::Assert(""); BREAK(); }}
#define VERIFY(action, ...) {bool b = (action); if(!b){ Log::Assert(""); BREAK(); }}


const glm::vec3 VEC3_0(0, 0, 0);
const glm::vec3 VEC3_1(1, 1, 1);
const glm::vec3 VEC3_X(1, 0, 0);
const glm::vec3 VEC3_Y(0, 1, 0);
const glm::vec3 VEC3_Z(0, 0, 1);

#define VEC3_RIGHT VEC3_X
#define VEC3_UP VEC3_Y
#define VEC3_FORWARD -VEC3_Z