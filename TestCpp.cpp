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
	//cout << "Enter file path: ";
	string filePath;
	//cin >> filePath;
	//ifstream infile(filePath);
	ifstream infile("/home/duclv/Downloads/data1M.csv");
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
	// update encoded velue vector
	cout << col1->getName() << " number of distinct values = " << colDict1->size() << endl;
	cout << col2->getName() << " number of distinct values = " << colDict2->size() << endl;
	cout << col3->getName() << " number of distinct values = " << colDict3->size() << endl;
	col1->updateEncodedVecValue(colValue1, colDict1->size());
	col2->updateEncodedVecValue(colValue2, colDict2->size());
	col3->updateEncodedVecValue(colValue3, colDict3->size());
	//col3->printEncodedVecValue(100);

	// init Table
	Table<string, int, string> table (col1, col2, col3);
	table.setName("orders");

	// print result
	//colDict2->print(100);
	//col3->printVecValue(100);
	//colDict3->print(100);

	// display current time
	t = time(NULL);
	cout << "Now is " << ctime(&t) << endl;

	// query
	while (true) {
		string query = "Select * from orders where o_totalprice < 56789 and o_totalprice > 5678";
		cout << "Enter a query: ";
		getline(cin, query);
		if ("quit" == query)
			break;

		// parse a given query
		hsql::SQLParserResult* pResult = hsql::SQLParser::parseSQLString(query);
		// check whether the parsing was successfull
		bool queryValid = pResult->isValid;
		if (queryValid) {
			printf("Parsed successfully!\n");
			// process the statements...
			string q_table;
			vector<string> q_select_fields;
			vector<string> q_where_fields;
			vector<ColumnBase::OP_TYPE> q_where_ops;
			vector<int> q_where_value;

			hsql::SQLStatement* stmt = pResult->getStatement(0);
			if (stmt->type() == hsql::StatementType::kStmtSelect) {
				hsql::SelectStatement* select = (hsql::SelectStatement*) stmt;
				q_table = select->fromTable->getName();
				cout << "Table name: " << q_table << endl;
				for (hsql::Expr* expr : *select->selectList) {
					if (expr->type == hsql::ExprType::kExprStar) {
						q_select_fields.push_back("*");
					}
					else if (expr->type == hsql::ExprType::kExprColumnRef)
						q_select_fields.push_back(expr->name);
				}
				for (size_t i = 0; i < q_select_fields.size(); i++) {
					cout << "select fields[" << i << "] = " << q_select_fields[i] << endl;
				}
				if (select->whereClause != NULL) {
					hsql::Expr* expr = select->whereClause;
					if (expr->type == hsql::ExprType::kExprOperator) {
						if (expr->op_type == hsql::Expr::OperatorType::SIMPLE_OP) {
							q_where_fields.push_back(expr->expr->name);
							if (expr->op_char == '>')
								q_where_ops.push_back(ColumnBase::OP_TYPE::gtOp);
							else if (expr->op_char == '<')
								q_where_ops.push_back(ColumnBase::OP_TYPE::ltOp);
							if (expr->expr2->type == hsql::ExprType::kExprLiteralInt)
								q_where_value.push_back(expr->expr2->ival);
						}
						else if (expr->op_type == hsql::Expr::OperatorType::AND) {
							hsql::Expr* expr1 = expr->expr;
							hsql::Expr* expr2 = expr->expr2;
							if (expr1->op_type == hsql::Expr::OperatorType::SIMPLE_OP) {
								q_where_fields.push_back(expr1->expr->name);
								if (expr1->op_char == '>')
									q_where_ops.push_back(ColumnBase::OP_TYPE::gtOp);
								else if (expr1->op_char == '<')
									q_where_ops.push_back(ColumnBase::OP_TYPE::ltOp);
								if (expr1->expr2->type == hsql::ExprType::kExprLiteralInt)
									q_where_value.push_back(expr1->expr2->ival);
							}
							if (expr2->op_type == hsql::Expr::OperatorType::SIMPLE_OP) {
								q_where_fields.push_back(expr2->expr->name);
								if (expr2->op_char == '>')
									q_where_ops.push_back(ColumnBase::OP_TYPE::gtOp);
								else if (expr2->op_char == '<')
									q_where_ops.push_back(ColumnBase::OP_TYPE::ltOp);
								if (expr2->expr2->type == hsql::ExprType::kExprLiteralInt)
									q_where_value.push_back(expr2->expr2->ival);
							}
						}
					}
					for (size_t i = 0; i < q_where_fields.size(); i++) {
						cout << "fields[" << i << "] = " << q_where_fields[i] << endl;
					}
					for (size_t i = 0; i < q_where_ops.size(); i++) {
						cout << "ops[" << i << "] = " << q_where_ops[i] << endl;
					}
					for (size_t i = 0; i < q_where_value.size(); i++) {
						cout << "value[" << i << "] = " << q_where_value[i] << endl;
					}
				}
			}
			else
				cout << "Please enter a SELECT query !!!" << endl;
		}
		if (!queryValid) {
			printf("The SQL query is invalid!\n");
			break;
		}
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

	string input;
	cout << "Enter anything: ";
	cin >> input;

	return EXIT_SUCCESS;
}

