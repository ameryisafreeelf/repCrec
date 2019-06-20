#include "catch.hpp"

#include "TransactionManager.cpp"

TEST_CASE( "Initialize transactionManager" ) {
	TransactionManager tm;
	REQUIRE( tm.getNumOfTransactions() == 0 );
}

TEST_CASE( "Begin transaction" ) {
	TransactionManager tm;
	Transaction* t1 = new Transaction("T1", 1);	
	Operation* op = new Operation(Operator::begin, "T1");
	tm.begin("T1", 1, op);
	REQUIRE( tm.findTransaction("T1") );
	REQUIRE( tm.getNumOfTransactions() == 1 );

	SECTION( "Change transaction status" ) {
		tm.setTransactionStatus("T1", transactionStatus::Waiting);
		REQUIRE( tm.getTransactionStatus("T1") == Waiting );
	}
}

TEST_CASE( "Add wait operation to transactionManager" ) {
	TransactionManager tm;
	Transaction* tran = new Transaction("T1", 1);	
	Operation* op = new Operation(Operator::begin, "T1");
	tm.begin("T1", 1, op);


	SECTION( "Should work even if we haven't added Transaction to transactionMap" ) {
		Operation* op = new Operation(Operator::R, "T1", "X1");
		tm.addOperationToWaitList(op, "T1");
		REQUIRE( tm.getNumOfWaitingOperations() == 1);
	}

	SECTION( "If transaction is waiting and it ends, it should abort" ) {
		Operation* op2 = new Operation(Operator::R, "T1", "X1");		
		Operation* op3 = new Operation(Operator::end, "T1", "X1");
		REQUIRE( tm.getTransactionStatus("T1") == transactionStatus::Running );				
		tm.addOperationToWaitList(op2, "T1");
		REQUIRE( tm.getNumOfWaitingOperations() == 1);		
		REQUIRE( tm.getTransactionStatus("T1") == transactionStatus::Waiting );				
		tm.end("T1", 100, op3);
		REQUIRE( tm.getTransactionStatus("T1") == transactionStatus::Aborted );				
	}


	SECTION( "If transaction is running and it ends, it should commit" ) {
		Operation* op2 = new Operation(Operator::R, "T1", "X1");		
		Operation* op3 = new Operation(Operator::end, "T1", "X1");
		REQUIRE( tm.getTransactionStatus("T1") == transactionStatus::Running );				
		REQUIRE( tm.getNumOfWaitingOperations() == 0);		
		tm.end("T1", 100, op3);
		REQUIRE( tm.getTransactionStatus("T1") == transactionStatus::Committed );				
	}	

}
