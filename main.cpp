#include "server.h"




int main(int argc, char * argv[]){
    server * s = new server();
    if (argc < 1)
        return -1;
    cout << s->menu(argv[0]) << endl;
    return 0;
}


