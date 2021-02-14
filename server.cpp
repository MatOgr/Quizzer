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
    int player_id;
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
    

    return 0;
}