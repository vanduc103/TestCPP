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
#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <string>
#include "ColumnBase.h"
#include "Dictionary.h"
#include "PackedArray.h"

namespace std {

template<typename T>
class Column: public ColumnBase {
private:
	// value vector for column
	vector<size_t>* vecValue;
	// encoded value vector
	vector<boost::dynamic_bitset<>>* encodedVecValue;
	PackedArray* packed;
	// dictionary vector for column
	Dictionary<T>* dictionary;
public:
	Column() {
		encodedVecValue = new vector<boost::dynamic_bitset<>>();
		dictionary = new Dictionary<T>();
		vecValue = new vector<size_t>();
		packed = new PackedArray();
	}
	virtual ~Column() {
		delete vecValue;
		delete encodedVecValue;
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
		// used for column has few distinct values
		vecValue->push_back(value);
	}

	void updateEncodedVecValue() {
		// #bit to represent encode dictionary value
		size_t numOfBit = (size_t) ceil(log2((double) dictionary->size()));
		for (size_t i = 0; i < vecValue->size(); i++) {
			encodedVecValue->push_back(
					boost::dynamic_bitset<>(numOfBit, vecValue->at(i)));
		}
		// free vecValue
		vecValue->resize(0);
	}

	vector<boost::dynamic_bitset<>>* getEncodedVecValue() {
		return encodedVecValue;
	}

	void printEncodedVecValue(int row) {
		for (size_t i = 0; i < (*encodedVecValue).size() && i < row; i++) {
			cout << "encodedVecValue[" << i << "] = "
					<< ((*encodedVecValue)[i]).to_ulong() << "\n";
		}
	}

	Dictionary<T>* getDictionary() {
		if (dictionary == NULL) {
			dictionary = new Dictionary<T>();
		}
		return dictionary;
	}

	// Update new value for dictionary
	void updateDictionary(T& value, bool sorted = true) {
		dictionary->addNewElement(value, vecValue, sorted);
	}

	bool selection(T& searchValue, ColumnBase::OP_TYPE q_where_op,
					vector<bool>* q_resultRid, bool initQueryResult) {
		vector<size_t> result;
		this->getDictionary()->search(searchValue, q_where_op, result);

		// find rowId with appropriate encodeValue
		for (size_t rowId = 0; !result.empty() && rowId < this->vecValueSize(); rowId++) {
			size_t encodeValue = this->vecValueAt(rowId);
			if (encodeValue >= result.front() && encodeValue <= result.back()) {
				// first where expr => used to init query result
				if (initQueryResult)
					q_resultRid->push_back(true); //rowId is in query result
				else {
					if (q_resultRid->at(rowId)) {
						// keep this row id in result - do nothing
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
};

} /* namespace std */

#endif /* COLUMN_H_ */
