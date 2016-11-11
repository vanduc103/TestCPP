
#include "server/ServerSocket.h"
#include "server/SocketException.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include "TestCpp.h"
#include "Table.h"

using namespace std;

int main(int argc, char* argv[]) {
	puts("***** Simple Column-Store Database start ******");
	// Create table orders
	string createQuery = "create table orders (o_orderkey integer, o_orderstatus text, o_totalprice integer, o_comment text)";
	Table* orders = createTable(createQuery);

    try {
        // Create the Socket
    	int port = 30000;
        ServerSocket server(port);
        std::cout << ">>>> Server is listening at port: " << port << std::endl;

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
					cout << commandType << endl;
					if (commandType == "UPDATE") {
						cout << ">> Update command" << endl;
						updateCommand(*orders, command);
					}
					else if (commandType == "INSERT") {
						cout << ">> Insert command" << endl;
						insertCommand(*orders, command);
					}
					else if (commandType == "SCAN") {
						cout << ">> Scan command" << endl;
						string result = scanCommand(*orders, command);
						cout << "SCAN result: " << endl;
						cout << result;
					}

                    client << data;
                    /* CODE  END  */
                }
            } catch(SocketException&) {}
        }
    } catch(SocketException& e) {
        std::cout<< "Exception caught: " << e.description() << std::endl;
    }

    return 0;
}

