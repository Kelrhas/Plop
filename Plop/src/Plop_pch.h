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

#include <Constants.h>
#include <Types.h>
#include <Debug/Log.h>

#define COMPILE_ASSERT(b, m) static_assert(b, m);
#define ASSERT_COMPILE_TIME(b, m) COMPILE_ASSERT(b, m);

#define BREAK() __debugbreak()
#define ASSERT(action, ...) {bool b = (action); if(!b){ Log::Assert(""); BREAK(); }}
#define VERIFY(action, ...) {bool b = (action); if(!b){ Log::Assert(""); BREAK(); }}

