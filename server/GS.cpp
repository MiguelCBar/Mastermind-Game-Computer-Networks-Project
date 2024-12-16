#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cctype>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <fstream>
#include <ctime>
#include <iomanip>

#include "utils.h"

#define PORT "58058" //58000 + GN, where GN is 58
#define NOT_VERBOSE 2
#define VERBOSE 3
#define COLOR_NUMBER 6


//provavelmente, falta mais close(tcp_socket) e close(udp_socket), nos erros


// Gera um código de cores no formato "RRRR"
void generateColorCode(char* color_code) {
    const char colors[] = "RGBYOP";
    for (int i = 0; i < 4; i++) {
        color_code[i] = colors[rand() % COLOR_NUMBER];
    }
    color_code[4] = '\0'; // Add the '\0' character
}


int startGame(const char* plid, const char* max_playtime, const char* mode, const char* c1, const char* c2, const char* c3, const char* c4, char* response_buffer) {

    time_t fulltime;                                         
    struct tm* currentTime;
    char first_file_line[128], file_name[64], time_str[20];
    char color_code[5]; // 4 chars + null terminator

    if(!verifyPLID(plid) || !verifyMaxPlaytime(max_playtime)) {
        sprintf(response_buffer, "RSG ERR\n");
        return ERROR;
    }

    else if(!strcmp(mode, "D") && (!verifyColor(c1) || !verifyColor(c2) || !verifyColor(c3) || !verifyColor(c4))) {
        sprintf(response_buffer, "RSG ERR\n");
        return ERROR;
    }



    // File name
    sprintf(file_name, "./server/GAMES/GAME_%s.txt", plid);

    // Tenta abrir o arquivo no modo de leitura
    FILE* file = fopen(file_name, "r");
    if (file) {
        char existing_plid[6];
        // Arquivo existe; verificar o `plid`
        if (fscanf(file, "%s", existing_plid) == 1) {
            if (!strcmp(existing_plid, plid)) {
                // game with plid already on course, close arquive and return
                fclose(file);
                printf("Game on course with same PLID: %s\n", existing_plid);
                return 0; // Ou um código específico para indicar que não foi criado
            }
        }
        fclose(file);
    }   // file doesnt exist, start a new game

    //memset(color_code, 0, sizeof(color_code));

    if (strcmp(mode, "P") == 0) {
        generateColorCode(color_code);
    }
    else {
        sprintf(color_code, "%s%s%s%s", c1, c2, c3, c4);
    }
    //printf(" ------------------- color code: %s\n", color_code);

    time(&fulltime);            // Current time in seconds since 1970
    currentTime = gmtime(&fulltime);

    long momentoInicio = fulltime;  // Time in seconds sin 1970

    sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d",
             currentTime->tm_year + 1900, currentTime->tm_mon + 1, currentTime->tm_mday,
             currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);
    
    // Create the file
    file = fopen(file_name, "w");
    if (!file) {
        perror("Erro ao criar ficheiro");
        return -1;
    }

    // First line of the file
    sprintf(first_file_line, "%s %s %s %s %s %ld", plid, mode, color_code, max_playtime, time_str, momentoInicio);
    fprintf(file, "%s\n", first_file_line);

    // Close the file
    fclose(file);
    
    return 1;
}


int testTrial(const char* plid, const char* c1, const char* c2, const char* c3, const char* c4, const char* trial_number) {

    // testar a trial
    // ler do ficheiro o codigo suposto de acertar
    
    /* if(!verifyColor(c1) || !verifyColor(c2) || !verifyColor(c3) || !verifyColor(c4)) {
        std::cerr << "Incorrect color Code. Possible colors are:\nR -> Red\nG -> Green\nB -> Blue\nY -> Yellow\nO -> Orange\nP -> purple\n";
        return 0;
    } */
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

    // escrever msg no pointer que veio como argumento

    return 1;
}


int resolveUDPCommands(const char* input, char* response_buffer) {

    int num_args;
    char plid[32], cmd[32], arg1[32], arg2[32], arg3[32], arg4[32], arg5[32], arg6[32];

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
            

        case 3:
            if (!strcmp(cmd, "SNG")){
                if(!startGame(arg1, arg2, "P", arg3, arg4, arg5, arg6)){
                    return 0; // nao vai haver returns em principio, só se houver erros que seja suposto mandar o programa abaixo
                }
            }
            else {
                std::cerr << "Communication error.\n";
            }
            break;

        case 7:
            if (!strcmp(cmd, "DBG")){
                if(!startGame(arg1, arg2, "D", arg3, arg4, arg5, arg6)){
                    return 0; // nao vai haver returns em principio, só se houver erros que seja suposto mandar o programa abaixo
                }
            }
            else if (!strcmp(cmd, "TRY")) {
                if(!testTrial(arg1, arg2, arg3, arg4, arg5, arg6)){
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
            case -1:
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
                    if (sendto(udp_socket, buffer, n, 0, (struct sockaddr*)&addr, addrlen) == -1) {
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
                    n = read(new_fd, buffer, 128);
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
                    }
                    
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