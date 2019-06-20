#include "Operation.h"

using namespace std;

Operation::Operation(Operator op) {
	this -> op = op;
}

Operation::Operation(Operator op, string transactionName) {
	this -> op = op;
	this -> transactionName = transactionName;
}

Operation::Operation(Operator op, string transactionName, string dataName) {
	this -> op = op;
	this -> transactionName = transactionName;
	this -> dataName = dataName;
}

Operation::Operation(Operator op, string transactionName, string dataName, 
int writeValue) {
	this -> op = op;
	this -> transactionName = transactionName;
	this -> dataName = dataName;
	this -> writeValue = writeValue;
}

// Constructor for fail, recover
Operation::Operation(Operator op, int siteIndex) {
	this -> op = op;
	this -> siteIndex = siteIndex;
}

Operator Operation::getOperator() {
	return this -> op;
}

int Operator::getDataIndex() {
	int dataIndex = 0;
	string dataIndexBuff = ""; 
	char* charP = &(this -> getDataName()[1]);
	while (*charP != '\0') {
		dataIndexBuff += *charP;
		charP++;
	}
	stringstream str(dataIndexBuff);
	str >> dataIndex;
	return dataIndex;
}

string Operation::getTransactionName() {
	try {
		if (this -> transactionName == "")
			throw (50);
		else {
			return this -> transactionName;
		}
	}
	catch(int x) {
		cout << "Operation has no associated transaction" << endl;
		throw;
	}
}

string Operation::getDataName() {
	return this -> dataName;
}

int Operation::getWriteValue() {
	try {
		if (this -> writeValue == INT_MIN)
			throw (52);
		else 
			return this -> writeValue;
	}
	catch(int x) {
		cout << "Operation has no associated Write Value" << endl;
		throw;
	}
}

int Operation::getSiteIndex() {
	try {
		if (this -> siteIndex == INT_MIN)
			throw (53);
		else {
			return this -> siteIndex;
		}
	}
	catch(int x) {
		cout << "Operation has no associated Site Index" << endl;
		throw;
	}
}
