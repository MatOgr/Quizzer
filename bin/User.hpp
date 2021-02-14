#ifndef User_hpp
#define User_hpp

#include <string>

using namespace std;

class User {
private:
    string nick;    // or id 
    int socket_id;
    int score;
    bool start_game;
    bool admin;
public: 
    User();
    User(const string nick, const int socket, const bool admin);
    ~User();

    int getSocket();
    int getScore();
    string getNick();
    bool getReady();
    bool getAdmin();
    void setSocket(const int socket);
    void setScore(const int score);
    void addToScore(const int points);
    void setNick(const string nik);
    void setReady(const bool ready);
    void setAdmin(const bool admin);
};
/*
    nick, can be admin/host
*/
#endif