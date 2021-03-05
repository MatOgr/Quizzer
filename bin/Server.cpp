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

//  creates Server instance using #define(d) data
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
    else if (bind(socket_nr, (sockaddr*) &serv_addr, sizeof(serv_addr))) {
        perror("Binding failed");
        this->~Server();
    }

    else if(listen(socket_nr, 1)) {
        perror("Listen failed");
        this->~Server();
    }

    else 
        readQuestions("resources/quest_base.txt");
}


//  closes file descriptor also
Server::~Server() {
    saveQuestions("resources/quest_base.txt");
    cout << "Starting shutdown" << endl;
    int socket = this->socket_nr;
    this->socket_nr = -1;    
    close(socket);
    sleep(2);
    rooms_list.clear();         cout << "Cleared the rooms list" << endl;           sleep(1);
    users_list.clear();         cout << "Cleared the users list" << endl;           sleep(1);
    questions_list.clear();     cout << "Cleared the questions list" << endl;       sleep(1);
    cout << "Farewell" << endl;
}


//  Creates new User and pushes it to the users_list
void Server::connectUser(shared_ptr<User> usr) {
    unique_lock<mutex> lock_user{usr_mutex};
    users_list.push_back(usr);
    string greetings = "Welcome user " + usr->getNick() + "!\n";
    sendMsg(usr->getSocket(), greetings);
}


//  remove User by given 'usr' id from the users_list on a Server
void Server::disconnectUser(const int usr) {
    unique_lock<mutex> lock_user{usr_mutex};
    auto found = find_if(users_list.begin(), users_list.end(), [&usr](auto u) {
        return u->getSocket() == usr;
    });
    users_list.erase(found);
}


//  return server status - whether runs or not
bool* Server::getStatus() {
    return &running;
}


//  set nick of 'usr' to given 'new_nick' string 
bool Server::setNick(shared_ptr<User> usr, const string new_nick) {
    for(auto u : users_list){
        if (u->getNick() == new_nick)
            return false;
    }
    usr->setNick(new_nick);
    return true;
}


//  create new Room and push it on the rooms_list on the Server
int Server::createRoom() {
    unique_lock<mutex> lck {rm_mutex};
    if(rooms_list.size() >= ROOMS_NR) {
        cout << "There is no space for another room, choose one of already created\n";
        return -1;
    }
    
    rooms_list.push_back(make_shared<Room>(this));
    return rooms_list.size()-1;
}


//  removes pointer to User of 'usr' id from Room of room_id
void Server::popUserOut(shared_ptr<User> usr) {
    if (usr->getRoom() != -1) {
        unique_lock<mutex> lock_rooms{rm_mutex};
        unique_lock<mutex> lock_users{usr_mutex};
        this->rooms_list[usr->getRoom()]->removePlayer(usr->getSocket());
        usr->setRoom(-1);
        usr->setAdmin(false);
    }
}


//  pushes pointer to User of 'usr' id to the Room's players_list
bool Server::putUserInRoom(shared_ptr<User> usr, const int room_id) {
    unique_lock<mutex> lock_rooms{rm_mutex};
    unique_lock<mutex> lock_users{usr_mutex};
    
    return rooms_list[room_id]->addPlayer(usr);
}


//  set User status while being in the Room 
void Server::setUserReady(shared_ptr<User> usr, bool ready) {
    unique_lock<mutex> lock_users{usr_mutex};
    usr->setReady(ready);
    int room_id = usr->getRoom();
    lock_users.unlock();        //      not sure if needed

    if (rooms_list[room_id]->checkReady() && !rooms_list[room_id]->getGameState())
        thread (&Room::start, rooms_list[room_id]).detach();
}


// sends message by Server of given 'content' to provided socket 'id'
void Server::sendMsg(const int id, const string content) {
    write(id, content.c_str(), content.length());
}


//  provides managment of messages exchange between Server and Clients 
void Server::clientRoutine(weak_ptr<User> usr) {

    bool connected = true;
    shared_ptr<User> this_usr = usr.lock();
    if (this_usr) {
        char buffer[BUFFER_SIZE+2] = {}, header = 0;
        string response;

        while(connected && this->running) {
            if(read(this_usr->getSocket(), &header, 1) < 0)
                break;

            // instructions
            if (header == '@') {        //  change nick     -   '@new_nick:'
                read(this_usr->getSocket(), buffer, BUFFER_SIZE);
                string nick(buffer); 
                response = setNick(this_usr, nick.substr(0, nick.find(':'))) ?  
                    "+:Your nick was set to:" + this_usr->getNick() + "\n" : "-:There's been that NICK, already\n";
            }        
            else if(header == 'Q') {            //  shutdown the server
                this->running = false;
                response = "U told the Server to SHUTDOWN...\n";
                
            }
            else if(header == '#') {           // leaving the server
                response = "U decided to leave Server, " + this_usr->getNick() + "\n";
                connected = false;
            }  
            else if (this_usr->getRoom() == -1) {
                if (header == '*') {           // create the room
                    int id = this->createRoom();
                    response = (id >= 0) ? "+:Room has been created\n" : "-:Couldn't create new room - may be the limit was reached...\n";
                    this_usr->setRoom(id);
                    if(id >= 0)
                        putUserInRoom(this_usr, id);
                    
                }
                else if(header == '_') {        //  lobby info
                    if(rooms_list.size() - 1 >= 0)
                        response = getLobbyInfo();
                    else 
                        response = "There is no room to choose\n";
                }
                else if(header == '>') {      // join the room  - '>room_nr:'
                    read(this_usr->getSocket(), buffer, BUFFER_SIZE);
                    response = buffer;
                    int r_id = stoi(response.substr(0, response.find(':')));
                    if (putUserInRoom(this_usr, r_id)) {
                        this_usr->setRoom(r_id);
                        response = "+:Welcome to Room number "+ to_string(r_id) + (this_usr->getAdmin() ? " (ADM)" : " (REG)") + "\n";
                    } else 
                        response = "-:Couldn't join selected room - try another one!\n";
                }
                else if (header == '+') {      // add question      -   '+:content:ans1?ans2?ans3?ans4:correct_id_ans:'
                    read(this_usr->getSocket(), buffer, BUFFER_SIZE);
                    addQuestion(buffer);
                    response = "Question added!\n";
                }      
            }
            else {    
                if(header == '!') {      // ready to play
                    setUserReady(this_usr, true);
                    response = "You are READY!\n";
                }
                else if(header == '_') {
                    response = rooms_list[this_usr->getRoom()]->getRoomInfo();
                }
                else if(header == '%') {      // add points to your score      -   '%points:'
                    read(this_usr->getSocket(), buffer, BUFFER_SIZE);
                    this_usr->addToScore(stoi(buffer));
                    response = "Your current score: " + to_string(this_usr->getScore()) + "\n";
                }
                else if(header == '<') {      // leaving the room
                    popUserOut(this_usr);
                    this_usr->setRoom(-1);
                    response = "+:So you left our room, take care, mate!\n";
                }
                else if(header == '$') {      // change category        -   '$new_category:'
                    read(this_usr->getSocket(), buffer, BUFFER_SIZE);
                    if(this_usr->getAdmin()) {
                        response = buffer;
                        response = response.substr(0, response.find(':'));
                        rooms_list[this_usr->getRoom()]->setCategory(response);
                        response = "+:So you have changed the Room's category to " + response + "\n";
                    }
                    else 
                        response = "-:You have no right to do that!\n";
                }
                else if(header == '&') {        //  change the players number       -   '&new_limit:'
                    read(this_usr->getSocket(), buffer, BUFFER_SIZE);   
                    if(this_usr->getAdmin()) {
                        response = buffer;
                        auto room_in = rooms_list[this_usr->getRoom()];
                        room_in->setPlayersNumber(stoi(response));\
                        response = "+:You have changed the players number to " + to_string(room_in->getMaxPlayersNumber()) + "\n";
                    }
                    else 
                        response = "-:You have no right to do that!\n";
                }
                else if(header == '?') {        //  change the questions number     -   '?new_limit:'
                    read(this_usr->getSocket(), buffer, BUFFER_SIZE);   
                    if(this_usr->getAdmin()) {
                        response = buffer;
                        auto room_in = rooms_list[this_usr->getRoom()];
                        room_in->setQuestionNumber(stoi(response));\
                        response = "+:You have changed the question number to " + to_string(room_in->getQuestionsNumber()) + "\n";
                    }
                    else 
                        response = "-:You have no right to do that!\n";
                }
            }
            sendMsg(this_usr->getSocket(), response);
            sleep(1);             
            response = "";
            memset(buffer, 0, BUFFER_SIZE+2);
            header = 0;
        }
        // handler closing - user leaving connection
        popUserOut(this_usr);
        response = "You have been disconnected - world's cruel, hope U kno what'ya doin'...\n";
        sendMsg(this_usr->getSocket(), response);
        disconnectUser(this_usr->getSocket());
        close(this_usr->getSocket());
    } else 
        cout << "Sth wrong with clientRoutine()" << endl;
}


//  return vector of Questions of given 'category'
vector<shared_ptr<Question>> Server::getQuestions(const string category, const int number) {
    vector<shared_ptr<Question>> filtered;
    for(auto q : questions_list) { 
        if((int)filtered.size() >= number)
            break;
        if (q->getTopic() == category || q->getTopic() == "random")
            filtered.push_back(q);
    }
    
    return filtered;
}


//  save questions_list to resource file 
void Server::saveQuestions(string const fdir) {
    ofstream write_it(fdir, ios::out | ios::trunc);
    for(auto quest : questions_list) {
        write_it << quest->getTopic() << ":" << quest->getContent() << ":" << quest->getAnswers() << ":" << quest->getCorrect() << endl ;
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


//  create Question object parsing given 'content' into constructor and push it to questions_list vector
void Server::addQuestion(string content) {
    string category, q_content, answers;
    int correct;
    category = content.substr(0, content.find(':'));
    content.erase(0, content.find(':') + 1);
    q_content = content.substr(0, content.find(':'));
    content.erase(0, content.find(':') + 1);
    answers = content.substr(0, content.find(':'));
    content.erase(0, content.find(':') + 1);
    correct = stoi(content);
    questions_list.push_back(make_shared<Question>(q_content, answers, correct, category));
}


//  return string containing informations in format 'room_id:category:users_in:users_limit:game_state:' (e.g. 2:italian cuisine:6:11:OFF:)
string Server::getLobbyInfo() {
    string info = "%:";
    int i = 1;
    for(auto r : rooms_list) {
        info.append(to_string(i)).append(":").
            append(r->getCategory()).append(":").
            append(to_string(r->getCurrentPlayersNumber())).append(":").
            append(to_string(r->getMaxPlayersNumber())).
            append(r->getGameState() ? ":ON:" : ":OFF:").
            append("\n");
        i++;
    }
    return info;
}


vector<shared_ptr<Room>> Server::getRoomsList() {
    return rooms_list;
}


//  main job of this class, loop while server is running accepts incoming connections
int Server::run() {
            ///             MAIN LOOP
    sockaddr_in sdr{};
    socklen_t len = sizeof(sdr);
    while(this->running) {
        int client = accept(socket_nr, (sockaddr*)&sdr, &len);
        if(client < 0) {
            // if(errno == EINTR) {
            //     cout << "accept() restarted\n";
            //     continue;
            // }
            perror("Accept failed");
            return 1;
        }

        auto that_user = make_shared<User>("X___CrazyTrumpeter___X", client, false);
        connectUser(that_user);
        thread (&Server::clientRoutine, this, that_user).detach();
        cout << "New " << that_user->getNick() << " minion merged" << endl;
        sleep(1);
    }

    return 0;
}