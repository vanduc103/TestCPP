//============================================================================
// Name        : TestCpp.cpp
// Author      : Le Van Duc
// Version     :
// Copyright   : 
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <tuple>

#include "Dictionary.h"
#include "Column.h"
#include "ColumnBase.h"
#include "Table.h"

using namespace std;
class TestCPP {

};

int main(void) {
	puts("Hello World!!!");

	// test Column
	/*Column<int, string> * col = new Column<int, string>();
	col->setName("FullName");
	col->setType(ColumnBase::charType);
	col->setSize(30);
	Dictionary<string>* colDict = col->getDictionary();
	vector<int> colValue = col->getVecValue();
	string i = "2016-09-01";
	colDict->addNewElement(i, colValue);
	i = "2016-08-31";
	colDict->addNewElement(i, colValue);
	i = "2016-08-30";
	colDict->addNewElement(i, colValue);
	int index = 2;
	string * a = colDict->lookup(index);
	cout << "lookup " << index << ", got " << *a << "\n";
	cout << "vecValue[" << index << "] = " << colValue[index] << ", size =" << colValue.size() << "\n";*/


	// Column 1
	Column<string> * col1 = new Column<string>();
	col1->setName("o_orderstatus");
	col1->setType(ColumnBase::charType);
	col1->setSize(1);
	Dictionary<string>* colDict1 = col1->getDictionary();
	vector<size_t> * colValue1 = col1->getVecValue();

	// Column 2
	Column<int> * col2 = new Column<int>();
	col2->setName("o_totalprice");
	col2->setType(ColumnBase::intType);
	col2->setSize(4);
	Dictionary<int>* colDict2 = col2->getDictionary();
	vector<size_t> * colValue2 = col2->getVecValue();

	// Table
	Table<Column<string>, Column<int>>* table = new Table<Column<string>, Column<int>>();
	table->setName("orders");

	//get the starting value of clock
	//clock_t start = clock();
	// display current time
	time_t t = time(NULL);
	cout << "Now is " << ctime(&t) << endl;

	// read file
	ifstream infile("/home/duclv/Downloads/data.csv");
	//ifstream infile("/home/duclv/homework/data1M.csv");
	string line;
	string delim = ",";
	size_t pos = 0;
	int row = 0;
	while(getline(infile, line)) {
		char i = 0;
		while ((pos = line.find(delim)) != string::npos) {
		    string token = line.substr(0, pos);
		    //std::cout << token << std::endl;
		    line.erase(0, pos + delim.length());
		    i++;
		    // status is 2nd column
		    if (i == 2) {
		    	colDict1->addNewElement(token, *colValue1);
		    }
		    // totalprice is 3rd column
		    if (i == 3) {
		    	int totalprice = stoi(token);
		    	colDict2->addNewElement(totalprice, *colValue2);
		    }
		}
		cout << "Row: " << ++row << endl;
	}
	infile.close();
	// tuple of columns
	auto colList = make_tuple(&col1, &col2);
	table->setColumnList(colList);

	// print result
	colDict2->print(100);
	col2->printVecValue(100);

	// display current time
	t = time(NULL);
	cout << "Now is " << ctime(&t) << endl;

	// search on totalprice
	int searchTotalprice = 56789;
	cout << "Select totalprice > " << searchTotalprice << endl;
	vector<size_t> result;
	colDict2->search(searchTotalprice, ColumnBase::gtOp, result);
	// print result
	for (size_t i = 0; i < result.size() && i < 100; i++) {
		size_t encode = result[i];
		int * a = colDict2->lookup(encode);
		for (size_t j = 0; j < (*colValue2).size(); j++) {
			if (encode == (*colValue2)[j]) {
				cout << "Result[" << j << "] = " << *a << endl;
			}
		}
	}

	// search 2
	int totalprice1 = 5678;
	int totalprice2 = 56789;
	cout << "Select " << totalprice1 << " < totalprice < " << totalprice2 << endl;
	vector<size_t> result1;
	colDict2->search(totalprice1, ColumnBase::gtOp, result1);
	vector<size_t> result2;
	colDict2->search(totalprice2, ColumnBase::ltOp, result2);
	vector<size_t> resultJoin;
	// join from min value of result1 to max value of result2
	for (size_t i = result1.front(); i < result2.back(); i++) {
		resultJoin.push_back(i);
	}
	// print result
	for (size_t i = 0; i < resultJoin.size() && i < 100; i++) {
		size_t encode = resultJoin[i];
		int * a = colDict2->lookup(encode);
		for (size_t j = 0; j < (*colValue2).size(); j++) {
			if (encode == (*colValue2)[j]) {
				cout << "Result[" << j << "] = " << *a << endl;
			}
		}
	}

	// display current time
	t = time(NULL);
	cout << "Now is " << ctime(&t) << endl;

	return EXIT_SUCCESS;
}

