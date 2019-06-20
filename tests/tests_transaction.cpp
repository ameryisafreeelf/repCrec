#include "catch.hpp"

#include "Transaction.cpp"

TEST_CASE( "Create a Transaction" ){
	Transaction* myTransaction = new Transaction("T1", 1);

	SECTION( "Status of transaction defaults to created" ) {
		REQUIRE( myTransaction -> getTransactionStatus() == Created );
	}

	SECTION( "Transaction is not read-only by default" ) {
		REQUIRE( myTransaction -> getIsReadOnly() == false );
	}

	SECTION( "Transaction has startTime that it was instantiated with" ) {
		REQUIRE( myTransaction -> getTransactionStartTime() == 1 );
	}

	SECTION( "Transaction has empty waitingOpList by default" ) {
		REQUIRE( myTransaction -> getNumOfWaitingOperations() == 0 );
	}
}

TEST_CASE( "Add and remove operation pointer to a Transaction's pendingOpList" ) {
	Transaction* myTransaction = new Transaction("T1", 1);
	Operation* myOp = new Operation(Operator::R, "T1");
	myTransaction -> addOperationToPending(myOp);

	SECTION( "Adding an operation to the Transaction's opList" ) {
		Operation* myOp2 = new Operation(Operator::W, "T1");
		myTransaction -> addOperationToPending(myOp2);
		REQUIRE( myTransaction -> getNumOfWaitingOperations() == 2 );
	}

	SECTION( "Removing an operation from the Transaction's opList" ) {
		Operation* myOp3 = new Operation(Operator::W, "T1");
		myTransaction -> addOperationToPending(myOp3);
		myTransaction -> removeOperationFromPending(myOp3);
		REQUIRE( myTransaction -> getNumOfWaitingOperations() == 1 );
		myTransaction -> removeOperationFromPending(myOp);
		REQUIRE( myTransaction -> getNumOfWaitingOperations() == 0 );
	}
}

TEST_CASE( "Update the status of a transaction" ) {
	Transaction* myTransaction = new Transaction("T1", 1);
	myTransaction -> setTransactionStatus(Running);
	REQUIRE( myTransaction -> getTransactionStatus() == Running );	
} 
