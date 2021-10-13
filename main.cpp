#include "my_server.h"

int main() {
    my_server s = my_server();
    signal(SIGINT, my_server::my_kill);
    return 0;
}
