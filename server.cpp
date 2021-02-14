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

#include "bin/Room.hpp"

#define ROOMS_NR 3
#define PLAYERS_NR 5
#define PORT 8081
#define BUFFER_SIZE 64

using namespace std;

struct threadData {
    int con_sock_desc;
    int *con_desc_arr;
    Room *rooms_list;
    int room_id;
    int player_id;  // or nick???
    bool *server_state;
    pthread_mutex_t rooms_list_mutex;
    pthread_mutex_t con_desc_mutex;
};

void sendMsg(int id, string content, int length) {
    write(id, content.c_str(), length);
}

char * readThread(int fd, threadData *thread_data, bool * connection) {
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

void * clientRoutine(void *thread_data) {
    pthread_detach(pthread_self());
    bool connected = true;
    bool room_used = false;
    threadData *current_thread = (threadData *)thread_data;
    char *buffer;
    while(connected && *(current_thread->server_state)) {
        if(!(buffer = readThread(
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
        current_thread->rooms_list[current_thread->room_id].removePlayer(current_thread->player_id);
}


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
        that_thread->con_desc_arr = con_desc_arr_ptr;
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