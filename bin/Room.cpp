#include "Room.hpp"

Room::Room(Server *srv) :
    srv(srv), category("life"), players_number(PLAYERS_NR), questions_number(QUESTION_NR), game_running(false) {};
// probably unnecessary 
Room::Room(Server *srv, User *player) :
    srv(srv), category("life"), players_number(PLAYERS_NR), questions_number(QUESTION_NR), game_running(false) {
        //  make player admin - pointers needed
        players.clear();
        // players.reserve(players_number);
        player->setAdmin(true);
        players.push_back(player);      
    };

 Room::~Room() {
     cout << "Room of category '" << category << "' has been closed..." << endl;
 };

// Sorts list of players playing in the Room by their scores and returns string containing whole scoretable
string Room::getRanking() {
    string ranking;
    sort(players.begin(), players.end(), [](User* fst, User* snd) {
        return fst->getScore() < snd->getScore();
    });
    int i = 1;
    for(User * usr : players) {
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
bool Room::addPlayer(User * plyr) {
    if(players.size() == 0) 
        plyr->setAdmin(true);

    if(players.size() < players_number) {
        this->players.push_back(plyr);
        return true;
    }
    return false;
}
// removes pointer to the leaving player from 'players' list
bool Room::removePlayer(int plyr_id) {
    
    auto leaving = find(players.begin(), players.end(), 
        [&plyr_id](User * u) { return u && (u->getSocket() == plyr_id); });
    
    if (leaving != players.end()) {
        cout << "Player " << (*leaving)->getNick() << " is leaving..." << endl;
        if((*leaving)->getAdmin()) {
            (*leaving)->setAdmin(false);
            players.erase(leaving);
            (*players.begin())->setAdmin(true);
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
// checks whether each player in the Room is ready to play (pressed the button PLAY)
bool Room::checkReady() {
    for(User * u : players) 
        if (!u->getReady())
            return false;
    
    return true;
}
// ### TODO some time control/response listener needed
void Room::sendQuestionsToUsers() {
    for(Question * q : questions) {
        string q_temp;
        q_temp.append("?").
            append(q->getContent()).
            append(q->getAnswers()).
            append(to_string(q->getCorrect()));
        for (User * x : players) 
            srv->sendMsg(x->getSocket(), q_temp);

        //  wait for user response ???
    }
}
//  ## TODO - some countdown before the beginning
void Room::start() {
    if (checkReady()) {
        loadQuestions(srv->getQuestions(category, questions_number));
        //  sleep(5);
        setGameState(true);
        sendQuestionsToUsers();
    }
}
//  To CHECK
void Room::end() {
    setGameState(false);
    string finalRanking = getRanking();
    for(User* u : players) {
        srv->sendMsg(u->getSocket(), finalRanking + "\n~");
        u->setReady(true);
    }
}
