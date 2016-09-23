/*
 * ColumnBase.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: duclv
 */

#include "ColumnBase.h"

namespace std {

ColumnBase::ColumnBase() {
	name = "";
	type = charType;
	size = 0;
}

ColumnBase::~ColumnBase() {
}

//get column name
string ColumnBase::getName() {
	return name;
}
//set column name
void ColumnBase::setName(string nameValue) {
	name = nameValue;
}

//get column type
DB_Type ColumnBase::getType() {
	return type;
}
//set column type
void ColumnBase::setType(DB_Type typeValue) {
	type = typeValue;
}

//get column size
int ColumnBase::getSize() {
	return size;
}
//set column size
void ColumnBase::setSize(int sizeValue) {
	size = sizeValue;
}

} /* namespace std */
