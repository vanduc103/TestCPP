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
#include <boost/dynamic_bitset.hpp>

#include "Dictionary.h"
#include "Column.h"
#include "ColumnBase.h"
#include "Table.h"
#include "SQLParser.h"

using namespace std;
class TestCPP {

};

int main(void) {
	puts("***** Simple Column-Store Database start ******");

	// Column 0
	Column<int>* col0 = new Column<int>();
	col0->setName("o_orderkey");
	col0->setType(ColumnBase::intType);
	col0->setSize(4);

	// Column 1
	Column<string>* col1 = new Column<string>();
	col1->setName("o_orderstatus");
	col1->setType(ColumnBase::charType);
	col1->setSize(1);
	//Dictionary<string>* colDict1 = col1->getDictionary();
	//vector<size_t>* colValue1 = col1->getVecValue();

	// Column 2
	Column<int>* col2 = new Column<int>();
	col2->setName("o_totalprice");
	col2->setType(ColumnBase::intType);
	col2->setSize(4);
	//Dictionary<int>* colDict2 = col2->getDictionary();
	//vector<size_t>* colValue2 = col2->getVecValue();

	// Column 3
	Column<string>* col3 = new Column<string>();
	col3->setName("o_comment");
	col3->setType(ColumnBase::charType);
	col3->setSize(30);
	//Dictionary<string>* colDict3 = col3->getDictionary();
	//vector<size_t>* colValue3 = col3->getVecValue();

	// calculate time execution
	clock_t begin_time = clock();

	// read file
	cout << "Enter file path: ";
	string filePath;
	getline(cin, filePath);
	ifstream infile(filePath);
	//ifstream infile("/home/duclv/Downloads/data1M.csv");
	//ifstream infile("/home/duclv/homework/data1M.csv");
	string line;
	string delim = ",";
	size_t pos = 0;
	//int row = 0;
	while(getline(infile, line)) {
		char i = 0;
		while ((pos = line.find(delim)) != string::npos) {
		    string token = line.substr(0, pos);
		    //std::cout << token << std::endl;
		    line.erase(0, pos + delim.length());
		    i++;
		    // key is 1st column
			if (i == 1) {
				int key = stoi(token);
				// no encode by dictionary
				col0->updateVecValue(key);
			}
		    // status is 2nd column
			else if (i == 2) {
		    	//colDict1->addNewElement(token, *colValue1);
		    	col1->updateDictionary(token);
		    }
		    // totalprice is 3rd column
		    else if (i == 3) {
		    	int totalprice = stoi(token);
		    	//colDict2->addNewElement(totalprice, *colValue2);
		    	col2->updateDictionary(totalprice);
		    }
		    // comment is 4th column
		    else if (i == 4) {
		    	//boost::erase_all(token, "\"");
		    	//boost::trim(token);
		    	//colDict3->addNewElement(token, *colValue3);
		    	col3->updateDictionary(token);
		    }
		}
		//cout << "Row: " << ++row << endl;
	}
	infile.close();
	// update encoded value vector
	cout << col0->getName() << " number of distinct values = " << col1->getDictionary()->size() << endl;
	cout << col1->getName() << " number of distinct values = " << col1->getDictionary()->size() << endl;
	cout << col2->getName() << " number of distinct values = " << col2->getDictionary()->size() << endl;
	cout << col3->getName() << " number of distinct values = " << col3->getDictionary()->size() << endl;
	//col1->updateEncodedVecValue(colValue1, colDict1->size());
	//col2->updateEncodedVecValue(colValue2, colDict2->size());
	//col3->updateEncodedVecValue(colValue3, colDict3->size());
	col1->bitPackingVecValue();
	col2->bitPackingVecValue();
	col3->bitPackingVecValue();

	// init Table
	vector<ColumnBase*> columns;
	columns.push_back(col1);
	columns.push_back(col2);
	columns.push_back(col3);
	Table* table = new Table(columns);
	table->setName("orders");

	// print result
	//col1->printVecValue(10);
	//col2->printVecValue(10);
	//col3->printVecValue(10);

	// loaded time
	cout << "Table Load time: " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " seconds " << endl;

	// query
	while (true) {
		string query = "Select * from orders where o_totalprice < 56789 and o_totalprice > 5678";
		cout << "Enter a query: ";
		getline(cin, query);
		if ("quit" == query)
			break;

		begin_time = clock();
		// parse a given query
		hsql::SQLParserResult* pResult = hsql::SQLParser::parseSQLString(query);
		// check whether the parsing was successfull
		bool queryValid = pResult->isValid;
		while (queryValid) {
			printf("Parsed successfully!\n");
			// process the statements...
			string q_table;
			vector<string> q_select_fields;
			vector<string> q_where_fields;
			vector<ColumnBase::OP_TYPE> q_where_ops;
			vector<int> q_where_values;

			hsql::SQLStatement* stmt = pResult->getStatement(0);
			if (stmt->type() == hsql::StatementType::kStmtSelect) {
				hsql::SelectStatement* select = (hsql::SelectStatement*) stmt;
				q_table = select->fromTable->getName();
				cout << "Table name: " << q_table << endl;
				if (q_table != table->getName()) {queryValid = false; break;}

				for (hsql::Expr* expr : *select->selectList) {
					if (expr->type == hsql::ExprType::kExprStar) {
						q_select_fields.push_back("o_orderstatus");
						q_select_fields.push_back("o_totalprice");
						q_select_fields.push_back("o_comment");
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
							else if (expr->op_char == '=')
								q_where_ops.push_back(ColumnBase::OP_TYPE::equalOp);
							if (expr->expr2->type == hsql::ExprType::kExprLiteralInt)
								q_where_values.push_back(expr->expr2->ival);
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
									q_where_values.push_back(expr1->expr2->ival);
							}
							if (expr2->op_type == hsql::Expr::OperatorType::SIMPLE_OP) {
								q_where_fields.push_back(expr2->expr->name);
								if (expr2->op_char == '>')
									q_where_ops.push_back(ColumnBase::OP_TYPE::gtOp);
								else if (expr2->op_char == '<')
									q_where_ops.push_back(ColumnBase::OP_TYPE::ltOp);
								else if (expr2->op_char == '=')
									q_where_ops.push_back(ColumnBase::OP_TYPE::equalOp);
								if (expr2->expr2->type == hsql::ExprType::kExprLiteralInt)
									q_where_values.push_back(expr2->expr2->ival);
							}
						}
						else {
							// not support
							queryValid = false;
							break;
						}
					}
					for (size_t i = 0; i < q_where_fields.size(); i++) {
						cout << "where fields[" << i << "] = " << q_where_fields[i] << endl;
					}
					for (size_t i = 0; i < q_where_ops.size(); i++) {
						cout << "where operators[" << i << "] = " << q_where_ops[i] << endl;
					}
					for (size_t i = 0; i < q_where_values.size(); i++) {
						cout << "value values[" << i << "] = " << q_where_values[i] << endl;
					}
				}
				// execute query
				vector<bool>* q_resultRid = new vector<bool>(); // contain query result row id
				vector<vector<size_t>> q_results;
				for (size_t fidx = 0; fidx < q_where_fields.size(); fidx++) {
					string q_where_field = q_where_fields[fidx];
					ColumnBase::OP_TYPE q_where_op = q_where_ops[fidx];
					int q_where_value = q_where_values[fidx];

					Column<int>* t = (Column<int>*) table->getColumnByName(q_where_field);
					if (t == NULL)
						continue;
					vector<size_t> result;
					t->getDictionary()->search(q_where_value, q_where_op, result);

					// find rowId with appropriate encodeValue
					for (size_t rowId = 0; !result.empty() && rowId < t->vecValueSize(); rowId++) {
						// convert from bitset to encodeValue
						//size_t encodeValue = (t->getEncodedVecValue()->at(rowId)).to_ulong();
						size_t encodeValue = t->vecValueAt(rowId);
						if (encodeValue >= result.front() && encodeValue <= result.back()) {
							// first where expr
							if (fidx == 0)
								q_resultRid->push_back(1); //rowId is in query result
							else {
								// only keep rid that existed on previous rid vector
								if (q_resultRid->at(rowId)) {
									// keep this row id in result - do nothing
								}
							}
						}
						else {
							// rowId is not in query result
							if(fidx == 0)
								q_resultRid->push_back(0);
							else
								q_resultRid->at(rowId) = 0;
						}
					}
				}
				/*for (size_t rid = 0; rid < q_resultRid->size(); rid++) {
					if (q_resultRid->at(rid))
						cout << rid << endl;
				}*/
				// print result
				size_t limit = 10;
				vector<string> outputs (limit + 1);
				for (size_t idx = 0; idx < q_select_fields.size(); idx++) {
					string select_field_name = q_select_fields[idx];
					outputs[0] += select_field_name + ", ";
					ColumnBase* colBase = (ColumnBase*) table->getColumnByName(select_field_name);
					if (colBase == NULL)
						continue;
					size_t limitCount = 0;
					if (colBase->getType() == ColumnBase::intType) {
						Column<int>* t = (Column<int>*) colBase;
						for (size_t rid = 0; rid < q_resultRid->size(); rid++) {
							//size_t rid = i;
							if (q_resultRid->at(rid)) {
								// convert from bitset to encode value
								//size_t encodeValue = (t->getEncodedVecValue()->at(rid)).to_ulong();
								size_t encodeValue = t->vecValueAt(rid);
								int* a = t->getDictionary()->lookup(encodeValue);
								outputs[limitCount+1] += to_string(*a) + ", ";
								if (++limitCount >= limit)
									break;
							}
						}
					}
					else {
						Column<string>* t = (Column<string>*) colBase;
						for (size_t rid = 0; rid < q_resultRid->size(); rid++) {
							//size_t rid = i;
							if (q_resultRid->at(rid)) {
								// convert from bitset to encode value
								//size_t encodeValue = (t->getEncodedVecValue()->at(rid)).to_ulong();
								size_t encodeValue = t->vecValueAt(rid);
								string* a = t->getDictionary()->lookup(encodeValue);
								outputs[limitCount+1] += *a + ", ";
								if (++limitCount >= limit)
									break;
							}
						}
					}
				}
				for (string output : outputs) {
					cout << output << endl;
				}
				cout << "Showing only "<<limit<<" result !" << endl;
				// query time
				std::cout << "Table Selection time: " << float(clock() - begin_time)/CLOCKS_PER_SEC << " seconds " << endl;
				// Processe done !
				break;
			}
			else
				cout << "Please enter a SELECT query !!!" << endl;
		}
		if (!queryValid) {
			printf("The SQL query is invalid or not supported !!!\n");
		}
	}

	return EXIT_SUCCESS;
}

