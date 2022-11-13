/*
*   Grupo 12
*   Duarte Lopes Pinheiro nº54475
*   Filipe Henriques nº55228
*   Márcio Moreira nº41972
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/network_server.h"


int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Server takes 1 argument: port\n");
        exit(-1);
    }
    int port;
    if(sscanf(argv[1], "%i", &port) != 1) {
        printf("Port must be an integer\n");
        exit(-1);
    }

    int listening_socket = network_server_init((short)port);
    if (listening_socket == -1) {
        printf("socket creation error\n");
        exit(-1);
    }

    tree_skel_init();

    int result = network_main_loop(listening_socket);
    
    tree_skel_destroy();
    exit(result);
}