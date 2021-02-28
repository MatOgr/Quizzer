#include "Room.hpp"

Room::Room(Server *srv) :
    srv(srv), category("life"), players_number(PLAYERS_NR), questions_number(QUESTION_NR), game_running(false) {};
// probably unnecessary 
Room::Room(Server *srv, shared_ptr<User> player) :
    srv(srv), category("life"), players_number(PLAYERS_NR), questions_number(QUESTION_NR), game_running(false) {
        //  make player admin - pointers needed
        players.clear();
        // players.reserve(players_number);
        player->setAdmin(true);
        players.push_back(player);      
    };

 Room::~Room() {
     cout << "Room of category '" << category << "' has been closed..." << endl;
     questions.clear();
    //  players.clear();
 };

string Room::getRoomInfo() {
    string info = "";
    info.append(this->category).append(":");
    for(auto u : players) {
        info.append(u->getNick());
    }
    return info;
}

// Sorts list of players playing in the Room by their scores and returns string containing whole scoretable
string Room::getRanking() {
    string ranking;
    sort(players.begin(), players.end(), [](auto fst, auto snd) {
        return fst->getScore() < snd->getScore();
    });
    int i = 1;
    for(auto usr : players) {
        ranking.append(to_string(i)).
            append(". ").
            append(usr->getNick()).
            append("\t: \t").
            append(to_string(usr->getScore())).
            append("\n");
        i++;
    }
    
    return ranking;
}
// returns maximal players number
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
bool Room::addPlayer(shared_ptr<User> plyr) {

    if((int)players.size() < players_number) {
        if(players.size() == 0) 
            plyr->setAdmin(true);
        this->players.push_back(plyr);
        return true;
    }
    return false;
}
// removes pointer to the leaving player from 'players' list
bool Room::removePlayer(int plyr_id) {
    
    auto leaving = find_if(players.begin(), players.end(), [&plyr_id](auto u) { 
        return u->getSocket() == plyr_id;
    });
    
    if (leaving != players.end()) {
        cout << "Player " << (*leaving)->getNick() << " is leaving..." << endl;
        if((*leaving)->getAdmin() && players.size() > 1) {
            players.erase(leaving);
            players[0]->setAdmin(true);
        } else 
            players.erase(leaving);
        return true;
    }
    return false;
}
// Sets category of Questions to 'cat' 
void Room::setCategory(const string cat) {
    this->category = cat;
}
// Sets max number of questions in the Room to 'quest_num'
void Room::setQuestionNumber(const int quest_num) {
    this->questions_number = quest_num;
}
// Sets max number of Users allowed to be inside this Room
void Room::setPlayersNumber(const int plyr_number) {
    this->players_number = plyr_number;
}
// Sets Questions list to provided 'q_list' list 
void Room::loadQuestions(vector<shared_ptr<Question>> q_list) {
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
// checks whether each player in the Room is ready to play (pressed the button PLAY)
bool Room::checkReady() {
    for(auto u : players) 
        if (!u->getReady())
            return false;
    
    return true && players.size() > 1;
}
// ### TODO some time control/response listener needed
void Room::sendQuestionToUsers(const int idx) {
    for(auto q : questions) {
        string q_temp = "";
        q_temp.append("?").
            append(q->getContent() + ":").
            append(q->getAnswers() + ":").
            append(to_string(q->getCorrect()) + "\n");
        for (auto x : players) 
            srv->sendMsg(x->getSocket(), q_temp);        
        sleep(15);
    }
}
//  ## TODO - some countdown before the beginning
void Room::start() {
    
    loadQuestions(srv->getQuestions(category, questions_number));
    cout << "Room " + this->getCategory() + " imported " + to_string(questions.size()) + " questions, game begins soon...\n";
    sleep(3);
    setGameState(true);
    int len = questions.size();
    for (int i = 0; i < len; i++) {
        sendQuestionToUsers(i);
    }
    cout << "All questions sent in room " + this->category + ", time to count points\n";
    sleep(15);
    this->end();
}
//  To CHECK
void Room::end() {
    setGameState(false);
    string finalRanking = getRanking();
    for(auto u : players) {
        srv->sendMsg(u->getSocket(), "~" + finalRanking);
        u->setReady(false);
    }
}
