/*
 * Dictionary.h
 *
 *  Created on: Sep 23, 2016
 *      Author: duclv
 */
#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include <vector>
#include <algorithm>

using namespace std;

template<class T>
class Dictionary {
private:
	vector<T> items;
public:
	Dictionary();
	virtual ~Dictionary();

	T* lookup(int index);
	typename vector<T>::iterator search(T& value);
	int addNewElement(T& value, vector<int>& vecValue);
	void print(int row);
};


#endif /* DICTIONARY_H_ */
