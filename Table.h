/*
 * Table.h
 *
 *  Created on: Sep 26, 2016
 *      Author: duclv
 */

#ifndef TABLE_H_
#define TABLE_H_

#include <string>
#include <tuple>
#include "Column.h"

namespace std {

template<class... T>
class Table {
private:
	tuple<Column<T>...> m_columns;
	string name;
public:
	virtual ~Table() {
		//delete &m_columns;
	}

	Table(Column<T>*... columns) : m_columns () {}

	string getName() {
		return name;
	}
	void setName(string tableName) {
		name = tableName;
	}
};

} /* namespace std */

#endif /* TABLE_H_ */
