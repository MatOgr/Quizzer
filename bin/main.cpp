#include "Server.hpp"

using namespace std;

int main() {
    Server *server = new Server();
    
    cout << "Say hello!\n";

    server->run();

    delete server;

    return 0;
}