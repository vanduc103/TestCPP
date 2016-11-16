
#include "ClientSocket.h"
#include "SocketException.h"
#include <string>
#include <iostream>
#include <fstream>
#include <thread>

using namespace std;

void startNewClient(int interval, std::string command) {
	std::thread([interval, &command]()
	{
		while (true) {
			// Create the Socket
			ClientSocket c_socket("localhost", 30000);
			std::string reply;

			//std::ifstream in("orders_sample.tbl");
			std::ofstream out("client.log");

			cout << "Client command: " << command;
			if(command != "") {
				try {
					// send command
					out << "[Sending]\t" + command + "\n";
					std::cout << "[Sending]\t" + command + "\n";
					c_socket << command;
					// receive reply from server
					c_socket >> reply;
				} catch (SocketException&) {}
				out << "[Response]\n" << reply << "\n";
				out.flush();
				std::cout << "[Response]\t" << endl << reply << "\n";
			}
		}
	}).detach();
}

int main(int argc, char* argv[]) {
    try {
    	string command = "SCAN|56789|o_totalprice|>";
        startNewClient(1000, command);
    } catch(SocketException& e) {
        std::cout << "Exception caught: " << e.description() << std::endl;
    }

    return 0;
}
