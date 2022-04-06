#pragma once

#include <bitset>

template<typename Enum>
struct EnumFlags
{
	std::bitset<(size_t)Enum::COUNT> m_bitset;

	EnumFlags()
	{
	}

	EnumFlags( Enum e )
	{
		m_bitset.set( (size_t)e );
	}

	EnumFlags( const EnumFlags<Enum>& e )
	{
		m_bitset = e.m_bitset;
	}

	auto operator=( Enum e )
	{
		m_bitset.reset();
		m_bitset.set( (size_t)e );
		return *this;
	}

	auto operator|=( Enum e )
	{
		m_bitset.set( (size_t)e );
		return *this;
	}

	auto operator&=( Enum e )
	{
		bool b = m_bitset.test( (size_t)e );
		m_bitset.reset();
		m_bitset.set( (size_t)e, b );
		return *this;
	}

	auto operator~() const
	{
		auto self = *this;
		self.m_bitset.flip();
		return self;
	}

	auto operator^=( Enum e )
	{
		m_bitset.flip( (size_t)e );
		return *this;
	}

	EnumFlags<Enum> operator&=( const EnumFlags<Enum>& flags )
	{
		m_bitset &= flags.m_bitset;
		return *this;
	}
	EnumFlags<Enum> operator&( const EnumFlags<Enum>& flags ) const
	{
		EnumFlags<Enum> newFlags = *this;
		return newFlags &= flags;
	}

	EnumFlags<Enum> operator|=( const EnumFlags<Enum>& flags )
	{
		m_bitset |= flags.m_bitset;
		return *this;
	}
	EnumFlags<Enum> operator|( const EnumFlags<Enum>& flags ) const
	{
		EnumFlags<Enum> newFlags = *this;
		return newFlags |= flags;
	}
	EnumFlags<Enum> operator^=( const EnumFlags<Enum>& flags )
	{
		m_bitset ^= flags.m_bitset;
		return *this;
	}
	EnumFlags<Enum> operator^( const EnumFlags<Enum>& flags ) const
	{
		EnumFlags<Enum> newFlags = *this;
		return newFlags ^= flags;
	}

	auto Remove( Enum e )
	{
		m_bitset.reset( (size_t)e );
		return *this;
	}

	bool Has( Enum e )
	{
		return m_bitset.test( (size_t)e );
	}

	bool HasAll( const EnumFlags<Enum>& e )
	{
		return (m_bitset & e.m_bitset) == e.m_bitset;
	}

	bool HasOne( const EnumFlags<Enum>& e )
	{
		return (m_bitset & e.m_bitset) != 0;
	}

};

template<typename Enum>
std::ostream& operator<<( std::ostream& os, const EnumFlags<Enum>& flags )
{
	os << flags.m_bitset.to_string();
	return os;
}
//
//
//#define MACRO_ENUM_FLAGS_OPERATOR(Enum) \
//EnumFlags<Enum> operator|( Enum lhs, Enum rhs ) { \
//	return EnumFlags<Enum>( lhs ) | EnumFlags<Enum>( rhs ); \
//}