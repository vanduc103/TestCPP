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
public:
	Column() {
		dictionary = new Dictionary<T>();
		vecValue = new vector<size_t>();
		packed = new PackedArray();
	}
	virtual ~Column() {
		delete vecValue;
		delete dictionary;
		PackedArray_destroy(packed);
	}

	vector<size_t>* getVecValue() {
		if (vecValue == NULL) {
			vecValue = new vector<size_t>();
		}
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
		for (size_t rowId = 0; !result.empty() && rowId < this->vecValueSize(); rowId++) {
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

	// Loop through vecValue (at bit packed) to build hashmap of valueId
	void buildHashmap(map<size_t, vector<size_t>*>& hashmap) {
		// unpack vecValue to build hash map
		vecValue = getVecValue();
		for (size_t rowId = 0; rowId < vecValue->size(); rowId++) {
			size_t valueId = vecValue->at(rowId);
			if (hashmap[valueId] == NULL)
				hashmap[valueId] = new vector<size_t>();
			hashmap[valueId]->push_back(rowId);
		}
	}

	// Return vector of matching row ids
	vector<size_t>* probe(map<size_t, vector<size_t>*>* hashmap, size_t probedValue) {
		if (hashmap != NULL) {
			try {
				return hashmap->at(probedValue);
			} catch (exception& e) {
				return NULL;
			}
		}
		return NULL;
	}
};

} /* namespace std */

#endif /* COLUMN_H_ */
