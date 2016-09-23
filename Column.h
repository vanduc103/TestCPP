/*
 * Column.h
 *
 *  Created on: Sep 23, 2016
 *      Author: duclv
 */
#include <vector>

#ifndef COLUMN_H_
#define COLUMN_H_

namespace std {

template<class T1, class T2>
class Column : public ColumnBase {
private:
	//value vector for column
	vector<T1> vecValue;
	//dictionary vector for column
	Dictionary<T2> dictionary;
public:
	Column();
	virtual ~Column();

	vector<T1> getVecValue();
	void updateVecValue(vector<T1> newVecValue);

	Dictionary<T2> getDictionary();
};

} /* namespace std */

#endif /* COLUMN_H_ */
