/*
 * Column.h
 *
 *  Created on: Sep 23, 2016
 *      Author: duclv
 */
#include <vector>

#ifndef COLUMN_H_
#define COLUMN_H_

#include <math.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <algorithm>
#include "ColumnBase.h"
#include "Dictionary.h"
#include "PackedArray.h"

namespace std {

template<typename T>
class Column: public ColumnBase {
private:
	// value vector for column
	vector<size_t>* vecValue;
	// bit packing
	PackedArray* packed;
	// dictionary vector for column
	Dictionary<T>* dictionary;
	// bulk insert ?
	bool bulkInsert = false;

	// DATA SPACE
	struct data_column {
		bool versionFlag;
		size_t encodedValue;
		uint64_t csn;
	};
	vector<data_column>* dataColumn;

	// VERSION SPACE
	vector<size_t>* versionVecValue;
	struct version_column {
		size_t encodedValue;
		uint64_t csn;
		version_column* next;
	};
	vector<version_column>* versionColumn;
	Dictionary<T>* deltaSpace;
	map<size_t, size_t>* hashtable;
public:
	Column() {
		dictionary = new Dictionary<T>();
		vecValue = new vector<size_t>();
		packed = new PackedArray();
		dataColumn = new vector<data_column>();
		versionVecValue = new vector<size_t>();
		versionColumn = new vector<version_column>();
		deltaSpace = new Dictionary<T>();
		hashtable = new map<size_t, size_t>();
	}
	virtual ~Column() {
		delete vecValue;
		delete dictionary;
		PackedArray_destroy(packed);
		delete dataColumn;
		delete versionVecValue;
		delete versionColumn;
		delete deltaSpace;
		delete hashtable;
	}

	vector<size_t>* getVecValue() {
		if (vecValue == NULL) {
			vecValue = new vector<size_t>();
		}
		vecValue->clear();
		for (int i = 0; i < packed->count; i++) {
			vecValue->push_back(PackedArray_get(packed, i));
		}
		return vecValue;
	}

	size_t vecValueSize() {
		return packed->count;
	}

	size_t vecValueAt(size_t index) {
		if (index < 0 || index >= packed->count) {
			return -1; // indicate no result
		}
		return PackedArray_get(packed, index);
	}

	void printVecValue(int row) {
		vecValue = getVecValue();
		for (size_t i = 0; i < (*vecValue).size() && i < row; i++) {
			cout << "vecValue[" << i << "] = " << (*vecValue)[i] << "\n";
		}
		// free vecValue
		vecValue->resize(0);
	}

	void bitPackingVecValue() {
		// #bit to represent encode dictionary value
		size_t numOfBit = (size_t) ceil(log2((double) dictionary->size()));
		// init bit packing array
		packed = PackedArray_create(numOfBit, vecValue->size());

		for (size_t i = 0; i < vecValue->size(); i++) {
			size_t value = vecValue->at(i);
			PackedArray_set(packed, i, value);
		}
		// free vecValue
		vecValue->resize(0);
	}

	vector<size_t>* unpackingVecValue() {
		if (vecValue == NULL) {
			vecValue = new vector<size_t>();
		}
		vecValue->clear();
		for (int i = 0; i < packed->count; i++) {
			vecValue->push_back(PackedArray_get(packed, i));
		}
		return vecValue;
	}

	void updateVecValue(T& value) {
		// used for column has few duplicate values
		vecValue->push_back(value);
	}

	Dictionary<T>* getDictionary() {
		if (dictionary == NULL) {
			dictionary = new Dictionary<T>();
		}
		return dictionary;
	}

	// Update new value for dictionary
	void updateDictionary(T& value, bool sorted = true, bool bulkInsert = true) {
		// no bulk insert if no sort
		if (!sorted) bulkInsert = false;
		this->bulkInsert = bulkInsert;

		dictionary->addNewElement(value, vecValue, sorted, bulkInsert);
	}

	bool isBulkInsert() {
		return bulkInsert;
	}
	void bulkBuildVecVector() {
		// bulk insert -> update vecValue after building entire dictionary
		vecValue->resize(0);
		vector<T>* bulkVecValue = dictionary->getBulkVecValue();
		if (bulkVecValue != NULL) {
			for (size_t i = 0; i < bulkVecValue->size(); i++) {
				// find position of valueId in dictionary
				vector<size_t> result;
				dictionary->search(bulkVecValue->at(i), ColumnBase::equalOp, result);
				size_t pos = result[0];
				if (pos != -1) vecValue->push_back(pos);
			}
		}
		bulkVecValue->resize(0);
	}

	void createInvertedIndex() {
		if (dictionary != NULL)
			dictionary->buildInvertedIndex();
	}

	bool selection(T& searchValue, ColumnBase::OP_TYPE q_where_op,
					vector<bool>* q_resultRid, bool initQueryResult = false) {
		vector<size_t> result;
		this->getDictionary()->search(searchValue, q_where_op, result);

		// find rowId with appropriate dictionary position
		for (size_t rowId = 0; rowId < this->vecValueSize(); rowId++) {
			size_t dictPosition = this->vecValueAt(rowId);
			if ((q_where_op != ColumnBase::containOp && dictPosition >= result.front() && dictPosition <= result.back())
				|| (q_where_op == ColumnBase::containOp && binary_search(result.begin(), result.end(), dictPosition))) {
				// first where expr => used to init query result
				if (initQueryResult)
					q_resultRid->push_back(true); //rowId is in query result
				else {
					if (!q_resultRid->at(rowId)) {
						q_resultRid->at(rowId) = true;
					}
				}
			}
			else {
				// rowId is not in query result
				if(initQueryResult)
					q_resultRid->push_back(false);
				else
					q_resultRid->at(rowId) = false;
			}
		}
		return true;
	}

	vector<T> projection(vector<bool>* q_resultRid, size_t limit, size_t& limitCount) {
		vector<T> outputs; // output result
		limitCount = 0; // reset limit count
		for (size_t rid = 0; rid < q_resultRid->size(); rid++) {
			if (q_resultRid->at(rid)) {
				size_t encodeValue = this->vecValueAt(rid);
				T* a = this->getDictionary()->lookup(encodeValue);
				outputs.push_back(*a);
				if (++limitCount >= limit) break;
			}
		}

		return outputs;
	}

	// Build hashmap of valueId based on selected row ids
	void buildHashmap(map<size_t, vector<size_t>>& hashmap, vector<bool>* vecRowId) {
		hashmap.clear();
		for (size_t rowId = 0; rowId < vecRowId->size(); rowId++) {
			// get valueId from bit packing if row id is selected
			// then build hashmap
			if (vecRowId->at(rowId)) {
				size_t valueId = vecValueAt(rowId);
				hashmap[valueId].push_back(rowId);
			}
		}
	}

	// Return vector of matching row ids
	vector<size_t> probe(map<size_t, vector<size_t>>* hashmap, size_t probedValue) {
		if (hashmap != NULL) {
			try {
				return hashmap->at(probedValue);
			} catch (exception& e) {
				// empty vector
				return vector<size_t>();
			}
		}
		return vector<size_t>();
	}

	// DATA SPACE
	void insertDataVecValue(T&value, uint64_t csn) {
		// uncompress vecValue vector from bit packing
		vecValue = unpackingVecValue();
		// add new value to dictionary
		size_t encodedValue = dictionary->addNewElement(value, vecValue, true, false);
		// bit packing vecValue again
		bitPackingVecValue();
		// create new data space value
		data_column newData;
		newData.encodedValue = encodedValue;
		newData.csn = csn;
		newData.versionFlag = false;
		dataColumn->push_back(newData);
	}

	// VERSION SPACE
	void addVersionVecValue(T& value, uint64_t csn, size_t rid) {
		// check if value existed on dictionary or not
		vector<size_t> result;
		dictionary->search(value, ColumnBase::OP_TYPE::equalOp, result);
		// not existed -> create new entry on delta space
		size_t dictPos = result.at(0);
		if (dictPos == -1) {
			// add to delta space and version vector (start from last dictionary position)
			dictPos = deltaSpace->addNewElementInDeltaSpace(value, versionVecValue, dictionary->size());
		}
		else {
			// append inserted value to version vecValue
			versionVecValue->push_back(dictPos);
		}
		// encoded value is index in vecValue
		size_t encodedValue = vecValue->size() - 1;
		// create new version
		version_column newVersion;
		newVersion.encodedValue = encodedValue;
		newVersion.next = NULL;
		newVersion.csn = csn;
		// check previous version on hash table
		size_t preVersionIdx = -1;
		try {
			preVersionIdx = hashtable->at(rid);
		} catch (exception& e) {
			// not existed on hashtable, keep -1
		}
		if (preVersionIdx >= 0) {
			version_column preVersion = versionColumn->at(preVersionIdx);
			// point the next pointer of new created version to previous version
			newVersion.next = &preVersion;
			// replace the previous version on Version space vector by new version
			versionColumn->at(preVersionIdx) = newVersion;
		}
		else {
			// add new version to Version space vector
			versionColumn->push_back(newVersion);
			// create a new entry for rid on Hash table
			(*hashtable)[rid] = versionColumn->size() - 1;
		}
		// update version_flag on DATA space
		data_column dataValue = dataColumn->at(rid);
		dataValue.versionFlag = true;
		dataColumn->at(rid) = dataValue;
	}
};

} /* namespace std */

#endif /* COLUMN_H_ */
