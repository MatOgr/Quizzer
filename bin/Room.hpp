#ifndef Room_hpp
#define Room_hpp

#include <bits/stdc++.h>
#include "User.hpp"
#include "Question.hpp"
#include "../Server.hpp"
#include <unistd.h>

#define PLAYERS_NR 5
#define QUESTION_NR 10

using namespace std;

class Room {
private:
    Server *srv;
    vector<User*> players;
    vector<Question*> questions;
    string category;
    int players_number;
    int questions_number;
    bool game_running;
    mutex room_mutex;
public:
    Room(Server* serv);
    Room(Server* serv, User * player);        // not needed???
    ~Room();

    string getRanking();  
    int getMaxPlayersNumber();
    int getCurrentPlayersNumber();
    int getQuestionsNumber();
    string getCategory();
    bool getGameState();

    bool addPlayer(User* plyr);
    bool removePlayer(int plyr_id);

    void setGameState(const bool state);
    void setCategory(const string cat);
    void setQuestionNumber(const int quest_num);
    void setPlayersNumber(const int number);

    void loadQuestions(const vector<Question*> q_list);
    bool checkReady();
    void sendQuestionToUsers(const int idx);
    void start();
    void end();
};

/*
    First player becomes host, minimum 2 players to start the game,
    creating the Room - what parameters???
    players should contain pointers of Users - any other idea??
*/
#endif