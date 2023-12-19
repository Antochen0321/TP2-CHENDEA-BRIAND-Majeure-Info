#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 512

// WRQ Structure
struct tftp_wrq {
    uint16_t opcode;
    char filename[MAX_BUFFER_SIZE];
    char mode[MAX_BUFFER_SIZE];
};

struct tftp_data {
    uint16_t opcode;
    uint16_t block_number;
    char data[MAX_BUFFER_SIZE - 4];
};

struct tftp_ack {
    uint16_t opcode;
    uint16_t block_number;
};

void send_wrq(int sock, const char *file) {
    struct tftp_wrq wrq;
    wrq.opcode = htons(2);
    strncpy(wrq.filename, file, sizeof(wrq.filename));
    strncpy(wrq.mode, "octet", sizeof(wrq.mode));

    // Server TFTP address (replace with the actual address)
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(IPPROTO_UDP);
    server_addr.sin_port = htons(1069);

    // Use sendto to send the WRQ request
    if (sendto(sock, &wrq, sizeof(wrq), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur d'envoi de la WRQ");
        exit(EXIT_FAILURE);
    }
}

void send_file(int sock, const char *filename) {
    FILE *file = fopen(filename, "rb"); 
    if (file == NULL) {
        perror("EErreur de lecture du fichier");
        exit(EXIT_FAILURE);
    }

    struct tftp_data data_packet;
    struct tftp_ack ack_packet;
    ssize_t bytes;
    struct sockaddr src_addr;
    socklen_t addr_len = sizeof(src_addr);
    uint16_t block_number = 1;

    while (1) {
        // Read data from file
        bytes = fread(data_packet.data, 1, sizeof(data_packet.data), file);

        // Check if it's the last block
        if (feof(file)) {
            break;
        }

        // Construct data packet
        data_packet.opcode = htons(3);
        data_packet.block_number = htons(block_number);

        // Send data packet to the server
        if (sendto(sock, &data_packet, bytes + 4, 0, &src_addr, addr_len) < 0) {
            perror("Erreur d'envoi des données");
            exit(EXIT_FAILURE);
        }

        // Wait for acknowledgment (ACK) from the server
        bytes = recvfrom(sock, &ack_packet, sizeof(ack_packet), 0, &src_addr, &addr_len);
        if (bytes < 0) {
            perror("Erreur de reception");
            exit(EXIT_FAILURE);
        }

        // Check if received opcode is ACK (4) and block number is expected
        if (ntohs(ack_packet.opcode) == 4 && ntohs(ack_packet.block_number) == block_number) {
            // Increment block number for the next iteration
            block_number++;
        } else {
            // Duplicate data packet for the previous block
            fseek(file, -bytes, SEEK_CUR);
        }
    }

    fclose(file);
}

void puttftp(char *host, char *file) {
    // Define the socket
    int sock;

    // Configuration of getaddrinfo structure
    struct addrinfo hints, *serverinfo, *p;
    int error;
    char buffer[MAX_BUFFER_SIZE];
    memset(&hints, 0, sizeof hints);

    // Configure getaddrinfo criteria
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_socktype = SOCK_DGRAM;

    // For obtaining server address
    error = getaddrinfo(host, "1069", &hints, &serverinfo);
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

    send_wrq(sock, file);
    send_file(sock, file);

    // Close the socket and free addrinfo
    close(sock);
    freeaddrinfo(serverinfo);
}

int main(int argc, char *argv[]) {
    // Verify the correct number of arguments
    if (argc != 3) {
        printf("Pas le bon nombre d'arguments, il en faut 3, pas un de moins ou de plus");
        exit(EXIT_FAILURE);
    }

    puttftp(argv[1], argv[2]);

    return 0;
}
