#pragma once

namespace Plop
{
#pragma region CTOR & DTOR

	template<typename value_type, size_t Size>
	CircleBuffer<value_type, Size>::~CircleBuffer()
	{
		// call destructors
		for (size_type i = 0; i < m_Size; ++i)
			m_Data[i * sizeof( value_type )].~value_type();
	}

#pragma endregion

#pragma region Modifiers

	template<typename value_type, size_t Size>
	template<typename...Args>
	void CircleBuffer<value_type, Size>::emplace_back( Args...args )
	{
		if (m_LastInsertedIndex >= Size - 1)
			m_LastInsertedIndex = 0;
		else
			m_LastInsertedIndex++;

		if (m_Size < Size)
			++m_Size;
		else
			m_Data[m_LastInsertedIndex * sizeof( value_type )].~value_type(); // element already created so we need to call its destructor

		new(m_Data[m_LastInsertedIndex * sizeof( value_type )])(std::forward<Args>( args )...);
	}

	template<typename value_type, size_t Size>
	void CircleBuffer<value_type, Size>::push_back( const value_type& _Element )
	{
		if (m_LastInsertedIndex >= Size - 1)
			m_LastInsertedIndex = 0;
		else
			m_LastInsertedIndex++;

		if (m_Size < Size)
			++m_Size;
		else
			m_Data[m_LastInsertedIndex * sizeof( value_type )].~value_type(); // element already created so we need to call its destructor

		new(m_Data[m_LastInsertedIndex * sizeof( value_type )])(_Element);
	}

	template<typename value_type, size_t Size>
	void CircleBuffer<value_type, Size>::push_back( value_type&& _Element )
	{
		if (m_LastInsertedIndex >= Size - 1)
			m_LastInsertedIndex = 0;
		else
			m_LastInsertedIndex++;
	
		if (m_Size < Size)
			++m_Size;
		else
			m_Data[m_LastInsertedIndex * sizeof( value_type )].~value_type(); // element already created so we need to call its destructor
	
		new(m_Data[m_LastInsertedIndex * sizeof( value_type )])(std::move( _Element ));
	}

	template<typename value_type, size_t Size>
	void CircleBuffer<value_type, Size>::clear()
	{
		for (size_type i = 0; i < m_Size; ++i)
			m_Data[i * sizeof( value_type )].~value_type();

		m_LastInsertedIndex = Size;

		m_Size = 0;
	}

#pragma endregion

#pragma region Getters

	template<typename value_type, size_t Size>
	typename CircleBuffer<value_type, Size>::reference CircleBuffer<value_type, Size>::front()
	{
		if (m_Size == Size) // every slot has been filled
		{
			if (m_LastInsertedIndex < Size - 1)
				return m_Data[m_LastInsertedIndex + 1];
			else
				return m_Data[0];
		}
		else
		{
			ASSERT( m_Size > 0, "Array is empty" );
			return m_Data[0];
		}
	}

	template<typename value_type, size_t Size>
	typename CircleBuffer<value_type, Size>::const_reference CircleBuffer<value_type, Size>::front() const
	{
		if (m_Size == Size) // every slot has been filled
		{
			if (m_LastInsertedIndex < Size - 1)
				return m_Data[m_LastInsertedIndex + 1];
			else
				return m_Data[0];
		}
		else
		{
			ASSERT(m_Size > 0, "Array is empty" );
			return m_Data[0];
		}
	}

	template<typename value_type, size_t Size>
	typename CircleBuffer<value_type, Size>::reference CircleBuffer<value_type, Size>::back()
	{
		ASSERT( m_Size > 0, "Array is empty" );
		return m_Data[m_LastInsertedIndex];
	}

	template<typename value_type, size_t Size>
	typename CircleBuffer<value_type, Size>::const_reference CircleBuffer<value_type, Size>::back() const
	{
		ASSERT( m_Size > 0, "Array is empty" );
		return m_Data[m_LastInsertedIndex];
	}

#pragma endregion

#pragma region operators

	template<typename value_type, size_t Size>
	typename CircleBuffer<value_type, Size>::reference CircleBuffer<value_type, Size>::operator[]( size_type _Index )
	{
		ASSERT( _Index < m_Size, "Index {0} is outside the array of {1} elements", _Index, m_Size );
		if (m_Size == Size) // every slot has been filled
		{
			return m_Data[(m_LastInsertedIndex + 1 + _Index) % Size];
		}
		else
			return m_Data[_Index];
	}

	template<typename value_type, size_t Size>
	typename CircleBuffer<value_type, Size>::const_reference CircleBuffer<value_type, Size>::operator[]( size_type _Index ) const
	{
		ASSERT( _Index < m_Size, "Index {0} is outside the array of {1} elements", _Index, m_Size );
		if (m_Size == Size) // every slot has been filled
		{
			return m_Data[(m_LastInsertedIndex + 1 + _Index) % Size];
		}
		else
			return m_Data[_Index];
	}

#pragma endregion

}
