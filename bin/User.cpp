#include "User.hpp"


User::User() : 
    nick("JoshNoName"), socket_id(-1), room_id(-1), score(0), ready_to_play(false), admin(false) {};

User::User(const string nick, const int socket, const bool admin) : 
    nick(nick), socket_id(socket), room_id(-1), score(0), ready_to_play(false), admin(admin) {};

// return socket descriptor that this User is connected to
int User::getSocket() {
    return this->socket_id;
}
// returns Room's id that User is in - -1 if not in any Room
int User::getRoom() {
    return room_id;
}
// return current amount of points gained by User
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
// set Room's id that User uses - -1 if none
void User::setRoom(const int id) {
    room_id = id;
}
// unique in the whole server 
void User::setNick(const string nick) {
    this->nick = nick;
}
// specifies whether the User is ready to start quizz
void User::setReady(const bool ready) {
    this->ready_to_play = ready;
}
// if User created/entered the room first becomes host
void User::setAdmin(const bool admin) {
    this->admin = admin;
}