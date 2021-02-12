#ifndef Room_hpp
#define Room_hpp

#include <bits/stdc++.h>
#include "User.hpp"
#include "Question.hpp"

using namespace std;

class Room {
private:
    vector<User>* players;
    vector<Question>* questions;
    string category;
    int players_number;
    int questions_number;
public:
    Room();
    Room(User player);
    ~Room();

    string getRanking();
    int getMaxPlayersNumber();
    int getCurrentPlayersNumber();
    int getQuestionsNumber();
    string getCategory();
    void addPlayer(User plyr);
    void removePlayer(User plyr);
    void setCategory(const string cat);
    void setQuestionNumber(const int quest_num);
    void setPlayersNumber(const int number);
    void loadQuestions();
};

/*
    First player becomes host, minimum 2 players to start the game,
    creating the Room - what parameters???
    players should contain pointers of Users - any other idea??
*/
#endif