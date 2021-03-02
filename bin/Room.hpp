#ifndef Room_hpp
#define Room_hpp

#include <bits/stdc++.h>
#include <unistd.h>

#include "Server.hpp"
#include "Question.hpp"
#include "User.hpp"

#define PLAYERS_NR 5
#define QUESTION_NR 10

using namespace std;

class Server;

class Room {
private:
    Server *srv;
    vector<shared_ptr<User>> players;
    vector<shared_ptr<Question>> questions;
    string category;
    int players_number;
    int questions_number;
    bool game_running;
public:
    Room(Server* serv);
    Room(Server* serv, shared_ptr<User> player);        // not needed???
    ~Room();

    string getRanking();  
    int getMaxPlayersNumber();
    int getCurrentPlayersNumber();
    int getQuestionsNumber();
    string getCategory();
    bool getGameState();
    string getRoomInfo();

    bool addPlayer(shared_ptr<User> plyr);
    bool removePlayer(int plyr_id);

    void resetScores();
    void setGameState(const bool state);
    void setCategory(const string cat);
    void setQuestionNumber(const int quest_num);
    void setPlayersNumber(const int number);

    void loadQuestions(vector<shared_ptr<Question>> q_list);
    bool checkReady();
    bool sendQuestionsToUsers();
    void start();
    void end();
};

/*
    First player becomes host, minimum 2 players to start the game,
    creating the Room - what parameters???
    players should contain pointers of Users - any other idea??
*/
#endif