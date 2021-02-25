#ifndef Server_hpp
#define Server_hpp

#include "Room.hpp"
#include "Question.hpp"
#include "User.hpp"

using namespace std;

struct userThread {
    int usr_con_sock;
    int room_id;
    string player_nick;  
    bool *server_state;
    // mutex rooms_list_mutex;
    // mutex con_desc_mutex;
};

class Room;

class Server {
private:
    int socket_nr;
    mutex usr_mutex;
    mutex rm_mutex;
    mutex qtns_mutex;
    vector<Room> rooms_list;
    vector<User> users_list;
    vector<Question> questions_list;
    bool running;
     
public:
    Server();
    ~Server();
    
    void run();

    void connectUser(const int usr);
    void disconnectUser(const int usr);
    
    // void closeRoom();
    bool createRoom();
    
    void putUserOut(const int usr, const int room_id);
    void putUserInRoom(const int usr, const int room_id);
    void setUserReady(const int usr_id, const bool ready);

    string readThread(const int fd, userThread *thread_data, bool *connection);
    void sendMsg(const int id, const string content);
    void clientRoutine(void *thread_data);
    
    void saveQuestions(const string fdir);
    void readQuestions(const string fdir);
    void addQuestion(string content);
    vector<Question*> getQuestions(const string category, const int number);
};

// class Room {
// private:
//     Server *srv;
//     vector<User*> players;
//     vector<Question*> questions;
//     string category;
//     int players_number;
//     int questions_number;
//     bool game_running;
//     mutex room_mutex;
// public:
//     Room(Server * serv);
//     Room(Server * serv, User * player);        // not needed???
//     ~Room();

//     string getRanking();  
//     int getMaxPlayersNumber();
//     int getCurrentPlayersNumber();
//     int getQuestionsNumber();
//     string getCategory();
//     bool getGameState();

//     bool addPlayer(User* plyr);
//     bool removePlayer(int plyr_id);

//     void setGameState(const bool state);
//     void setCategory(const string cat);
//     void setQuestionNumber(const int quest_num);
//     void setPlayersNumber(const int number);

//     void loadQuestions(const vector<Question*> q_list);
//     bool checkReady();
//     void sendQuestionToUsers(const int idx);
//     void start();
//     void end();
// };

#endif