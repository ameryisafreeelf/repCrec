//#include <string>
#include <vector>

#include "catch.hpp"

#include "Driver.cpp"

TEST_CASE( "Test main" ) {

	REQUIRE( DRIVER::opsVector.size() == 8 );

} 