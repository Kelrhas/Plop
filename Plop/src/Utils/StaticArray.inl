#pragma once

namespace Plop
{
#pragma region CTOR & DTOR

	template<class value_type, size_t Size>
	StaticArray<value_type, Size>::StaticArray( const StaticArray<value_type, Size>& _Other )
	{
		for (size_type i = 0; i < Size; ++i)
			m_Data[i] = _Other[i];
	}

	template<class value_type, size_t Size>
	StaticArray<value_type, Size>::StaticArray( StaticArray<value_type, Size>&& _Other )
	{
		for (size_type i = 0; i < Size; ++i)
			m_Data[i] = std::move( _Other[i] );
	}

#pragma endregion

#pragma region Modifiers

	template<class value_type, size_t Size>
	void StaticArray<value_type, Size>::fill( value_type&& _Element )
	{
		for (size_type i = 0; i < Size; ++i)
			m_Data[i] = std::move( _Element );
	}

#pragma endregion

#pragma region Getters

	template<class value_type, size_t Size>
	typename StaticArray<value_type, Size>::reference StaticArray<value_type, Size>::front()
	{
		return m_Data[0];
	}

	template<class value_type, size_t Size>
	typename StaticArray<value_type, Size>::const_reference StaticArray<value_type, Size>::front() const
	{
		return m_Data[0];
	}

	template<class value_type, size_t Size>
	typename StaticArray<value_type, Size>::reference StaticArray<value_type, Size>::back()
	{
		return m_Data[Size - 1];
	}

	template<class value_type, size_t Size>
	typename StaticArray<value_type, Size>::const_reference StaticArray<value_type, Size>::back() const
	{
		return m_Data[Size - 1];
	}

	template<class value_type, size_t Size>
	typename StaticArray<value_type, Size>::pointer StaticArray<value_type, Size>::data()
	{
		return m_Data;
	}

	template<class value_type, size_t Size>
	typename StaticArray<value_type, Size>::const_pointer StaticArray<value_type, Size>::data() const
	{
		return m_Data;
	}

#pragma endregion

#pragma region operators

	template<class value_type, size_t Size>
	typename StaticArray<value_type, Size>::reference StaticArray<value_type, Size>::operator[]( size_type _Index )
	{
		ASSERT( _Index >= 0 && _Index < Size, "Index {0} is outside the array of {1} elements", _Index, Size );
		return m_Data[_Index];
	}

	template<class value_type, size_t Size>
	typename StaticArray<value_type, Size>::const_reference StaticArray<value_type, Size>::operator[]( size_type _Index ) const
	{
		ASSERT( _Index >= 0 && _Index < Size, "Index {0} is outside the array of {1} elements", _Index, Size );
		return m_Data[_Index];
	}
	
	template<class value_type, size_t Size>
	StaticArray<value_type, Size>& StaticArray<value_type, Size>::operator=( const StaticArray<value_type, Size>& _Other )
	{
		for (size_type i = 0; i < Size; ++i)
			m_Data[i] = _Other[i];

		return *this;
	}

	template<class value_type, size_t Size>
	StaticArray<value_type, Size>& StaticArray<value_type, Size>::operator=( StaticArray<value_type, Size>&& _Other )
	{
		for (size_type i = 0; i < Size; ++i)
			m_Data[i] = std::move( _Other[i] );

		return *this;
	}

#pragma endregion

}