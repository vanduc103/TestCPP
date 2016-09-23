/*
 * ColumnBase.h
 *
 *  Created on: Sep 23, 2016
 *      Author: duclv
 */
#include <string>
#ifndef COLUMNBASE_H_
#define COLUMNBASE_H_

namespace std {

enum DB_Type {intType, charType, varcharType};

class ColumnBase {
private:
	string name;
	DB_Type type;
	int size;
public:
	ColumnBase();
	virtual ~ColumnBase();

	string getName();
	void setName(string nameValue);

	DB_Type getType();
	void setType(DB_Type typeValue);

	int getSize();
	void setSize(int sizeValue);
};

} /* namespace std */

#endif /* COLUMNBASE_H_ */
