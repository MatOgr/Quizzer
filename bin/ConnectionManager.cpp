#include "ConnectionManager.hpp"

ConnectionManager::ConnectionManager(User *usr, Server *srv) :
    user(usr), server(srv) {};

ConnectionManager::~ConnectionManager() {
    shutdown(user->getSocket(), SHUT_RDWR);
    close(user->getSocket());
    cout << "User " << user->getNick() << " disconnected...\n";
}

void ConnectionManager::sendMsg(const string msg) {
    const char *message = msg.c_str();
    write(user->getSocket(), message, (ssize_t)msg.length());
}

// ### TODO other message options
void ConnectionManager::listen(const int socket) { {
    user->setSocket(socket);
    const char *resp = ("Hello " + user->getNick() + "...\n").c_str();
    char buffer[255];
    char operation;
    write(user->getSocket(), resp, (ssize_t)strlen(resp));
    int anything = 1;

    do {
        anything = read(user->getSocket(), &operation, 1);
        if(anything > 0) {
            // cout << "User " << user->getNick() << " sent: \n";
            read(user->getSocket(), buffer, 3);
            ssize_t size = atoi(buffer);
            char *msg = (char*)malloc(size + 2);    // for consideration 
            read(user->getSocket(), msg, size + 1);
            string message = (string)msg;

            //  answer for question
            if(message[0] == '?') {
                // verify answer
            }

            free(msg);
        }
    } while(anything > 0);
    cout << user->getNick() << " has left...\n";
}
