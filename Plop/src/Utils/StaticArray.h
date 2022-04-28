#pragma once

namespace Plop
{
	/* StaticArray is a simple version of std::array
	 */
	template<class T, size_t Size>
	class StaticArray final
	{
		static_assert(Size > 0, "A StaticArray must have a positive Size");

	public:
		// Types
		using value_type = T;
		using size_type = size_t;
		using pointer = value_type * ;
		using const_pointer = const value_type *;
		using reference = value_type & ;
		using const_reference = const value_type &;

		// CTOR & DTOR
		StaticArray() = default;
		StaticArray( const StaticArray<value_type, Size>& _Other );
		StaticArray( StaticArray<value_type, Size>&& _Other );
		~StaticArray() = default;

		// Modifiers
		void						fill( const value_type& _Element );
		
		// Getters
		constexpr bool				empty() const { return Size == 0; }
		constexpr size_type			size() const { return Size; }
		reference					front();
		const_reference				front() const;
		reference					back();
		const_reference				back() const;
		pointer						data();
		const_pointer				data() const;

		// operators
		reference					operator[]( size_type _Index );
		const_reference				operator[]( size_type _Index ) const;
		StaticArray<value_type,Size>&	operator=( const StaticArray<value_type, Size>& _Other );
		StaticArray<value_type,Size>&	operator=( StaticArray<value_type, Size>&& _Other );

		// iterators
		struct iterator
		{
			friend class StaticArray<value_type, Size>;
			
			iterator& operator=( const iterator& )			{ dataRef = _Other.dataRef; index = _Other.index; return *this; }
			bool operator==( const iterator& _Other ) const	{ ASSERT( &dataRef == &_Other.dataRef, "Iterators are not for the same StaticArray" ); return index == _Other.index; }
			bool operator!=( const iterator& _Other ) const	{ return !(*this == _Other); }
			iterator& operator++()							{ index++; return *this; }							// pre-increment
			iterator operator++(int)						{ iterator it = *this; index++; return it; }		// post-increment
			reference operator*() const						{ return dataRef[index]; }
			pointer operator->() const						{ return &dataRef[index]; }

		private:
			iterator( StaticArray<value_type,Size>& _vecRef, size_type _Index = 0 ) : dataRef( _vecRef ), index( _Index ) {}

			StaticArray<value_type,Size>& dataRef;
			size_type index = 0;
		};

		struct const_iterator
		{
			friend class StaticArray<value_type, Size>;

			const_iterator& operator=(const const_iterator& _Other) { dataRef = _Other.dataRef; index = _Other.index; return *this; }
			bool operator==(const const_iterator& _Other) const { ASSERT(&dataRef == &_Other.dataRef, "Iterators are not for the same StaticArray"); return index == _Other.index; }
			bool operator!=(const const_iterator& _Other) const { return !(*this == _Other); }
			const_iterator operator++() const { return const_iterator(dataRef, index+1); }				// pre-increment
			const_iterator operator++(int) const { const_iterator it = *this; index++; return it; }		// post-increment
			const_reference operator*() const { return dataRef[index]; }
			const_pointer operator->() const { return &dataRef[index]; }

		private:
			const_iterator(const StaticArray<value_type, Size>& _vecRef, size_type _Index = 0) : dataRef(_vecRef), index(_Index) {}

			const StaticArray<value_type, Size>& dataRef;
			size_type index = 0;
		};

		iterator		begin() { return iterator( *this, 0 ); }
		const_iterator	begin() const { return const_iterator( *this, 0 ); }
		iterator		end() { return iterator( *this, Size ); }
		const_iterator	end() const { return const_iterator( *this, Size ); }

	private:
		value_type		m_pData[Size];
	};
}

#include "StaticArray.inl"
