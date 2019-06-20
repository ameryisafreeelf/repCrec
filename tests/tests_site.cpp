#include "catch.hpp"

#include "Site.cpp"

TEST_CASE( "Create a site" ) {
	Site* s1 = new Site(1, Normal);
	REQUIRE( s1 -> getSiteName() == "S1" );
	REQUIRE( s1 -> getSiteIndex() == 1 );
	REQUIRE( s1 -> getSiteStatus() == siteStatus::Normal ); 
	//s1-> dump();
	
	SECTION( "Instantiation of site has 10 in dataSet and empty Lock Manager" ) {
		REQUIRE(s1 -> getNumOfLocks() == 0);
		REQUIRE(s1 -> getNumOfDataItems() == 10);
		REQUIRE(s1 -> getDataItem("x2") -> getFullName() == "x2.1" );
	}
	
	//s1 currently has all even-indexed Data items
	SECTION( "Add Data item to site and ensure that its fullName is updated " ) {
		REQUIRE(s1 -> getNumOfDataItems() == 10);
		Data* testData = new Data("x1");
		Data* testData2 = new Data("x3");
		s1 -> addDataToSite(testData);
		s1 -> addDataToSite(testData2);		
		REQUIRE(s1 -> getNumOfDataItems() == 12);		
		REQUIRE(s1 -> getDataItem("x1") -> getFullName() == "x1.1");
		REQUIRE(s1 -> getDataItem("x3") -> getFullName() == "x3.1");	
		//s1 -> dump();	
	}

} 

TEST_CASE( "Add a Data Item to dataSet and update its value" ) {
	Site* s1 = new Site(1, Normal);	
	Data *d1 = new Data("x1");
	d1 -> setValue("T1", 5);
	s1 -> addDataToSite(d1);
	REQUIRE( s1 -> getNumOfDataItems() == 11 );
	REQUIRE( s1 -> getDataItem("x1") -> getValue() == 5);
	//s1 -> dump();
	
	SECTION( "Adding an existing Data Item does not change dataSet's size" ) {
		// s1 already has x2
		Data* d2 = new Data("x2");
		d2 -> setValue("T1", 10);
		s1 -> addDataToSite(d2);
		REQUIRE( s1 -> getNumOfDataItems() == 11 );
		// x2's value should still be default INT_MIN
		REQUIRE_FALSE( s1 -> getDataItem("x2") -> getValue() == 10 );
		REQUIRE( s1 -> getDataItem("x2") -> getFullName() == "x2.1");
	}

	SECTION( "Adding a new Data item changes dataSet's size" ) {
		Data* d2 = new Data("x3");
		d2 -> setValue("T1", 10);
		s1 -> addDataToSite(d2);
		REQUIRE( s1 -> getNumOfDataItems() == 12 );
		REQUIRE_FALSE( s1 -> getDataItem("x2") -> getValue() == 10 );
		// Here's how you update the value of a Data item in the Site
		s1-> getDataItem("x3") -> setValue("T1", 10); 
		REQUIRE( s1 -> getDataItem("x3") -> getValue() == 10 );
		REQUIRE( s1 -> getDataItem("x3") -> getFullName() == "x3.1");
		//s1 -> dump();
	}

}

TEST_CASE( "Remove Data Item from DataManager" ) {
	Site* s1 = new Site(1, Normal);	
	Data* d1 = new Data("x1");
	d1 -> setValue("T1", 10);
	s1 -> addDataToSite(d1);

	SECTION( "Can remove pointer to Data Item from DataManager" ) {
		s1 -> removeDataFromSite("x1");
		REQUIRE( s1 -> getNumOfDataItems() == 10 );
		// Below should be false since we removed x1
		REQUIRE_FALSE( s1 -> getDataItem("x1") );
		s1 -> removeDataFromSite("x2");
		REQUIRE( s1 -> getNumOfDataItems() == 9 );
	}

	SECTION( "Removing a Data item that is not in the DataManager does nothing" ) {
		s1 -> removeDataFromSite("x25");
		REQUIRE( s1 -> getNumOfDataItems() == 11 );
	}

	SECTION( "Removing a Data item means it will not appear in dump" ) {
		s1 -> removeDataFromSite("x1");
		// x1 was added to dataSet, but it should not appear in dump
		//s1 -> dump();
	}
}

TEST_CASE( "Updating a Data item causes it to appear in dump" ) {
	Site* s1 = new Site(1, Normal);	
	s1 -> writeToDataOnSite("x2", "T1", 5);
	REQUIRE( s1 -> getValueOfDataOnSite("x2") == 5 );	
	//s1 -> dump();
}

TEST_CASE( "Issuing Lock on Data item that's not in dataSet does nothing" ) {
	Site* s1 = new Site(1, Normal);	
	s1 -> issueLock("T1", "x25", LockType::RO);
	REQUIRE_FALSE( s1 -> getDataItem("x25") );
}

TEST_CASE( "Can create Lock on data item and change its lock status" ) {
	Site* s1 = new Site(1, Normal);	
	s1 -> issueLock("T1", "x2", LockType::RO);
	REQUIRE( s1 -> getNumOfLocks() == 1 );
	REQUIRE( s1 -> getDataItem("x2") -> isReadLocked() );
	REQUIRE_FALSE( s1 -> getDataItem("x2") -> isWriteLocked() );	
	REQUIRE( s1 -> isReadLockOnData("x2") );
	REQUIRE_FALSE( s1 -> isWriteLockOnData("x2") );

	SECTION( "Can upgrade RO-locked Data by issuing RW_lock on it from same T" ) {
		s1 -> issueLock("T1", "x2", LockType::RW);
		// The number of locks should not change
		REQUIRE( s1 -> getNumOfLocks() == 1 );
		REQUIRE_FALSE( s1 -> getDataItem("x2") -> isReadLocked() );
		REQUIRE( s1 -> getDataItem("x2") -> isWriteLocked() );	
		REQUIRE_FALSE( s1 -> isReadLockOnData("x2") );
		REQUIRE( s1 -> isWriteLockOnData("x2") );
	}

	SECTION( "Issuing RO-lock on RO-locked Data item from same Tdoes nothing" ) {
		s1 -> issueLock("T1", "x2", LockType::RO);
		REQUIRE( s1 -> getNumOfLocks() == 1 );
	}

}

TEST_CASE( "Issuing a Lock from another T on locked Data item" ) {
	Site* s1 = new Site(1, Normal);	
	s1 -> issueLock("T1", "x2", LockType::RO);	
	s1 -> issueLock("T2", "x2", LockType::RO);
	s1 -> issueLock("T3", "x2", LockType::RW);	
	REQUIRE( s1 -> getNumOfLocks() == 1 );
	REQUIRE( s1 -> findLock("x2") -> getLockType() == LockType::RO );
}


TEST_CASE( "Searching the empty lockSet and lockedDataSet returns false" ) {
	Site* s1 = new Site(1, Normal);		
	Data* testData = new Data("x1");
	testData -> setValue("T1", 5);
	Lock* testLock = new Lock("T1", testData, RO);
	REQUIRE( s1 -> isReadLockOnData("x1") == false);
	REQUIRE( s1 -> isWriteLockOnData("x1") == false);	

	SECTION( "Issuing a lock on a Data item does nothing if it's not in the Site" ) {
		s1 -> issueLock("T1", testData -> getName(), RO);
		REQUIRE( s1 -> isReadLockOnData( testData -> getName() ) == false);		
		REQUIRE( s1 -> isWriteLockOnData( testData -> getName() ) == false);		
	}	

	SECTION( "Insert a Lock into the Lock Manager" ) {	
		s1 -> addDataToSite(testData);
		s1 -> issueLock("T1", testData -> getName(), RO);
		REQUIRE( s1 -> getNumOfLocks() == 1 );
		REQUIRE( s1 -> isReadLockOnData(testData -> getName()) == true );
	}

	SECTION( "Can remove a Lock in the Lock Manager" ) {
		s1 -> issueUnlock(testData);
		REQUIRE( s1 -> isReadLockOnData(testData -> getName()) == false );
		REQUIRE( s1 -> isWriteLockOnData(testData -> getName()) == false );
	}

	SECTION( "Searching for a Lock that we never inserted returns nothing" ) {
		REQUIRE( s1 -> isReadLockOnData("x5") == false );
		REQUIRE( s1 -> getNumOfLocks() == 0 );
	}

}

TEST_CASE( "Dump site prints written values of Data items" ) {
	Site* s1 = new Site(1);
	Data* d1 = new Data("x1");
	Data* d2 = new Data("x2");
	d1 -> setValue("T1", 10);
	d2 -> setValue("T2", 20);
	s1 -> addDataToSite(d1);
	s1 -> addDataToSite(d2);
	s1 -> dump();
}

TEST_CASE( "Failing site will empty the lockSet and set siteStatus to fail" ) {
	Site* s1 = new Site(1, Normal);
	s1 -> fail();
	REQUIRE( s1 -> getSiteStatus() == siteStatus::Fail );
	REQUIRE( s1 -> getNumOfLocks() == 0 );

	SECTION( "Recovery sets data items to isReplicated" ) {
		s1 -> recover();
		REQUIRE( s1 -> getSiteStatus() == siteStatus::Normal );
		REQUIRE( s1 -> getDataItem("x2") -> getIsReplicated() );
	}
}