#pragma once

namespace Plop
{
	/* DynamicArray is a simple version of std::vector
	 */
	template<class T>
	class DynamicArray final
	{
	public:
		// Types
		using value_type = T;
		using size_type = size_t;
		using pointer = value_type * ;
		using const_pointer = const value_type * ;
		using reference = value_type & ;
		using const_reference = const value_type &;

		// CTOR & DTOR
		DynamicArray() = default;
		DynamicArray( const DynamicArray<value_type>& _Other );
		DynamicArray( DynamicArray<value_type>&& _Other );
		~DynamicArray();


		// Modifiers
		template<typename...Args>
		void						emplace_back( Args...args );
		void						push_back( const value_type& _Element );
		void						push_back( value_type&& _Element );
		void						pop_back();
		void						clear();
		void						reserve( size_type _Capacity );
		void						set_granularity( size_type _Granularity);

		// Getters
		bool						empty() const { return m_Size == 0; }
		size_type					size() const { return m_Size; }
		size_type					capacity() const { return m_Capacity; }
		size_type					granularity()  const { return m_Granularity; }
		reference					front();
		const_reference				front() const;
		reference					back();
		const_reference				back() const;
		pointer						data();
		const_pointer				data() const;

		// operators
		reference					operator[](size_type _Index );
		const_reference				operator[]( size_type _Index ) const;
		DynamicArray<value_type>&	operator=( const DynamicArray<value_type>& _Other );
		DynamicArray<value_type>&	operator=( DynamicArray<value_type>&& _Other );

		// iterators
		struct iterator
		{
			friend class DynamicArray<value_type>;

			iterator& operator=( const iterator& )		{ dataRef = _Other.dataRef; index = _Other.index; return *this; }
			bool operator==( const iterator& _Other )	{ ASSERT( &dataRef == &_Other.dataRef, "Iterators are not for the same DynamicArray" ); return index == _Other.index; }
			bool operator!=( const iterator& _Other )	{ return !(*this == _Other); }
			iterator& operator++()						{ index++; return *this; }						// pre-increment
			iterator operator++(int)					{ iterator it = *this; index++; return it; }	// post-increment
			reference operator*() const					{ return dataRef[index]; }
			pointer operator->() const					{ return &dataRef[index]; }

		private:
			iterator( DynamicArray<value_type>& _vecRef, size_type _Index = 0 ) : dataRef( _vecRef ), index( _Index ) {}

			DynamicArray<value_type>& dataRef;
			size_type index = 0;
		};

		iterator		begin() { return iterator( *this, 0 ); }
		//const_iterator	begin() const;
		iterator		end() { return iterator( *this, m_Size ); }
		//const_iterator	end() const;
		//iterator		rbegin();
		//const_iterator	rbegin() const;
		//iterator		rend();
		//const_iterator	rend() const;

	private:
		void			Reallocate( size_type _NewCapacity );
		size_type		ComputeNewCapacity();


	private:
		value_type*		m_pData = nullptr;
		size_type		m_Size = 0;
		size_type		m_Capacity = 0;
		size_type		m_Granularity = 0;	// 0 meaning doubling the capacity each allocation


	/* Possible enhancements :
	 *	- check the pointerness of value_type to call ctor and dtor with std::is_pointer
		- reverse iterators
	 */

	};
}

#include "DynamicArray.inl"
