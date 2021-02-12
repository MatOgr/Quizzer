
class User {
private:
    int socket_id;
    int score;
    bool start_game;
public: 
    User();
    User(const int socket);

    int getSocket();
    int getScore();
    bool getReady();
    void setSocket(int socket);
    void setScore(int score);
    void addToScore(int points);
    void setReady(bool ready);
};