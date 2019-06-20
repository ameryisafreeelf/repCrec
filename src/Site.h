#ifndef SITE_H
#define SITE_H

#include <iostream>
#include <string>
#include <iterator>
#include <unordered_map>
#include <set>

#include "Lock.cpp"
#include "Data.cpp"
#include "Transaction.cpp"

enum siteStatus {Normal, Fail, Recovering};

class Site {

private:

	// Corresponds to siteName, e.g. S2 has index 2
	int siteIndex;
	string siteName;
	siteStatus status;
	unordered_map< string, Data* > dataSet; 	// key is name of Data item
	unordered_map< string, Lock* > lockSet;		// key is name of Data item

public:

	// The constructor creates all Data elements that will appear at the Site
	Site(int siteIndex, siteStatus status = Normal);

	// Accessors
	string getSiteName();
	int getSiteIndex();
	siteStatus getSiteStatus();
	int getNumOfDataItems();
	// Only for testing
	int getNumOfLocks();
	Data* getDataItem(string dataName);
	// We'll use this in Read
	int getValueOfDataOnSite(string dataName);
	Lock* findLock(string dataName);
	bool isReadLockOnData(string dataName);
	bool isWriteLockOnData(string dataName)
	set<string>* getTransactionsWrittenToSite();
	void dump();

	// Mutators
	void setSiteStatus(siteStatus status);
	void addDataToSite(Data* d);
	void removeDataFromSite(string dataName);
	void writeToDataOnSite(string dataName, string transactionName, 
		int value);
	// Method to issue lock on a data item
	void issueLock(string transactionName, string dataName, 
		LockType lType);
	// Method to issue an unlock on a data item
	void issueUnlock(string dataName);
	void issuePromoteLock(string dataName);
	// Fail method
	void fail();
	void updateDataLastCommittedValues(string transactionName);
	void revertDataToLastCommitted(string dataname);
	// Recovery method
	void recover();

#endif