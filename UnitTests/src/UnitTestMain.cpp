#include "UnitTestMain.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#define ASSERT(action, ...) {bool b=(action); REQUIRE_FALSE (!b);}

#include <Utils/StaticArray.h>
#include <Utils/DynamicArray.h>

using namespace Plop;

size_t TestObject::memAllocated = 0;



TEMPLATE_TEST_CASE( "StaticArray", "[data structure][StaticArray]", float, int, std::string, TestObject )
{
	StaticArray<TestType, 10> arr;
	arr.fill( TestType() );

	REQUIRE( arr.size() == 10 );
	REQUIRE( arr.empty() == false );

	SECTION( "Accessing" )
	{
		REQUIRE( arr[0] == TestType() );
		REQUIRE( arr.front() == arr[0] );
		REQUIRE( arr.back() == arr[arr.size() -1] );
		REQUIRE( *arr.data() == arr.front() );
	}

	const StaticArray<TestType, 10> arrConst = arr;

	SECTION( "Checking the copy" )
	{
		REQUIRE( arrConst[0] == TestType() );
		REQUIRE( arrConst.front() == arrConst[0] );
		REQUIRE( arrConst.back() == arrConst[arrConst.size() - 1] );
		REQUIRE( *arrConst.data() == arrConst.front() );
	}

	SECTION( "Iterators" )
	{
		int i = 0;
		for (TestType& o : arr)
		{
			REQUIRE( &arr[i] == &o );
			i++;
		}
		REQUIRE( i == arr.size() );

		i = 0;
		for (auto& it = arr.begin(); it != arr.end(); ++it)
		{
			REQUIRE( &arr[i] == &(*it) );
			i++;
		}
		REQUIRE( i == arr.size() );
	}
	/*
	SECTION( "Iterators on const" )
	{
		int i = 0;
		for (TestType& o : arrConst)
		{
			REQUIRE( &arrConst[i] == &o );
			i++;
		}
		REQUIRE( i == arrConst.size() );

		i = 0;
		for (auto& it = arrConst.begin(); it != arrConst.end(); ++it)
		{
			REQUIRE( &arrConst[i] == &(*it) );
			i++;
		}
		REQUIRE( i == arrConst.size() );
	}
	*/
}

TEMPLATE_PRODUCT_TEST_CASE( "Vector", "[data structure][DynamicArray]", (std::vector/*, DynamicArray*/), (float, int, /*std::string,*/ TestObject) )
{
	TestType vec;

	REQUIRE( vec.size() == 0 );
	REQUIRE( vec.capacity() == 0 );
	REQUIRE( vec.empty() == true );

	vec.push_back( TestType::value_type() );
	REQUIRE( vec.size() == 1 );

	SECTION( "Adding" )
	{
		TestType::value_type t;
		vec.push_back( t );
		vec.emplace_back( (TestType::value_type)10 );

		REQUIRE( vec.size() == 3 );
		if(typeid(TestType::value_type) == typeid(TestObject))
			REQUIRE( (int)*(TestObject*)&vec[2] == 10 );
	}

	SECTION( "Accessing" )
	{
		REQUIRE( vec[0] == TestType::value_type() );
		REQUIRE( vec.front() == vec[0] );
		REQUIRE( vec.back() == vec[vec.size() - 1] );
		REQUIRE( *vec.data() == vec.front() );
	}

	for (int i = 0; i < 5; ++i)
	{
		vec.emplace_back( (TestType::value_type)i );
	}

	SECTION( "Iterators" )
	{
		REQUIRE( vec.size() == 6 );

		int i = 0;
		for (TestType::value_type& o : vec)
		{
			REQUIRE( &vec[i] == &o );
			i++;
		}
		REQUIRE( i == vec.size() );

		i = 0;
		for (auto& it = vec.begin(); it != vec.end(); ++it)
		{
			REQUIRE( &vec[i] == &(*it) );
			i++;
		}
		REQUIRE( i == vec.size() );
	}

	const TestType vecCopy = vec;
	SECTION( "Copying vector" )
	{
		REQUIRE( vecCopy.size() == vec.size() );
		for (int i=0; i < vec.size(); ++i)
			REQUIRE( vec[i] == vecCopy[i] );
	}

	SECTION( "Iterators on const" )
	{
		REQUIRE( vecCopy.size() == 6 );

		int i = 0;
		for (TestType::value_type o : vecCopy)
		{
			REQUIRE( vecCopy[i] == o );
			i++;
		}
		REQUIRE( i == vecCopy.size() );

		i = 0;
		for (auto& it = vecCopy.begin(); it != vecCopy.end(); ++it)
		{
			REQUIRE( &vecCopy[i] == &(*it) );
			i++;
		}
		REQUIRE( i == vecCopy.size() );
	}
}