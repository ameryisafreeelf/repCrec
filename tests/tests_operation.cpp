#include "catch.hpp"

#include "Operation.cpp"

TEST_CASE( "Create dump Operation" ) {
	Operation op(Operator::dump);
	REQUIRE( op.getOperator() == Operator::dump );
}

TEST_CASE( "Create begin, end, beginRO Operations" ) {
	Operation op(Operator::begin, "T6");
	Operation op1(Operator::end, "T7");
	Operation op2(Operator::beginRO, "T8");

	SECTION( "Get Operator and transaction for begin" ) {
		REQUIRE( op.getOperator() == Operator::begin );
		REQUIRE( op.getTransactionName() == "T6" );
	}

	SECTION( "Get Operator and transaction for end" ) {
		REQUIRE( op1.getOperator() == Operator::end );
		REQUIRE( op1.getTransactionName() == "T7" );
	}

	SECTION( "Get Operator and transaction for beginRO" ) {
		REQUIRE_FALSE( op2.getOperator() == Operator::begin );
		REQUIRE( op2.getTransactionName() == "T8" );
	}

}

TEST_CASE( "Create fail, recover Operations" ) {
	Operation op(Operator::fail, 6);
	Operation op2(Operator::recover, 7); 

	SECTION( "Get operator and Site Index for fail" ) {
		REQUIRE( op.getOperator() == Operator::fail );
		REQUIRE( op.getSiteIndex() == 6 );		
	}

	SECTION( "Get operator and Site Index for recover" ) {
		REQUIRE( op2.getOperator() == Operator::recover );
		REQUIRE( op2.getSiteIndex() == 7 );		
	}

}

TEST_CASE( "Can get Data index from Operation" ) {
	Operation op(Operator::R, "T1", "X2");
	REQUIRE( op.getDataName() == "X2" );
	REQUIRE( op.getDataIndex() == 2 );
}