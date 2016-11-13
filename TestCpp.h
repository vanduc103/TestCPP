/*
 * TestCpp.h
 *
 *  Created on: Nov 11, 2016
 *      Author: duclv
 */

#ifndef TESTCPP_H_
#define TESTCPP_H_

#include <string>
#include <vector>
#include "Table.h"

using namespace std;

Table* createTable(string createQuery);
string updateCommand(Table &table, vector<string> command);
string insertCommand(Table &table, vector<string> command);
string scanCommand(Table &table, vector<string> command);

#endif /* TESTCPP_H_ */
