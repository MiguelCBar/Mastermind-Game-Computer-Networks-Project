#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cctype>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SV_IP "127.0.0.1"
#define PORT "58001"
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

int  end_game(const char* sv_ip, const char* port, const char* plid) {

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
    std::cout << "Comando: show_trials\n";
    std::cout << "sv_ip: " << sv_ip << "\n";
    std::cout << "port: " << port << "\n";
    // mostrar trials (TCP)
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


int main(int argc, char* argv[]) {

    int num_args, nT;
    const char *sv_ip, *port;
    char plid[32], cmd[32], arg1[32], arg2[32], arg3[32], arg4[32], arg5[32], arg6[32];
    std::string input;

    if (argc > 1 && argv[1] != NULL) {
        sv_ip = argv[1];
    } else {
        sv_ip = SV_IP;
    }
    if (argc > 2 && argv[2] != NULL) {
        port = argv[2];
    } else {
        port = PORT;
    }

    while(true) {
        //le comando
        std::cout << "comando: ";
        std::getline(std::cin, input); // Captura a entrada do terminal como string

        num_args = sscanf(input.c_str(), "%s %s %s %s %s %s %s", cmd, arg1, arg2, arg3, arg4, arg5, arg6);

        switch (num_args) {
            case 1:

                if (strcmp(cmd, "quit") == 0){
                    if(!end_game(sv_ip, port, plid)){
                        return 0; // nao vai haver returns em principio, só se houver erros que seja suposto mandar o programa abaixo
                    }
                nT = 1;
                }
                else if (strcmp(cmd, "exit") == 0){
                    end_game(sv_ip, port, plid);
                    return 0; //sair do programa
                }
                else if (strcmp(cmd, "st") == 0 or strcmp(cmd, "show_trials") == 0) {
                    if(!show_trials(sv_ip, port)){
                        return 0; // nao vai haver returns em principio, só se houver erros que seja suposto mandar o programa abaixo
                    }
                }
                else if (strcmp(cmd, "sb") == 0 or strcmp(cmd, "scoreboard") == 0) {
                    if(!scoreboard(sv_ip, port)){
                        return 0; // nao vai haver returns em principio, só se houver erros que seja suposto mandar o programa abaixo
                    }
                }
                else {
                    printf("comando inválido\n");
                }
                break;

            case 3:
                if(!start_game(sv_ip, port, arg1, arg2)){
                    return 0; // nao vai haver returns em principio, só se houver erros que seja suposto mandar o programa abaixo
                }
                nT = 1;
                strncpy(plid, arg1, PLID_SIZE);
                //printf("plid: %s/n", plid);
                break;

            case 5:
                if(!try_command(sv_ip, port, arg1, arg2, arg3, arg4, nT, plid)) {
                    return 0; // nao vai haver returns em principio, só se houver erros que seja suposto mandar o programa abaixo
                }
                nT++;
                break;

            case 7:
                if(!debug_command(sv_ip, port, arg1, arg2, arg3, arg4, arg5, arg6)) {
                    return 0; // nao vai haver returns em principio, só se houver erros que seja suposto mandar o programa abaixo
                } break;

            default:
                // numero de argumentos inválido
                return 0;
        }
    }
    return 0;
}
