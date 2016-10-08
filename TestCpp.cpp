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
#include <stdexcept>
#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/dynamic_bitset.hpp>

#include "Dictionary.h"
#include "Column.h"
#include "ColumnBase.h"
#include "Table.h"
#include "SQLParser.h"

using namespace std;

// print operator type
std::ostream& operator<<(std::ostream& out, const ColumnBase::OP_TYPE value){
    const char* s = 0;
#define PROCESS_VAL(p, str) case(p): s = str; break;
    switch(value){
        PROCESS_VAL(ColumnBase::OP_TYPE::equalOp, "=");
        PROCESS_VAL(ColumnBase::OP_TYPE::neOp, "<>");
        PROCESS_VAL(ColumnBase::OP_TYPE::gtOp, ">");
        PROCESS_VAL(ColumnBase::OP_TYPE::geOp, ">=");
        PROCESS_VAL(ColumnBase::OP_TYPE::ltOp, "<");
        PROCESS_VAL(ColumnBase::OP_TYPE::leOp, "<=");
        PROCESS_VAL(ColumnBase::OP_TYPE::likeOp, "LIKE");
    }
#undef PROCESS_VAL
    return out << s;
}

// print column type
std::ostream& operator<<(std::ostream& out, const ColumnBase::COLUMN_TYPE value){
    const char* s = 0;
#define PROCESS_VAL(p, str) case(p): s = str; break;
    switch(value){
        PROCESS_VAL(ColumnBase::intType, "INTEGER");
        PROCESS_VAL(ColumnBase::charType, "TEXT");
        PROCESS_VAL(ColumnBase::varcharType, "TEXT");
    }
#undef PROCESS_VAL
    return out << s;
}

int main(void) {
	puts("***** Simple Column-Store Database start ******");

	string createQuery;
	cout << "Enter create table statement: ";
	getline(cin, createQuery);
	hsql::SQLParserResult* pCreateQuery = hsql::SQLParser::parseSQLString(createQuery);
	string tableName;
	vector<string> cColumnName;
	vector<ColumnBase::COLUMN_TYPE> cColumnType;
	if (pCreateQuery->isValid) {
		hsql::SQLStatement* stmt = pCreateQuery->getStatement(0);
		if (stmt->type() == hsql::StatementType::kStmtCreate) {
			hsql::CreateStatement* createStmt = (hsql::CreateStatement*) stmt;
			tableName = createStmt->tableName;
			vector<hsql::ColumnDefinition*>* cols = createStmt->columns;
			for (hsql::ColumnDefinition* colDef : *cols) {
				cColumnName.push_back(colDef->name);
				switch (colDef->type) {
					case hsql::ColumnDefinition::INT:
					case hsql::ColumnDefinition::DOUBLE:
						cColumnType.push_back(ColumnBase::intType);
						break;
					case hsql::ColumnDefinition::TEXT:
						cColumnType.push_back(ColumnBase::charType);
						break;
				}
			}
		}
	}
	else {
		cout << "Create table statement is Invalid !" << endl;
		return -1;
	}
	cout << "tablename: " << tableName << endl;
	// init table
	vector<ColumnBase*> columns;
	Table* table = new Table(columns);
	table->setName(tableName);
	// init column
	for (size_t i = 0; i < cColumnName.size(); i++) {
		string name = cColumnName[i];
		ColumnBase::COLUMN_TYPE type = cColumnType[i];
		cout << "colname: " << name << endl;
		cout << "col type: " << type << endl;
		// create new column
		ColumnBase* colBase = new ColumnBase();
		if (type == ColumnBase::intType) {
			Column<int>* col = new Column<int>();
			colBase = col;
		}
		else {
			Column<string>* col = new Column<string>();
			colBase = col;
		}
		colBase->setName(name);
		colBase->setType(type);
		if (i == 0)
			colBase->setPrimaryKey(true);
		columns.push_back(colBase);
	}

	// Column 0
	/*Column<int>* col0 = new Column<int>();
	col0->setName("o_orderkey");
	col0->setType(ColumnBase::intType);
	col0->setSize(4);
	col0->setPrimaryKey(true);

	// Column 1
	Column<string>* col1 = new Column<string>();
	col1->setName("o_orderstatus");
	col1->setType(ColumnBase::charType);
	col1->setSize(1);

	// Column 2
	Column<int>* col2 = new Column<int>();
	col2->setName("o_totalprice");
	col2->setType(ColumnBase::intType);
	col2->setSize(4);

	// Column 3
	Column<string>* col3 = new Column<string>();
	col3->setName("o_comment");
	col3->setType(ColumnBase::charType);
	col3->setSize(30);*/

	// calculate time execution
	clock_t begin_time = clock();

	// read file
	//cout << "Enter file path: ";
	string filePath;
	//getline(cin, filePath);
	//ifstream infile(filePath);
	ifstream infile("/home/duclv/Downloads/data1M.csv");
	//ifstream infile("/home/duclv/homework/data1M.csv");
	string line;
	string delim = ",";
	int row = 0;
	while(getline(infile, line)) {
		size_t last = 0; size_t next = 0;
		char idx = 0;
		vector<string> items;	// split items
		string lastItem;
		string token;
		while ((next = line.find(delim, last)) != string::npos) {
		    token = line.substr(last, next - last);
		    last = next + delim.length();
		    if (idx >= table->numOfColumns() - 1)
		    	lastItem += token + delim;
		    else
		    	items.push_back(token);
		    ++idx;

		    /*i++;
		    // key is 1st column
			if (i == 1) {
				int key = stoi(token);
				col0->updateDictionary(key, !col0->primaryKey());
			}
		    // status is 2nd column
			else if (i == 2) {
				boost::replace_all(token, "\"", "");
		    	col1->updateDictionary(token, col1->getType() == ColumnBase::intType);
		    }
		    // totalprice is 3rd column
		    else if (i == 3) {
		    	int totalprice = stoi(token);
		    	col2->updateDictionary(totalprice, col2->getType() == ColumnBase::intType);
		    }
		    // comment is from 4th column
			if (i >= 4) comment += token + delim;*/
		}
		// get the last token and add to last item
		token = line.substr(last);
		lastItem += token;
		items.push_back(lastItem);
		/*boost::replace_all(comment, "\"", "");
		col3->updateDictionary(comment, col3->getType() == ColumnBase::intType);*/

		// process input data based on column type
		for (int i = 0; i < table->numOfColumns(); i++) {
			string item = items[i];
			ColumnBase* colBase = columns[i];
			if (colBase->getType() == ColumnBase::intType) {
				int intValue = stoi(item);
				bool sorted = colBase->primaryKey();
				// update dictionary
				Column<int>* col = (Column<int>*) colBase;
				col->updateDictionary(intValue, sorted);
			}
			else {
				// char or varchar type
				boost::replace_all(item, "\"", "");
				bool sorted = false;
				// update dictionary
				Column<string>* col = (Column<string>*) colBase;
				col->updateDictionary(item, sorted);
			}
		}
		++row;
	}
	infile.close();
	// print distinct numbers
	for (ColumnBase* colBase : columns) {
		if (colBase->getType() == ColumnBase::intType) {
			Column<int>* col = (Column<int>*) colBase;
			cout << col->getName() << " #distinct values = " << col->getDictionary()->size()<<"/"<<row << endl;
		}
		else {
			Column<string>* col = (Column<string>*) colBase;
			cout << col->getName() << " #distinct values = " << col->getDictionary()->size()<<"/"<<row << endl;
		}
	}
	/*cout << col0->getName() << " #distinct values = " << col0->getDictionary()->size()<<"/"<<row << endl;
	cout << col1->getName() << " #distinct values = " << col1->getDictionary()->size()<<"/"<<row << endl;
	cout << col2->getName() << " #distinct values = " << col2->getDictionary()->size()<<"/"<<row << endl;
	cout << col3->getName() << " #distinct values = " << col3->getDictionary()->size()<<"/"<<row << endl;*/


	// process columns of table
	table->processColumn();

	// print result
	//col1->printVecValue(10);
	//col2->printVecValue(10);
	//((Column<string>*)columns[1])->getDictionary()->print(10);

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
			vector<string> q_where_values;

			hsql::SQLStatement* stmt = pResult->getStatement(0);
			if (stmt->type() == hsql::StatementType::kStmtSelect) {
				hsql::SelectStatement* select = (hsql::SelectStatement*) stmt;
				q_table = select->fromTable->getName();
				cout << "Table name: " << q_table << endl;
				if (q_table != table->getName()) {queryValid = false; break;}

				for (hsql::Expr* expr : *select->selectList) {
					if (expr->type == hsql::ExprType::kExprStar) {
						q_select_fields.push_back("o_orderkey");
						q_select_fields.push_back("o_orderstatus");
						q_select_fields.push_back("o_totalprice");
						q_select_fields.push_back("o_comment");
					}
					else if (expr->type == hsql::ExprType::kExprColumnRef)
						q_select_fields.push_back(expr->name);
					else if (expr->type == hsql::ExprType::kExprFunctionRef) {
						q_select_fields.push_back(string(expr->name) + string("#") + string(expr->expr->name));
					}
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

							hsql::ExprType literalType = expr->expr2->type;
							if (literalType == hsql::ExprType::kExprLiteralInt)
								q_where_values.push_back(to_string(expr->expr2->ival));
							else if (literalType == hsql::ExprType::kExprColumnRef)
								q_where_values.push_back(expr->expr2->name);
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
								else if (expr1->op_char == '=')
									q_where_ops.push_back(ColumnBase::OP_TYPE::equalOp);

								if (expr1->expr2->type == hsql::ExprType::kExprLiteralInt)
									q_where_values.push_back(to_string(expr1->expr2->ival));
								else if (expr1->expr2->type == hsql::ExprType::kExprColumnRef)
									q_where_values.push_back(expr1->expr2->name);
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
									q_where_values.push_back(to_string(expr2->expr2->ival));
								else if (expr2->expr2->type == hsql::ExprType::kExprColumnRef)
									q_where_values.push_back(expr2->expr2->name);
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
						cout << "where values[" << i << "] = " << q_where_values[i] << endl;
					}
				}
				// execute query
				vector<bool>* q_resultRid = new vector<bool>(); // contain query result row id
				vector<vector<size_t>> q_results;
				for (size_t fidx = 0; fidx < q_where_fields.size(); fidx++) {
					string q_where_field = q_where_fields[fidx];
					ColumnBase::OP_TYPE q_where_op = q_where_ops[fidx];
					string q_where_value = q_where_values[fidx];

					// get column by name then cast to appropriate column object based on type
					ColumnBase* colBase = table->getColumnByName(q_where_field);
					if (colBase == NULL) continue;
					switch (colBase->getType()) {
						case ColumnBase::COLUMN_TYPE::intType:
						{
							Column<int>* t = (Column<int>*) colBase;
							int searchValue = 0;
							try {
								searchValue = stoi(q_where_value);
							} catch (exception& e) {
								cerr << "Exception: " << e.what() << endl;
								break;
							}
							bool initQueryResult = (fidx == 0);
							t->selection(searchValue, q_where_op, q_resultRid, initQueryResult);
							// build hashmap
							map<int, vector<size_t>*> hashmap;
							t->buildHashmap(hashmap);
							break;
						}
						case ColumnBase::charType:
						case ColumnBase::varcharType:
						{
							Column<string>* t = (Column<string>*) colBase;
							string searchValue = q_where_value;
							bool initQueryResult = (fidx == 0);
							t->selection(searchValue, q_where_op, q_resultRid, initQueryResult);
							break;
						}
					}
				}
				size_t totalResult = 0;
				for (size_t rid = 0; rid < q_resultRid->size(); rid++) {
					if (q_resultRid->at(rid))
						++totalResult;
				}
				// print result
				cout << "*** Query result ***"<< endl;
				size_t limit = 10;
				size_t limitCount = 0;
				vector<string> outputs (limit + 1);
				for (size_t idx = 0; idx < q_select_fields.size(); idx++) {
					string select_field_name = q_select_fields[idx];
					outputs[0] += select_field_name + ", ";
					ColumnBase* colBase = (ColumnBase*) table->getColumnByName(select_field_name);
					if (colBase == NULL) continue;
					if (colBase->getType() == ColumnBase::intType) {
						Column<int>* t = (Column<int>*) colBase;
						vector<int> tmpOut = t->projection(q_resultRid, limit, limitCount);
						for (size_t i = 0; i < tmpOut.size(); i++) {
							outputs[i+1] += to_string(tmpOut[i]) + ",   ";
						}
					}
					else {
						Column<string>* t = (Column<string>*) colBase;
						vector<string> tmpOut = t->projection(q_resultRid, limit, limitCount);
						for (size_t i = 0; i < tmpOut.size(); i++) {
							outputs[i+1] += "\"" + tmpOut[i] + "\"" + ",   ";
						}
					}
				}
				for (string output : outputs) {
					if (!output.empty())
						cout << output << endl;
				}
				if (limitCount >= limit)
					cout << "Showing "<<limit<<"/"<<totalResult<<" results !" << endl;
				else if (limitCount == 0)
					cout << "No result found !" << endl;
				else
					cout << "Showing "<<limitCount<<"/"<<totalResult<<" results !" << endl;
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

