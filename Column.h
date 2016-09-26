/*
 * Column.h
 *
 *  Created on: Sep 23, 2016
 *      Author: duclv
 */
#include <vector>

#ifndef COLUMN_H_
#define COLUMN_H_

#include "ColumnBase.h"
#include "Dictionary.h"

namespace std {

template<typename T>
class Column : public ColumnBase {
private:
	//value vector for column
	vector<size_t> * vecValue;
	//dictionary vector for column
	Dictionary<T> * dictionary;
public:
	Column();
	virtual ~Column();

	vector<size_t> * getVecValue();
	void printVecValue(int row);

	Dictionary<T> * getDictionary();
};

} /* namespace std */

#endif /* COLUMN_H_ */
