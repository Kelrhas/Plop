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


#define BREAK() __debugbreak()
#define ASSERT(action, ...) {bool b = (action); if(!b){ Log::Assert(""); BREAK(); }}
#define VERIFY(action, ...) {bool b = (action); if(!b){ Log::Assert(""); BREAK(); }}