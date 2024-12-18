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
#include "../utils.h"
#include "../constants.h"


int end_game(const char* sv_ip, const char* port, const char* plid) {

    int fd, errcode, num_args, return_value;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char request_buffer[256], response_buffer[256];
    char cmd[32], status[32], arg1[32], arg2[32], arg3[32], arg4[32];

    // Create UDP socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        std::cerr << "ERROR: failed to create socket\n";
        return ERROR;
    }

    // Prepare the hints structure
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    
    // Get address info
    errcode = getaddrinfo(sv_ip, port, &hints, &res);
    if (errcode != 0) {
        std::cerr << "ERROR: failed to getaddrinfo: " << gai_strerror(errcode) << "\n";
        close(fd);
        return ERROR;
    }

    memset(request_buffer, 0, sizeof(request_buffer));
    sprintf(request_buffer, "%s %s\n", "QUT", plid);

    // conectar com server
    n = sendto(fd, request_buffer, strlen(request_buffer), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        std::cerr << "ERROR: failed to send data\n";
        freeaddrinfo(res);
        close(fd);
        return ERROR;
    }

    // Receive response
    addrlen = sizeof(addr);
    memset(response_buffer, 0, sizeof(response_buffer));
    n = recvfrom(fd, response_buffer, sizeof(response_buffer), 0, (struct sockaddr*)&addr, &addrlen);
    if (n == -1) {
        std::cerr << "ERROR: failed to receive data\n";
        freeaddrinfo(res);
        close(fd);
        return ERROR;
    }
    num_args = sscanf(response_buffer, "%s %s %s %s %s %s\n", cmd, status, arg1, arg2, arg3, arg4);

    if(strcmp(cmd, "RQT")) {
        std::cerr << "ERROR: Communication error.\n";
        freeaddrinfo(res);
        close(fd);
        return ERROR;
    }
    switch (num_args){
        case 2:
            if(!strcmp(status, "NOK")) {
                std::cout << "You do not have an ongoing game.\n";
                return_value = ERROR;
            }
            else if(!strcmp(status, "ERR")) {
                std::cout << "FAIL: Something went wrong.\n";
                return_value = ERROR;
            }
            else {
                std::cerr << "ERROR: Communication error.\n";
                return_value = ERROR;
            }
            break;
        case 6:
            if(!strcmp(status, "OK")) {
                std::cout << "Game terminated. The color code was:\n\n\t### " << arg1 << " " << arg2 << " " << arg3 << " " << arg4 << " ###\n";
                return_value = 1;
            }
            else {
                std::cerr << "Communication error.\n";
                return_value = ERROR;
            }
            break;
        default:
            std::cerr << "Communication error.\n";
            return_value = ERROR;
            break;
    }
    // clean up
    freeaddrinfo(res);
    close(fd);

    return return_value;
}

int show_trials(const char* sv_ip, const char* port, const char* plid) {

    int fd, errcode; 
    struct addrinfo hints, *res;
    char request_buffer[256], response_buffer[MAX_FILE_SIZE + HEADER_SIZE], aux_buffer[MAX_FILE_SIZE + HEADER_SIZE];
    char cmd[32], status[32], file_name[128];
    FILE *st_file;
    ssize_t n, total_bytes = 0, file_size;

    // Set up the hints for getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    // Get address info
    errcode = getaddrinfo(sv_ip, port, &hints, &res);
    if (errcode != 0) {
        std::cerr << "ERROR: failed to getaddrinfo: "<< gai_strerror(errcode) << "\n";
        return ERROR;
    }
    // Create the socket
    fd = socket(AF_INET, SOCK_STREAM, res->ai_protocol); // TCP socket
    if (fd == ERROR) {
        std::cerr << "ERROR: failed to create socket.\n";
        return ERROR;
    }
    // Connect to the server
    n = connect(fd, res->ai_addr, res->ai_addrlen);
    if (n == ERROR) {
        std::cerr << "ERROR: failed to connect to the server.\n";
        freeaddrinfo(res);
        return ERROR;
    }

    /*REQUEST SHOW TRIALS TO THE SERVER*/
    memset(request_buffer, 0, sizeof(request_buffer));
    sprintf(request_buffer, "STR %s\n", plid);
    
    ssize_t buffer_length = sizeof(request_buffer);
    while(total_bytes < buffer_length) {
        ssize_t bytes_sent = write(fd, request_buffer + total_bytes, buffer_length - total_bytes);
        if(bytes_sent < 0) {
            std::cerr << "ERROR: failed while writing to TCP connection\n";
            freeaddrinfo(res);
            close(fd);
            return ERROR;
        }
        total_bytes += bytes_sent;
    }

    /*GET THE RESPONSE FROM THE SERVER*/
    memset(response_buffer, 0, sizeof(response_buffer));
    total_bytes = 0;
    size_t header_size;
    ssize_t max_size = MAX_FILE_SIZE + HEADER_SIZE;

    // start reading from the tcp connection
    // stop when the header was fully read to the response buffer
    while (true) {
        memset(aux_buffer, 0, sizeof(aux_buffer));
        ssize_t bytes_read = read(fd, aux_buffer, max_size);
        printf("aux-buffer: %s\n", aux_buffer);
        if(bytes_read < 0) {
            std::cerr << "ERROR: failed while reading from TCP connection\n";
            freeaddrinfo(res);
            close(fd);
            return ERROR;
        }
        if(bytes_read == 0) {
            break;
        }
        strncat(response_buffer, aux_buffer, bytes_read);
        total_bytes += bytes_read;
        max_size -= bytes_read;

        if((header_size = containsChar(response_buffer, strlen(response_buffer), '\n'))) {
            sscanf(response_buffer, "%s %s %s %ld\n", cmd, status, file_name, &file_size);
            break;
        }
    }

    // verify if args are correct
    if(strlen(file_name) > 24) {
        std::cerr << "ERROR: Fname cannot be bigger than 24B\n";
        return ERROR;
    }
    if(file_size > MAX_FILE_SIZE) {
        std::cerr << "ERROR: Fsize cannot be bigger than 2KiB (2x1024B)\n";
        return ERROR;
    }
    
    // create a file to store the information of the game and read the rest
    // of the file, if needed
    if(!strcmp(status, "ACT") || !strcmp(status, "FIN")) {

        st_file = fopen(file_name, "w+");
        total_bytes -= header_size;
        fwrite(response_buffer + header_size, 1, total_bytes, st_file);

        // run while the file was not fully read from the server
        while(total_bytes < file_size) {
            memset(aux_buffer, 0, sizeof(aux_buffer));
            ssize_t bytes_read = read(fd, aux_buffer, max_size);
            if(bytes_read < 0) {
                std::cerr << "ERROR: failed while reading from TCP connection\n";
                freeaddrinfo(res);
                close(fd);
                return ERROR;
            }
            if(bytes_read == 0) {
                break;
            }
            strncat(response_buffer, aux_buffer, bytes_read);
            total_bytes += bytes_read;
            max_size -= bytes_read;
            fwrite(aux_buffer, 1, bytes_read, st_file);
        }

        //print in the terminal the game information
        std::cout << "Fname: " << file_name << "\n";
        std::cout << "Fsize: " << file_size << "\n";
        char line[256];
        fseek(st_file, 0, SEEK_SET);
        while (fgets(line, sizeof(line), st_file))
            std::cout << line;
    }

    // the player PLID does not have any game history
    else if(!strcmp(status, "NOK")) {
        std::cout << "Something went wrong, probably player has no game history\n";
        return 0;
    }

    // error
    else {
        std::cerr << "ERROR: Communication error.\n";
    }
    freeaddrinfo(res);
    fclose(st_file);
    close(fd);
    return 1;
}



int scoreboard(const char* sv_ip, const char* port) {
    int fd, errcode; 
    struct addrinfo hints, *res;
    char request_buffer[256], response_buffer[MAX_FILE_SIZE + HEADER_SIZE], aux_buffer[MAX_FILE_SIZE + HEADER_SIZE];
    char cmd[32], status[32], file_name[128];
    FILE *st_file;
    ssize_t n, total_bytes = 0, file_size;

    // Set up the hints for getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    // Get address info
    errcode = getaddrinfo(sv_ip, port, &hints, &res);
    if (errcode != 0) {
        std::cerr << "ERROR: failed to getaddrinfo: "<< gai_strerror(errcode) << "\n";
        return ERROR;
    }
    // Create the socket
    fd = socket(AF_INET, SOCK_STREAM, res->ai_protocol); // TCP socket
    if (fd == ERROR) {
        std::cerr << "ERROR: failed to create socket.\n";
        return ERROR;
    }
    // Connect to the server
    n = connect(fd, res->ai_addr, res->ai_addrlen);
    if (n == ERROR) {
        std::cerr << "ERROR: failed to connect to the server.\n";
        freeaddrinfo(res);
        return ERROR;
    }

    /*REQUEST SHOW TRIALS TO THE SERVER*/
    memset(request_buffer, 0, sizeof(request_buffer));
    sprintf(request_buffer, "SSB\n");
    
    ssize_t buffer_length = sizeof(request_buffer);
    while(total_bytes < buffer_length) {
        ssize_t bytes_sent = write(fd, request_buffer + total_bytes, buffer_length - total_bytes);
        if(bytes_sent < 0) {
            std::cerr << "ERROR: failed while writing to TCP connection\n";
            freeaddrinfo(res);
            close(fd);
            return ERROR;
        }
        total_bytes += bytes_sent;
    }

    /*GET THE RESPONSE FROM THE SERVER*/
    memset(response_buffer, 0, sizeof(response_buffer));
    total_bytes = 0;
    size_t header_size;
    ssize_t max_size = MAX_FILE_SIZE + HEADER_SIZE;

    // start reading from the tcp connection
    // stop when the header was fully read to the response buffer
    while (true) {
        memset(aux_buffer, 0, sizeof(aux_buffer));
        ssize_t bytes_read = read(fd, aux_buffer, max_size);
        if(bytes_read < 0) {
            std::cerr << "ERROR: failed while reading from TCP connection\n";
            freeaddrinfo(res);
            close(fd);
            return ERROR;
        }
        if(bytes_read == 0) {
            break;
        }
        strncat(response_buffer, aux_buffer, bytes_read);
        total_bytes += bytes_read;
        max_size -= bytes_read;

        if((header_size = containsChar(response_buffer, strlen(response_buffer), '\n'))) {
            sscanf(response_buffer, "%s %s %s %ld\n", cmd, status, file_name, &file_size);
            break;
        }
    }

    // verify if args are correct
    if(strlen(file_name) > 24) {
        std::cerr << "ERROR: Fname cannot be bigger than 24B\n";
        return ERROR;
    }
    if(file_size > MAX_FILE_SIZE) {
        std::cerr << "ERROR: Fsize cannot be bigger than 2KiB (2x1024B)\n";
        return ERROR;
    }
    
    // create a file to store the information of the game and read the rest
    // of the file, if needed
    if(!strcmp(status, "OK")) {

        st_file = fopen(file_name, "w+");
        total_bytes -= header_size;
        fwrite(response_buffer + header_size, 1, total_bytes, st_file);

        // run while the file was not fully read from the server
        while(total_bytes < file_size) {
            memset(aux_buffer, 0, sizeof(aux_buffer));
            ssize_t bytes_read = read(fd, aux_buffer, max_size);
            if(bytes_read < 0) {
                std::cerr << "ERROR: failed while reading from TCP connection\n";
                freeaddrinfo(res);
                close(fd);
                return ERROR;
            }
            if(bytes_read == 0) {
                break;
            }
            strncat(response_buffer, aux_buffer, bytes_read);
            total_bytes += bytes_read;
            max_size -= bytes_read;
            fwrite(aux_buffer, 1, bytes_read, st_file);
        }

        //print in the terminal the game information
        std::cout << "Fname: " << file_name << "\n";
        std::cout << "Fsize: " << file_size << "\n";
        char line[256];
        fseek(st_file, 0, SEEK_SET);
        while (fgets(line, sizeof(line), st_file))
            std::cout << line;
    }

    // the player PLID does not have any game history
    else if(!strcmp(status, "EMPTY")) {
        std::cout << "No scoreboard history: no game was yet won by any player.\n";
        return 0;
    }

    // error
    else {
        std::cerr << "ERROR: Communication error.\n";
    }
    freeaddrinfo(res);
    fclose(st_file);
    close(fd);
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

    if(!validPLID(plid)) {
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
    if(!validPLID(plid)) {
        std::cerr << "Invalid PLID. Variable must be six digits long.\n";
        return 0;
    }

    if(!validColor(c1) || !validColor(c2) || !validColor(c3) || !validColor(c4)) {
        std::cerr << "Incorrect color Code. Possible colors are:\nR -> Red\nG -> Green\nB -> Blue\nY -> Yellow\nO -> Orange\nP -> purple\n";
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
    // traduzir msg para enviar para o server (plid precisam ser substituidos pelos valores corretos)
    sprintf(request_buffer, "%s %s %s %s %s %s %d\n", "TRY", plid, c1, c2, c3, c4, *nT);

    // connect with the server
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
    //printf("RESPONSE BUFFER: %s", response_buffer);
    num_args = sscanf(response_buffer, "%s %s %s %s %s %s\n", cmd, status, arg1, arg2, arg3, arg4);

    if(strcmp(cmd, "RTR")) {
        std::cerr << "Communication error.\n";
        freeaddrinfo(res);
        close(fd);
        return 0;
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
                    int trials_left = 8 - std::stoi(arg1);
                    std::cout << "nB: " << arg2 << "\tnW: " << arg3 << "\ttrials left: " << trials_left << "\n"; // meter numero de trials
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

    if(!validPLID(plid)) {
        std::cerr << "Invalid PLID. Variable must be six digits long.\n";
        return 0;
    }
    if(!verifyMaxPlaytime(max_playtime)) {
        std::cerr << "Invalid Max Playtime. Variable must be less than 600 seconds.\n";
        return 0;
    }
    if(!validColor(c1) || !validColor(c2) || !validColor(c3) || !validColor(c4)) {
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