/*
 * Column.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: duclv
 */

#include "Column.h"
#include <iostream>

namespace std {

template<typename T1, typename T2>
Column<T1, T2>::Column() {
	 vector<T1> vecValue;
	 Dictionary<T2>* dictionary = new Dictionary<T2>();
}

template<typename T1, typename T2>
vector<T1> Column<T1, T2>::getVecValue() {
	return vecValue;
}

template<typename T1, typename T2>
void Column<T1, T2>::printVecValue(int row) {
	for (int i = 0; i < vecValue.size() && i < row; i++) {
		cout << "vecValue[" << i << "] = " << vecValue[i] << "\n";
	}
}

template<typename T1, typename T2>
Dictionary<T2>* Column<T1, T2>::getDictionary() {
	if (dictionary == NULL) {
		dictionary = new Dictionary<T2>();
	}
	return dictionary;
}

template<typename T1, typename T2>
Column<T1, T2>::~Column() {
	delete &vecValue;
	delete dictionary;
}

template class Column<int, string>;

}; /* namespace std */
