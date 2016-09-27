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
#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "Dictionary.h"
#include "Column.h"
#include "ColumnBase.h"
#include "Table.h"
#include "SQLParser.h"

using namespace std;
class TestCPP {

};

int main(void) {
	puts("Hello World!!!");

	// Column 1
	Column<string>* col1 = new Column<string>();
	col1->setName("o_orderstatus");
	col1->setType(ColumnBase::charType);
	col1->setSize(1);
	Dictionary<string>* colDict1 = col1->getDictionary();
	vector<size_t>* colValue1 = col1->getVecValue();

	// Column 2
	Column<int>* col2 = new Column<int>();
	col2->setName("o_totalprice");
	col2->setType(ColumnBase::intType);
	col2->setSize(4);
	Dictionary<int>* colDict2 = col2->getDictionary();
	vector<size_t>* colValue2 = col2->getVecValue();

	// Column 3
	Column<string>* col3 = new Column<string>();
	col3->setName("o_comment");
	col3->setType(ColumnBase::charType);
	col3->setSize(30);
	Dictionary<string>* colDict3 = col3->getDictionary();
	vector<size_t>* colValue3 = col3->getVecValue();

	//get the starting value of clock
	//clock_t start = clock();
	// display current time
	time_t t = time(NULL);
	cout << "Now is " << ctime(&t) << endl;

	// read file
	//ifstream infile("/home/duclv/Downloads/data.csv");
	ifstream infile("/home/duclv/homework/data1M.csv");
	string line;
	string delim = ",";
	size_t pos = 0;
	int row = 0;
	while(getline(infile, line)) {
		char i = 0;
		while ((pos = line.find(delim)) != string::npos) {
		    string token = line.substr(0, pos);
		    std::cout << token << std::endl;
		    line.erase(0, pos + delim.length());
		    i++;
		    // status is 2nd column
		    if (i == 2) {
		    	colDict1->addNewElement(token, *colValue1);
		    }
		    // totalprice is 3rd column
		    else if (i == 3) {
		    	int totalprice = stoi(token);
		    	colDict2->addNewElement(totalprice, *colValue2);
		    }
		    // comment is 4th column
		    else if (i == 4) {
		    	boost::erase_all(token, "\"");
		    	boost::trim(token);
		    	colDict3->addNewElement(token, *colValue3);
		    }
		}
		//cout << "Row: " << ++row << endl;
	}
	infile.close();
	// init Table
	Table<string, int> table (col1, col2);
	table.setName("orders");

	// print result
	//colDict2->print(100);
	//col2->printVecValue(100);
	colDict3->print(100);

	// display current time
	t = time(NULL);
	cout << "Now is " << ctime(&t) << endl;

	// parse a given query
	string query = "Select * from orders where o_totalprice < 56789 and o_totalprice > 5678";
	hsql::SQLParserResult* pResult = hsql::SQLParser::parseSQLString(query);

	// check whether the parsing was successful
	if (pResult->isValid) {
		printf("Parsed successfully!\n");
		// process the statements...
		hsql::SQLStatement* stmt = pResult->getStatement(0);
		if (stmt->type() == hsql::StatementType::kStmtSelect) {
			hsql::SelectStatement* select = (hsql::SelectStatement*) stmt;
			string tableName = select->fromTable->getName();
			cout << "Table name: " << tableName << endl;
			if (select->whereClause != NULL) {
				hsql::Expr* expr = select->whereClause;
				hsql::Expr* expr1 = expr->expr;
				hsql::Expr* expr2 = expr->expr2;
				hsql::Expr::OperatorType opType = expr->op_type;
				cout << "where " << expr1->expr->name
						<< " " << expr1->op_char
						<< " " << expr1->expr2->ival
						<< ((opType == hsql::Expr::OperatorType::AND) ? " AND " : " ")
						<< expr2->expr->name
						<< " " << expr2->op_char
						<< " " << expr2->expr2->ival << endl;
			}
		}
	} else {
		printf("The SQL string is invalid!\n");
		return -1;
	}

	/*// search on totalprice
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
	cout << "-------------------------" << endl;
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
	cout << "Now is " << ctime(&t) << endl;*/

	return EXIT_SUCCESS;
}

