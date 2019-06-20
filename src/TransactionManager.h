#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <string>
#include <unordered_map>
#include <iterator>

#include "Data.cpp"
#include "SiteManager.cpp" 
#include "Transaction.cpp"
#include "Graph.cpp"
#include "Lock.cpp"
#include "Operation.cpp"

class TransactionManager {

private:

	// Key is the Transaction's name
	unordered_map<string, Transaction*> transactionMap;
	unordered_map<string, string> waitsForGraph;
	// Stores operations in waiting
	vector<Operation*> operationsWaitList;

public:

	// Accessors
	int getNumOfTransactions();
	int getNumOfWaitingOperations();
	transactionStatus getTransactionStatus(string transactionName);
	Operation* getFirstWaitingOperation();
	vector<Operation*>* getOperationsWaitList();
	Transaction* findTransaction(string transactionName);

	// Mutators
	void setTransactionStatus(string transactionName, transactionStatus ts);
	void addTransaction(Transaction* t);
	void addWaitsForRelationship(string t1, string t2);
	void addOperationToWaitList(Operation* op, string lockingTransaction);
	void clearOpsWaitList();

	void begin(string transactionName, int startTime, Operation* op);
	void beginRO(string transactionName, int startTime, Operation* op);
	void end(string transactionName, int endTime, Operation* op);
	bool isTransactionWaiting(string transactionName);
	// Implementation of a R operation
	int transactionIssuesRead(Operation* op, Data* d);
	// Implementation of a W operation
	void transactionIssuesWrite(Operation* op, Data* d);
	void commitTransaction(string transactionName);
	void abortTransaction(string transactionName);


	// Utility functions for detecting deadlocks and killing youngest
	
	// Constructs a graph whenever it's run and looks for deadlock
	// Returns the youngest transaction (of all transactions )if a deadlock is detected
	// But this works because we run it at every Read and Write
	bool detectDeadlock();
	string detectYoungestInDeadlock();
};
