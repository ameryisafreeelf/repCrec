##ifndef DATA_H
#define DATA_H

#include <iostream>
#include <string>
#include <climits>
#include <unordered_map>
#include <iterator>

class Data {

private:

	string name;
	// Only used for glamour purposes, no functionality
	string fullName;
	int value;
	bool readLocked;
	bool writeLocked;
	bool deployAll;
	// Pair holding the last committed transaction and value
	pair<string, int> lastCommittedValue;
	// If true, Data cannot be read until written, since its site had just recovered
	// Can handle this in client
	bool isReplicated;	
	// Indicates the transactionName that holds the lock, if any
	string lockingTransaction;
	// Key is name of the writing Transaction, value is value
	unordered_map<string, int> writeHistory;

public:

	Data(string name, bool readLocked = false, bool writeLocked = 
	false, bool deployAll = false, bool isReplicated = false, 
	string lockingTransaction = "");

	// Accessors
	string getName();
	string getFullName();
	int getValue();
	int getLastCommittedValue();
	bool isAnyLocked();
	bool isReadLocked();
	bool isWriteLocked();
	bool isDeployedAll();
	bool getIsReplicated();
	string getLockingTransaction();
	vector<string>* getTransactionsWritingToData();

	// Mutators
	void modifyName(string name);
	void giveFullName(int siteNumber);
	void setValue(string transactionName, int value);
	void updateLastCommittedValue(string transactionName);
	// Update lastCommittedValue when a the Writing Transaction commits
	void revertToLastCommittedValue();
	// Caution!! Use carefully
	void setToReadLocked(string lockingTransaction);
	// Caution!! Use carefully.
	void setToWriteLocked(string lockingTransaction);
	// If a data item is read locked then written to, give write lock
	void promoteLockStatus();
	// Caution!! Use carefully.
	void setToUnlocked();
	// Makes data item unreadable until it is to again
	// Probably won't ever use
	void setIsReplicated();
	void printEntireWriteHistory();
	// Replicate Data item in heap, give the replicated copy a destination
	Data* replicate(int destination);
}

#endif