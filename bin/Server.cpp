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
/*
class Room {
    Room(Server *srv) :
        srv(srv), category("life"), players_number(PLAYERS_NR), questions_number(QUESTION_NR), game_running(false) {};
    // probably unnecessary 
    Room(Server *srv, User *player) :
        srv(srv), category("life"), players_number(PLAYERS_NR), questions_number(QUESTION_NR), game_running(false) {
            //  make player admin - pointers needed
            players.clear();
            // players.reserve(players_number);
            player->setAdmin(true);
            players.push_back(player);      
        };

    ~Room() {
        cout << "Room of category '" << category << "' has been closed..." << endl;
    };

    // Sorts list of players playing in the Room by their scores and returns string containing whole scoretable
    string getRanking() {
        string ranking;
        sort(players.begin(), players.end(), [](User* fst, User* snd) {
            return fst->getScore() < snd->getScore();
        });
        int i = 1;
        for(User * usr : players) {
            ranking.append(to_string(i)).
                append(". ").
                append(usr->getNick()).
                append("\t: \t").
                append(to_string(usr->getScore())).
                append("\n");
            i++;
        }
        
        return ranking;
    }
    // returns maximal players number
    int getMaxPlayersNumber() {
        return this->players_number;
    }
    // returns current number of players in the room
    int getCurrentPlayersNumber() {
        return this->players_number;
    }
    // returns maximal question number for considered room
    int getQuestionsNumber() {
        return this->questions_number;
    }
    // returns room's category 
    string getCategory() {
        return this->category;
    }
    // TO CHECK
    bool addPlayer(User * plyr) {
        if(players.size() == 0) 
            plyr->setAdmin(true);

        if(players.size() < players_number) {
            this->players.push_back(plyr);
            return true;
        }
        return false;
    }
    // removes pointer to the leaving player from 'players' list
    bool removePlayer(int plyr_id) {
        
        auto leaving = find(players.begin(), players.end(), [&plyr_id](User * u) { 
            return u && (u->getSocket() == plyr_id); 
        });
        
        if (leaving != players.end()) {
            cout << "Player " << (*leaving)->getNick() << " is leaving..." << endl;
            if((*leaving)->getAdmin()) {
                (*leaving)->setAdmin(false);
                players.erase(leaving);
                (*players.begin())->setAdmin(true);
            } else 
                players.erase(leaving);
            return true;
        }
        return false;
    }
    // Sets category of Questions to 'cat' 
    void setCategory(const string cat) {
        this->category = cat;
    }
    // Sets max number of questions in the Room to 'quest_num'
    void setQuestionNumber(const int quest_num) {
        this->questions_number = quest_num;
    }
    // Sets max number of Users allowed to be inside this Room
    void setPlayersNumber(const int plyr_number) {
        this->players_number = plyr_number;
    }
    // Sets Questions list to provided 'q_list' list 
    void loadQuestions(const vector<Question*> q_list) {
        questions = q_list;
    }
    // checks whether the game is in progress
    bool getGameState() {
        return this->game_running;
    }
    // sets the game status - 'true' in progress, 'false' in other case 
    void setGameState(const bool state) {
        this->game_running = state;
    }
    // checks whether each player in the Room is ready to play (pressed the button PLAY)
    bool Room::checkReady() {
        for(User * u : players) 
            if (!u->getReady())
                return false;
        
        return true;
    }
    // ### TODO some time control/response listener needed
    void sendQuestionToUsers(const int idx) {
        for(Question * q : questions) {
            string q_temp = "";
            q_temp.append("?").
                append(q->getContent() + ":").
                append(q->getAnswers() + ":").
                append(to_string(q->getCorrect()));
            for (User * x : players) 
                srv->sendMsg(x->getSocket(), q_temp);

            //  wait for user response ???
            
        }
    }
    //  ## TODO - some countdown before the beginning
    void start() {
        if (checkReady()) {
            loadQuestions(srv->getQuestions(category, questions_number));
            //  sleep(5);
            setGameState(true);
            for (int i = 0; i < questions.size(); i++)
                sendQuestionToUsers(i);
                sleep(20);
        }
    }
    //  To CHECK
    void end() {
        setGameState(false);
        string finalRanking = getRanking();
        for(User* u : players) {
            srv->sendMsg(u->getSocket(), finalRanking + "\n~");
            u->setReady(true);
        }
    }

};
*/
//  to check
Server::Server() {
    running = true;
    
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
//  remove User by given 'usr' id from the users_list on a Server
void Server::disconnectUser(const int usr) {
    unique_lock<mutex> lock_user{usr_mutex};
    remove_if(users_list.begin(), users_list.end(), [&usr](User& u) {
        return u.getSocket() == usr;
    });
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
void Server::putUserOut(const int usr, const int room_id) {
    if (room_id != -1) {
        unique_lock<mutex> lock_rooms{rm_mutex};
        unique_lock<mutex> lock_users{usr_mutex};
        this->rooms_list.at(room_id).removePlayer(usr);
    }
}
//  pushes pointer to User of 'usr' id to the Room's players_list
void Server::putUserInRoom(const int usr, const int room_id) {
    unique_lock<mutex> lock_rooms{rm_mutex};
    unique_lock<mutex> lock_users{usr_mutex};
    auto user_found = find_if(users_list.begin(), users_list.end(), [&usr](User& u) {
        return u.getSocket() == usr;
    });
    rooms_list.at(room_id).addPlayer(user_found.base());
}
//  set User status while being in the Room 
void Server::setUserReady(const int usr, const bool ready) {
    auto it = find_if(users_list.begin(), users_list.end(), [&usr](User& u) {
        return u.getSocket() == usr;
    });
    it->setReady(ready);
}

// sends message by Server of given 'content' to provided socket 'id'
void Server::sendMsg(const int id, const string content) {
    write(id, content.c_str(), content.length());
}


//  provides managment of messages exchange between Server and Clients 
void  Server::clientRoutine(void *that_user) {
    
    bool connected = true;
    userThread *this_usr = (userThread*)that_user;

    char buffer[BUFFER_SIZE], header;
    string response;

    while(connected && *(this_usr->server_state)) {
        if(read(this_usr->usr_con_sock, &header, 1) <= 0)
            break;

        // instructions
        if (header == '@') {        //  change nick
            read(this_usr->usr_con_sock, buffer, BUFFER_SIZE);
            setNick(this_usr->usr_con_sock, buffer);
            this_usr->player_nick = buffer;
        }
        else if (header == '+') {      // add question
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
            else if(header == '&') {      // ready to play
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

//  return vector of Questions of given 'category'
vector<Question*> Server::getQuestions(const string category, const int number) {
    vector<Question*> filtered;
    for(Question q : questions_list) { 
        if(filtered.size() < number)
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

//  main job of this class, loop while server is running accepts incoming connections
void Server::run() {
    
    sockaddr_in serv_addr {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons((uint16_t)PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    socket_nr = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_nr == -1) {
        perror("Creating socket failed");
        this->~Server();
    }
    
    
    if (int binding = bind(socket_nr, (sockaddr*) &serv_addr, sizeof(serv_addr))) {
        perror("Binding failed");
        this->~Server();
    }

    if(listen(socket_nr, 1)) {
        perror("Listen failed");
        this->~Server();
    }

            ///             MAIN LOOP
    while(this->running) {
        int client = accept(socket_nr, nullptr, nullptr);
        // if(client == -1) {
        //     perror("Accept failed");
        //     return 1;
        // }

        // userThread *that_user = new userThread;
        // that_user->server_state = &running;
        // that_user->usr_con_sock = client;
        // that_user->room_id = -1;
        // connectUser(client);
        // thread(clientRoutine, that_user);
        cout << "I'm RUNNING\n";
        sleep(5);
        this->running = false;
    }

    this->~Server();
    // return 0;
}