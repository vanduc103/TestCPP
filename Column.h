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

template<typename T1, typename T2>
class Column : public ColumnBase {
private:
	//value vector for column
	vector<T1> vecValue;
	//dictionary vector for column
	Dictionary<T2>* dictionary;
public:
	Column();
	virtual ~Column();

	vector<T1> getVecValue();
	void printVecValue(int row);

	Dictionary<T2>* getDictionary();
};

} /* namespace std */

#endif /* COLUMN_H_ */
