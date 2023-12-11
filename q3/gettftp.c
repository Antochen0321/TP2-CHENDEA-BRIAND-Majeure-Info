#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 512
#define PORT 69

void gettftp(char *host, char *file){
    //define the socket
    int sock;
    
    //configuration of getaddrinfo structure
    struct addrinfo hints, *serverinfo, *p;
    int error;
    char buffer[MAX_BUFFER_SIZE];
    memset(&hints, 0, sizeof(struct addrinfo));
    
    //configure getaddrinfo criteria
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    //for optaining server adress
    error = getaddrinfo(host,PORT, &hints, &serverinfo);
    if (error != 0) {
        perror("Adresse introuvable");
        exit(EXIT_FAILURE);
    }
    
    //trying to connect to host by browsing results of getaddrinfo
    sock = -1;
    for(p = serverinfo; serverinfo; serverinfo = serverinfo ->ai_next){
        sock = socket(serverinfo->ai_family, serverinfo->ai_socktype,
        serverinfo->ai_protocol);
    if (sock < 0) {
        perror("Erreur lors de la création de la socket");
        continue;
    }
    if (connect(sock, serverinfo->ai_addr, serverinfo->ai_addrlen) < 0) {
        perror("Erreur de connexion");
        close(sock);
        sock = -1;
        continue;
    }
    break;
    }

    if (p == NULL) {
        fprintf(stderr, "Impossible de se connecter au serveur\n");
        exit(EXIT_FAILURE);
    }

    //close the socket and make free addrinfo
    close(sock);
    freeaddrinfo(serverinfo);
}




int main(int *argc, char *argv){
    //First we must verify we call the function with the good length of argv, we must have only 3 arguments, the command, the host and the filename
    if(argc != 3){
        printf("Pas le bon nombre d'arguments, il en faut 3, pas un de moins ou de plus");
        exit(EXIT_FAILURE);
    }
    const char *host = argv[1];
    const char *filename = argv[2];

    printf("gettftp -Serveur: %s, Fichier: %s", host, filename);

    return 0;
}