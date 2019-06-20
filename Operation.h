#ifndef OPERATION_H
#define OPERATION_H

#include <iostream>
#include <sstream>
#include <climits>

#include "Data.h"

using namespace std;

enum Operator{R = 0, W = 1, begin, beginRO, end, dump, fail, recover}; 

class Operation {

protected:

	Operator op;
	string transactionName;
	string dataName;
	int writeValue;
	int siteIndex;

public: 

	// Constructor for dump operation
	Operation(Operator op);
	// Constructor for begin, beginRO, end
	Operation(Operator op, string transactionName);
	// Constructor for read
	Operation(Operator op, string transactionName, string dataName);
	// Constructor for write
	Operation(Operator op, string transactionName, string dataName, 
		int writeValue);
	// Constructor for fail, recover
	Operation(Operator op, int siteIndex);

	// Accessors
	Operator getOperator();
	int getDataIndex();
	string getTransactionName();
	string getDataName();
	int getWriteValue();
	int getSiteIndex();
};

#endif