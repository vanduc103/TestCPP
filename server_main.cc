
#include "server/ServerSocket.h"
#include "server/SocketException.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        // Create the Socket
    	int port = 30000;
        ServerSocket server(port);
        std::cout << "Server is listening at port: " << port << std::endl;

        while(true) {
            ServerSocket new_sock;
            server.accept(new_sock);

            try {
                while(true) {
                    std::string data;
                    new_sock >> data;
                    std::cout << "[Received]\t" << data << std::endl;

                    /* CODE BEGIN */
                    new_sock << data;
                    /* CODE  END  */
                }
            } catch(SocketException&) {}
        }
    } catch(SocketException& e) {
        std::cout<< "Exception caught: " << e.description() << std::endl;
    }

    return 0;
}

