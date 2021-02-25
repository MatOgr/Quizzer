#ifndef User_hpp
#define User_hpp

#include <string>

using namespace std;

class User {
private:
    string nick;   
    int socket_id;
    int score;
    bool ready_to_play;
    bool admin;
public: 
    User();
    User(const string nick, const int socket, const bool admin);
    // ~User();

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


#endif