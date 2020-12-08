#pragma once

#include <algorithm> 
#include <cctype>
#include <locale>

// https://stackoverflow.com/a/217605

namespace Utils
{
	// Trim from start (in place)
	static inline void TrimLeft( std::string& s )
	{
		s.erase( s.begin(), std::find_if( s.begin(), s.end(), []( unsigned char ch ) {
			return !std::isspace( ch );
			} ) );
	}

	// Trim from end (in place)
	static inline void TrimRight( std::string& s )
	{
		s.erase( std::find_if( s.rbegin(), s.rend(), []( unsigned char ch ) {
			return !std::isspace( ch );
			} ).base(), s.end() );
	}

	// Trim from both ends (in place)
	static inline void TrimSpaces( std::string& s )
	{
		TrimLeft( s );
		TrimRight( s );
	}

}
