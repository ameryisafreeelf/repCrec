#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <iostream>
#include <climits>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "Operation.cpp"
#include "Data.cpp"

enum transactionStatus{Created, Running, Waiting, Committed, Aborted};

class Transaction {

private:

	string name;
	int startTime;
	int endTime;
	bool isReadOnly;
	transactionStatus ts;
	// list of waiting operations
	vector<Operation*> waitingOpList;
	// map of written data distributed about site indices
	// map of data for RO transactions, string is dataName and int is value
	//unordered_map<string,int> readOnlyCopies;

public:

	vector<Data*> writeSet;

	Transaction(string name, int startTime, bool isReadOnly = false, 
		transactionStatus ts = Created);

	// Accessors
	string getTransactionName();
	int getTransactionStartTime();
	int getTransactionEndTime();
	bool getIsReadOnly();
	transactionStatus getTransactionStatus();
	int getNumOfWaitingOperations();

	// Mutators
	void setTransactionStatus(transactionStatus ts);
	// Should never have to use this, set by nstruction
	void setTransactionStartTime(int startTime);
	void setTransactionEndTime(int endTime);
	void addOperationToPending(Operation* op);
	void addToWriteSet(Data* d);
	void removeFromPending(Operation* op);

#endif