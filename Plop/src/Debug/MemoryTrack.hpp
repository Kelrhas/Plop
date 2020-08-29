#include "Plop_pch.h"


#ifdef ENABLE_MEMORY_TRACKING
template<class T>
struct MAllocator : public std::allocator<T>
{
	MAllocator() {}
	template<typename U> struct rebind { typedef MAllocator<U> other; };
	template<typename U> MAllocator( MAllocator<U> const& u ) :std::allocator<T>( u ) {}

	typename std::allocator_traits<std::allocator<T>>::pointer allocate( typename std::allocator_traits<std::allocator<T>>::size_type n, std::allocator_traits<std::allocator<void>>::const_pointer = 0 )
	{
		return (std::allocator_traits<std::allocator<T>>::pointer)malloc( n * sizeof( T ) );
	}

	void deallocate( typename std::allocator_traits<std::allocator<T>>::pointer p, typename std::allocator_traits<std::allocator<T>>::size_type )
	{
		std::free( p );
	}
};

struct AllocationLocation
{
	const char* pFile = nullptr;
	const size_t uLine = 0;
};

namespace std
{
	template<>
	struct hash<AllocationLocation>
	{
		size_t operator()( const AllocationLocation& loc ) const
		{
			return hash<const char*>()(loc.pFile) ^ hash<size_t>()(loc.uLine);
		}
	};
	template<>
	struct equal_to<AllocationLocation>
	{
		constexpr bool operator()( const AllocationLocation& _Left, const AllocationLocation& _Right ) const {
			if (_Left.uLine != _Right.uLine)
				return false;
			if (_Left.pFile == _Right.pFile)
				return true;

			// in case __FILE__ did not return the same pointer or the path was given by hand and somehow is a different pointer
			return strcmp( _Left.pFile, _Right.pFile ) == 0;
		}
	};
}

struct GlobalAllocationInfo
{
	size_t uFrameNumberAllocation = 0;
	size_t uFrameAllocation = 0;

	size_t uCurrentNumberAllocation = 0;
	size_t uCurrentAllocation = 0;

	size_t uTotalNumberAllocation = 0;
	size_t uTotalAllocation = 0;

	using Key = void*;
	using T = size_t;

	std::unordered_map < void*, size_t, std::hash<void*>, std::equal_to<void*>, MAllocator< std::pair<const Key, size_t> >> mapAllocations;
	std::unordered_map < void*, AllocationLocation, std::hash<void*>, std::equal_to<void*>, MAllocator< std::pair<const Key, AllocationLocation> >> mapAllocationsCalls;
	std::unordered_map < AllocationLocation, size_t, std::hash<AllocationLocation>, std::equal_to<AllocationLocation>, MAllocator< std::pair<const AllocationLocation, size_t> >> mapAllocationsSizes;

	void NewAllocation( void* _Ptr, size_t _Size, const char* _pFile, const size_t _Line )
	{
		uFrameNumberAllocation++;
		uFrameAllocation += _Size;
		uCurrentNumberAllocation++;
		uCurrentAllocation += _Size;
		uTotalNumberAllocation++;
		uTotalAllocation += _Size;

		AllocationLocation location{ _pFile, _Line };
		mapAllocations.insert_or_assign( _Ptr, _Size );
		mapAllocationsCalls.emplace( _Ptr, location );

		if (mapAllocationsSizes.find( location ) != mapAllocationsSizes.end())
			mapAllocationsSizes[location] += _Size;
		else
			mapAllocationsSizes[location] = _Size;
	}

	void DeleteAllocation( void* _Ptr )
	{
		if (mapAllocations.find( _Ptr ) != mapAllocations.end()) // we may have cleared the maps
		{
			size_t uSize = mapAllocations[_Ptr];
			AllocationLocation location = mapAllocationsCalls[_Ptr];
			mapAllocations.erase( _Ptr );
			mapAllocationsCalls.erase( _Ptr );
			mapAllocationsSizes[location] -= uSize;

			uCurrentNumberAllocation--;
			uCurrentAllocation -= uSize;
		}
	}

	void ResetFrameStats()
	{
		uFrameNumberAllocation = 0;
		uFrameAllocation = 0;
	}
};

static GlobalAllocationInfo s_GlobalAllocation;

void* operator new(size_t _Size)
{
	return operator new(_Size, "", 0);
}

void* operator new(size_t _Size, const char* _pFile, const size_t _Line)
{
	void* ptr = malloc( _Size );
	s_GlobalAllocation.NewAllocation( ptr, _Size, _pFile, _Line );
	return ptr;
}

void* operator new[]( size_t _Size )
{
	return operator new[]( _Size, "", 0 );
}

void* operator new[]( size_t _Size, const char* _pFile, const size_t _Line )
{
	void* ptr = malloc( _Size );
	s_GlobalAllocation.NewAllocation( ptr, _Size, _pFile, _Line );
	return ptr;
}

void operator delete(void* _Ptr)
{
	s_GlobalAllocation.DeleteAllocation( _Ptr );
	free( _Ptr );
}

void operator delete(void* _Ptr, const char* _pFile, const size_t _Line)
{
	s_GlobalAllocation.DeleteAllocation( _Ptr );
	free( _Ptr );
}

void operator delete[]( void* _Ptr )
{
	s_GlobalAllocation.DeleteAllocation( _Ptr );
	free( _Ptr );
}

void operator delete[]( void* _Ptr, const char* _pFile, const size_t _Line )
{
	s_GlobalAllocation.DeleteAllocation( _Ptr );
	free( _Ptr );
}
#endif // ENABLE_MEMORY_TRACKING