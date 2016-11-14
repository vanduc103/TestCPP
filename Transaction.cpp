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

vector<Transaction::transaction>* Transaction::vecTransaction = new vector<Transaction::transaction>();
vector<size_t>* Transaction::vecActiveTransaction = new vector<size_t>();

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
	//copy to active transaction
	vecActiveTransaction->push_back(txIdx);
}

void Transaction::updateRid2Transaction(size_t txIdx, vector<size_t> vecRid) {
	transaction tx = vecTransaction->at(txIdx);
	tx.vecRid = vecRid;
}

uint64_t Transaction::getStartTimestamp(size_t txIdx) {
	transaction tx = vecTransaction->at(txIdx);
	return tx.startTs;
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
	// remove in active transaction
	for (size_t i = 0; i < vecActiveTransaction->size(); i++) {
		if (vecActiveTransaction->at(i) == txIdx) {
			vecActiveTransaction->erase(vecActiveTransaction->begin() + i);
			break;
		}
	}
}

void Transaction::abortTx(size_t txIdx) {
	transaction tx = vecTransaction->at(txIdx);
	tx.status = TRANSACTION_STATUS::ABORTED;
	tx.csn = 0;
	vecTransaction->at(txIdx) = tx;
	// remove in active transaction
	for (size_t i = 0; i < vecActiveTransaction->size(); i++) {
		if (vecActiveTransaction->at(i) == txIdx) {
			vecActiveTransaction->erase(vecActiveTransaction->begin() + i);
			break;
		}
	}
}

vector<size_t> Transaction::listActiveTransaction() {
	return *vecActiveTransaction;
}

Transaction::transaction Transaction::getTransaction(size_t txIdx) {
	return vecTransaction->at(txIdx);
}

} /* namespace std */
