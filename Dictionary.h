/*
 * Dictionary.h
 *
 *  Created on: Sep 23, 2016
 *      Author: duclv
 */
#import <vector>

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

namespace std {

template<class T>
class Dictionary {
private:
	vector<T> items;
public:
	Dictionary();
	virtual ~Dictionary();

	T lookup(int index);
	int search(T value);
};

} /* namespace std */

#endif /* DICTIONARY_H_ */
