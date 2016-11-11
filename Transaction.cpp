/*
 * Transaction.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: duclv
 */

#include "Transaction.h"

namespace std {

Transaction::Transaction() {
}

Transaction::~Transaction() {
}

vector<Transaction::transaction>* Transaction::vecTransaction;

size_t Transaction::createTx() {
	transaction newTx;
	// current time in miliseconds
	newTx.txnId = chrono::duration_cast < chrono::milliseconds
			> (chrono::steady_clock::now().time_since_epoch()).count();
	newTx.startTs = 0;
	newTx.csn = 0;
	newTx.status = TRANSACTION_STATUS::PENDING;
	vecTransaction->push_back(newTx);

	// return index to this new transaction
	return vecTransaction->size() - 1;
}

void Transaction::startTx(size_t txIdx) {
	transaction tx = vecTransaction->at(txIdx);
	tx.status = TRANSACTION_STATUS::STARTED;
	tx.startTs = chrono::duration_cast < chrono::milliseconds
			> (chrono::steady_clock::now().time_since_epoch()).count();
	vecTransaction->at(txIdx) = tx;
}

uint64_t Transaction::getTimestampAsCSN() {
	return chrono::duration_cast < chrono::milliseconds
			> (chrono::steady_clock::now().time_since_epoch()).count();
}

void Transaction::commitTx(size_t txIdx, uint64_t csn) {
	transaction tx = vecTransaction->at(txIdx);
	tx.status = TRANSACTION_STATUS::COMMITED;
	tx.csn = csn;
	vecTransaction->at(txIdx) = tx;
}

void Transaction::abortTx(size_t txIdx) {
	transaction tx = vecTransaction->at(txIdx);
	tx.status = TRANSACTION_STATUS::ABORTED;
	tx.csn = 0;
	vecTransaction->at(txIdx) = tx;
}

} /* namespace std */
