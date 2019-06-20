#ifndef LOCK_H
#define LOCK_H

#include <iostream>

#include "Data.h"

enum LockType {RO = 0, RW};

class Lock {

private:

	// Transaction which issues the Lock
	string requestor;
	Data* data;
	LockType type;

public:

	/* We implement two types of locks- read locks (RO) and write locks (RW), which 
	are also called Shared locks and Exclusive locks, respectively. 

	The Locks and Locking manager are naive. They create locks without regard to
	whether or not a Lock is viable. The Sites manage proper issue of locks.
	*/

	// Creating a lock on a Data item changes the Data item's lock status
	Lock(string requestor, Data* data, LockType type);

	// Accessors
	Data* getData();
	LockType getLockType();
	string getRequestor();

	// Mutators
	void promoteLock();
};

#endif