#include "my_server.h"

int main() {
    signal(SIGINT, my_server::signalHandler);
    my_server s = my_server();

    return 0;
}
