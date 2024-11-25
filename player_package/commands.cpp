#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cctype>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "commands.h"

#define SV_IP "127.0.0.1"
#define PORT "58058" //58000 + GN, where GN is 58
#define PLID_SIZE 6

/*
bool isSixDigitNumber(const std::string& input) {
    // Verifica se tem exatamente 6 caracteres e todos são dígitos
    if (input.length() == 6) {
        for (char c : input) {
            if (!std::isdigit(c)) {
                return false;
            }
        }
        return true;
    }
    return false;
}
*/

/*
int exit(const char* sv_ip, const char* port) {
    std::cout << "Comando: exit\n";
    std::cout << "sv_ip: " << sv_ip << "\n";
    std::cout << "port: " << port << "\n";
    //mandar msg (UDP) a avisar que vai terminar, caso esteja a meio do jogo
    return 1;
}
*/

int end_game(const char* sv_ip, const char* port, const char* plid) {

    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[256];


    std::cout << "Comando: quit\n";
    std::cout << "sv_ip: " << sv_ip << "\n";
    std::cout << "port: " << port << "\n";
    std::cout << "plid: " << plid << "\n";

    // Create UDP socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Prepare the hints structure
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    
    // Get address info
    errcode = getaddrinfo(sv_ip, port, &hints, &res);
    if (errcode != 0) {
        std::cerr << "Error in getaddrinfo: " << gai_strerror(errcode) << "\n";
        close(fd);
        return 1;
    }

    // traduzir msg para enviar para o server
    sprintf(buffer, "%s %s\n", "QUT", plid);
    printf("input: %s\n", buffer);

    // conectar com server
    n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        std::cerr << "Error sending data\n";
        freeaddrinfo(res);
        close(fd);
        return 1;
    }

    // Receive response
    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addrlen);
    if (n == -1) {
        std::cerr << "Error receiving data\n";
        freeaddrinfo(res);
        close(fd);
        return 1;
    }

    // Output the received message
    printf("output: %s\n", buffer);

    // clean up
    freeaddrinfo(res);
    close(fd);

    return 1;
}

int show_trials(const char* sv_ip, const char* port) {
/*     std::cout << "Comando: show_trials\n";
    std::cout << "sv_ip: " << sv_ip << "\n";
    std::cout << "port: " << port << "\n"; */

    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];

    // Create the socket
    fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (fd == -1) {
        perror("socket");
        exit(1);
    }

    // Set up the hints for getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    // Get address info
    errcode = getaddrinfo(sv_ip, port, &hints, &res);
    if (errcode != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(errcode));
        exit(1);
    }

    // Connect to the server
    n = connect(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        perror("connect");
        exit(1);
    }

    // Write to the server
    n = write(fd, "Hello!\n", 7);
    if (n == -1) {
        perror("write");
        exit(1);
    }

    // Read response from the server
    n = read(fd, buffer, 128);
    if (n == -1) {
        perror("read");
        exit(1);
    }

    // Print the response
    write(1, "echo: ", 6);
    write(1, buffer, n);

    // Clean up
    freeaddrinfo(res);
    close(fd);

    return 1;
}

int scoreboard(const char* sv_ip, const char* port) {
    std::cout << "Comando: scoreboard\n";
    std::cout << "sv_ip: " << sv_ip << "\n";
    std::cout << "port: " << port << "\n";
    //mostrar scoreboard (TCP)
    return 1;
}

int start_game(const char* sv_ip, const char* port, const char* plid, const char* max_playtime) {

    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[256];

    // printar argumentos para confirmar
    std::cout << "Comando: start_game\n";
    std::cout << "sv_ip: " << sv_ip << "\n";
    std::cout << "port: " << port << "\n";
    std::cout << "PLID: " << plid << "\n";
    std::cout << "max_playtime: " << max_playtime << "\n";

    // Create UDP socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Prepare the hints structure
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    
    // Get address info
    errcode = getaddrinfo(sv_ip, port, &hints, &res);
    if (errcode != 0) {
        std::cerr << "Error in getaddrinfo: " << gai_strerror(errcode) << "\n";
        close(fd);
        return 1;
    }

    // traduzir msg para enviar para o server
    sprintf(buffer, "%s %s %s\n", "SNG", plid, max_playtime);
    printf("input: %s\n", buffer);

    // conectar com server
    n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        std::cerr << "Error sending data\n";
        freeaddrinfo(res);
        close(fd);
        return 1;
    }

    
    // Receive response
    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addrlen);
    if (n == -1) {
        std::cerr << "Error receiving data\n";
        freeaddrinfo(res);
        close(fd);
        return 1;
    }

    // Output the received message
    //std::cout << "echo: " << std::string(buffer, n) << "\n";
    printf("output: %s\n", buffer);

    // clean up
    freeaddrinfo(res);
    close(fd);

    return 1;
}

int try_command(const char* sv_ip, const char* port, const char* c1, const char* c2, const char* c3, const char* c4, int nT, const char* plid) {


    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[256];

    //printf("\n\n\n --------------------------------- plid: %s --------------------\n\n\n", plid);

    std::cout << "Comando: try_command\n";
    std::cout << "sv_ip: " << sv_ip << "\n";
    std::cout << "port: " << port << "\n";
    std::cout << "C1: " << c1 << "\n";
    std::cout << "C2: " << c2 << "\n";
    std::cout << "C3: " << c3 << "\n";
    std::cout << "C4: " << c4 << "\n";
    std::cout << "nT: " << nT << "\n";
    std::cout << "plid: " << plid << "\n";

    // Create UDP socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Prepare the hints structure
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    
    // Get address info
    errcode = getaddrinfo(sv_ip, port, &hints, &res);
    if (errcode != 0) {
        std::cerr << "Error in getaddrinfo: " << gai_strerror(errcode) << "\n";
        close(fd);
        return 1;
    }

    // traduzir msg para enviar para o server (plid precisam ser substituidos pelos valores corretos)
    sprintf(buffer, "%s %s %s %s %s %s %d\n", "TRY", plid, c1, c2, c3, c4, nT);

    printf("input: %s\n", buffer);

    // conectar com server
    n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        std::cerr << "Error sending data\n";
        freeaddrinfo(res);
        close(fd);
    }
    return 1;
}

int debug_command(const char* sv_ip, const char* port, const char* plid, const char* max_playtime, const char* c1, const char* c2, const char* c3, const char* c4) {

    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[256];

    std::cout << "Comando: debug_command\n";
    std::cout << "PLID: " << plid << "\n";
    std::cout << "max_playtime: " << max_playtime << "\n";
    std::cout << "C1: " << c1 << "\n";
    std::cout << "C2: " << c2 << "\n";
    std::cout << "C3: " << c3 << "\n";
    std::cout << "C4: " << c4 << "\n";

    // Create UDP socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Prepare the hints structure
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    
    // Get address info
    errcode = getaddrinfo(sv_ip, port, &hints, &res);
    if (errcode != 0) {
        std::cerr << "Error in getaddrinfo: " << gai_strerror(errcode) << "\n";
        close(fd);
        return 1;
    }

    // traduzir msg para enviar para o server
    sprintf(buffer, "%s %s %s %s %s %s %s\n", "DBG", plid, max_playtime, c1, c2, c3, c4);
    printf("input: %s\n", buffer);

    // conectar com server
    n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        std::cerr << "Error sending data\n";
        freeaddrinfo(res);
        close(fd);
        return 1;
    }

    // Receive response
    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addrlen);
    if (n == -1) {
        std::cerr << "Error receiving data\n";
        freeaddrinfo(res);
        close(fd);
        return 1;
    }

    // Output the received message
    printf("output: %s\n", buffer);

    // clean up
    freeaddrinfo(res);
    close(fd);

    return 1;
}
