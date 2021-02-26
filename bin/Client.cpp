#include "Client.hpp"

#define BUFFER_SIZE 255

// Client::Client() : srv() {};

Client::Client(const int sock, Server* server, bool *state) : 
    connection_sock(sock), room_id(-1), server_state(state), srv(server) {};

void Client::setConnSock(const int sock) {
    connection_sock = sock;
}

void Client::setRoomId(const int id) {
    room_id = id;
}

void Client::setSrvState(bool* state) {
    server_state = state;
}

int Client::getConnSock() {
    return connection_sock;
}

int Client::getRoomId() {
    return room_id;
}

bool Client::getSrvState() {
    return server_state;
}

Server* Client::getSrv() {
    return srv;
}

void Client::connectionHandler() {
    
    bool connected = true;

    char buffer[BUFFER_SIZE], header;
    string response;

    while(connected && *(server_state)) {
        if(read(connection_sock, &header, 1) <= 0)
            break;

        // instructions
        if (header == '@') {        //  change nick
            read(connection_sock, buffer, BUFFER_SIZE);
            getSrv()->setNick(connection_sock, buffer);
            nick = buffer;
        }        
        else if(header == '#') {           // leaving the server
            getSrv()->popUserOut(connection_sock, room_id);
            connected = false;
        }  
        else if (room_id == -1) {
            if (header == '*') {           // create the room
                response = (getSrv()->createRoom()) ? 
                "Room has been created" : "Couldn't create new room - may be the limit was reached...";
                getSrv()->sendMsg(connection_sock, response);
            }
            else if(header == '>') {      // join the room
                read(connection_sock, buffer, 2);
                int r_id = stoi(buffer);
                if (getSrv()->putUserInRoom(connection_sock, r_id)) {
                    room_id = r_id;
                    response = "Welcome to Room number" + to_string(r_id);
                    getSrv()->sendMsg(connection_sock, response);
                } else 
                    getSrv()->sendMsg(connection_sock, "Couldn't join selected room - try another one!");
            }
            else if (header == '+') {      // add question
                read(connection_sock, buffer, BUFFER_SIZE);
                getSrv()->addQuestion(buffer);
                getSrv()->sendMsg(connection_sock, "Question added!");
            }      
        }
        else {    
            if(header == '!') {      // ready to play
                getSrv()->setUserReady(connection_sock, true);
            }
            else if(header == '%') {      // answer ?? may be verification on client side???
                
            }
            else if(header == '<') {      // leaving the room
                getSrv()->popUserOut(connection_sock, room_id);
                response = "So you left, take care, mate!";
                getSrv()->sendMsg(connection_sock, response);
            }
            else if(header == '$') {      // change category
                read(connection_sock, buffer, 255);
                response = buffer;
                int r_id = stoi(response.substr(0, response.find(':')));
                getSrv()->getRoomsList()->at(r_id).
                    setCategory(response.erase(0, response.find(':')));
                response = "So you have changed the Room's category to " + response;
                getSrv()->sendMsg(connection_sock, response);
            }
        }
        memset(buffer, 0, BUFFER_SIZE);
        header = 0;
    }
    // handler closing - user leaving connection
    response = "You have been disconnected - world's cruel, hope U kno what'ya doin'...";
    getSrv()->sendMsg(connection_sock, response);
    getSrv()->disconnectUser(connection_sock);


    // return (void *)0;
}