#include "User.hpp"


User::User() : 
    nick(""), socket_id(-1), score(-1), start_game(false), admin(false) {
        //cout << "No data provided" << endl;
    };

User::User(const string nick, const int socket, const bool admin) : 
    nick(nick), socket_id(socket), score(0), start_game(false), admin(admin) {};

int User::getSocket() {
    return this->socket_id;
}

int User::getScore() {
    return this->score;
}

string User::getNick() {
    return this->nick;
}

bool User::getReady() {
    return this->start_game;
}

bool User::getAdmin() {
    return this->admin;
}

void User::setSocket(const int socket) {
    this->socket_id = socket;
}

void User::setScore(const int score) {
    this->score = score;
}

void User::addToScore(const int points) {
    this->score += points;
}

void User::setNick(const string nik) {
    this->nick = nik;
}

void User::setReady(const bool ready) {
    this->start_game = ready;
}

void User::setAdmin(const bool admin) {
    this->admin = admin;
}