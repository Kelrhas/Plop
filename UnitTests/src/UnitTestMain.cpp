#include "UnitTestMain.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#define ASSERT(action, ...) {bool b=(action); REQUIRE_FALSE (!b);}

#include <array>
#include <vector>

#include <Utils/StaticArray.h>
#include <Utils/DynamicArray.h>
#include <Utils/CircleBuffer.h>

using namespace Plop;

size_t TestObject::memAllocated = 0;



TEMPLATE_PRODUCT_TEST_CASE_SIG( "StaticArray", "[data structure][StaticArray]", ((class T, size_t Size), T, Size), (std::array, StaticArray), ((float,10), (int,10))/*, std::string, TestObject*/ )
{
	TestType arr;
	arr.fill(TestType::value_type());

	REQUIRE( arr.size() == 10 );
	REQUIRE( arr.empty() == false );

	SECTION( "Accessing" )
	{
		REQUIRE( arr[0] == TestType::value_type() );
		REQUIRE( arr.front() == arr[0] );
		REQUIRE( arr.back() == arr[arr.size() -1] );
		REQUIRE( *arr.data() == arr.front() );
	}

	SECTION( "Checking the copy" )
	{
		TestType arrCopy = arr;
		REQUIRE(arrCopy.size() == arr.size());
		REQUIRE(arrCopy.front() == arr.front());
		REQUIRE(arrCopy.data() != arr.data());
		REQUIRE(arrCopy[0] == TestType::value_type() );
		REQUIRE(arrCopy.front() == arrCopy[0] );
		REQUIRE(arrCopy.back() == arrCopy[arrCopy.size() - 1] );
		REQUIRE( *arrCopy.data() == arrCopy.front() );
	}

	SECTION( "Iterators" )
	{
		int i = 0;
		for (TestType::value_type& o : arr)
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

	SECTION("Iterators on const")
	{
		const TestType arrConst = arr;
		int i = 0;
		for (const TestType::value_type& o : arrConst)
		{
			REQUIRE(&arrConst[i] == &o);
			i++;
		}
		REQUIRE(i == arrConst.size());

		i = 0;
		for (auto& it = arrConst.begin(); it != arrConst.end(); ++it)
		{
			REQUIRE(&arrConst[i] == &(*it));
			i++;
		}
		REQUIRE(i == arrConst.size());
	}

}

TEMPLATE_PRODUCT_TEST_CASE( "Vector", "[data structure][DynamicArray]", (std::vector, DynamicArray), (float, int, /*std::string,*/ TestObject) )
{
	TestType vec;

	REQUIRE( vec.size() == 0 );
	REQUIRE( vec.capacity() == 0 );
	REQUIRE( vec.empty() == true );

	SECTION( "Adding & Removing" )
	{
		{
			TestType::value_type t = TestType::value_type();
			vec.push_back(1);
			t = 2;
			vec.push_back(t);
			t = 3;
			vec.push_back(std::move(t));
		}
		vec.push_back( 4 );
		vec.emplace_back( TestType::value_type(10) );

		REQUIRE( vec.size() == 5 );
		REQUIRE( vec.front() == TestType::value_type(1) );
		REQUIRE( vec.back() == TestType::value_type(10) );

		vec.pop_back();
		REQUIRE( vec.size() == 4 );
		REQUIRE( vec.back() == TestType::value_type(4) );

		vec.clear();
		REQUIRE( vec.size() == 0 );
	}

	vec.push_back( TestType::value_type() );
	REQUIRE( vec.size() == 1 );

	SECTION( "Accessing" )
	{
		REQUIRE( vec[0] == TestType::value_type() );
		REQUIRE(vec.front() == vec[0]);
		vec.push_back(TestType::value_type(2));
		REQUIRE( vec.back() == vec[vec.size() - 1] );
		REQUIRE( *vec.data() == vec.front() );
	}

	vec.clear();
	for (int i = 0; i < 5; ++i)
	{
		vec.emplace_back( TestType::value_type(i) );
	}

	SECTION( "Iterators" )
	{
		REQUIRE( vec.size() == 5 );

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

	SECTION( "Copying vector" )
	{
		const TestType vecCopy = vec;
		REQUIRE( vecCopy.size() == vec.size() );
		for (int i=0; i < vec.size(); ++i)
			REQUIRE( vec[i] == vecCopy[i] );
	}

	SECTION( "Iterators on const" )
	{
		const TestType vecConst = vec;
		REQUIRE(vecConst.size() == 5 );

		int i = 0;
		for (const TestType::value_type &o : vecConst)
		{
			REQUIRE(vecConst[i] == o );
			i++;
		}
		REQUIRE( i == vecConst.size() );

		i = 0;
		for (auto& it = vecConst.begin(); it != vecConst.end(); ++it)
		{
			REQUIRE((void*)&vecConst[i] == (void*)&(*it) );
			i++;
		}
		REQUIRE( i == vecConst.size() );
	}

	SECTION( "Moving vector" )
	{
		size_t count = vec.size();
		TestType vecMove = std::move(vec);
		REQUIRE( vecMove.size() == count );
		REQUIRE( vec.size() == 0 );
		REQUIRE( vec.data() == nullptr );
		REQUIRE( vec.data() != vecMove.data() );
	}
}

TEMPLATE_TEST_CASE( "CircleBuffer", "[data structure][CircleBuffer]", float, int, /*std::string,*/ TestObject)
{
	CircleBuffer<TestType, 4> circle;

	REQUIRE( circle.size() == 0 );
	REQUIRE( circle.empty() == true );

	SECTION( "Adding" )
	{
		circle.push_back(TestType(0));
		circle.push_back(TestType(1));
		circle.push_back(TestType(2));

		REQUIRE(circle.size() == 3);

		for (int i = 0; i < 3; ++i)
			REQUIRE(circle[i] == TestType(i));

		{
			TestType t = TestType(3);
			circle.push_back(t);
			t = 4;
			circle.push_back(std::move(t));
		}
		circle.push_back(TestType(5));
		circle.emplace_back(TestType(6));

		REQUIRE( circle.size() == 4 );
		REQUIRE( circle.front() == TestType(3) );
		REQUIRE( circle.back() == TestType(6) );

		circle.clear();
		REQUIRE( circle.size() == 0 );
	}
}
