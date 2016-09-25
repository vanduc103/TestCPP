/*
 * Dictionary.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: duclv
 */
#ifndef _Dictionary_
#define _Dictionary_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "Dictionary.h"

using namespace std;

template<class T>
Dictionary<T>::Dictionary() {
	vector<T> items;
}

template<class T>
T* Dictionary<T>::lookup(int index) {
	if (items.empty() || index < 0 || index >= items.size()) {
		return (T*) 0;
	} else {
		return &items.at(index);
	}
}

template<class T>
typename vector<T>::iterator Dictionary<T>::search(T& value) {
	if (items.empty()) {
		return items.end();	// first position in vector
	} else {
		// find the lower bound for value in vector
		typename vector<T>::iterator lower;
		lower = std::lower_bound(items.begin(), items.end(), value);
		//printf("compFunc(%d, %d) is %d \n", value, *lower, compFunc(value, *lower));
		return lower;
	}
}

template<class T>
bool compFunc(T value1, T value2) {
	//cout << "compare " << value1 << " , " << value2 << ". result: " << (value1 < value2) << "\n";
	return value1 < value2;
}

template<class T>
bool equalFunc(T value1, T value2) {
	return value1 == value2;
}

template<class T>
int Dictionary<T>::addNewElement(T& value, vector<int>& vecValue) {
	if (items.empty()) {
		items.push_back(value);
		vecValue.push_back(0);
		return 0;
	} else {
		// find the lower bound for value in vector
		typename vector<T>::iterator lower;
		lower = std::lower_bound(items.begin(), items.end(), value, compFunc<T>);
		//cout << "compFunc(" << value <<", " << *lower <<") is "<< compFunc(value, *lower) <<"\n";
		// value existed
		if (lower != items.end() && equalFunc(value, *lower)) {
			// return the position of lower
			long elementPos = lower - items.begin();
			vecValue.push_back(elementPos);
			return elementPos;
		}
		else {
			// The position of new element in dictionary
			long newElementPos = 0L;
			if (lower == items.end()) {
				// insert to the end of dictionary
				newElementPos = items.size();
				items.push_back(value);
				vecValue.push_back(newElementPos);
			}
			else {
				newElementPos = lower - items.begin();
				// insert into dictionary
				items.insert(lower, value);
				// update (+1) to all elements in vecValue have value >= newElementPos
				for (int i = 0; i < vecValue.size(); i++) {
					if (vecValue[i] >= newElementPos) {
						++vecValue[i];
					}
				}
				vecValue.push_back(newElementPos);
			}

			// return the position of new element
			return newElementPos;
		}
	}
}

template<class T>
void Dictionary<T>::print(int row) {
	for (int i = 0; i < items.size() && i < row; i++) {
		cout << "Dictionary[" << i << "] = " << items[i] << "\n";
	}
}

template<class T>
Dictionary<T>::~Dictionary() {
	delete &items;
}

template class Dictionary<string>;

#endif
