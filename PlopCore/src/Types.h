#pragma once

#include <string>
#include <filesystem>

using String		= std::string;
using StringView	= std::string_view;
using StringVec		= std::vector<String>;
using StringPath	= std::filesystem::path;
using U8			= uint8_t;
using U16			= uint16_t;
using U32			= uint32_t;
using U64			= uint64_t;
using S8			= int8_t;
using S16			= int16_t;
using S32			= int32_t;
using S64			= int64_t;

enum class VisitorFlow : uint8_t
{
	CONTINUE,
	BREAK
};

template<typename Visitor, typename ...Args>
concept VisitorConcept = requires(Visitor v, Args...args) { {v(args...)} -> std::same_as<VisitorFlow>; };

