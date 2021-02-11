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
#include <iostream>

#include "bin/Room.hpp"
#include "bin/User.hpp"

#define ROOMS_NR 3
#define PLAYERS_NR 5
#define PORT 8081

using namespace std;

int main() {
    cout << "just kike this" << endl;

    return 0;
}