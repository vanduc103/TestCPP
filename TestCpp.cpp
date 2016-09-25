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

#include "Dictionary.h"
#include "Column.h"
#include "ColumnBase.h"

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

	// Column
	Column<int, string> * col1 = new Column<int, string>();
	col1->setName("o_orderstatus");
	col1->setType(ColumnBase::charType);
	col1->setSize(1);
	Dictionary<string>* colDict1 = col1->getDictionary();
	vector<int> colValue1 = col1->getVecValue();

	// read file
	ifstream infile("/home/duclv/Downloads/data.csv");
	string line;
	string delim = ",";
	int pos = 0;
	while(getline(infile, line)) {
		// extract status column
		int i = 0;
		while ((pos = line.find(delim)) != string::npos) {
		    string token = line.substr(0, pos);
		    //std::cout << token << std::endl;
		    line.erase(0, pos + delim.length());
		    i++;
		    // status is 2nd column
		    if (i == 2) {
		    	colDict1->addNewElement(token, colValue1);
		    }
		}
	}
	infile.close();
	// print result
	colDict1->print(100);
	for (int i = 0; i < colValue1.size() && i < 100; i++) {
		cout << "vecValue[" << i << "] = " << colValue1[i] << "\n";
	}

	return EXIT_SUCCESS;
}

