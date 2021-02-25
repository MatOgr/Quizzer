#include "User.hpp"


User::User() : 
    nick("JoshNoName"), socket_id(-1), score(-1), ready_to_play(false), admin(false) {
        //cout << "No data provided" << endl;
    };

User::User(const string nick, const int socket, const bool admin) : 
    nick(nick), socket_id(socket), score(0), ready_to_play(false), admin(admin) {};

// name says all
int User::getSocket() {
    return this->socket_id;
}
// name says all
int User::getScore() {
    return this->score;
}
// returns User's nick 
string User::getNick() {
    return this->nick;
}
// checks wheter the User is ready to start the quizz
bool User::getReady() {
    return this->ready_to_play;
}
// checks whether the User got host privileges
bool User::getAdmin() {
    return this->admin;
}
// name says all
void User::setSocket(const int socket) {
    this->socket_id = socket;
}
// name says all
void User::setScore(const int score) {
    this->score = score;
}
// increments User score by given #points value
void User::addToScore(const int points) {
    this->score += points;
}
// unique in the whole server 
void User::setNick(const string nik) {
    this->nick = nik;
}
// specifies whether the User is ready to start quizz
void User::setReady(const bool ready) {
    this->ready_to_play = ready;
}
// if User created/entered the room first becomes host
void User::setAdmin(const bool admin) {
    this->admin = admin;
}