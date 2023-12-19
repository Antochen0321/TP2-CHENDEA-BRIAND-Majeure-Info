#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 512

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

struct tftp_ack {
    uint16_t opcode;
    uint16_t block_number;
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

    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(IPPROTO_UDP);
    server_addr.sin_port = htons(1069);
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
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_socktype = SOCK_DGRAM;
    
    //for optaining server adress
    error = getaddrinfo(host,"1069", &hints, &serverinfo);
    if (error != 0) {
        perror(gai_strerror(error));
        exit(EXIT_FAILURE);
    }
    
    // Create a socket for UDP communication
    sock = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol);
    if (sock < 0) {
        perror("Erreur lors de la creation de la socket");
        exit(EXIT_FAILURE);
    }

    send_rrq(sock, file);
    receive_file(sock); 

    // Close the socket and free addrinfo
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
