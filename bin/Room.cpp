#include "Room.hpp"

Room::Room(Server *srv) :
    srv(srv), category("life"), players_number(PLAYERS_NR), questions_number(QUESTION_NR), game_running(false) {};

Room::Room(Server *srv, User *player) :
    srv(srv), category("life"), players_number(PLAYERS_NR), questions_number(QUESTION_NR), game_running(false) {
        //  make player admin - pointers needed
        players.clear();
        // players.reserve(players_number);
        player->setAdmin(true);
        players.push_back(player);      
    };

// Room::~Room() {};

// ### TODO
string Room::getRanking() {

    return "NOTHING";
}
// returns maximal player 
int Room::getMaxPlayersNumber() {
    return this->players_number;
}
// returns current number of players in the room
int Room::getCurrentPlayersNumber() {
    return this->players_number;
}
// returns maximal question number for considered room
int Room::getQuestionsNumber() {
    return this->questions_number;
}
// returns room's category 
string Room::getCategory() {
    return this->category;
}
// TO CHECK
bool Room::addPlayer(User * plyr) {
    if(players.size() == 0) 
        plyr->setAdmin(true);

    if(players.size() < players_number) {
        this->players.push_back(plyr);
        return true;
    }
    return false;
}
/*  ## NEED TO BE CHECKED
    looks for leaving player (by socket_id) - if found, erases it and returns true, false otherwise */
bool Room::removePlayer(int plyr_id) {
    // or removing by player_id created by the room joined
    auto leaving = find(players.begin(), players.end(), 
        [&plyr_id](User * u) { return u && (u->getSocket() == plyr_id); });
    if (leaving != players.end()) {
        players.erase(leaving);
        return true;
    }
    return false;
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
    // TODO - some questions base needed
void Room::loadQuestions(const vector<Question*> q_list) {
    questions = q_list;
}
// checks whether the game is in progress
bool Room::getGameState() {
    return this->game_running;
}
// sets the game status - 'true' in progress, 'false' in other case 
void Room::setGameState(const bool state) {
    this->game_running = state;
}
//  ## TODO
void Room::start() {
    loadQuestions(srv->getQuestions(category, questions_number));
}
//  ## TODO
void Room::end() {
    
}
