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

#include "Server.hpp"

#define ROOMS_NR 3
#define PLAYERS_NR 5
#define PORT 8081
#define BUFFER_SIZE 255

//  to check
Server::Server() {
    running = true;
    
    sockaddr_in serv_addr {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons((uint16_t)PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    socket_nr = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_nr == -1) {
        perror("Creating socket failed");
        this->~Server();
    }
    
    if (bind(socket_nr, (sockaddr*) &serv_addr, sizeof(serv_addr))) {
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
    cout << "Starting shutdown" << endl;
    sleep(5);
    close(this->socket_nr);
    saveQuestions("./resources/quest_base.txt");
    rooms_list.clear();
    users_list.clear();
    questions_list.clear();
}

//  Creates new User and pushes it to the users_list
void Server::connectUser(shared_ptr<User> usr) {
    unique_lock<mutex> lock_user{usr_mutex};
    users_list.push_back(*usr);
    string greetings = "Welcome user " + to_string(usr->getSocket()) + "!";
    sendMsg(usr->getSocket(), greetings);
}
//  remove User by given 'usr' id from the users_list on a Server
void Server::disconnectUser(const int usr) {
    unique_lock<mutex> lock_user{usr_mutex};
    remove_if(users_list.begin(), users_list.end(), [&usr](User& u) {
        return u.getSocket() == usr;
    });
}

bool* Server::getStatus() {
    return &running;
}

void Server::setNick(const int usr, const string new_nick) {
    auto user_found = find_if(users_list.begin(), users_list.end(), [&usr](User& u) {
        return u.getSocket() == usr;
    });
    user_found->setNick(new_nick);
}

//  create new Room and push it on the rooms_list on the Server
bool Server::createRoom() {
    unique_lock<mutex> lck {rm_mutex};
    if(rooms_list.size() >= ROOMS_NR) {
        cout << "There is no space for another room, choose one of already created\n";
        return false;
    }

    rooms_list.push_back(Room(this));
    return true;
}
//  removes pointer to User of 'usr' id from Room of room_id
void Server::popUserOut(const int usr, const int room_id) {
    if (room_id != -1) {
        unique_lock<mutex> lock_rooms{rm_mutex};
        unique_lock<mutex> lock_users{usr_mutex};
        this->rooms_list.at(room_id).removePlayer(usr);
    }
}
//  pushes pointer to User of 'usr' id to the Room's players_list
bool Server::putUserInRoom(const int usr, const int room_id) {
    unique_lock<mutex> lock_rooms{rm_mutex};
    unique_lock<mutex> lock_users{usr_mutex};
    auto user_found = find_if(users_list.begin(), users_list.end(), [&usr](User& u) {
        return u.getSocket() == usr;
    });
    return rooms_list.at(room_id).addPlayer(user_found.base());
}
//  set User status while being in the Room 
void Server::setUserReady(const int usr, const bool ready) {
    auto user_found = find_if(users_list.begin(), users_list.end(), [&usr](User& u) {
        return u.getSocket() == usr;
    });
    user_found->setReady(ready);
}

// sends message by Server of given 'content' to provided socket 'id'
void Server::sendMsg(const int id, const string content) {
    write(id, content.c_str(), content.length());
}


//  provides managment of messages exchange between Server and Clients 
void Server::clientRoutine(shared_ptr<User> this_usr) {
    
    bool connected = true;
    // userThread *this_usr = (userThread*)that_user;

    char buffer[BUFFER_SIZE], header;
    string response;

    while(connected && this->running) {
        if(read(this_usr->getSocket(), &header, 1) <= 0)
            break;

        // instructions
        if (header == '@') {        //  change nick
            read(this_usr->getSocket(), buffer, BUFFER_SIZE);
            setNick(this_usr->getSocket(), buffer);
            this_usr->getNick() = buffer;
        }        
        else if(header == 'Q') {
            this->running = false;
        }
        else if(header == '#') {           // leaving the server
            popUserOut(this_usr->getSocket(), this_usr->getRoom());
            connected = false;
        }  
        else if (this_usr->getRoom() == -1) {
            if (header == '*') {           // create the room
                response = (this->createRoom()) ? 
                "Room has been created" : "Couldn't create new room - may be the limit was reached...";
                sendMsg(this_usr->getSocket(), response);
            }
            else if(header == '>') {      // join the room
                read(this_usr->getSocket(), buffer, 2);
                int r_id = stoi(buffer);
                if (putUserInRoom(this_usr->getSocket(), r_id)) {
                    this_usr->setRoom(r_id);
                    response = "Welcome to Room number" + to_string(r_id);
                    sendMsg(this_usr->getSocket(), response);
                } else 
                    sendMsg(this_usr->getSocket(), "Couldn't join selected room - try another one!");
            }
            else if (header == '+') {      // add question
                read(this_usr->getSocket(), buffer, BUFFER_SIZE);
                addQuestion(buffer);
                sendMsg(this_usr->getSocket(), "Question added!");
            }      
        }
        else {    
            if(header == '!') {      // ready to play
                setUserReady(this_usr->getSocket(), true);
            }
            else if(header == '%') {      // answer ?? may be verification on client side???
                
            }
            else if(header == '<') {      // leaving the room
                popUserOut(this_usr->getSocket(), this_usr->getRoom());
                response = "So you left, take care, mate!";
                sendMsg(this_usr->getSocket(), response);
            }
            else if(header == '$') {      // change category
                read(this_usr->getSocket(), buffer, 255);
                response = buffer;
                int r_id = stoi(response.substr(0, response.find(':')));
                rooms_list.at(r_id).
                    setCategory(response.erase(0, response.find(':')));
                response = "So you have changed the Room's category to " + response;
                sendMsg(this_usr->getSocket(), response);
            }
        }
        memset(buffer, 0, BUFFER_SIZE);
        header = 0;
    }
    // handler closing - user leaving connection
    response = "You have been disconnected - world's cruel, hope U kno what'ya doin'...";
    sendMsg(this_usr->getSocket(), response);
    disconnectUser(this_usr->getSocket());
}

//  return vector of Questions of given 'category'
vector<Question*> Server::getQuestions(const string category, const int number) {
    vector<Question*> filtered;
    for(Question q : questions_list) { 
        if((int)filtered.size() < number)
            break;
        if (q.getTopic() == category || q.getTopic() == "random")
            filtered.push_back(&q);
    }
    
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

vector<User>* Server::getUsersList() {
    return &users_list;
}

vector<Room>* Server::getRoomsList() {
    return &rooms_list;
}

//  main job of this class, loop while server is running accepts incoming connections
int Server::run() {
            ///             MAIN LOOP
    //  First Approach
    while(this->running) {
        int client = accept(socket_nr, nullptr, nullptr);
        if(client == -1) {
            perror("Accept failed");
            return 1;
        }

        auto that_user = make_shared<User>("NewOne", client, false);
        connectUser(that_user);
        thread th(&Server::clientRoutine, this, that_user);
        cout << "I'm RUNNING (client_" << th.get_id() << ")" << endl;
        sleep(2);
        // this->running = false;
    }

    return 0;
    /*   Second approach
    return accept(socket_nr, nullptr, nullptr);
    */
}