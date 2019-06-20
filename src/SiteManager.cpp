void SiteManager::initializeSites() {
	// Create odd Data items
	Data* x1 = new Data("x1");
	Data* x3 = new Data("x3");
	Data* x5 = new Data("x5");
	Data* x7 = new Data("x7");
	Data* x9 = new Data("x9");
	Data* x11 = new Data("x11");
	Data* x13 = new Data("x13");
	Data* x15 = new Data("x15");
	Data* x17 = new Data("x17");
	Data* x19 = new Data("x19");
	// Initialize sites and add non-global Data items
	Site* s1 = new Site(1); 

	Site* s2 = new Site(2); 
	s2 -> addDataToSite(x1);
	s2 -> addDataToSite(x11);

	Site* s3 = new Site(3); 

	Site* s4 = new Site(4);
	s4 -> addDataToSite(x3);
	s4 -> addDataToSite(x13);

	Site* s5 = new Site(5); 

	Site* s6 = new Site(6); 
	s6 -> addDataToSite(x5);
	s6 -> addDataToSite(x15);

	Site* s7 = new Site(7); 

	Site* s8 = new Site(8); 
	s8 -> addDataToSite(x7);
	s8 -> addDataToSite(x17);

	Site* s9 = new Site(9); 

	Site* s10 = new Site(10); 
	s10 -> addDataToSite(x9);
	s10 -> addDataToSite(x19);

	// Add these to the siteMap
	sitesMap.insert( pair<int, Site*>(1, s1 ) );
	sitesMap.insert( pair<int, Site*>(2, s2 ) );
	sitesMap.insert( pair<int, Site*>(3, s3 ) );
	sitesMap.insert( pair<int, Site*>(4, s4 ) );
	sitesMap.insert( pair<int, Site*>(5, s5 ) );
	sitesMap.insert( pair<int, Site*>(6, s6 ) );
	sitesMap.insert( pair<int, Site*>(7, s7 ) );
	sitesMap.insert( pair<int, Site*>(8, s8 ) );
	sitesMap.insert( pair<int, Site*>(9, s9 ) );
	sitesMap.insert( pair<int, Site*>(10, s10 ) );
}

Data* SiteManager::getReadableDataItem(int dataIndex, string dataName) {
	Data* d = nullptr;
	if (dataIndex % 2 == 0) {
		unordered_map<int, Site*>::iterator itr = sitesMap.begin();
		while ( itr != sitesMap.end() ) {
			if (itr -> second -> getSiteStatus() == siteStatus::Normal) {
				//cout << "Looking for " << dataName << endl;
				//cout << "Checking Site " << itr -> second -> getSiteName() << endl;					
				d = itr -> second -> getDataItem(dataName);
				if ( ! d ) {
					itr++;
				}
				else if (d -> getIsReplicated()) {
					itr++;
					continue;
				}
				else {	
					//cout << "Found " << dataName << " at " << itr -> second -> getSiteName() <<endl;
					return d;
				}
				//d = itr -> second -> getDataItem(dataName);
			}
			else {
				itr++;
			}
		} 
		return d;
	}
	else {
		int key = (dataIndex % 10) + 1;
		if (sitesMap[key] -> getSiteStatus() == Normal) {
			//cout << "Found " << dataName << " at " << "S" << key <<endl;
			return sitesMap[key] -> getDataItem(dataName);
		}
		else {
			return d;
		}
	}
}

void SiteManager::writeToAllSites(int dataIndex, string dataName, string 
transactionName, int value) {
	if (dataIndex % 2 == 0) {
		unordered_map<int, Site*>::iterator itr = sitesMap.begin();
		while ( itr != sitesMap.end() ) {
			if (itr -> second -> getSiteStatus() == siteStatus::Normal) {
				itr -> second -> writeToDataOnSite(dataName, transactionName, value);
			}
			else {
				itr++;
				continue;;
			}
			itr++;
		}
		return;
	}
	else {
		int key = (dataIndex % 10) + 1;
		if (sitesMap[key] -> getSiteStatus() == siteStatus::Normal ) {
			sitesMap[key] -> writeToDataOnSite(dataName, transactionName, value);
		}
		else {
			return;
		}
	}
}

unordered_map<string,int> SiteManager::getReadOnlyCopies() {
	unordered_map<string,int> allAvailableCopies;
	for (int i = 1; i < 21; i++) {
		string key = "x" + to_string(i);
		int value = getReadableDataItem(i, key) -> getValue();
		allAvailableCopies[key] = i;
	}
	return allAvailableCopies;
}

void SiteManager::dumpAllSites() {
	unordered_map<int, Site*>::iterator itr = sitesMap.begin();
	while ( itr != sitesMap.end() ) {
		itr -> second -> dump();
		cout << "\n";
		itr++;
	}
}

int SiteManager::getNumOfSites() {
	return sitesMap.size();
}

Site* SiteManager::getSiteByIndex(int i) {
	return sitesMap[i]; 
}

bool SiteManager::allSitesNormal() {
	for (pair<int, Site*> element : sitesMap) {
		if ( element.second -> getSiteStatus() == siteStatus::Normal ) {
			continue;
		}
		else {
			return false;;
		}
	}
	return true;
}

void SiteManager::issueLockOnAllSites(string dataName, string 
transactionName, LockType lType) {
	for (int i = 1; i < 11; i++) {
		sitesMap[i] -> issueLock(transactionName, dataName, lType);
	}
}

void SiteManager::issueUnlockOnAllSites(string dataName) {
	for (int i = 1; i < 11; i++) {
		sitesMap[i] -> issueUnlock(dataName);
	}
}

void SiteManager::issuePromoteLockOnAllSites(string dataName) {
	for (int i = 1; i < 11; i++) {
		sitesMap[i] -> issuePromoteLock(dataName);
	}
}

// Won't ever need this
void SiteManager::addSite(int i, Site* s) {
	sitesMap.insert( {i, s} ) ;
}

void SiteManager::fail(int i) {
	sitesMap[i] -> fail();
	cout << "Failure on Site " << i << endl;
}

void SiteManager::recover(int i) {
	sitesMap[i] -> recover();
	cout << "Recovery on Site " << i << endl;
}

void SiteManager::updateDataAtAllSitesLastCommitted(string transactionName) {
	unordered_map<int, Site*>::iterator itr = sitesMap.begin();
	while ( itr != sitesMap.end() ) {
		itr -> second -> updateDataLastCommittedValues(transactionName);
		itr++;
	}
}

void SiteManager::revertDataAtAllSitesToLastCommitted(string dataname) {
	unordered_map<int, Site*>::iterator itr = sitesMap.begin();
	while ( itr != sitesMap.end() ) {
		itr -> second -> revertDataToLastCommitted(dataname);
		itr++;
	}
}