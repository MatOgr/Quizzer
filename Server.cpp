#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <pthread.h>
#include <mutex>
#include <cstring>
#include <ctime>
#include <vector>

#include "bin/Room.hpp"
#include "bin/Question.hpp"
#include "bin/User.hpp"

#define ROOMS_NR 3
#define PLAYERS_NR 5
#define PORT 8081
#define BUFFER_SIZE 64

//  ### TODO
Server::Server() {}

//  ### TODO
Server::~Server() {}

//  ### TODO
void Server::closeRoom() {}
//  ### TODO
void Server::createRoom() {}
//  ### TODO
void Server::putUserOut(User* usr, const int room_id) {}
//  ### TODO
void Server::putUserInRoom(User* usr, const int room_id) {}


void Server::sendMsg(const int id, const string content, const int length) {
    write(id, content.c_str(), length);
}
//  TO CHECK
char * Server::readThread(int fd, threadData *thread_data, bool * connection) {
    char *buffer = new char[BUFFER_SIZE];
    char *temp = new char[2];
    memset(buffer, '\0', sizeof(char) * BUFFER_SIZE - 1);
    memset(temp, '\0', 2);
    int read_status;
    do {
        if((read_status = read(fd, temp, 1)) < 0) {
            cout << "Couldn't read message" << endl;
            pthread_exit(NULL);
        }
        else if (read_status == 0) {
            *connection = false;
            break;
        }
        strcat(buffer, temp);
    } while(strcmp(temp, "\n"));

    delete buffer;
    delete temp;
}
//  TO CHECK
void * Server::clientRoutine(void *thread_data) {
    pthread_detach(pthread_self());
    bool connected = true;
    bool room_used = false;
    threadData *current_thread = (threadData *)thread_data;
    char *buffer;
    while(connected && *(current_thread->server_state)) {
        if(!(buffer = this->readThread(
            current_thread->con_sock_desc, 
            current_thread, 
            &connected))) 
                break;

        // instructions
        if(!strcmp(buffer, "create") && !room_used) {}      // TODO
        else if(!strcmp(buffer, "join") && !room_used) {}      // TODO
        else if(!strcmp(buffer, "leave") && room_used) {}      // TODO
        else if(!strcmp(buffer, "ready") && room_used) {}      // TODO
        else if(!strcmp(buffer, "answer") && room_used) {}      // TODO
        else if(!strcmp(buffer, "playernum") && room_used) {}      // TODO
        else if(!strcmp(buffer, "questionnum") && room_used) {}      // TODO
        else if(!strcmp(buffer, "category") && room_used) {}      // TODO

        delete buffer;
    }
    // handler closing - user leaving connection
    close(current_thread->con_sock_desc);
    if(current_thread->room_id >= 0) 
        current_thread->rooms_list. //.removePlayer(current_thread->player_id);
}
//  TO CHECK
vector<Question> Server::getQuestions(const string category, const int number) {
    vector<Question> filtered;
    for(Question q : questions_list) 
        if (q.getTopic() == category && filtered.size() < number)
            filtered.push_back(q);
    
    return filtered;
}
//  TO CHECK
void Server::readQuestions(const string fdir) {
    string buffer;
    ifstream read_it("quest_base.txt");
    while(getline(read_it, buffer)) {
        addQuestion(buffer);
    }
    read_it.close();
}
//  TO CHECK
void Server::addQuestion(string content) {
    string category, q_content, answers, correct;
    category = content.substr(0, content.find(':'));
    content.erase(0, content.find(':') + 1);
    q_content = content.substr(0, content.find(':'));
    content.erase(0, content.find(':') + 1);
    answers = content.substr(0, content.find(':'));
    content.erase(0, content.find(':') + 1);
    correct = content.substr(0, 1);
    questions_list.push_back(Question(q_content, answers, stoi(correct), category));
}

/*
int main() {
    sockaddr_in serv_addr {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons((uint16_t)PORT);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("Creating socket failed");
        return 1;
    }

    if (int binding = bind(fd, (sockaddr*) &serv_addr, sizeof(serv_addr))) {
        perror("Binding failed");
        return 1;
    }

    if(listen(fd, 1)) {
        perror("Listen failed");
        return 1;
    }

    // TODO mutexes & structure fill in 
    int * con_desc_arr_ptr = new int[ROOMS_NR * PLAYERS_NR];
    Room * rooms_list_ptr = new Room[ROOMS_NR];
    bool * server_state_ptr = new bool;
    pthread_mutex_t con_desc_mutex;
    pthread_mutex_t rooms_list_mutex;

    memset(con_desc_arr_ptr, -1, ROOMS_NR * PLAYERS_NR * sizeof(int));
    *server_state_ptr = true;
    pthread_mutex_init(&con_desc_mutex, NULL);
    pthread_mutex_init(&rooms_list_mutex, NULL);

    while(*server_state_ptr) {
        int client = accept(fd, nullptr, nullptr);
        if(client == -1) {
            perror("Accept failed");
            return 1;
        }

        pthread_t thread_id;
        threadData *that_thread = new threadData;
        that_thread->con_sock_desc = client;
        that_thread->con_desc_vec = con_desc_arr_ptr;
        that_thread->rooms_list = rooms_list_ptr;
        that_thread->room_id = -1;
        that_thread->server_state = server_state_ptr;
        that_thread->rooms_list_mutex = rooms_list_mutex;
        that_thread->con_desc_mutex = con_desc_mutex;

        pthread_create(&thread_id, NULL, clientRoutine, (void *)that_thread);
    }

    close(fd);
    pthread_mutex_destroy(&con_desc_mutex);
    pthread_mutex_destroy(&rooms_list_mutex);
    delete [] con_desc_arr_ptr;
    delete [] rooms_list_ptr;
    delete server_state_ptr;

    return 0;
}
*/