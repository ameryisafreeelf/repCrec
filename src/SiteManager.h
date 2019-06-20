#ifndef SITEMANAGER_H
#define SITEMANAGER_H

#include <iostream>
#include <sstream> 
#include <string>
#include <vector>
#include <unordered_set>

#include "Site.cpp"
#include "Operation.cpp"
#include "Data.cpp"

class SiteManager {

private:

	unordered_map<int, Site*> sitesMap;

public:

	// "Factory method"
	void initializeSites();

	// Accessors

	// Method to access available copy of data
	// This more or less is how we implement R
	// This will return data as long as it's not replicated
	// Implement waiting in Driver
	Data* getReadableDataItem(int dataIndex, string dataName);
	void writeToAllSites(int dataIndex, string dataName, string 
		transactionName, int value);
	unordered_map<string,int> getReadOnlyCopies();
	void dumpAllSites();
	int getNumOfSites();
	Site* getSiteByIndex(int i);
	bool allSitesNormal();

	// Mutators
	void issueLockOnAllSites(string dataName, string transactionName, 
		LockType lType);
	void issueUnlockOnAllSites(string dataName);
	void issuePromoteLockOnAllSites(string dataName);
	// Won't ever need this
	void addSite(int i, Site* s);
	void fail(int i);
	void SiteManager::recover(int i);
	void updateDataAtAllSitesLastCommitted(string transactionName);
	void revertDataAtAllSitesToLastCommitted(string dataname);

#endif