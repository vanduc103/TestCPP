/*
 * Column.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: duclv
 */

#include "Column.h"
#include <iostream>

namespace std {

template<typename T>
Column<T>::Column() {
}

template<typename T>
vector<size_t> * Column<T>::getVecValue() {
	if (vecValue == NULL) {
		vecValue = new vector<size_t>();
	}
	return vecValue;
}

template<typename T>
void Column<T>::printVecValue(int row) {
	for (int i = 0; i < (*vecValue).size() && i < row; i++) {
		cout << "vecValue[" << i << "] = " << (*vecValue)[i] << "\n";
	}
}

template<typename T>
Dictionary<T>* Column<T>::getDictionary() {
	if (dictionary == NULL) {
		dictionary = new Dictionary<T>();
	}
	return dictionary;
}

template<typename T>
Column<T>::~Column() {
	delete vecValue;
	delete dictionary;
}

template class Column<string>;
template class Column<int>;

}; /* namespace std */
