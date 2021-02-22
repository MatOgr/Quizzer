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
#define BUFFER_SIZE 255


//  to check
Server::Server() {
    running = true;
    socket_nr = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socket_nr == -1) {
        perror("Creating socket failed");
        this->~Server();
    }
    
    sockaddr_in serv_addr {
        .sin_family = AF_INET,
        .sin_port = htons((uint16_t)PORT),
        .sin_addr = htonl(INADDR_ANY)
    };
    
    if (int binding = bind(socket_nr, (sockaddr*) &serv_addr, sizeof(serv_addr))) {
        perror("Binding failed");
        this->~Server();
    }

    if(listen(socket_nr, 1)) {
        perror("Listen failed");
        this->~Server();
    }
    readQuestions("./resources/quest_base.txt");
}

//  closes file descriptor also
Server::~Server() {
    close(this->socket_nr);
    saveQuestions("./resources/quest_base.txt");
    rooms_list.clear();
    users_list.clear();
    questions_list.clear();
}

//  Creates new User and pushes it to the users_list
void Server::connectUser(const int usr) {
    unique_lock<mutex> lock_user{usr_mutex};
    users_list.push_back(User(to_string(usr), usr, false));
    string greetings = "Welcome user " + to_string(usr) + "!";
    sendMsg(usr, greetings);
}
//  TO CHECK
void Server::disconnectUser(const int usr) {
    unique_lock<mutex> lock_user{usr_mutex};
    remove_if(users_list.begin(), users_list.end(), [&](User& u) {
        u.getSocket() == usr;
    });
}

//  To check
bool Server::createRoom() {
    unique_lock<mutex> lck {rm_mutex};
    if(rooms_list.size() >= ROOMS_NR) {
        cout << "There is no space for another room, choose one of already created\n";
        return false;
    }

    rooms_list.push_back(Room(this));
    return true;
}
//  To CHECK
void Server::putUserOut(const int usr, const int room_id) {
    if (room_id != -1) {
        unique_lock<mutex> lock_rooms{rm_mutex};
        unique_lock<mutex> lock_users{usr_mutex};
        this->rooms_list.at(room_id).removePlayer(usr);
    }
}
//  To CHECK
void Server::putUserInRoom(const int usr, const int room_id) {
    unique_lock<mutex> lock_rooms{rm_mutex};
    unique_lock<mutex> lock_users{usr_mutex};
    auto user_found = find_if(users_list.begin(), users_list.end(), [&](User& u) {
        u.getSocket() == usr;
    });
    rooms_list.at(room_id).addPlayer(user_found.base());
}

void Server::setUserReady(const int usr, const bool ready) {
    auto it = find_if(users_list.begin(), users_list.end(), [&](User& u) {
        u.getSocket() == usr;
    });
    it.base()->setReady(ready);
}

// sends message by Server of given 'content' to provided socket 'id'
void Server::sendMsg(const int id, const string content) {
    write(id, content.c_str(), content.length());
}


//  provides managment of messages exchange between Server and Clients 
void * Server::clientRoutine(void *that_user) {
    // pthread_detach(pthread_self());
    
    bool connected = true;
    userThread *this_usr = (userThread*)that_user;

    char buffer[BUFFER_SIZE], header;
    string response;

    while(connected && *(this_usr->server_state)) {
        if(read(this_usr->usr_con_sock, &header, 1) <= 0)
            break;

        // instructions
        if (header == '+') {      // add question
            read(this_usr->usr_con_sock, buffer, BUFFER_SIZE);
            addQuestion(buffer);
        }
        else if (this_usr->room_id == -1) {
            if (header == '*') {           // create the room
                response = (this->createRoom()) ? 
                "Room has been created" : "Couldn't create new room - may be the limit was reached...";
                sendMsg(this_usr->usr_con_sock, response);
            }
            else if(header == '^') {      // join the room
                read(this_usr->usr_con_sock, buffer, 2);
                int r_id = stoi(buffer);
                putUserInRoom(this_usr->usr_con_sock, r_id);
                this_usr->room_id = r_id;
                response = "Welcome to Room number" + to_string(r_id);
                sendMsg(this_usr->usr_con_sock, response);
            }      
        }
        else {
            if(header == '#') {           // leaving the server
                connected = false;
            }      
            else if(header == '@') {      // ready to play
                setUserReady(this_usr->usr_con_sock, true);
            }
            else if(header == '%') {      // answer  ?? may be verification on client side???
                
            }
            else if(header == '!') {      // leaving the room
                putUserOut(this_usr->usr_con_sock, this_usr->room_id);
                response = "So you left, take care, mate!";
                sendMsg(this_usr->usr_con_sock, response);
            }
            else if(header == '$') {      // change category
                read(this_usr->usr_con_sock, buffer, 255);
                response = (string)buffer;
                int r_id = stoi(response.substr(0, response.find(':')));
                rooms_list.at(r_id).
                    setCategory(response.erase(0, response.find(':')));
                response = "So you have changed the Room category to " + response;
                sendMsg(this_usr->usr_con_sock, response);
            }
        }
        memset(buffer, 0, BUFFER_SIZE);
        header = 0;
    }
    // handler closing - user leaving connection
    response = "You have been disconnected - world's cruel, hope U kno what'ya doin'...";
    sendMsg(this_usr->usr_con_sock, response);
    disconnectUser(this_usr->usr_con_sock);
    // if(this_usr->room_id >= 0) 
        // this_usr->rooms_list.removePlayer(this_usr->player_id);
}

//  TO CHECK
vector<Question*> Server::getQuestions(const string category, const int number) {
    vector<Question*> filtered;
    for(Question q : questions_list) 
        if (q.getTopic() == category && filtered.size() < number)
            filtered.push_back(&q);
    
    return filtered;
}
//  save questions_list to resource file 
void Server::saveQuestions(string const fdir) {
    ofstream write_it;
    write_it.open(fdir, ios::out | ios::trunc);
    for(Question& quest : questions_list) {
        write_it << endl << quest.getTopic() << ":" << quest.getContent() << ":" << quest.getAnswers() << ":" << quest.getCorrect();
    }
    write_it.close();
}
//  read questions from question-base file to the questions_list on the Server
void Server::readQuestions(const string fdir) {
    string buffer;
    ifstream read_it(fdir);
    while(getline(read_it, buffer)) {
        if(buffer.size() > 5)
            addQuestion(buffer);
    }
    read_it.close();
}
/*
    create Question object parsing given 'content' into constructor and push it to questions_list vector

*/
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

// TO CHECK    ####    main job of this class, loop while server is running accepts incoming connections
int Server::run() {
    
    while(this->running) {
        int client = accept(socket_nr, nullptr, nullptr);
        if(client == -1) {
            perror("Accept failed");
            return 1;
        }

        // pthread_t thread_id;
        userThread *that_user = new userThread;
        that_user->server_state = &running;
        that_user->usr_con_sock = client;
        that_user->room_id = -1;
        connectUser(client);
        thread(clientRoutine, that_user);
        // pthread_create(&thread_id, NULL, clientRoutine, (void *)that_user);
    }

    // close(socket_nr);

    return 0;
}