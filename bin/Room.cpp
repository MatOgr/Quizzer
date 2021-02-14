#include "Room.hpp"

Room::Room() :
    players(nullptr), questions(nullptr), category("life"), players_number(4), questions_number(5) {};

Room::Room(User player) :
    players(nullptr), questions(nullptr), category("life"), players_number(4), questions_number(5) {
        //  make player admin - pointers needed
        players->push_back(player);        
    };

string Room::getRanking() {
    //  TODO
    return "NOTHING";
}

int Room::getMaxPlayersNumber() {
    return this->players_number;
}

int Room::getCurrentPlayersNumber() {
    return this->players_number;
}

int Room::getQuestionsNumber() {
    return this->questions_number;
}

string Room::getCategory() {
    return this->category;
}

void Room::addPlayer(User plyr) {
    this->players->push_back(plyr);
}

// ### TODO
void Room::removePlayer(int plyr_id) {
    // remove(this->players->begin(), this->players->end(), plyr);      # removing in vector 
    // or removing by player_id created by the room joined
}

void Room::setCategory(const string cat) {
    this->category = cat;
}

void Room::setQuestionNumber(const int quest_num) {
    this->questions_number = quest_num;
}

void Room::setPlayersNumber(const int plyr_number) {
    this->players_number = plyr_number;
}

void Room::loadQuestions() {
    // TODO - some question base needed
}