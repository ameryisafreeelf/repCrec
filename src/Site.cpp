Site::Site(int siteIndex, siteStatus status = Normal) {
	this -> siteIndex = siteIndex;
	this -> siteName = "S" + to_string(siteIndex);		
	this -> status = status;
	// Deploy even-numbered data items in constructor
	Data* x2 = new Data("x2");
	Data* x4 = new Data("x4");
	Data* x6 = new Data("x6");
	Data* x8 = new Data("x8");
	Data* x10 = new Data("x10");
	Data* x12 = new Data("x12");
	Data* x14 = new Data("x14");
	Data* x16 = new Data("x16");
	Data* x18 = new Data("x18");
	Data* x20 = new Data("x20");
	this -> addDataToSite(x2);
	this -> addDataToSite(x4);
	this -> addDataToSite(x6);
	this -> addDataToSite(x8);
	this -> addDataToSite(x10);
	this -> addDataToSite(x12);
	this -> addDataToSite(x14);
	this -> addDataToSite(x16);
	this -> addDataToSite(x18);
	this -> addDataToSite(x20);
}

string Site::getSiteName() {
	return this -> siteName;
}

int Site::getSiteIndex() {
	return this -> siteIndex;
}

siteStatus Site::getSiteStatus() {
	return this -> status;
}

int Site::getNumOfDataItems() {
	return this -> dataSet.size();
}

int Site::getNumOfLocks() {
	return this -> lockSet.size();
}

Data* Site::getDataItem(string dataName) {
	unordered_map< string, Data* >::iterator itr = dataSet.find(dataName);
	if( itr == dataSet.end() ) {
		return nullptr;
	}
	else
		return itr -> second;
}	

int Site::getValueOfDataOnSite(string dataName) {
	return this -> getDataItem(dataName) -> getValue();
}

Lock* Site::findLock(string dataName) {
	unordered_map< string, Lock* >::iterator itr = lockSet.find(dataName);
	if( itr == lockSet.end() )
		return nullptr;
	else
		return itr -> second;
}

bool Site::isReadLockOnData(string dataName) {
	unordered_map<string, Lock*>::iterator itr = lockSet.find(dataName);
	if ( itr == lockSet.end() )
		return false;
	if (itr -> second -> getLockType() == LockType::RO)
		return true;
	return false;
}	

bool Site::isWriteLockOnData(string dataName) {
	unordered_map<string, Lock*>::iterator itr = lockSet.find(dataName);
	if ( itr == lockSet.end() )
		return false;
	if (itr -> second -> getLockType() == LockType::RW)
		return true;
	return false;
}

set<string>* Site::getTransactionsWrittenToSite() {
	set<string>* writersToSite = new set<string>;
	unordered_map< string, Data* >::iterator itr = dataSet.begin();
	while (itr != dataSet.end()) {
		vector<string>* dataWrites = itr -> second -> getTransactionsWritingToData();
		if (dataWrites -> empty()) {
			itr++;
			continue;
		}
		else {
			for (string s : *dataWrites) {
				//cout << "Boo" << endl;
				//cout << s << endl;
				writersToSite -> insert(s);
			}
			dataWrites -> clear();
		}
		itr++;
	}
	return writersToSite;
}

void Site::dump() {
	cout << "******************** " << "Site " << this -> getSiteName() <<
		" ********************" << endl;
	if (this -> getSiteStatus() == 0) {
		cout << "Site Status: Normal\n" << endl;
	}
	else if (this -> getSiteStatus() == 1) {
		cout << "Site Status: Fail\n<< endl";
	}
	else {
		cout << "Error: Site should either be in Normal or Fail state" << endl;
	}
	for (auto d : dataSet) {
		if (d.second -> getValue() != INT_MIN ) {
			cout << d.second -> getName() << ":	" << 
				d.second -> getValue() << endl;
		}
	} 
	cout << "Other variables unchanged" << endl; 
}

void Site::setSiteStatus(siteStatus status) {
	this -> status = status;
}

void Site::addDataToSite(Data* d) {
	// If the Data item is already in the site, do nothing
	if (getDataItem(d -> getName()))
		return;
	d -> giveFullName(this -> siteIndex);
	dataSet[d -> getName()] = d;
	string key = d -> getName();
	dataSet[key] = d;
}

void Site::removeDataFromSite(string dataName) {
	dataSet.erase(dataName);
}

void Site::writeToDataOnSite(string dataName, string transactionName, 
int value) {
	Data* d = this -> getDataItem(dataName);
	if ( d ) {
		d -> setValue(transactionName, value);
	}
	else {
		return;
	}
}

// Method to issue lock on a data item
void Site::issueLock(string transactionName, string dataName, 
LockType lType) {
	// If data item is not in dataSet, it can't  be in the lockSet
	if (! getDataItem(dataName) )
		return;
	else if (! findLock(dataName) ) {
		// If there's no lock on the Data item, create it
		// This will also change the status of the Data item
		Data* d = this -> getDataItem(dataName);
		Lock* l = new Lock(transactionName, d, lType);
		// Insert the new Lock into the lockSet
		this -> lockSet[dataName] = l;
	} 
	string currentLockingTransaction = findLock(dataName) -> getRequestor();
	// If the transaction that currently locks d tries to issue another lock on d
	if (currentLockingTransaction == transactionName) {
		// Then the only thing we might do is promote the Lock
		// Otherwise, there's nothing to do
		if ( lType == LockType::RW && getDataItem(dataName) -> isReadLocked() ) {
			findLock(dataName) -> promoteLock();
		}
		else {
			return;
		}
	}
	// Otherwise, if different transaction tries to get a lock on d, it can't
	else {
		return;
	}
}

void Site::issueUnlock(string dataName) {
	if ( isReadLockOnData(dataName) || isWriteLockOnData(dataName) ) {
		dataSet[dataName] -> setToUnlocked();
		lockSet.erase(dataName);
	}
	else
		return;
}

void Site::issuePromoteLock(string dataName) {
	if ( isReadLockOnData(dataName) ) {
		dataSet[dataName] -> promoteLockStatus();
	}
}

void Site::fail() {
	// Set status of site to fail
	this -> setSiteStatus(Fail);
	for (auto i : dataSet) {
		i.second -> setToUnlocked();
	}
	// Release all locks
	lockSet.clear();
}

void Site::updateDataLastCommittedValues(string transactionName) {
	unordered_map< string, Data* >::iterator itr = this -> dataSet.begin();
	while ( itr != this -> dataSet.end() ) {
		itr -> second -> updateLastCommittedValue(transactionName);
		itr++;
	}	
}

void Site::revertDataToLastCommitted(string dataname) {
	unordered_map< string, Data* >::iterator itr = dataSet.find(dataname);
	itr -> second -> revertToLastCommittedValue();
}

void Site::recover() {
	if (! (getSiteStatus() == Fail) ) {
		cout << "Site should be in Fail state to recover" << endl;
		return;
	}
	else {
		this -> setSiteStatus(siteStatus::Recovering);
		unordered_map< string, Data* >::iterator itr = this -> dataSet.begin();
		while ( itr != this -> dataSet.end() ) {
			itr -> second -> setIsReplicated();
			itr++;
		}
		this -> setSiteStatus(siteStatus::Normal); 
	}
}