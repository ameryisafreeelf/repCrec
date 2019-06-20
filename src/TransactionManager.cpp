#include "TransactionManager.h"

using namespace std;

int TransactionManager::getNumOfTransactions() {
	return this -> transactionMap.size();
}

int TransactionManager::getNumOfWaitingOperations() {
	return this -> operationsWaitList.size();
}

transactionStatus TransactionManager::getTransactionStatus(string transactionName) {
	try {
		if (! findTransaction(transactionName) ) {
			throw "No transaction found";
		}
		else {

			return findTransaction(transactionName) -> getTransactionStatus();
		}

	}
	catch (string s) {
		cout << "Error: transaction is not found in TM" << endl;
		return Aborted;
	}
}

Operation* TransactionManager::getFirstWaitingOperation() {
	Operation* firstInLine = operationsWaitList.front();
	// Remove first operation
	operationsWaitList.erase( operationsWaitList.begin() );
	return firstInLine;
}

vector<Operation*>* TransactionManager::getOperationsWaitList() {
	vector<Operation*>* waitingListP = &operationsWaitList;
	return waitingListP; 
}

Transaction* TransactionManager::findTransaction(string transactionName) {
	unordered_map< string, Transaction* >::iterator itr = transactionMap.find(transactionName);
	if( itr == transactionMap.end() ) {
		return nullptr;
	}
	else
		return itr -> second;
}

void TransactionManager::begin(string transactionName, int startTime, Operation* op) {
	try {
		if (op -> getOperator() != Operator::begin) {
			throw "Requires begin Operator";
		}
		else {
			// If Transaction is already in transactionMap, it has begun already
			if ( findTransaction(transactionName) )
				return;
			else {
				// Add transaction to transactionMap
				Transaction* t = new Transaction(transactionName, startTime, Created);
				this -> addTransaction(t);
			}
		}
	}
	catch(string s) {
		cout << "Cannot run \"begin\" method except with begin operation" << endl;
	}
}

void TransactionManager::beginRO(string transactionName, 
int startTime, Operation* op) {
	try {
		if (op -> getOperator() != Operator::beginRO) {
			throw "Requires beginRO Operator";
		}
		else {
			// If Transaction is already in transactionMap, it has begun already
			if ( findTransaction(transactionName) )
				return;
			// Add transaction to transactionMap
			Transaction* t = new Transaction(transactionName, startTime, true);
			this -> addTransaction(t);
			//cout << "BeginRO: " << transactionName << endl;
		}
	}
	catch(string s) {
		cout << "Cannot run \"beginRO\" method except with beginRO operation" 
			<< endl;
	}
}

void TransactionManager::end(string transactionName, int endTime, Operation* op) {
	try {
		if (op -> getOperator() != Operator::end)
			throw 12;
		else {
			if (getTransactionStatus(transactionName) == transactionStatus::Aborted) {
				return;
			} 
			else if (getTransactionStatus(transactionName) == transactionStatus::Waiting) {
				
				setTransactionStatus(transactionName, transactionStatus::Aborted);
				
				//transactionMap.erase(transactionName);					
			}
			else if (getTransactionStatus(transactionName) == transactionStatus::Running) {					
				setTransactionStatus(transactionName, transactionStatus::Committed);
				unordered_map<string,string>::iterator itr = waitsForGraph.begin();
				while ( itr != waitsForGraph.end() ) {
					if (itr -> first == transactionName) {
						//cout << "Erasing " << itr -> first << ", " << itr -> second << endl;
						itr = waitsForGraph.erase(itr);
						//itr++;
					} 
					else if (itr -> second == transactionName) {
						findTransaction( itr -> first) -> setTransactionStatus(Running);
						itr = waitsForGraph.erase(itr);
						//itr++;

					} 
					else {
						itr++;
					}
				}
			}
			else {
				cout << "Error: Transaction is in an unexpected state" << endl;
			}
			/*
			if (findTransaction(transactionName)) {
				this -> findTransaction(transactionName) -> setTransactionEndTime(endTime);
			}
			else {
				return;
			}
			*/
			return;
		}
	}
	catch(int x) {
		cout << "Cannot run \"end\" method except with end operation" << endl;
	}
}

bool TransactionManager::isTransactionWaiting(string transactionName) {
	unordered_map<string,string>::iterator itr = waitsForGraph.begin();
	if (waitsForGraph.empty()) {
		return false;
	}
	while ( itr != waitsForGraph.end() ) {
		if (itr -> first == transactionName) {
			return true;
		}
		itr++;
	}
	return false;
}

void TransactionManager::setTransactionStatus(string transactionName, 
transactionStatus ts) {
	if (! findTransaction(transactionName)) {
		cout << "Error: Attempted to set Status of " << transactionName << " which is not in the TM" << endl;
		return;
	}
	else {
		findTransaction(transactionName) -> setTransactionStatus(ts);
		return;
	}
}

void addTransaction(Transaction* t) {
	string key = t -> getTransactionName();
	t -> setTransactionStatus(transactionStatus::Running);
	transactionMap[key] = t;
}

void TransactionManager::addWaitsForRelationship(string t1, string t2) {
	waitsForGraph.insert(make_pair(t1, t2));
}

void TransactionManager::addOperationToWaitList(Operation* op, string lockingTransaction) {
	try {
		if (! (op -> getOperator() == Operator::R || op -> getOperator() == Operator::W) ) {
			throw "Wrong operator";
		}
		else {
			string transactionToWaiting = op -> getTransactionName();
			//cout << "The transaction that will be waiting is " << transactionToWaiting << endl;
			//cout << "The transaction that currently locks data is " << lockingTransaction << endl;
			// op's Transaction must be in the transactionMap
			if (! findTransaction(transactionToWaiting)) {
				return;
			}
			else {
				operationsWaitList.push_back(op);
				this -> addWaitsForRelationship(transactionToWaiting, lockingTransaction);
				setTransactionStatus(transactionToWaiting, transactionStatus::Waiting);
			}
		}
	}
	catch(string s) {
		cout << "Only R and W operations go in wait list" << endl;
	}
}

void clearOpsWaitList() {
	this -> operationsWaitList.clear();
}

int TransactionManager::transactionIssuesRead(Operation* op, Data* d) {
	try {
		if (op -> getOperator() != Operator::R)
			throw 13;
		else {
			bool isRequestorRO = findTransaction(op -> getTransactionName()) -> getIsReadOnly();
			//cout << "Is requestor RO " << isRequestorRO << endl;

			// If Data item  is write locked
			if ( ( d -> isWriteLocked() ) && ( d -> getLockingTransaction() != 
			op -> getTransactionName() )  && (! isRequestorRO ) ) {
				addOperationToWaitList(op, d -> getLockingTransaction());
				string currentLockingTransaction = d -> getLockingTransaction();
				// Add the waits-for relationship
				waitsForGraph[currentLockingTransaction] = op -> getTransactionName();
				//cout << currentLockingTransaction << " locks out " << op -> getTransactionName() << endl;
				return INT_MIN;
			}
			else {
				//d -> setToReadLocked(op -> getTransactionName());
				return d -> getValue();
			}
		}
	}
	catch(int x) {
		cout << "Cannot run \"read\" method except with R operation" << endl;	
		return INT_MIN;
	}
}

void TransactionManager::transactionIssuesWrite(Operation* op, Data* d) {
	try {
		if (op -> getOperator() != Operator::W)
			throw 14;
		else {
			if ( (d -> isReadLocked() || d -> isWriteLocked()) && 
			( d -> getLockingTransaction() != op -> getTransactionName() ) ) {
				addOperationToWaitList(op, d -> getLockingTransaction() );
				string currentLockingTransaction = d -> getLockingTransaction();
				waitsForGraph[currentLockingTransaction] = op -> 
					getTransactionName();
				//cout << currentLockingTransaction << " locks out " << op -> getTransactionName() << endl;
			}
			else {
				string transactionName = op -> getTransactionName();
				findTransaction(transactionName) -> addToWriteSet(d);
				return;
			}
		}
	}
	catch(int x) {
		cout << "Cannot run \"write\" method except with W operation" << endl;			
	}
}

void TransactionManager::commitTransaction(string transactionName) {
	findTransaction(transactionName) -> setTransactionStatus(transactionStatus::Committed);
}

void TransactionManager::abortTransaction(string transactionName) {
	findTransaction(transactionName) -> setTransactionStatus(transactionStatus::Aborted);
	//cout << "The waits for graph is " << waitsForGraph.size() << endl;
	if ( isTransactionWaiting(transactionName) ) {
		waitsForGraph.erase(transactionName);
		if (operationsWaitList.empty()) {
			return;
		}
		else {
			vector<Operation*>::iterator itr = operationsWaitList.begin();
			while ( itr != operationsWaitList.end() ) {
				if ( ( (*itr) -> getTransactionName() ) == transactionName ) {
					//cout << "Deleted " << (*itr) -> getTransactionName() << " " << (*itr) -> getOperator() << " from the operations waitlist" << endl;
					itr = operationsWaitList.erase(itr);
				}
				else {
					itr++;
				}
			}
			if (! waitsForGraph.empty() ) {
				/*
				for (auto i : waitsForGraph) {
					cout << i.first << " " << i.second << endl;
				}
				*/
				unordered_map<string,string>::iterator itr2 = waitsForGraph.begin();
				while (itr2 != waitsForGraph.end()) {
					if (itr2 -> first == transactionName) {
						//cout << "Found " << itr2 -> first << " waits on " << itr2 -> second << " and deleted" << endl;
						itr2 = waitsForGraph.erase(itr2);
					}
					else if (itr2 -> second == transactionName) {
						setTransactionStatus(itr2 -> first, Running); 
						//cout << "Set " << itr2 -> first << " to running" << endl;
						//cout << "Found " << itr2 -> first << " waits on " << itr2 -> second << " and deleted" << endl;
						itr2 = waitsForGraph.erase(itr2);
					}
					else {
						itr2++;
					}
				}
			}
		}
	}
	else {
		return;
	} 
}

bool TransactionManager::detectDeadlock() {
	set<int> vertices;
	unordered_map<string, string>::iterator itr = waitsForGraph.begin();
	while (itr != waitsForGraph.end()) {
		char s1 = itr -> first.at(1);
		char s2 = itr -> second.at(1);
		int x = s1 - '0';
		int y = s2 - '0';
		itr++;
	}
	Graph g(4);
	for (auto i : waitsForGraph) {
		char s1 = i.first.at(1);
		char s2 = i.second.at(1);
		int x = s1 - '0';
		int y = s2 - '0';
		//cout << "Added edge from " << x << " to " << y << endl;
		g.addEdge(x, y);
	}
	if ( g.isCyclic() ) {
		return true;
	}
	else {
		return false;;
	}
}

string TransactionManager::detectYoungestInDeadlock() {
	string youngestTransaction = "";
	if (! detectDeadlock() ) {
		cout << "Error: No deadlock detected" << endl;
		return youngestTransaction;
	}
	else {
		cout << "Deadlock detected" << endl; 
		/*
		for (auto i : waitsForGraph) {
			cout << "Edge from " << i.first << " to " << i.second << endl;
		}
		*/
		unordered_map<string, Transaction*>::iterator itr = transactionMap.begin();
		int maxStartTime = INT_MIN;
		while (itr != transactionMap.end()) {
			//cout << itr -> second -> getTransactionName() << endl;
			if (itr -> second -> getTransactionStartTime() > maxStartTime ) {
				maxStartTime = itr -> second -> getTransactionStartTime();
				youngestTransaction = itr -> second -> getTransactionName();
			}
			itr++;
		}
		return youngestTransaction;
	}
}