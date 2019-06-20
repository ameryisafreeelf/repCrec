#include "Data.h"

using namespace std;

Data::Data(string name, bool readLocked = false, bool writeLocked = 
false, bool deployAll = false, bool isReplicated = false, 
string lockingTransaction = "") {
	this -> name = name;
	this -> fullName = "";
	this -> value = INT_MIN;
	this -> readLocked = readLocked;
	this -> writeLocked = writeLocked;
	this -> deployAll = deployAll;
	this -> lastCommittedValue = make_pair("", INT_MIN);
	// Replicated data cannot be read, must  be written first
	this -> isReplicated = isReplicated;
}

string Data::getName() {
	return this -> name;
}

string Data::getFullName() {
	return this -> fullName;
}

int Data::getValue() {
	return this -> value;
}

int Data::getLastCommittedValue() {
	try {
		if (this -> lastCommittedValue.second == INT_MIN)
			throw 20;
		else
			return this -> lastCommittedValue.second;				
	}
	catch(int x) {
		cout << "No committed values on " << this -> fullName << endl;
		return INT_MIN;
	}
} 

bool Data::isAnyLocked() {
	if ( ( isReadLocked()) || ( isWriteLocked()) )
		return true;
	return false;
}

bool Data::isReadLocked() {
	 if ( (! this -> writeLocked) && (this -> readLocked) )
	 	return true;
	return false;
}

bool Data::isWriteLocked() {
	if ( (this -> writeLocked) && (this -> readLocked) )
		return true;
	return false;
}

bool Data::isDeployedAll() {
	return this -> deployAll;
}

bool Data::getIsReplicated() {
	return this -> isReplicated;
}

string Data::getLockingTransaction() {
	return this -> lockingTransaction; 
}

vector<string>* Data::getTransactionsWritingToData() {
	vector<string>* writers = new vector<string>;
	unordered_map<string,int>::iterator itr = writeHistory.begin();	
	while(itr != writeHistory.end() ) {
		writers -> push_back(itr -> first);
		itr++;
	}	
	return writers;
}


void Data::modifyName(string name) {
	this -> name = name;
}

void Data::giveFullName(int siteNumber) {
	string nameBuff = this -> getName();
	nameBuff += ".";
	nameBuff += to_string(siteNumber);
	this -> fullName = nameBuff;
}

void Data::setValue(string transactionName, int value) {
	writeHistory[transactionName] = value;
	this -> isReplicated = false;
	this -> value = value;
}

void Data::updateLastCommittedValue(string transactionName) {
	unordered_map<string,int>::iterator itr = writeHistory.find(transactionName);		
	try {
		if ( itr == writeHistory.end() ) {
			throw 25;
		}
		else {
			this -> lastCommittedValue = make_pair(transactionName, itr -> second);
		}
	}
	catch(int x) {
		this -> lastCommittedValue = make_pair("", INT_MIN);
	}
}

// Update lastCommittedValue when a the Writing Transaction commits
void Data::revertToLastCommittedValue() {
	this -> value = lastCommittedValue.second;
}

// Caution!! Use carefully. 
void Data::setToReadLocked(string lockingTransaction) {
	this -> readLocked = true;
	this -> writeLocked = false;
	this -> lockingTransaction = lockingTransaction;
}

// Caution!! Use carefully.
void Data::setToWriteLocked(string lockingTransaction) {
	this -> readLocked = true;
	this -> writeLocked = true;
	this -> lockingTransaction = lockingTransaction;
}

void Data::promoteLockStatus() {
	setToWriteLocked( this -> lockingTransaction );
}

// Caution!! Use carefully.
void Data::setToUnlocked() {
	this -> readLocked = false;
	this -> writeLocked = false;
	this -> lockingTransaction = "";
}

// Makes data item unreadable until it is to again
// Probably won't ever use
void Data::setIsReplicated() {
	this -> isReplicated = true;
}

void Data::printEntireWriteHistory() {
	cout << "Write history for " << fullName << endl;
	for (auto i: writeHistory) {
		cout << i.first << ": " << i.second << endl;
	}
}

// Replicate Data item in heap, give the replicated copy a destination
Data* Data::replicate(int destination) {
	Data* copy = new Data(this -> name, this -> readLocked, 
		this -> writeLocked, this -> deployAll, isReplicated = true, 
		lockingTransaction = "");
	copy -> value = this -> value;
	copy -> lastCommittedValue = this -> lastCommittedValue;
	copy -> giveFullName(destination);
	// Give the copy the same write history
	copy -> writeHistory = this -> writeHistory;
	return copy;
}
