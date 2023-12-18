#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 512
#define PORT '69'

//RRQ Structure
struct tftp_rrq {
    uint16_t opcode; 
    char filename[MAX_BUFFER_SIZE];
    char mode[MAX_BUFFER_SIZE];
};

struct tftp_data{
    uint16_t opcode;
    uint16_t block_number;
    char data[MAX_BUFFER_SIZE - 4];
};

void receive_file(int sock){
    struct tftp_data data_packet;
    ssize_t bytes;
    struct sockaddr src_addr;
    socklen_t addr_len = sizeof(src_addr);
    //for receive datapacket from the server
    bytes = recvfrom(sock, &data_packet, sizeof(data_packet), 0, &src_addr, &addr_len);
    if (bytes < 0) {
        perror("Error from packet reception");
        exit(EXIT_FAILURE);
    }
}

void send_rrq(int sock, const char *file){
    struct tftp_rrq rrq;
    rrq.opcode = htons(1);
    strncpy(rrq.filename, file, sizeof(rrq.filename));
    strncpy(rrq.mode, "octet", sizeof(rrq.mode));
    // Send RRQ to server
    if (send(sock, &rrq, sizeof(rrq), 0) < 0) {
        perror("Erreur lors de l'envoi de la requête RRQ");
        exit(EXIT_FAILURE);
    }
}

void gettftp(char *host, char *file){
    //define the socket
    int sock;
    
    //configuration of getaddrinfo structure
    struct addrinfo hints, *serverinfo, *p;
    int error;
    char buffer[MAX_BUFFER_SIZE];
    memset(&hints, 0, sizeof hints);
    
    //configure getaddrinfo criteria
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    //for optaining server adress
    error = getaddrinfo(host,"69", &hints, &serverinfo);
    if (error != 0) {
        perror(gai_strerror(error));
        exit(EXIT_FAILURE);
    }
    
    //trying to connect to host by browsing results of getaddrinfo
    sock = -1;
    for (p = serverinfo; p != NULL; p = p->ai_next) {
    sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sock < 0) {
        perror("Erreur lors de la création de la socket");
        continue;
    }
    if (connect(sock, p->ai_addr, p->ai_addrlen) < 0) {
        perror("Erreur de connexion");
        close(sock);
        sock = -1;
        continue;
    }
    break;
    }

    if(p == NULL) {
        fprintf(stderr, "Impossible de se connecter au serveur\n");
        exit(EXIT_FAILURE);
    }

    send_rrq(sock, file);
    receive_file(sock);
    //close the socket and make free addrinfo
    close(sock);
    freeaddrinfo(serverinfo);
}

int main(int argc, char *argv[]){
    //First we must verify we call the function with the good length of argv, we must have only 3 arguments, the command, the host and the filename
    if(argc != 3){
        printf("Pas le bon nombre d'arguments, il en faut 3, pas un de moins ou de plus");
        exit(EXIT_FAILURE);
    }

    gettftp(argv[1], argv[2]);

    return 0;
}