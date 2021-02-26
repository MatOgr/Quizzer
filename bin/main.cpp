#include "Server.hpp"
#include "Client.hpp"

using namespace std;

int main() {
    Server server;
    
    cout << "Say hello!\n";

    //  First approach
    server.run();

    /*      Second approach - 

    while(true) {
        int client_socket = server.run();
        Client newClient(client_socket, &server, server.getStatus());
        thread th(&Client::connectionHandler, &newClient);
        cout << "Connected some user" << endl;
    }
    */

    return 0;
}