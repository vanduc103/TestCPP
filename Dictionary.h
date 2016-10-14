/*
 * Dictionary.h
 *
 *  Created on: Sep 23, 2016
 *      Author: duclv
 */
#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include "ColumnBase.h"
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

template<class T>
class Dictionary {
private:
	struct classcomp {
	  bool operator() (const T lhs, const T rhs) const
	  {return lhs<rhs;}
	};

	struct invertedIndex {
		string word;
		char position;	 // position on text
		vector<size_t> location; // position on dictionary
	};

	vector<T>* items;
	std::map<T, size_t, classcomp>* sMap;
	vector<T>* bulkVecValue;
	vector<invertedIndex>* vecInvertedIndex;
public:
	Dictionary() {
		items = new vector<T>();
		sMap = new map<T, size_t, classcomp>();
		bulkVecValue = new vector<T>();
		vecInvertedIndex = new vector<invertedIndex>();
	}
	virtual ~Dictionary() {
		delete items;
		delete vecInvertedIndex;
		delete sMap;
		delete bulkVecValue;
	}

	T* lookup(size_t index);
	void search(T& value, ColumnBase::OP_TYPE opType, vector<size_t>& result);
	size_t addNewElement(T& value, vector<size_t>* vecValue, bool sorted, bool bulkInsert);
	size_t size();
	void print(int row);

	vector<T>* getBulkVecValue() {
		return bulkVecValue;
	}

	void clearTemp() {
		sMap->clear();
		bulkVecValue->resize(0);
	}

	void buildInvertedIndex();
};


#endif /* DICTIONARY_H_ */
