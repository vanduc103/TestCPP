/*
 * Table.cpp
 *
 *  Created on: Sep 26, 2016
 *      Author: duclv
 */

#include <string>
#include "Table.h"

namespace std {

template<class... T>
Table<T>::Table() {

}

template<class... T>
tuple<Column<T>...>* Table<T>::getColumnList() {
	return &m_columns;
}

template<class... T>
void Table<T>::setColumnList(tuple<Column<T>...>& colList) {
	m_columns = colList;
}

template<class... T>
Table<T>::~Table() {
	delete &m_columns;
}

template class Table<int, string, int, string>;

} /* namespace std */
