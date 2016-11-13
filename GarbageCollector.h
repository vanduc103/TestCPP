/*
 * GarbageCollector.h
 *
 *  Created on: Nov 13, 2016
 *      Author: duclv
 */

#ifndef GARBAGECOLLECTOR_H_
#define GARBAGECOLLECTOR_H_

#include <vector>
#include <stdexcept>
#include "Transaction.h"
#include "ColumnBase.h"
#include "Column.h"

namespace std {

class GarbageCollector {
private:
	// list of recently updated rids
	vector<size_t> recentlyUpdatedRids;

public:
	GarbageCollector() {
		recentlyUpdatedRids = vector<size_t>(0);
	}
	virtual ~GarbageCollector();

	void updateRecentlyUpdateRids(vector<size_t> updateRids) {
		recentlyUpdatedRids.insert(recentlyUpdatedRids.end(), updateRids.begin(), updateRids.end());
	}

	// run garbage collection
	void run(vector<Transaction::transaction> vecActiveTx, ColumnBase* colBase) {
		if (colBase->getType() == ColumnBase::intType) {
			Column<int>* col = (Column<int>*) colBase;
			// from rid -> update Version space to Data space
			// -> delete old versions
			for (size_t i = 0; i < recentlyUpdatedRids.size(); i++) {
				size_t rid = recentlyUpdatedRids.at(i);
				// update Version space to Data space
				col->updateVersionSpace2DataSpace(rid);
				// check active Transaction to delete old versions
				for (size_t k = 0; k < vecActiveTx.size(); k++) {
					Transaction::transaction tx = vecActiveTx.at(k);
					vector<size_t> vecRid = tx.vecRid;
					bool beUsing = false;
					for (size_t m = 0; m < vecRid.size(); m++) {
						if (vecRid.at(m) == rid) {
							beUsing = true; break;
						}
					}
					if (beUsing) {
						col->removeOldVersion(rid, tx.startTs);
					}
				}
			}
		}
	}
};

} /* namespace std */

#endif /* GARBAGECOLLECTOR_H_ */
