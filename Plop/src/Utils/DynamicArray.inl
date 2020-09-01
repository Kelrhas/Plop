
namespace Plop
{
#pragma region CTOR & DTOR

	template<class value_type>
	DynamicArray<value_type>::DynamicArray( const DynamicArray<value_type>& _Other )
	{
		Reallocate( _Other.m_Capacity );

		m_Size = _Other.m_Size;
		m_Granularity = _Other.m_Granularity;

		for (size_type i = 0; i < m_Size; ++i)
			m_pData[i] = _Other[i];
	}

	template<class value_type>
	DynamicArray<value_type>::DynamicArray( DynamicArray<value_type>&& _Other )
	{
		// move storage
		m_pData = _Other.m_pData;
		m_Capacity = _Other.m_Capacity;
		m_Size = _Other.m_Size;
		m_Granularity = _Other.m_Granularity;

		_Other.m_pData = nullptr;
		_Other.m_Capacity = 0;
		_Other.m_Size = 0;
		_Other.m_Granularity = 0;
	}

	template<class value_type>
	DynamicArray<value_type>::~DynamicArray()
	{
		for (size_type i = 0; i < m_Size; ++i)
			m_pData[i].~value_type();

		operator delete[]( m_pData, m_Capacity * sizeof( value_type ) );
	}

#pragma endregion

#pragma region Modifiers

	template<class value_type>
	template<typename...Args>
	void DynamicArray<value_type>::emplace_back( Args...args )
	{
		if (m_Size >= m_Capacity)
			Reallocate( ComputeNewCapacity() );

		new(&m_pData[m_Size++]) value_type( std::forward<Args>( args )... );
	}

	template<class value_type>
	void DynamicArray<value_type>::push_back( const value_type& _Element )
	{
		if (m_Size >= m_Capacity)
			Reallocate( ComputeNewCapacity() );

		new(&m_pData[m_Size++]) value_type( _Element );
	}

	template<class value_type>
	void DynamicArray<value_type>::push_back( value_type&& _Element )
	{
		if (m_Size >= m_Capacity)
			Reallocate( ComputeNewCapacity() );

		new(&m_pData[m_Size++]) value_type( std::move( _Element ) );
	}

	template<class value_type>
	void DynamicArray<value_type>::pop_back()
	{
		ASSERT( m_Size, "Array is empty" );
		m_pData[m_Size--].~value_type();
	}

	template<class value_type>
	void DynamicArray<value_type>::clear()
	{
		for (size_type i = 0; i < m_Size; ++i)
			m_pData[i].~value_type();
		m_Size = 0;
	}

	template<class value_type>
	void DynamicArray<value_type>::reserve( size_type _Capacity )
	{
		ASSERT( _Capacity >= m_Capacity, "Shrinking not handled yet" );
		if(_Capacity > m_Capacity)
			Reallocate( _Capacity );
	}

	template<class value_type>
	void DynamicArray<value_type>::set_granularity( size_type _Granularity )
	{
		m_Granularity = _Granularity;
	}

#pragma endregion

#pragma region Getters

	template<class value_type>
	typename DynamicArray<value_type>::reference DynamicArray<value_type>::front()
	{
		ASSERT( m_Size, "Array is empty" );
		return m_pData[0];
	}

	template<class value_type>
	typename DynamicArray<value_type>::const_reference DynamicArray<value_type>::front() const
	{
		ASSERT( m_Size, "Array is empty" );
		return m_pData[0];
	}

	template<class value_type>
	typename DynamicArray<value_type>::reference DynamicArray<value_type>::back()
	{
		ASSERT( m_Size, "Array is empty" );
		return m_pData[m_Size-1];
	}

	template<class value_type>
	typename DynamicArray<value_type>::const_reference DynamicArray<value_type>::back() const
	{
		ASSERT( m_Size, "Array is empty" );
		return m_pData[m_Size - 1];
	}

	template<class value_type>
	typename DynamicArray<value_type>::pointer DynamicArray<value_type>::data()
	{
		return m_pData;
	}

	template<class value_type>
	typename DynamicArray<value_type>::const_pointer DynamicArray<value_type>::data() const
	{
		return m_pData;
	}

#pragma endregion

#pragma region operators

	template<class value_type>
	typename DynamicArray<value_type>::reference DynamicArray<value_type>::operator[]( size_type _Index )
	{
		ASSERT( _Index >= 0 && _Index < m_Size, "Index {0} is outside the array of {1} elements", _Index, m_Size );
		return m_pData[_Index];
	}

	template<class value_type>
	typename DynamicArray<value_type>::const_reference DynamicArray<value_type>::operator[]( size_type _Index ) const
	{
		ASSERT( _Index >= 0 && _Index < m_Size, "Index {0} is outside the array of {1} elements", _Index, m_Size );
		return m_pData[_Index];
	}


	template<class value_type>
	DynamicArray<value_type>& DynamicArray<value_type>::operator=( const DynamicArray<value_type>& _Other )
	{
		// delete current storage
		for (size_type i = 0; i < m_Size; ++i)
			m_pData[i].~value_type();
		m_Size = 0;

		Reallocate( _Other.m_Capacity );

		m_Size = _Other.m_Size;
		m_Granularity = _Other.m_Granularity;

		for (size_type i = 0; i < m_Size; ++i)
			m_pData[i] = _Other[i];

		return *this;
	}

	template<class value_type>
	DynamicArray<value_type>& DynamicArray<value_type>::operator=( DynamicArray<value_type>&& _Other )
	{
		// delete current storage
		for (size_type i = 0; i < m_Size; ++i)
			m_pData[i].~value_type();
		operator delete[]( m_pData, m_Capacity * sizeof( value_type ) );

		// move storage
		m_pData = _Other.m_pData;
		m_Capacity = _Other.m_Capacity;
		m_Size = _Other.m_Size;
		m_Granularity = _Other.m_Granularity;

		_Other.m_pData = nullptr;
		_Other.m_Capacity = 0;
		_Other.m_Size = 0;
		_Other.m_Granularity = 0;

		return *this;
	}

#pragma endregion

#pragma region private

	template<class value_type>
	void DynamicArray<value_type>::Reallocate( size_type _NewCapacity )
	{
		ASSERT( _NewCapacity > m_Capacity, "Shrinking not handled yet" );

		value_type* pNewData = (value_type*)operator new(_NewCapacity * sizeof( value_type ));
		
		if (m_pData)
		{
			for (size_type i = 0; i < m_Size; ++i)
			{
				pNewData[i] = std::move( m_pData[i] );
				m_pData[i].~value_type();
			}

			operator delete[]( m_pData, m_Capacity * sizeof( value_type ) );
		}

		m_pData = pNewData;
		m_Capacity = _NewCapacity;
	}

	template<class value_type>
	typename DynamicArray<value_type>::size_type DynamicArray<value_type>::ComputeNewCapacity()
	{
		if (m_Granularity != 0)
			return m_Capacity + m_Granularity; // growing only by the granularity, even for first allocation

		if (m_Capacity == 0)
			return 8; // default capacity

		return m_Capacity * 2; // doubling capacity
	}

#pragma endregion
}
