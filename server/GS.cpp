#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cctype>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <filesystem>

#include <fstream>
#include <ctime>
#include <iomanip>

#include "../utils.h"
#include "../constants.h"

//provavelmente, falta mais close(tcp_socket) e close(udp_socket), nos erros

int endGame(const char* plid, const char finisher_mode) {

    char player_directory[256], file_name[256], time_str[16], file_line[256];
    time_t fulltime;                                         

    // get file name
    memset(file_name, 0, sizeof(file_name));
    sprintf(file_name, "./server/GAMES/GAME_%s.txt", plid);

    /*ADD LAST LINE TO THE PLAYER FILE*/
    memset(file_line, 0, sizeof(file_line));
    FILE* file = fopen(file_name, "r+");
    if(!file) {
        return ERROR;   
    }
    // get first line
    fseek(file, 0, SEEK_SET);
    if(fgets(file_line, sizeof(file_line), file) == NULL) {
        return ERROR;  
    }

    // get game start time
    int max_game_time;
    time_t start_time;
    sscanf(file_line + 15, "%03d %*04d-%*02d-%*02d %*02d:%*02d:%*02d %ld", &max_game_time, &start_time);

    time_t current_time_seconds = time(NULL);
    int game_duration = (int)difftime(current_time_seconds, start_time);

    // calculate final game duration
    if(game_duration > max_game_time) {game_duration = max_game_time;}

    // define last line
    time(&fulltime);                // Current time in seconds since 1970
    struct tm* current_time = gmtime(&fulltime);

    long initialTime = fulltime;    // Time in seconds since 1970
    memset(file_line, 0, sizeof(file_line));
    sprintf(file_line, "%04d-%02d-%02d %02d:%02d:%02d %d\n",
             current_time->tm_year + 1900, current_time->tm_mon + 1, current_time->tm_mday,
             current_time->tm_hour, current_time->tm_min, current_time->tm_sec, game_duration);

    fseek(file, 0, SEEK_END);                           // go to the end of the file
    fwrite(file_line, 1, strlen(file_line), file);      // write last line
    fclose(file);                                       // close file


    /*CHANGE PLAYER DIRECTORY PATH*/
    namespace fs = std::filesystem;

    //define player directory path
    memset(player_directory, 0, sizeof(player_directory));
    sprintf(player_directory, "server/GAMES/%s", plid);

    if(!fs::exists(player_directory)) {
        if(!fs::create_directory(player_directory)) {
            //ERRO
            return ERROR;
        }
    }
    memset(time_str, 0, sizeof(time_str));
    sprintf(time_str, "%04d%02d%02d_%02d%02d%02d",
             current_time->tm_year + 1900, current_time->tm_mon + 1, current_time->tm_mday,
             current_time->tm_hour, current_time->tm_min, current_time->tm_sec);

    // define new path of the file
    char new_file_name[256];
    memset(new_file_name, 0, sizeof(new_file_name));
    sprintf(new_file_name, "%s/%s_%c.txt", player_directory, time_str, finisher_mode);
    fs::path new_path = new_file_name;

    fs::path old_path = file_name;

    // rename file path and the player filename and move it to the new directory (./server/GAMES/PLID/file_name.txt)
    fs::rename(old_path, new_path);

    return 1;
}



int handlerQuitCommand(const char* plid, char* response_buffer) {

    FILE* file;
    char file_name[64];

    if(!validPLID(plid)) {
        sprintf(response_buffer, "RQT ERR\n");
        return ERROR;
    }
    if(!gameOn(plid)) {
        sprintf(response_buffer, "RQT NOK\n");      // not in a game, cant quit
        return 0;
    }
    if(timeExceeded(plid)) {
        endGame(plid, TIMEOUT);
        sprintf(response_buffer, "RQT NOK\n");      // not in a game, already timed out
        return 0;
    }
    endGame(plid, QUIT);
    sprintf(response_buffer, "RQT OK\n");           // game on, quit game
    return 1;
}



int startGame(const char* plid, const char* max_playtime, const char* mode, const char* c1, const char* c2, const char* c3, const char* c4, char* response_buffer) {

    time_t fulltime;                                         
    struct tm* currentTime;
    char file_line[128], file_name[64], time_str[20], buffer[128];
    char color_code[5]; // 4 chars + null terminator
    
    memset(response_buffer, 0, sizeof(response_buffer));
    if(!strcmp(mode, "P")) {sprintf(response_buffer, "RSG ");}   // normal game mode
    else {sprintf(response_buffer, "RDB ");}                     // debug game mode

    if(!validPLID(plid) || !verifyMaxPlaytime(max_playtime)) {
        sprintf(response_buffer + 4, "ERR\n");
        return ERROR;
    }
    if(!strcmp(mode, "D") && (!validColor(c1) || !validColor(c2) || !validColor(c3) || !validColor(c4))) {
        sprintf(response_buffer + 4, "ERR\n");
        return ERROR;
    }
    // define file name
    memset(file_name, 0, sizeof(file_name));
    sprintf(file_name, "./server/GAMES/GAME_%s.txt", plid);

    FILE* file = fopen(file_name, "r");     // open file in read mode
    if(file) {
        memset(file_line, 0, sizeof(file_line));
        if(fgets(file_line, sizeof(file_line), file) == NULL) {return ERROR;}   // get header of the file

        fgets(file_line, sizeof(file_line), file);
        fclose(file);
        // checks if the game already has tries
        if (file_line != NULL) {
            if (!timeExceeded(plid)) {
                sprintf(response_buffer + 4, "NOK\n");
                return 0;
            }
        }
    }
    memset(color_code, 0, sizeof(color_code));      // clear color_code
    if (!strcmp(mode, "P")) {                       // normal game
        generateColorCode(color_code);              // generates the color code
    }
    else {  // debug mode --> use color code requested by user
        sprintf(color_code, "%s%s%s%s", c1, c2, c3, c4);
    }

    time(&fulltime);                // Current time in seconds since 1970
    currentTime = gmtime(&fulltime);
    long initialTime = fulltime;    // Time in seconds sin 1970

    memset(time_str, 0, sizeof(time_str));
    sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d",
             currentTime->tm_year + 1900, currentTime->tm_mon + 1, currentTime->tm_mday,
             currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);
    
    // Create the file
    file = fopen(file_name, "w");
    if (!file) {return ERROR;}      // ERROR a abrir ficheiro

    // create first line of the file
    char first_file_line[128];
    memset(first_file_line, 0, sizeof(first_file_line));
    sprintf(first_file_line, "%s %s %s %s %s %ld", plid, mode, color_code, max_playtime, time_str, initialTime);
    fprintf(file, "%s\n", first_file_line);

    sprintf(response_buffer + 4, "OK\n");
    fclose(file);
    return 1;
}


int testTrial(const char* plid, const char* c1, const char* c2, const char* c3, const char* c4, const char* new_trial_number, char* response_buffer) {

    if(!validPLID(plid) || !validColor(c1) || !validColor(c2) || !validColor(c3) || !validColor(c4)){
        sprintf(response_buffer, "RTR ERR\n");
        return ERROR;
    }
    if (!gameOn(plid)) {    // does not have a game going
        sprintf(response_buffer, "RTR NOK\n");
        return 0;
    }
    if (timeExceeded(plid)) {
        endGame(plid, TIMEOUT); // TESTAR ERROS
        sprintf(response_buffer, "RTR ETM\n");
        return 0;
    }

    char old_guess[5], new_guess[5], file_name[64];
    char line[256]; // Buffer para armazenar cada linha lida do ficheiro
    int trials = 0;
    bool duplicated = false;

    sprintf(new_guess, "%s%s%s%s", c1, c2, c3, c4);

    memset(file_name, 0, sizeof(file_name));
    sprintf(file_name, "./server/GAMES/GAME_%s.txt", plid);
    FILE* file = fopen(file_name, "r");         // Opens the file in read mode

    fgets(line, sizeof(line), file);            // ignore first line
    while (fgets(line, sizeof(line), file)) {   // while there's more lines in the file
        trials++;                               // count how many trials have been done
        sscanf(line, "T: %4s ", old_guess);     // get color code of the trial
        if (!strcmp(old_guess, new_guess)) {    // check if there's a duplicated guess
            duplicated = true;
        }
    }
    fclose(file); // Closes file
    
    if(std::atoi(new_trial_number) == trials && !strcmp(old_guess, new_guess)) { 
        sprintf(response_buffer, "RTR OK\n");   // resend
        return SUCCESS;
    }
    else if (std::atoi(new_trial_number) != trials + 1) {                           
        sprintf(response_buffer, "RTR INV\n");  // invalid trial number
        return ERROR;
    }
    else if (duplicated) {                                                                   
        sprintf(response_buffer, "RTR DUP\n");  // duplicated guess
        return ERROR;
    }


    // valid trial, test it
    char color_code[5] = "RRRR";
    char guess[5];
    int verified_colors[4] = {false, false, false, false};
    int nB = 0, nW = 0;

    sprintf(guess, "%s%s%s%s", c1, c2, c3, c4);

    for (int i = 0; i < 4; i++) {
        if (guess[i] == color_code[i]) {
            verified_colors[i] = true;
            nB++;
        }
    }
    for (int i = 0; i < 4; i++) {
        if (!verified_colors[i]) {
            for(int j = 0; j < 4; j++) {
                if (i != j && !verified_colors[j] && guess[i] == color_code[j]) {
                    nW++;
                    verified_colors[j] = true;
                    break;
                }
            }
        }
    }
    if (nB == 4) {
        endGame(plid, 'W');
    }
    else if (trials == 7){
        endGame(plid, 'F');
    }
    sprintf(response_buffer + 4, "OK %s %s %s\n", new_trial_number, nB, nW);    // create response to player

    char trial_line[128];
    memset(trial_line, 0, sizeof(trial_line));
    int time_passed = getTimePassed(plid);
    sprintf(trial_line, "T: %s%s%s%s", c1, c2, c3, c4, nB, nW, time_passed);    // create trial line

    // Append new try to the game file
    FILE* file = fopen(file_name, "a");
    if (!file) {return ERROR;}      // ERROR a abrir ficheiro
    fprintf(file, "%s\n", trial_line);
    fclose(file);
    return 1;
}


int resolveUDPCommands(const char* input, char* response_buffer) {

    int num_args;
    char cmd[32], arg1[32], arg2[32], arg3[32], arg4[32], arg5[32], arg6[32];

    num_args = sscanf(input, "%s %s %s %s %s %s %s\n", cmd, arg1, arg2, arg3, arg4, arg5, arg6);

    switch(num_args) {
        
        case 2:
            if (!strcmp(cmd, "QUT")) {
                /* if(!endGame()) {
                    return 0;
                } */
               ;
            }
            else {
                std::cerr << "Communication error.\n";
            }
            break;
            
        case 3:
            if (!strcmp(cmd, "SNG")){
                if(!startGame(arg1, arg2, "P", arg3, arg4, arg5, arg6, response_buffer)){
                    return 0; // nao vai haver returns em principio, só se houver erros que seja suposto mandar o programa abaixo
                }
            }
            else {
                std::cerr << "Communication error.\n";
            }
            break;

        case 7:
            if (!strcmp(cmd, "DBG")){
                if(!startGame(arg1, arg2, "D", arg3, arg4, arg5, arg6, response_buffer)){
                    return 0; // nao vai haver returns em principio, só se houver erros que seja suposto mandar o programa abaixo
                }
            }
            else if (!strcmp(cmd, "TRY")) {
                if(!testTrial(arg1, arg2, arg3, arg4, arg5, arg6, response_buffer)){
                    return 0; // nao vai haver returns em principio, só se houver erros que seja suposto mandar o programa abaixo
                }
            }
            else {
                std::cerr << "Communication error.\n";
            }
            break;            
    }
    return 0;
}


int main(int argc, char* argv[]) {

    int mode = NOT_VERBOSE, udp_socket, tcp_socket, n;
    struct addrinfo hints, *res, *p;
    struct sockaddr_in addr;
    socklen_t addrlen;
    const char* port;
    char buffer[128];

    int out_fds;
    struct timeval timeout;
    fd_set fd_sockets;

    if (argc > 1 && argv[1] != NULL) {
        port = argv[1];
    } else {
        port = PORT;
    }
    if (argc > 2 && argv[2] != NULL) {
        mode = VERBOSE;
    }

    // Criar socket UDP
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1) {
        perror("Error in creating UDP Socket");
        exit(1);
    }

    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(tcp_socket == -1) {
        perror("Error in creating TCP Socket");
        exit(1);
    }

    // Configuração dos hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = 0; // Socket UDP
    hints.ai_flags = AI_PASSIVE;    // Aceitar conexões

    // Obter informações do endereço
    int errcode = getaddrinfo(NULL, port, &hints, &res);
    if (errcode != 0) {
        std::cerr << "Getaddrinfo: " << gai_strerror(errcode) << std::endl;
        exit(1);
    }

    /*
    No for abaixo passa-se o seguinte: o getaddrinfo, se não passares nenhuma hints.ai_socktype (está a 0),
    ele altera o res para ser uma linkedlist com as sockets possíveis. Não sei que sockets ele disponibiliza
    para além das que queremos, por isso temos de testar, mas desta forma é a que faz o código
    mais legível.
    */

    for (p = res; p != NULL; p = p->ai_next) {
        if(p->ai_socktype == SOCK_DGRAM) {
            if(bind(udp_socket, p->ai_addr, p->ai_addrlen) == -1) {
                perror("UDP Socket bind error");
                close(udp_socket);
                close(tcp_socket);
                exit(1);
            }
        }
        else if(p->ai_socktype == SOCK_STREAM) {
            if(bind(tcp_socket, p->ai_addr, p->ai_addrlen) == -1) {
                perror("TCP Socket bind error");
                close(udp_socket);
                close(tcp_socket);
                exit(1);
            }
            if(listen(tcp_socket, 5) == -1) {
                perror("Error while trying to listen to TCP Socket");
                close(udp_socket);
                close(tcp_socket);
                exit(1);
            }
        }
    }

    //adicionar as sockets ao set

    FD_ZERO(&fd_sockets);
    FD_SET(udp_socket, &fd_sockets);
    FD_SET(tcp_socket, &fd_sockets);

    while(true) {
        fd_set test_fd_sockets = fd_sockets; //o select altera o fd_sockets, por isso temos de enviar uma cópia
        memset((void*)&timeout, 0, sizeof(timeout)); //not sure se é preciso timeout, mas estava no ficheiro do stor
        timeout.tv_sec = 10;
        out_fds = select(FD_SETSIZE, &test_fd_sockets, (fd_set*) NULL, (fd_set*)NULL, (struct timeval*) &timeout);
        switch(out_fds) {
            case ERROR:
                perror("Error in Select function");
                exit(1);
            case 0:
                printf("TIMEOUT\n");
                break;
            default:
                addrlen = sizeof(addr);
                if(FD_ISSET(udp_socket, &test_fd_sockets)) {
                    n = recvfrom(udp_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addrlen);
                    if (n == -1) {
                        perror("recvfrom");
                        exit(1);
                    }
                    // Exibir a mensagem recebida
                    write(1, "received: ", 10);
                    write(1, buffer, n);
                    char response_buffer[128];
                    resolveUDPCommands(buffer, response_buffer);
                    

                    // Enviar resposta para o cliente
                    if (sendto(udp_socket, response_buffer, strlen(response_buffer), 0, (struct sockaddr*)&addr, addrlen) == ERROR) {
                        perror("sendto");
                        exit(1);
                    }
                }

                if(FD_ISSET(tcp_socket, &test_fd_sockets)) {
                    int new_fd = accept(tcp_socket, (struct sockaddr*) &addr, &addrlen);
                    if(new_fd == -1) {
                        perror("TCP Socket accept error");

                        exit(1);
                    }
                    /* n = read(new_fd, buffer, 128);
                    if(n == -1) {
                        perror("read");
                        exit(1);
                    }
                    write(1, "received: ", 10);
                    write(1, buffer, n);
                    n = write(new_fd, "RST ACT 101101_game.txt 62\n\t1 - R R P P nB=1, nW=0\n\t2 - R G G B nB=2, nW=1 - 73 s to go!\n", 
                        strlen("RST ACT 101101_game.txt 62\n\t1 - R R P P nB=1, nW=0\n\t2 - R G G B nB=2, nW=1 - 73 s to go!\n"));
                    if(n == -1) {
                        perror("write");
                        exit(1);
                    } */
                    
                    close(new_fd);
                }
        }

    }
    freeaddrinfo(res);
    close(udp_socket);
    close(tcp_socket);

    return 0;

}



    /* // Associar socket ao endereço
    if (bind(udp_socket, res_udp_socket->ai_addr, res_tcp_socket->ai_addrlen) == -1) {
        perror("bind");
        exit(1);
    }

    while (true) {
        addrlen = sizeof(addr);
        // Esperar por um datagrama
        n = recvfrom(udp_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addrlen);
        if (n == -1) {
            perror("recvfrom");
            exit(1);
        }

        // Exibir a mensagem recebida
        write(1, "received: ", 10);
        write(1, buffer, n);

        // Enviar resposta para o cliente
        if (sendto(udp_socket, buffer, n, 0, (struct sockaddr*)&addr, addrlen) == -1) {
            perror("sendto");
            exit(1);
        }
    }

    // Limpar recursos
    freeaddrinfo(res);
    close(udp_socket);

    return 0;
}
 */


// TESTE ---------