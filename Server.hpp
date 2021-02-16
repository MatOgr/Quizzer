#ifndef Server_hpp
#define Server_hpp

#include "bin/Room.hpp"

using namespace std;

struct threadData {
    int con_sock_desc;
    vector<int> con_desc_vec;
    vector<Room> rooms_list;
    int room_id;
    int player_id;  // or nick???
    bool *server_state;
    pthread_mutex_t rooms_list_mutex;
    pthread_mutex_t con_desc_mutex;
};

class Server {
private:
    int socket_nr;
    //mutex mutex
    vector<Room> rooms_list;
    vector<User> users_list;
    vector<Question> questions_list;
     
public:
    Server();
    ~Server();
    
    void connectUser(User * usr);
    void disconnectUser(User * usr);
    
    void closeRoom();
    void createRoom();
    
    void putUserOut(User* usr, const int room_id);
    void putUserInRoom(User* usr, const int room_id);

    char *readThread(int fd, threadData *thread_data, bool *connection);
    void sendMsg(const int id, const string content, const int length);
    void *clientRoutine(void *thread_data);
    
    void readQuestions(const string fdir);
    void addQuestion(string content);
    vector<Question*> getQuestions(const string category, const int number);
};

#endif