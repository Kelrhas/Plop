#pragma once


#ifdef _DEBUG
//#define ENABLE_MEMORY_TRACKING // not working because of static init order
#elif defined _RELEASE
#define ENABLE_MEMORY_TRACKING
#elif defined _MASTER
#endif

#define ENTT_ID_TYPE std::uint64_t

//#define USE_CONSTANT_RANDOM

#ifdef _MASTER
constexpr bool USE_EDITOR = false;
#else
constexpr bool USE_EDITOR = true;
#endif