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
#define GAME_WON 1

bool verifyPLID(const std::string& input) {
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

bool verifyMaxPlaytime(const std::string& input) {
    if(input.length() <= 3) {
        for(char c : input) {
            if(!std::isdigit(c)) {
                return false;
            }
        }
        if(std::stoi(input) > 600)
            return false;
        return true;
    }
    return false;
}


bool verifyColor(const std::string& color) {
    std::string validColors = "RGBYOP";
    return color.length() == 1 && validColors.find(color) != std::string::npos;
}


int end_game(const char* sv_ip, const char* port, const char* plid) {

    int fd, errcode, num_args;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char request_buffer[256], response_buffer[256];
    char cmd[32], status[32], arg1[32], arg2[32], arg3[32], arg4[32];

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

    memset(request_buffer, 0, sizeof(request_buffer));
    sprintf(request_buffer, "%s %s\n", "QUT", plid);

    // conectar com server
    n = sendto(fd, request_buffer, strlen(request_buffer), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        std::cerr << "Error sending data\n";
        freeaddrinfo(res);
        close(fd);
        return 1;
    }

    // Receive response
    addrlen = sizeof(addr);
    memset(response_buffer, 0, sizeof(response_buffer));
    n = recvfrom(fd, response_buffer, sizeof(response_buffer), 0, (struct sockaddr*)&addr, &addrlen);
    if (n == -1) {
        std::cerr << "Error receiving data\n";
        freeaddrinfo(res);
        close(fd);
        return 1;
    }
    num_args = sscanf(response_buffer, "%s %s %s %s %s %s\n", cmd, status, arg1, arg2, arg3, arg4);

    if(strcmp(cmd, "RQT")) {
        std::cerr << "Communication error.\n";
        freeaddrinfo(res);
        close(fd);
        return 1;
    }
    switch (num_args){
        case 2:
            if(!strcmp(status, "NOK")) {
                std::cout << "Player does not have an ongoing game.\n";
                return -1;
            }
            else if(!strcmp(status, "ERR")) {
                std::cout << "Something went wrong.\n";
                return -1;
            }
            else {
                std::cerr << "Communication error.\n";
                return -1;
            }
            break;
        case 6:
            if(!strcmp(status, "OK")) {
                std::cout << "Game terminated. The color code was:\n\t### " << arg1 << " " << arg2 << " " << arg3 << " " << arg4 << " ###\n";
            }
            else {
                std::cerr << "Communication error.\n";
            }
            break;
        default:
            std::cerr << "Communication error.\n";
            break;
    }
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

    int fd, errcode, num_args;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char request_buffer[256], response_buffer[256];
    char cmd[32], status[32];

    if(!verifyPLID(plid)) {
        std::cerr << "Invalid PLID. Variable must be six digits long.\n";
        return 0;
    }

    if(!verifyMaxPlaytime(max_playtime)) {
        std::cerr << "Invalid Max Playtime. Variable must be less than 600 seconds.\n";
        return 0;
    }
    // Create UDP socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        std::cerr << "Error creating socket\n";
        return 0;
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
        return 0;
    }

    memset(request_buffer, 0, sizeof(request_buffer));
    // traduzir msg para enviar para o server
    sprintf(request_buffer, "%s %s %s\n", "SNG", plid, max_playtime);

    // conectar com server
    n = sendto(fd, request_buffer, strlen(request_buffer), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        std::cerr << "Error sending data\n";
        freeaddrinfo(res);
        close(fd);
        return 0;
    }
    
    // Receive response
    addrlen = sizeof(addr);

    memset(response_buffer, 0, sizeof(response_buffer));
    n = recvfrom(fd, response_buffer, sizeof(response_buffer), 0, (struct sockaddr*)&addr, &addrlen);
    if (n == -1) {
        std::cerr << "Error receiving data\n";
        freeaddrinfo(res);
        close(fd);
        return 0;
    }

    num_args = sscanf(response_buffer, "%s %s\n", cmd, status);

    if(strcmp(cmd, "RSG")) {
        std::cerr << "Communication error.\n";
        freeaddrinfo(res);
        close(fd);
        return 1;
    }
    
    switch (num_args){
        case 2:
            if(!strcmp(status, "NOK")) {
                std::cout << "Player " << plid << " already has an ongoing game.\n";
            }
            else if(!strcmp(status, "OK")) {
                std::cout << "New game started (max " << max_playtime <<" sec).\n";
            }
            else if(!strcmp(status, "ERR")) {
                std::cout << "Something is not right. Probably player PLID is invalid, time is over 600 seconds or the syntax of the \"SNG\" is incorrect\n.";
            }
            else {
                std::cerr << "Communication error.\n";
            }
            break;
        default:
            std::cerr << "Communication error.\n";
            break;
    }

    // clean up
    freeaddrinfo(res);
    close(fd);

    return 1;
}

int try_command(const char* sv_ip, const char* port, const char* c1, const char* c2, const char* c3, const char* c4, int* nT, const char* plid) {


    int fd, errcode;
    ssize_t n;
    int num_args;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char request_buffer[256], response_buffer[256];
    char cmd[32], status[32], arg1[32], arg2[32], arg3[32], arg4[32];


    //talvez não seja preciso, pois já é testado no debug/start
    if(!verifyPLID(plid)) {
        std::cerr << "Invalid PLID. Variable must be six digits long.\n";
        return 0;
    }

    if(!verifyColor(c1) && !verifyColor(c2) && !verifyColor(c3) && !verifyColor(c4)) {
        std::cerr << "Incorrect color Code. Possible colors are:\nR -> Red\nG -> Green\nB -> Blue\nY -> Yellow\nO -> Orange\nP -> purple\n";
        return 0;
    }
    
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

    memset(request_buffer, 0, sizeof(request_buffer));
    // traduzir msg para enviar para o server (plid precisam ser substituidos pelos valores corretos)
    sprintf(request_buffer, "%s %s %s %s %s %s %d\n", "TRY", plid, c1, c2, c3, c4, *nT);

    // connect with the server
    n = sendto(fd, request_buffer, strlen(request_buffer), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        std::cerr << "Error sending data\n";
        freeaddrinfo(res);
        close(fd);
    }

    // Receive response
    addrlen = sizeof(addr);
    memset(response_buffer, 0, sizeof(response_buffer));
    n = recvfrom(fd, response_buffer, sizeof(response_buffer), 0, (struct sockaddr*)&addr, &addrlen);
    if (n == -1) {
        std::cerr << "Error receiving data\n";
        freeaddrinfo(res);
        close(fd);
        return 1;
    }

    num_args = sscanf(response_buffer, "%s %s %s %s %s %s\n", cmd, status, arg1, arg2, arg3, arg4);

    if(strcmp(cmd, "RTR")) {
        std::cerr << "Communication error.\n";
        freeaddrinfo(res);
        close(fd);
        return 1;
    }
    
    switch (num_args){
        case 2:
            if(!strcmp(status, "DUP")) {
                std::cout << "You already tried this color code. Number of trials not increased.\n";
            }
            else if(!strcmp(status, "INV")) {
                std::cerr << "Wrong trial number.\n";
            }
            else if(!strcmp(status, "NOK")) {
                std::cerr << "Something is not right, probably player " << plid << " does not have an ongoing game.\n";
            }
            else if(!strcmp(status, "ERR")) {
                std::cerr << "Invalid syntax. Either PLID or color code is not allowed.\n";
            }
            else {
                std::cerr << "Communication error.\n";
            }
            break;

        case 5:
            if(!strcmp(status, "OK")) {
                if (!strcmp(arg2, "4")) {
                    std::cout << "Congratulations!! You correctly guessed the secret color code in " << arg1 << "trials\n";
                    freeaddrinfo(res);
                    close(fd);
                    return GAME_WON;
                }
                else {
                    std::cout << "nB: " << arg2 << "\tnW: " << arg3 << "\n"; // meter numero de trials
                    (*nT)++;
                }
            }
            else {
                std::cerr << "Communication error.\n";
            }
            break;
        case 6:
            if(!strcmp(status, "ENT")) {
                std::cout << "Limit of trials reached. The correct color code was:\n\t\t### " << arg1 << " " << arg2 << " " << arg3 << " " << arg4 << " ###\n";
            }
            else if(!strcmp(status, "ETM")) {
                std::cout << "Maximum play time exceeded. The correct color code was:\n\t\t### " << arg1 << " " << arg2 << " " << arg3 << " " << arg4 << " ###\n";
            }
            else {
                std::cerr << "Communication error.\n";
            }
            break;
        default:
            std::cerr << "Communication error.\n";
            break;
    }

    // clean up
    freeaddrinfo(res);
    close(fd);
    return 1;
}

int debug_command(const char* sv_ip, const char* port, const char* plid, const char* max_playtime, const char* c1, const char* c2, const char* c3, const char* c4) {

    int fd, errcode, num_args;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char request_buffer[256], response_buffer[256];
    char cmd[32], status[32];

    if(!verifyPLID(plid)) {
        std::cerr << "Invalid PLID. Variable must be six digits long.\n";
        return 0;
    }
    if(!verifyMaxPlaytime(max_playtime)) {
        std::cerr << "Invalid Max Playtime. Variable must be less than 600 seconds.\n";
        return 0;
    }
    if(!verifyColor(c1) || !verifyColor(c2) || !verifyColor(c3) || !verifyColor(c4)) {
        std::cerr << "Incorrect color Code. Possible colors are:\nR -> Red\nG -> Green\nB -> Blue\nY -> Yellow\nO -> Orange\nP -> purple\n";
        return 0;
    }

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

    memset(request_buffer, 0, sizeof(request_buffer));
    sprintf(request_buffer, "%s %s %s %s %s %s %s\n", "DBG", plid, max_playtime, c1, c2, c3, c4);

    // conectar com server
    n = sendto(fd, request_buffer, strlen(request_buffer), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        std::cerr << "Error sending data\n";
        freeaddrinfo(res);
        close(fd);
        return 1;
    }


    // Receive response
    addrlen = sizeof(addr);
    memset(response_buffer, 0, sizeof(response_buffer));
    n = recvfrom(fd, response_buffer, sizeof(response_buffer), 0, (struct sockaddr*)&addr, &addrlen);
    if (n == -1) {
        std::cerr << "Error receiving data\n";
        freeaddrinfo(res);
        close(fd);
        return 1;
    }

    num_args = sscanf(response_buffer, "%s %s\n", cmd, status);

    if(strcmp(cmd, "RDB")) {
        std::cerr << "Communication error.\n";
        freeaddrinfo(res);
        close(fd);
        return 1;
    }
    
    switch (num_args){
        case 2:
            if(!strcmp(status, "NOK")) {
                std::cout << "Player " << plid << " already has an ongoing game.\n";
            }
            else if(!strcmp(status, "OK")) {
                std::cout << "New game started in debug mode (max " << max_playtime <<"sec).\n";
            }
            else if(!!strcmp(status, "ERR")) {
                std::cout << "Something is not right. Check the validity of the following arguments:\n" <<
                            "-> Syntax of \"DBG\" message\n" <<
                            "-> PLID\n-> Color code\n" <<
                            "-> Max play time (cannot be over 600 seconds)\n";
            }
            else {
                std::cerr << "Communication error.\n";
            }
            break;
        default:
            std::cerr << "Communication error.\n";
            break;
    }

    // clean up
    freeaddrinfo(res);
    close(fd);

    return 1;
}
