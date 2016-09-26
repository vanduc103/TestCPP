/*
 * Table.h
 *
 *  Created on: Sep 26, 2016
 *      Author: duclv
 */

#ifndef TABLE_H_
#define TABLE_H_

#include <tuple>
#include "Column.h"

namespace std {

template<class... T>
class Table {
private:
	tuple<Column<T>...> m_columns;
public:
	Table();
	virtual ~Table();

	tuple<Column<T>...>* getColumnList();
	void setColumnList(tuple<Column<T>...>& colList);
};

} /* namespace std */

#endif /* TABLE_H_ */
