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

#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/format-inl.h>
#include <fmt/color.h>

#include <Constants.h>
#include <Types.h>
#include <Debug/Debug.h>
#include <Debug/Log.h>

#define COMPILE_ASSERT(b, m) static_assert(b, m);
#define ASSERT_COMPILE_TIME(b, m) COMPILE_ASSERT(b, m);

#ifndef _MASTER
#define BREAK() __debugbreak()
#define ASSERT(action, ...) {bool b = (action); if(!b){ Plop::Log::Assert(__VA_ARGS__); BREAK(); }}
#define VERIFY(action, ...) {bool b = (action); if(!b){ Plop::Log::Assert(__VA_ARGS__); BREAK(); }}
#define VERIFY_NO_MSG(action) {bool b = (action); if(!b){ BREAK(); }}
#define EXCEPTION(action, ...) {bool b = (action); if(!b){ Plop::Log::Error(__VA_ARGS__); BREAK(); }}
#else
#define BREAK() do{}while(0)
#define ASSERT(action, ...) do{}while(0)
#define VERIFY(action, ...) do{(action)}while(0)
#define EXCEPTION(action, ...) {bool b = (action); if(!b){ Log::Error(__VA_ARGS__); Log::Flush(); }} // we flush to be sure that we have the log in the file before a possible crash
#endif

