#include "catch.hpp"

#include "Data.cpp"

TEST_CASE( "Create a Data item") {	
	Data testData("X1");
	testData.giveFullName(5);
	Data* dataP = &testData;
	REQUIRE(dataP == &testData);

	SECTION( "Get Data item's name" ) {
		REQUIRE( testData.getName() == "X1" );
	}

	SECTION( "Get Data item's value" ) {
		REQUIRE( testData.getValue() == INT_MIN );
	}
	
	SECTION( "Data item's last committed value is INT_MIN by default" ) {
		REQUIRE( testData.getLastCommittedValue() == INT_MIN );
	}
	
	SECTION( "Update Data item's name" ) {
		testData.modifyName("X2");	
		REQUIRE( testData.getName() == "X2" );
	}

	SECTION( "Give Data item a full name that indicates it's in Site 2" ) {
		testData.giveFullName(11);
		REQUIRE( testData.getFullName() == "X1.11" );	
	}
}

TEST_CASE ("Update a Data item's value") {
	Data testData("X1");
	testData.giveFullName(5);

	SECTION( "Update a Data item's value " ) {
		testData.setValue("T4", 10);
		REQUIRE( testData.getValue() == 10);
	}

	SECTION( "Update a Data item's value twice " ) {
		testData.setValue("T4", 10);
		REQUIRE( testData.getValue() == 10);
		testData.setValue("T5", 20);
		REQUIRE( testData.getValue() == 20);
		testData.printEntireWriteHistory();
	}

	SECTION( "Commit a value" ) {
		testData.setValue("T4", 10);
		testData.setLastCommittedValue("T4");
		REQUIRE( testData.getLastCommittedValue() == 10 );
		testData.setValue("T5", 50);
		REQUIRE( testData.getLastCommittedValue() == 10 );
		testData.setLastCommittedValue("T1");
		REQUIRE( testData.getLastCommittedValue() == 10 );
		testData.setLastCommittedValue("T5");
		REQUIRE( testData.getLastCommittedValue() == 50 );
	}
}

TEST_CASE( "Test Lock functionality" ) {
	Data testData("X1");

	SECTION( "Verify that Data item is unlocked by default ") {
		REQUIRE_FALSE( testData.isAnyLocked() );
		REQUIRE_FALSE( testData.isReadLocked() );
		REQUIRE_FALSE( testData.isWriteLocked() );
	}

	SECTION( "Change Data item to locked, then unlock it" ) {
		testData.setToReadLocked("T1");
		REQUIRE( testData.isReadLocked() == true );
		testData.setToUnlocked();
		REQUIRE( testData.isReadLocked() == false );
	}	

	SECTION( "Promote lock status from readLocked to writeLocked" ) {
		testData.setToReadLocked("T1");
		REQUIRE( testData.isReadLocked() == true );
		testData.promoteLockStatus();
		REQUIRE( testData.isWriteLocked() == true );
	}	

}

TEST_CASE( "Commit a value to a Data item" ) {
	Data testData("X1");
	testData.setValue("T1", 5);
	testData.setLastCommittedValue("T1");
	REQUIRE( testData.getLastCommittedValue() == 5 );	
}

TEST_CASE( "Copy a Data item" ) {
	Data testData("X1");
	testData.setValue("T1", 5);
	Data* testReplicate = testData.replicate(6);
	REQUIRE( testReplicate -> getValue() == 5); 
	REQUIRE( testReplicate -> getIsReplicated() == true);
}
