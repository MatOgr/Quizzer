#include "Server.hpp"

using namespace std;

int main() {
    shared_ptr<Server> server = make_shared<Server>();

    cout << "Say hello " << pthread_self() << "!\n";

    /*
    
    void noAction(int s) {
    cout << "Received signal " << s << endl;
}
    struct sigaction signalSpecs;
    sigaction(SIGUSR1, nullptr, &signalSpecs);
    signalSpecs.sa_flags&=~SA_RESTART;
    signalSpecs.sa_handler=noAction;
    sigaction(SIGUSR1, &signalSpecs, nullptr);
*/

    server->run();

    return 0;
}