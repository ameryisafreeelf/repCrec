#include "Lock.h"

using namespace std;

Lock::Lock(string requestor, Data* data, LockType type) {
	this -> requestor = requestor;
	this -> data = data;
	this -> type = type;
	if (type == RO)
		data -> setToReadLocked(requestor);
	else 
		data -> setToWriteLocked(requestor);
}


Data* Lock::getData() {
	return this -> data;
}

LockType Lock::getLockType() {
	return this -> type;
}

string Lock::getRequestor() {
	return this -> requestor;
}

void Lock::promoteLock() {
	if (this -> type == LockType::RO) {
		this -> type = LockType::RW;
		this -> data -> promoteLockStatus();
	}
	else
		return;
}
