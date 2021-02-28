#ifndef Server_hpp
#define Server_hpp

#include "Room.hpp"
#include "Question.hpp"
#include "User.hpp"

using namespace std;


class Room;

class Server {
private:
    int socket_nr;
    mutex usr_mutex;
    mutex rm_mutex;
    mutex qtns_mutex;
    vector<shared_ptr<Room>> rooms_list;        //  ptr to vector or vector of ptrs needed 
    vector<shared_ptr<User>> users_list;        //  
    vector<shared_ptr<Question>> questions_list;
    bool running;
     
public:
    Server();
    ~Server();
    
    int run();
    bool* getStatus();
    string getLobbyInfo();

    void connectUser(shared_ptr<User> usr);
    void disconnectUser(const int usr);
    bool setNick(shared_ptr<User> usr, const string new_nick);

    int createRoom();
    
    void popUserOut(shared_ptr<User> usr);
    bool putUserInRoom(shared_ptr<User> usr, const int room_id);
    void setUserReady(shared_ptr<User> usr, const bool ready);

    void sendMsg(const int id, const string content);
    void clientRoutine(weak_ptr<User> this_usr);
    
    void saveQuestions(const string fdir);
    void readQuestions(const string fdir);
    void addQuestion(string content);
    vector<shared_ptr<Question>> getQuestions(const string category, const int number);
    // vector<User>* getUsersList();
    vector<shared_ptr<Room>> getRoomsList();
};



#endif