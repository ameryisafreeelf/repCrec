#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <climits>
#include <vector>
#include <unordered_map>

#include "TransactionManager.cpp"
//#include "Transaction.cpp"
//#include "Operation.cpp"
//#include "Data.cpp"
#include "SiteManager.cpp"
//#include "Site.cpp"
//#include "Lock.cpp"

using namespace std;

//enum Operator{R, W, begin, beginRO, end, dump, fail, recover}; 

// Global time
static int globalTime = 0;
static TransactionManager tm;
static SiteManager sm;      

// Stores opperations and executes them in FIFO order
static vector<Operation*> opsVector;

// Keeps record of  all data read to by transactions
// Needed for aborting transactions that read from sites that subsequently fail
// Key is transactionName, string is dataName
static unordered_map< string, string > dataTransactionReads;

// Hash map for read only transactions- saves a copy of every data item for RO-Ts
// Key of outer layer is transaction name, key of inner layer is Data name
unordered_map< string, unordered_map<string, int> > readOnlyData;

// A hash map from string to operator, used in parseOperationString()
unordered_map<string, Operator> opMap = {
    {"R", Operator::R},
    {"W", Operator::W},
    {"begin", Operator::begin},
    {"beginRO", Operator::beginRO},
    {"end", Operator::end},
    // dump, fail, and recover don't return Operations, they just execute methods   
    {"dump", Operator::dump},         
    {"fail", Operator::fail},         
    {"recover", Operator::recover}
};

// Functions for trimming input strings
string& ltrim(string& str, const string& chars = "\t\n\v\f\r ") {
    str.erase(0, str.find_first_not_of(chars));
    return str;
}
 
string& rtrim(string& str, const string& chars = "\t\n\v\f\r ") {
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}
 
string& trim(string& str, const string& chars = "\t\n\v\f\r ") {
    return ltrim(rtrim(str, chars), chars);
}

void parseOperationString(string s) {
    char* cp = &s[0];
    // Intermediary buffer variables
    string operatorBuff = "";
    string valueBuff = "";
    string siteIndexBuff = "";
    string dataNameBuff = "";
    // Empty holder variables
    int value;
    int dataIndex;
    int siteIndex;
    string dataName = "";
    string transactionName = "";
    Operation* operation;
    // Stream object to convert strings to ints
    stringstream stream;
    while (*cp != '(') {
        operatorBuff += *cp;
        cp++;
    }
    operatorBuff = trim(operatorBuff);
    // Now we have the operator
    Operator op = opMap[operatorBuff];    
    switch(op) {
        // begin
        case Operator::begin :
            cp++;
            while ( *cp != ')' ) {
                transactionName += *cp;
                cp++;
            }
            //cout << op << "(" << transactionName << ")" << endl;            
            operation = new Operation(op, trim(transactionName) );
            break;   
        // beginRO 
        case Operator::beginRO :
            cp++;
            while ( *cp != ')' ) {
                transactionName += *cp;
                cp++;
            }
            //cout << op << "(" << transactionName << ")" << endl;
            operation = new Operation(op, trim(transactionName) );
            break;   
        // end
        case Operator::end :
            //cout << "Should print here if I have end" << endl;
            cp++;
            while ( *cp != ')' ) {
                transactionName += *cp;
                cp++;
            }
            //cout << op << "(" << transactionName << ")" << endl;
            operation = new Operation(op, trim(transactionName) );                       
            break;
        // dump
        case Operator::dump :
            //cout << op << endl;
            operation = new Operation(op);                        
            break;
        // fail
        case Operator::fail :
            cp++;
            while (*cp != ')') {
                siteIndexBuff += *cp;
                cp++;
            } 
            stream.str( trim(siteIndexBuff) );
            stream >> siteIndex;
            //cout << op << "(S" << siteIndexBuff << ")" << endl;
            operation = new Operation(op, siteIndex);           
            break;
        // recover
        case Operator::recover :
            cp++;
            while (*cp != ')') {
                siteIndexBuff += *cp;
                cp++;
            }
            stream.str( trim(siteIndexBuff) );
            stream >> siteIndex;
            //cout << op << "(S" << siteIndexBuff << ")" << endl;
            operation = new Operation(op, siteIndex);                                                
            break;      
        // R
        case Operator::R :
            cp++;
            while ( *cp != ',' ) {
                transactionName += *cp;
                cp++;
            }
            cp++;
            while ( *cp != ')' ) {
                dataNameBuff += *cp; 
                cp++;
            }
            stream.str( trim(dataNameBuff) );
            stream >> dataName;
            //cout << "Here " << transactionName << endl;
            //cout << op << "(" << transactionName << ", " << dataName << ")" << endl;
            operation = new Operation(op, trim(transactionName), dataName);
            break;
        // W
        case Operator::W :
            //cout << "Do I identify a W operator" << endl;
            cp++;
            while (*cp != ',') {
                transactionName += *cp;
                cp++;
            }
            cp++;
            while (*cp != ',') {
                dataName += *cp;
                cp++;
            }
            cp++;            
            while (*cp != ')') {
                valueBuff += *cp;
                cp++;
            }
            stream.str( trim(valueBuff) );
            stream >> value;                        
            operation = new Operation(op, trim(transactionName), trim(dataName), value);
            //cout << "Created write ok " << endl;
            break;
        default:
            //cout << "Something's wrong " << endl;
            break;
    }
    //cout << "Adding " << operation -> getOperator() << " to opsVector" << endl;
    opsVector.push_back(operation);    
} 

void beginTransaction(Operation* op) {
    string transactionName = op -> getTransactionName();
    tm.begin(transactionName, globalTime, op);
    cout << "Begin: " << transactionName << " at time " << globalTime << endl;
}

void beginReadOnlyTransaction(Operation* op) {
    string transactionName = op -> getTransactionName();
    // Segfault
    unordered_map<string,int> readOnlyValues = sm.getReadOnlyCopies();
    readOnlyData.insert(make_pair (transactionName, readOnlyValues) );
    tm.beginRO(transactionName, globalTime, op);
    cout << "BeginRO: " << transactionName << " at time " << globalTime << endl;    
}

void endTransaction(Operation* op) {
    string transactionName = op -> getTransactionName();
    // A transaction can't end if it has operations in waiting
    if ( tm.isTransactionWaiting(transactionName) ) {
        cout << "Can't end " << transactionName << " because it is waiting" << endl;
        //cout << "The size of the opsVector is " << opsVector.size() << endl;
        if (tm.detectDeadlock()) {
            string toBeKilled = tm.detectYoungestInDeadlock();
            tm.abortTransaction(toBeKilled);
            cout << "Killing youngest transaction: " << toBeKilled << endl;
            cout << "Abort: " << toBeKilled << " at time " << globalTime << " due to deadlock" << endl;    
            for (Data* d : tm.findTransaction(toBeKilled) -> writeSet ) {
                sm.revertDataAtAllSitesToLastCommitted(d -> getName());
            }
        }
        
        opsVector.push_back(op);

        //cout << "Added operation " << op -> getOperator() << " " << op -> getTransactionName() << endl;
        //cout << "The size of the opsVector is " << opsVector.size() << endl;
        return;
    }
    // Here
    for (auto i : tm.operationsWaitList) {
        if (i -> getTransactionName() == transactionName) {
            //cout << "Can't end " << transactionName << " because it has pending operations" << endl;
            //cout << "The size of the opsVector is " << opsVector.size() << endl;
            //cout << "Added operation " << op -> getOperator() << " " << op -> getTransactionName() << endl;
            opsVector.push_back(op);
            //cout << "The size of the opsVector is " << opsVector.size() << endl;
            return;
        }
    }
    tm.end(transactionName, globalTime, op);
    transactionStatus finalStatus = tm.findTransaction(transactionName) -> getTransactionStatus();
    if (finalStatus == Committed) {
        cout << "End: " << transactionName << " at time " << globalTime << " (Committed)" << endl;    
        tm.findTransaction(transactionName) -> setTransactionEndTime(globalTime);
        tm.commitTransaction(transactionName);
        sm.updateDataAtAllSitesLastCommitted(transactionName);
        // Here
        for (Data* d : tm.findTransaction(transactionName) -> writeSet ) {
            sm.issueUnlockOnAllSites(d -> getName());
        }
        for (auto i : dataTransactionReads) {
            if (i.first == transactionName) {
                sm.issueUnlockOnAllSites(i.second);
            }
        }
        // Here
    }
    else if (finalStatus == Aborted) {
        tm.findTransaction(transactionName) -> setTransactionEndTime(globalTime);
        cout << "End: " << transactionName << " at time " << globalTime << " (Aborted)" << endl;    
        tm.abortTransaction(transactionName);
        // Here
        for (Data* d : tm.findTransaction(transactionName) -> writeSet ) {
            sm.revertDataAtAllSitesToLastCommitted(d -> getName());
            sm.issueUnlockOnAllSites(d -> getName());
        }
        for (auto i : dataTransactionReads) {
            if (i.first == transactionName) {
                sm.issueUnlockOnAllSites(i.second);
            }
        }
    }
    else {
        cout << "Something went wrong. The transaction should end on Abort or Commit." << endl;   
    }
     
    // Release locks for transaction
}

int executeRead(Operation* op) {
    string transactionName = op -> getTransactionName();
    if ( tm.findTransaction(transactionName) -> getTransactionStatus() == Aborted) {
        return INT_MIN;        
    }
    string dataName = op -> getDataName();
    int dataIndex = op -> getDataIndex();
    // Returns non-replicated Data item from Normal site
    Data* d = sm.getReadableDataItem(dataIndex, dataName);
    // Returns nullptr if all sites containing that Data item have failed
    if (! d) {
        return INT_MIN;
    }
    else {
        // Otherwise, let tm take over handling locks
        if ( tm.findTransaction(transactionName) -> getIsReadOnly() ) {
            return readOnlyData[transactionName][dataName];
        }
        else if ( d -> isWriteLocked() && (d -> getLockingTransaction() != transactionName ) ) {
            //cout << "Attempting Read on " << dataName << " by " << transactionName << endl;
            cout << "The data item is currently locked by " << d -> getLockingTransaction() << endl;
            tm.addOperationToWaitList(op, d -> getLockingTransaction());
            cout << transactionName << " is blocked by " << d -> getLockingTransaction() << endl;            
            return INT_MIN;
        }
        else {
            sm.issueLockOnAllSites(dataName, transactionName, LockType::RO); 
            cout << transactionName << " successfully issues RO lock on " << op -> getDataName() << endl;
            int x = tm.transactionIssuesRead(op, d);               
            cout << transactionName << " reads " << op -> getDataName() << endl;                      
            return x;
        }
    }
}

void executeWrite(Operation* op) {
    string transactionName = op -> getTransactionName();  
    if ( tm.findTransaction(transactionName) -> getTransactionStatus() != Running) {
        cout << "Error: Attempt W by a Transaction that isn\'t running" << endl;
        return;        
    }      
    string dataName = op -> getDataName();
    int dataIndex = op -> getDataIndex();
    int value = op -> getWriteValue();
    // Returns non-replicated Data item from Normal site
    Data* d = sm.getReadableDataItem(dataIndex, dataName);  
    // Returns nullptr if all sites containing that Data item have failed
    if (! d) {
        // Abort
        return;
    }     
    else {
        if ( ( d -> isWriteLocked() || d -> isReadLocked() ) && (d -> getLockingTransaction() != transactionName)) {
            //cout << "Attempting Write on " << dataName << " by " << transactionName << endl;
            cout << "The data item is currently locked by " << d -> getLockingTransaction() << endl;
            tm.addOperationToWaitList(op, d -> getLockingTransaction());
            cout << transactionName << " is blocked by " << d -> getLockingTransaction() << endl;
            return;            
        } 
        else if ( (d -> isReadLocked() ) && (d -> getLockingTransaction() == transactionName) ) {
            cout << "Promoted lock on " << d -> getName() << " to RW lock" << endl;
            sm.issuePromoteLockOnAllSites(d -> getName());
        }
        else {
            sm.issueLockOnAllSites(dataName, transactionName, LockType::RW);                
            cout << transactionName << " successfully issues RW lock on " << op -> getDataName() << endl;
            tm.transactionIssuesWrite(op, d);  
            cout << transactionName << " writes " << op -> getWriteValue() << " on " << op -> getDataName() << endl;                      
            sm.writeToAllSites(dataIndex, dataName, transactionName, value);
        }
    }
}

void executeFail(Operation* op) {
    int siteIndex = op -> getSiteIndex();
    sm.fail( siteIndex );
    set<string>* writtenToFailedSite = sm.getSiteByIndex(siteIndex) -> getTransactionsWrittenToSite(); 
    /*
    for (auto s : *writtenToFailedSite) {
        cout << "Hello " << s << endl;
    }
    */
    //cout << "Size of writtenToFailedSite is " << writtenToFailedSite -> size() << endl;
    if (writtenToFailedSite -> size() != 0) { 
        set<string>::iterator itr = writtenToFailedSite -> begin();
        while ( itr != writtenToFailedSite -> end() ) {
            if (tm.getTransactionStatus(*itr) != Committed ) { 
                tm.abortTransaction(*itr);
                cout << *itr << " aborted because it wrote to failed Site " << siteIndex << endl;
                for (Data* d : tm.findTransaction(*itr) -> writeSet ) {
                    sm.revertDataAtAllSitesToLastCommitted(d -> getName());
                    sm.issueUnlockOnAllSites(d -> getName());
                }
                itr++;    
            }
            else {
                itr++;
            }
        }
    }
    if (dataTransactionReads.empty()) {
        return;
    }
    else {
        unordered_map<string,string>::iterator itr2 = dataTransactionReads.begin();
        while ( itr2 != dataTransactionReads.end() ) {
            int dataIndex = 0;
            char c = (itr2 -> second).at(1);
            dataIndex = c - '0';
            if (dataIndex % 2 != 0) {
                if (siteIndex == (dataIndex + 1) ) {
                    cout << itr2 -> first << " aborted because it read from failed site " << siteIndex << endl;
                    tm.abortTransaction(itr2 -> first);
                    for (Data* d : tm.findTransaction(itr2 -> first) -> writeSet ) {
                        sm.revertDataAtAllSitesToLastCommitted(d -> getName());
                        sm.issueUnlockOnAllSites(d -> getName());
                    }
                }
                dataTransactionReads.erase(itr2 -> first);
            }
            itr2++;
        }
    }
}

void executeRecover(Operation* op) {
    int siteIndex = op -> getSiteIndex();
    sm.recover( siteIndex );
}

int main(int argc, char* argv[]) {

    // Initialize size
    sm.initializeSites();

/*
    cout << "Number of sites in siteManager " << sm.getNumOfSites() << endl;
    cout << "Site 1 has: " << sm.getSiteByIndex(1) -> getNumOfDataItems() << endl;
    cout << "Site 2 has: " << sm.getSiteByIndex(2) -> getNumOfDataItems() << endl;
    cout << "Site 3 has: " << sm.getSiteByIndex(3) -> getNumOfDataItems() << endl;
    cout << "Site 4 has: " << sm.getSiteByIndex(4) -> getNumOfDataItems() << endl;
    cout << "Site 5 has: " << sm.getSiteByIndex(5) -> getNumOfDataItems() << endl;
    cout << "Site 6 has: " << sm.getSiteByIndex(6) -> getNumOfDataItems() << endl;
    cout << "Site 7 has: " << sm.getSiteByIndex(7) -> getNumOfDataItems() << endl;
*/

    // Parse the input into strings, each string is an operation
	string operationString;
    ifstream inFile;
    inFile.open(argv[1]);
    if (! inFile) {
        cerr << "Unable to open file";
        exit(1);
    }

    // Loop processes input strings into Operations and loads them into opsVector
    while (! inFile.eof() ) {  
        getline(inFile, operationString);
        parseOperationString(operationString);
	}
    
    /*
    // Check operators to prove we've read the file correctly and that parsing worked
    for (auto i : opsVector)
        cout << i -> getOperator() << " " << endl;
        cout << "\n\n\n\n" << endl;
    */
    
    // This works
    //cout << sm.getSiteByIndex(6) -> getDataItem("s2") -> getName() << endl;

    /* 
    // Testing
    cout << sm.getNumOfSites() << endl;
    Data* d = sm.getReadableDataItem(2, "x2");
    cout << d -> getFullName() << endl;
    sm.issueLockOnAllSites("x2", "T2", LockType::RO);
    cout << sm.getSiteByIndex(2) -> isReadLockOnData("x2") << endl;
    cout << d -> isReadLocked() << endl;
    */

    //dataTransactionReads.clear();
    for (auto i : dataTransactionReads) {
        cout << "Ass" << endl;
        cout << i.first << " " << i.second << endl;
    }


    cout << "\n";
    cout << "********************\n"; 
    cout << "Transaction History:\n";
    cout << "********************\n" << endl;

    // Run every operation until opsVector is empty
    while ( ! opsVector.empty() ) {
        //cout << "Size of opsVector: " << opsVector.size() << endl; 
        globalTime++;
        Operator currOp = opsVector.front() -> getOperator();
        //cout << "The current op is " << currOp << endl;
        switch( currOp ) {
            case Operator::begin: {
                //cout << "Begin operation " << currOp << endl;            
                beginTransaction( opsVector.front() );
                break;
            }
            case Operator::beginRO: {
                //cout << "BeginRO operation " << currOp << endl;
                beginReadOnlyTransaction( opsVector.front() );
                break;
            }
            case Operator::end: {
                endTransaction( opsVector.front() );
                if (! tm.operationsWaitList.empty()) {
                    Operation* firstWaitingOp = tm.getFirstWaitingOperation();
                    opsVector.insert( (opsVector.begin() + 1), firstWaitingOp);
                    tm.setTransactionStatus( (opsVector.at(1) ) -> getTransactionName(), Running );
                }
                break;  
            }
            case Operator::R: {
                string transactionName = opsVector.front() -> getTransactionName();
                if ( tm.findTransaction(transactionName) -> getIsReadOnly() ) {
                    cout << "R: " << opsVector.front() -> getDataName() << " by " << opsVector.front() -> getTransactionName() << " (Read-Only)" << endl;
                }
                else if ( tm.findTransaction(transactionName) -> getTransactionStatus() == Aborted ){
                     cout << "(Aborted) R: " << opsVector.front() -> getDataName() << " by " << opsVector.front() -> getTransactionName() << endl;   
                }
                else {
                    cout << "R: " << opsVector.front() -> getDataName() << " by " << opsVector.front() -> getTransactionName() << endl;
                    executeRead( opsVector.front() );
                    if (tm.detectDeadlock()) {
                        string toBeKilled = tm.detectYoungestInDeadlock();
                        tm.abortTransaction(toBeKilled);
                        cout << "Killing youngest transaction: " << toBeKilled << endl;
                        cout << "Abort: " << toBeKilled << " at time " << globalTime << " due to deadlock" << endl;    
                        for (Data* d : tm.findTransaction(toBeKilled) -> writeSet ) {
                            sm.revertDataAtAllSitesToLastCommitted(d -> getName());
                        }
                    }
                    else {
                        dataTransactionReads[transactionName] = opsVector.front() -> getDataName();
                    }
                }
                //cout << "Looking at " << sm.getSiteByIndex(1) -> getDataItem("x2") -> getFullName() << endl;          
                //cout << "Is readLock on " << sm.getSiteByIndex(2) ->  getDataItem("x2") -> getFullName() << ": " << sm.getSiteByIndex(2) -> isReadLockOnData("x2") << endl;
                //cout << "Is readlocked? " <<  sm.getSiteByIndex(1) -> getDataItem("x2") -> isAnyLocked() << endl;
                break;
            }
            case Operator::W: {
                string transactionName = opsVector.front() -> getTransactionName();
                if ( tm.findTransaction(transactionName) -> getTransactionStatus() == Aborted ) {
                    cout << "(Aborted) W: " << opsVector.front() -> getWriteValue() << " on " << opsVector.front() -> getDataName() << " by " << opsVector.front() -> getTransactionName() << endl;   
                }               
                else {
                    cout << "W: " <<  opsVector.front() -> getWriteValue() << " on " << opsVector.front() -> getDataName() << " by " << opsVector.front() -> getTransactionName() << endl;
                    executeWrite( opsVector.front() );
                    if (tm.detectDeadlock()) {
                        string toBeKilled = tm.detectYoungestInDeadlock();
                        tm.abortTransaction(toBeKilled);
                        cout << "Abort: " << toBeKilled << " at time " << globalTime << " due to deadlock" << endl;    
                        Operation* killOp = new Operation(Operator::end, transactionName);
                        tm.end(toBeKilled, globalTime, killOp);
                        for (Data* d : tm.findTransaction(toBeKilled) -> writeSet) {
                            //cout << "Unlocking " << d -> getName() << endl;
                            sm.issueUnlockOnAllSites(d -> getName());
                            sm.revertDataAtAllSitesToLastCommitted(d -> getName());
                        }
                        // Here- unlock read locks
                        for (auto i : dataTransactionReads) {
                            if (i.first == transactionName) {
                                sm.issueUnlockOnAllSites(i.second);
                            }
                        }
                        /*
                        Operation* firstWaitingOp = tm.getFirstWaitingOperation();
                        opsVector.insert( (opsVector.begin() + 1), firstWaitingOp );
                        string transactionNameOfNextOp;
                        transactionNameOfNextOp = opsVector.at(1) -> getTransactionName();
                        //cout << tm.findTransaction(transactionNameOfNextOp) -> getTransactionName() << " set to Running" << endl;
                        tm.findTransaction(transactionNameOfNextOp) -> setTransactionStatus(Running);
                        */
                    }
                    else {
                        break;
                    }
                    // Here

                }
                break;
            }
            case Operator::recover: {
                executeRecover( opsVector.front() );          
                break;
            }
            case Operator::fail: {
                executeFail( opsVector.front() );

                break;
            }
            case Operator::dump: {
                if (opsVector.size() != 1) {
                    Operation* delay = opsVector.front();
                    opsVector.push_back(delay);
                    break;
                }
                else { 
                    cout << "\n";
                    sm.dumpAllSites();
                    break;
                }
            }
            default: {
                cout << "Something's wrong" << endl;
                break;
            }
        }
        opsVector.erase( opsVector.begin() );
    }
        
}
