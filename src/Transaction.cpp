#include "Transaction.h"

using namespace std;

Transaction::Transaction(string name, int startTime, bool isReadOnly = 
false, transactionStatus ts = Created) {
	this -> name = name;
	this -> startTime = startTime;
	this -> endTime = INT_MIN;
	this -> isReadOnly = isReadOnly;
	this -> ts = ts;
}

string Transaction::getTransactionName() {
	return this -> name;
}

int Transaction::getTransactionStartTime() {
	return this -> startTime;
}

int Transaction::getTransactionEndTime() {
	return this -> endTime;
}

bool Transaction::getIsReadOnly() {
	return this -> isReadOnly;
}

transactionStatus Transaction::getTransactionStatus() {
	return this -> ts;
}

int Transaction::getNumOfWaitingOperations() {
	return waitingOpList.size();
}


void Transaction::setTransactionStatus(transactionStatus ts) {
	this -> ts = ts;
}

void Transaction::setTransactionStartTime(int startTime) {
	this -> startTime = startTime;
}

void Transaction::setTransactionEndTime(int endTime) {
	this -> endTime = endTime;
}

void Transaction::addOperationToPending(Operation* op) {
	waitingOpList.push_back(op);
}

void Transaction::addToWriteSet(Data* d) {
	writeSet.push_back(d);
}

void Transaction::removeFromPending(Operation* op) {
	vector<Operation*>::iterator opListItr;
	opListItr = find(waitingOpList.begin(), waitingOpList.end(), op);
	if (opListItr == waitingOpList.end())
		return;
	else {
		delete *opListItr;
		*opListItr = nullptr;
		waitingOpList.erase(opListItr);
	}
}