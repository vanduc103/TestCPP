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
#include "ColumnBase.h"
#include "Dictionary.h"
#include "PackedArray.h"

namespace std {

template<typename T>
class Column : public ColumnBase {
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
		return vecValue;
	}
	void printVecValue(int row) {
		vecValue = getEncodedVecValue();
		for (size_t i = 0; i < (*vecValue).size() && i < row; i++) {
			cout << "vecValue[" << i << "] = " << (*vecValue)[i] << "\n";
		}
	}
	void updateEncodedVecValue(vector<size_t>* vecValue, size_t sizeOfDict) {
		size_t numOfBit = (size_t) ceil(log2((double) sizeOfDict));
		/*packed = PackedArray_create(numOfBit, vecValue->size());
		uint32_t mask = (uint32_t)(1ULL << packed->bitsPerItem) - 1;
		uint32_t v = 25 & mask;
		cout << "v init = " << v << endl;
		PackedArray_set(packed, 0, v);
		cout << "v = " << PackedArray_get(packed, 0) << endl;*/
		for (size_t i = 0; i < vecValue->size(); i++) {
			encodedVecValue->push_back(boost::dynamic_bitset<>(numOfBit, vecValue->at(i)));
			//PackedArray_set(packed, i, 100 & mask);
		}
		// delete vecValue
		vecValue->resize(0);
	}
	vector<boost::dynamic_bitset<>>* getEncodedVecValue() {
		//vecValue = new vector<size_t>();
		/*uint32_t mask = (uint32_t)(1ULL << packed->bitsPerItem) - 1;
		for (int i = 0; i < packed->count; i++) {
			vecValue->push_back(PackedArray_get(packed, i) & mask);
		}
		return vecValue;*/
		return encodedVecValue;
	}
	void printEncodedVecValue(int row) {
		for (size_t i = 0; i < (*encodedVecValue).size() && i < row; i++) {
			cout << "encodedVecValue[" << i << "] = " << ((*encodedVecValue)[i]).to_ulong() << "\n";
		}
	}

	Dictionary<T>* getDictionary() {
		if (dictionary == NULL) {
			dictionary = new Dictionary<T>();
		}
		return dictionary;
	}
};

} /* namespace std */

#endif /* COLUMN_H_ */
