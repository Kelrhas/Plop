#pragma once

namespace Plop
{
	struct GUID
	{
		using base_t = uint64_t;

		GUID();
		GUID(base_t);
		explicit operator base_t() const { return m_uID; }
		bool operator==(const GUID &_o) const { return m_uID == _o.m_uID; }

	private:
		base_t m_uID;
	};
}

namespace std
{
	template<>
	struct hash<Plop::GUID>
	{
		size_t operator()(const Plop::GUID &_guid) const
		{
			return hash<Plop::GUID::base_t>()((Plop::GUID::base_t)_guid);
		}
	};
}

namespace fmt
{
	template <>
	struct formatter<Plop::GUID> : formatter<Plop::GUID::base_t>
	{
		constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin())
		{
			auto it = ctx.begin(), end = ctx.end();
			if (it != end && *it != '}') throw format_error("invalid format");
			return it;
		}


		template <typename FormatContext>
		auto format(const Plop::GUID &guid, FormatContext &ctx)
		{
			return formatter<Plop::GUID::base_t>::format((Plop::GUID::base_t)guid, ctx);
		}
	};
}