#pragma once

namespace Plop
{
	template<typename value_type, size_t Size>
	class CircleBuffer
	{
		static_assert(Size > 1, "A CircleBuffer must have more than one element");

		using size_type = size_t;
		using pointer = value_type * ;
		using const_pointer = const value_type *;
		using reference = value_type & ;
		using const_reference = const value_type &;

	public:
		// CTOR & DTOR
		CircleBuffer() = default;
		CircleBuffer( const CircleBuffer<value_type, Size>& ) = delete; // TODO
		CircleBuffer( CircleBuffer<value_type, Size>&& ) = delete; // TODO
		~CircleBuffer();

		// Modifiers
		template<typename...Args>
		void						emplace_back( Args&&...args );
		void						push_back( const value_type& _Element );
		void						push_back( value_type&& _Element );
		void						clear();

		// Getters
		bool						empty() const { return m_Size == 0; }
		size_type					size() const { return m_Size; }
		reference					front();				// retrieves the first element inserted
		const_reference				front() const;			// retrieves the first element inserted
		reference					back();					// retrieves the last element inserted
		const_reference				back() const;			// retrieves the last element inserted

		// operators
		reference					operator[]( size_type );
		const_reference				operator[]( size_type ) const;

		// iterators


	private:
		uint8_t			m_Data[Size * sizeof( value_type )]; // just a block of stack memory
		size_type		m_Size = 0; // the number of elements actually inserted
		size_type		m_LastInsertedIndex = Size-1;
	};
}

#include "CircleBuffer.inl"
