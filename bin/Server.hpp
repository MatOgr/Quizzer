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
    
    int run();
    bool* getStatus();

    void connectUser(shared_ptr<User> usr);
    void disconnectUser(const int usr);
    void setNick(const int usr, const string new_nick);

    int createRoom();
    
    void popUserOut(const int usr, const int room_id);
    bool putUserInRoom(const int usr, const int room_id);
    void setUserReady(const int usr_id, const bool ready);

    string readThread(const int fd, userThread *thread_data, bool *connection);
    void sendMsg(const int id, const string content);
    void clientRoutine(weak_ptr<User> this_usr);
    
    void saveQuestions(const string fdir);
    void readQuestions(const string fdir);
    void addQuestion(string content);
    vector<Question*> getQuestions(const string category, const int number);
    vector<User>* getUsersList();
    vector<Room>* getRoomsList();
};



#endif