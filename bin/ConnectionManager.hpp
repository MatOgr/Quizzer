#ifndef ConnectionManager_hpp
#define ConnectionManager_hpp

#include "../Server.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
// #include <fcntl.h>
#include <unistd.h>
// #include <netinet/in.h>
// #include <netdb.h>

class ConnectionManager {
private:
    User * user;
    Server * server;

public:
    ConnectionManager(User *usr, Server *srv);
    ~ConnectionManager();
    void sendMsg(const string msg);
    void listen(const int socket);
};

#endif