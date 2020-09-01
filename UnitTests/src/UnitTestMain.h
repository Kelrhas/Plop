#pragma once

#include <cstring>
#include <iostream>

struct TestObject
{
	TestObject()
	{
		pData = new int;
		*pData = 42;
		pDataArr = new char[256]{ "plop" };
	}

	TestObject(int i)
	{
		pData = new int;
		*pData = i;
		pDataArr = new char[256]{ "plop" };
	}

	TestObject( const TestObject& other )
	{
		pData = new int;
		*pData = *other.pData;
		pDataArr = new char[256]{ 0 };
		strcpy( pDataArr, other.pDataArr );
	}

	TestObject( TestObject&& other )
	{
		pData = other.pData;
		pDataArr = other.pDataArr;

		other.pData = nullptr;
		other.pDataArr = nullptr;
	}

	~TestObject()
	{
		delete pData;
		delete[] pDataArr;
	}
	int* pData = nullptr;
	char* pDataArr = nullptr;

	operator int() const
	{
		return *pData;
	}

	TestObject& operator=( const TestObject& other )
	{
		delete pData;
		delete[] pDataArr;

		pData = new int;
		*pData = *other.pData;
		pDataArr = new char[256]{ 0 };
		strcpy( pDataArr, other.pDataArr );

		return *this;
	}

	TestObject& operator=( TestObject&& other )
	{
		delete pData;
		delete[] pDataArr;

		pData = other.pData;
		pDataArr = other.pDataArr;

		other.pData = nullptr;
		other.pDataArr = nullptr;

		return *this;
	}

	bool operator==( const TestObject& other ) const
	{
		bool b = *pData == *other.pData;
		int comp = _stricmp( pDataArr, other.pDataArr );
		b &= comp == 0;
		return b;
	}

	std::ostream& operator<< ( std::ostream &out ) const
	{
		out << "int*:" << *pData << " char*:" << pDataArr;
		return out;
	}

	/*void* operator new(size_t _Size)
	{
		memAllocated += _Size;
		return malloc( _Size );
	}
	void operator delete(void* ptr, size_t _Size)
	{
		memAllocated -= _Size;
		free( ptr );
	}*/

	static size_t memAllocated;
};