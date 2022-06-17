#pragma once


#ifdef _DEBUG
//#define ENABLE_MEMORY_TRACKING // not working because of static init order
#elif defined _RELEASE
#define ENABLE_MEMORY_TRACKING
#elif defined _MASTER
#endif


#define USE_COMPONENT_MGR
#define USE_ENTITY_HANDLE
//#define USE_CONSTANT_RANDOM

#ifdef _MASTER
constexpr bool USE_EDITOR = false;
#else
constexpr bool USE_EDITOR = true;
#endif