#pragma once


#ifdef _DEBUG
#define ENABLE_MEMORY_TRACKING
#elif defined _RELEASE
#define ENABLE_MEMORY_TRACKING
#elif defined _MASTER
#endif
