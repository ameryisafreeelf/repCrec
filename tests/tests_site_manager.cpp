#include "catch.hpp"

#include "SiteManager.cpp"

TEST_CASE( "Initializing the Site Manager creates 10 Sites" ) {
	SiteManager sm;
	sm.initializeSites();	

	SECTION( "Get available Data items when all Sites are normal" ) {
		Site* s = sm.getSiteByIndex(2);
		Site* s2 = sm.getSiteByIndex(1);
		REQUIRE( s -> getNumOfDataItems() == 12 );
		REQUIRE( s2 -> getNumOfDataItems() == 10 );		
	}
}

TEST_CASE( "Writing to a Data item changes its value at all sites where it lives" ) {
	SiteManager sm;
	sm.initializeSites();

	SECTION( "Write to a Data item on all sites" ) {
		sm.writeToAllSites(2, "x2", "T2", 5);
		REQUIRE( sm.getReadableDataItem(2, "x2") -> getValue() == 5);
	}	
}

TEST_CASE( "Issuing RO Lock on all sites" ) {
	SiteManager sm;
	sm.initializeSites();

	SECTION( "All copies of x2 are RO locked" ) {
		sm.issueLockOnAllSites("x2", "T1", LockType::RO);
		Data* d1 = sm.getSiteByIndex(1) -> getDataItem("x2");
		Data* d2 = sm.getSiteByIndex(2) -> getDataItem("x2"); 
		Data* d9 = sm.getSiteByIndex(9) -> getDataItem("x2"); 
		REQUIRE( d1 -> isReadLocked() == true );
		REQUIRE_FALSE( d1 -> isWriteLocked() == true );
		REQUIRE( d2 -> isReadLocked() == true );
		REQUIRE_FALSE( d1 -> isWriteLocked() == true );
		REQUIRE( d9 -> isReadLocked() == true );
		REQUIRE_FALSE( d9 -> isWriteLocked() == true );
		REQUIRE( d1 -> getLockingTransaction() == "T1" );
		REQUIRE_FALSE( d1 -> getLockingTransaction() == "T2" );
	}

}


TEST_CASE( "Fail and Recover a site via the Site Manager" ) {
	SiteManager sm;
	sm.initializeSites();
	sm.fail(6);
	REQUIRE( sm.getSiteByIndex(6) -> getSiteStatus() == siteStatus::Fail );
	sm.recover(6);
	REQUIRE( sm.getSiteByIndex(6) -> getSiteStatus() == siteStatus::Normal );
}