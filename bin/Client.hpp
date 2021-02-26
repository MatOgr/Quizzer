#ifndef Client_hpp
#define Client_hpp

#include "Server.hpp"

class Client {
private:
    int connection_sock;
    int room_id;
    string nick;
    bool *server_state;
    Server* srv;

public:
    // Client();
    Client(const int sock, Server* server, bool* state);

    void setConnSock(const int sock);
    void setRoomId(const int id);
    void setNick(const string nick);
    void setSrvState(bool* state);

    int getConnSock();
    int getRoomId();
    string getNick();
    bool getSrvState();
    Server* getSrv();

    void connectionHandler();
};


#endif