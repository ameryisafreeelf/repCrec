#include <string>

#include "catch.hpp"

#include "Lock.cpp"

TEST_CASE( "Create a Lock" ) {
	// This simulates the actual workflow of locking and writing
	Data* myData = new Data("X1");	
	myData -> setValue("T1", 5);	
	Lock* myLock = new Lock("T1", myData, RW);	
	REQUIRE( myData -> isAnyLocked() == true );
	REQUIRE( myData -> isReadLocked() == false );
	REQUIRE( myData -> isWriteLocked() == true );
	REQUIRE( myLock -> getLockType() == RW );

	SECTION( "Can access Data item from Lock" ) {
		REQUIRE( myLock -> getData() -> getName() == "X1" );
		REQUIRE( myLock -> getData() -> getValue() == 5 );
	}
}

TEST_CASE( "Can create a second lock on a writeLocked Data item" ) {
	Data* myData = new Data("X1");	
	Lock* myLock = new Lock("T1", myData, RO);
	Lock* myLock2 = new Lock("T1", myData, RW);		
	REQUIRE( myData -> isAnyLocked() == true );
	REQUIRE( myData -> isReadLocked() == false );	
	REQUIRE( myData -> isWriteLocked() == true );
	REQUIRE( myLock -> getData() -> getName() == "X1" );
	REQUIRE( myLock2 -> getData() -> getName() == "X1" );
	REQUIRE( myLock2 -> getData() -> getName() == "X1" );	
}

TEST_CASE( "Issuing a writeLock on a readLocked Data item from same T promotes it" ) {
	Data* myData = new Data("X1", 5);
	myData -> setValue("T1", 5);
	Lock* myLock = new Lock("T1", myData, RO);
	Lock* myLock2 = new Lock("T1", myData, RW);
	REQUIRE( myLock -> getData() -> isReadLocked() == false );
	REQUIRE( myLock2 -> getData() -> isWriteLocked() == true );
}

TEST_CASE( "Issuing a readLock on a readLocked Data item does nothing " ) {
	Data* myData = new Data("X1");
	Lock* myLock = new Lock("T1", myData, RO);
	Lock* myLock2 = new Lock("T1", myData, RO);
	REQUIRE( myLock -> getData() -> isReadLocked() == true );
	REQUIRE( myLock -> getData() -> isWriteLocked() == false );
}