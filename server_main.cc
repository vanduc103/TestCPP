
#include "server/ServerSocket.h"
#include "server/SocketException.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <thread>
#include <map>
#include "TestCpp.h"
#include "Table.h"
#include "GarbageCollector.h"

using namespace std;


void restartWaitingTransaction(int interval, Transaction* transaction,
		Table* table, GarbageCollector* garbage, ServerSocket* server)
{
	std::thread([interval, transaction, table, garbage, server]()
	{
		while (true) {
			std::this_thread::sleep_for(
			std::chrono::milliseconds(interval));
			// get transaction and execute updateCommand
			vector<size_t> txWaitingList = transaction->getWaitingList();
			for (size_t i = 0; i < txWaitingList.size(); i++) {
				cout << "waiting transaction #" << i << endl;
				Transaction::transaction tx = transaction->getTransaction(txWaitingList.at(i));
				size_t txIdx = tx.txnId;
				ServerSocket* client = tx.client;
				// execute command and return to client
				Logging* logging = new Logging();
				string result = updateCommand(client, table, transaction, logging, tx.command, garbage, txIdx);
				if (client != NULL) {
					try {
						server->accept(*client);
						(*client) << result;
					} catch(SocketException& e) {
						std::cout<< "Exception caught: " << e.description() << std::endl;
					}
				}
			}
		}
	}).detach();
}

void flushRedoLogging(int interval, Logging* logging)
{
	std::thread([interval, logging]()
	{
		while (true) {
			std::this_thread::sleep_for(
			std::chrono::milliseconds(interval));
			logging->redoLogSave();
		}
	}).detach();
}

int main(int argc, char* argv[]) {
	puts("***** Simple Column-Store Database start ******");
	// Create table orders
	string createQuery = "create table orders (o_orderkey integer, o_orderstatus text, o_totalprice integer, o_comment text)";
	Table* ordersTable = createTable(createQuery);

	// Transaction
	Transaction* transaction = new Transaction();

	// Logging
	Logging* publicLogging = new Logging();
	flushRedoLogging(30000, publicLogging);

	// start Garbage collection as thread
	GarbageCollector* garbage = new GarbageCollector();
	garbage->setTransaction(transaction);
	garbage->setTable(ordersTable);
	// run each 10 s
	garbage->start(10000);

    try {
        // Create the Socket
    	int port = 30000;
        ServerSocket server(port);
        std::cout << ">>>> Server is listening at port: " << port << std::endl;
        // restart waiting transaction as thread
		restartWaitingTransaction(10000, transaction, ordersTable, garbage, &server);

        while(true) {
            ServerSocket client;
            server.accept(client);

            try {
                while(true) {
                    std::string data;
                    client >> data;
                    std::cout << "[Received]\t" << data << std::endl;

                    /* CODE BEGIN */
                    // split data into command
                    vector<string> command;
                    string token;
                    string delim = "|";
                    size_t last = 0; size_t next = 0;
                    while ((next = data.find(delim, last)) != string::npos) {
                    	token = data.substr(last, next - last);
						last = next + delim.length();
						command.push_back(token);
                    }
                    // get the last token
					token = data.substr(last);
					command.push_back(token);

					// command type
					string commandType = command.at(0);
					string result = "";
					// logging
					Logging* logging = new Logging();
					if (commandType == "UPDATE") {
						cout << ">> Update command" << endl;
						result = updateCommand(&client, ordersTable, transaction, logging, command, garbage);
					}
					else if (commandType == "INSERT") {
						cout << ">> Insert command" << endl;
						result = insertCommand(ordersTable, transaction, logging, command);
					}
					else if (commandType == "SCAN") {
						cout << ">> Scan command" << endl;
						result = scanCommand(ordersTable, transaction, command);
					}
					else {
						result = "NO VALID COMMAND FOUND !";
					}
					delete logging;
					// send result to client
					cout << result << endl;
					client << result;
                    /* CODE  END  */
                }
            } catch(SocketException&) {}
        }
    } catch(SocketException& e) {
        std::cout<< "Exception caught: " << e.description() << std::endl;
    }

    return 0;
}

