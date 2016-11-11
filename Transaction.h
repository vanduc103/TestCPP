/*
 * Transaction.h
 *
 *  Created on: Nov 11, 2016
 *      Author: duclv
 */

#ifndef TRANSACTION_H_
#define TRANSACTION_H_

#include <chrono>
#include <vector>

namespace std {

class Transaction {
public:
	enum TRANSACTION_STATUS {PENDING, STARTED, COMMITED, ABORTED};
	struct transaction {
		uint64_t txnId;		// transaction id
		uint64_t csn;		// commit squence number
		uint64_t startTs;	// start time stamp
		TRANSACTION_STATUS status;
	};
	static vector<transaction>* vecTransaction;

private:



public:
	Transaction();
	virtual ~Transaction();

	size_t createTx();

	void startTx(size_t txIdx);

	uint64_t getTimestampAsCSN();
	void commitTx(size_t txIdx, uint64_t csn);

	void abortTx(size_t txIdx);
};

} /* namespace std */

#endif /* TRANSACTION_H_ */
