
#include "server/ServerSocket.h"
#include "server/SocketException.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
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
					string result = "";
					if (commandType == "UPDATE") {
						cout << ">> Update command" << endl;
						result = updateCommand(*orders, command);
					}
					else if (commandType == "INSERT") {
						cout << ">> Insert command" << endl;
						result = insertCommand(*orders, command);
					}
					else if (commandType == "SCAN") {
						cout << ">> Scan command" << endl;
						result = scanCommand(*orders, command);
					}
					else {
						result = "NO VALID COMMAND FOUND !";
					}
					// send result to client
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

