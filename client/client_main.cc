
#include "ClientSocket.h"
#include "SocketException.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

int main(int argc, char* argv[]) {
    try {
        // Create the Socket
        ClientSocket c_socket("localhost", 30000);
        std::string line;
        std::string reply;

        std::ifstream in("orders_sample.tbl");
        std::ofstream out("client.log");

        while(!in.eof()) {
            getline(in, line);
            if(line != "") {
                try {
                    std::string message = "INSERT|" + line;
                    out << "[Sending]\t" + message + "\n";
                    std::cout << "[Sending]\t" + message + "\n";
                    c_socket << message;
                    c_socket >> reply;
                } catch (SocketException&) {}
                out << "[Response]\t" << reply << "\n";
                std::cout << "[Response]\t" << reply << "\n";
            }
        }
    } catch(SocketException& e) {
        std::cout << "Exception caught: " << e.description() << std::endl;
    }

    return 0;
}
