#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cctype>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT "58001"
#define NOT_VERBOSE 2
#define VERBOSE 3


int main(int argc, char* argv[]) {

    int mode = NOT_VERBOSE, fd, n;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    socklen_t addrlen;
    const char* port;
    char buffer[128];

    if (argv[1] != NULL) {
        port = argv[1];
    } else {
        port = PORT;
    }
    if (argv[2] != NULL) {
        mode = VERBOSE;
    }

    // Criar socket UDP
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(1);
    }

    // Configuração dos hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_DGRAM; // Socket UDP
    hints.ai_flags = AI_PASSIVE;    // Aceitar conexões

    // Obter informações do endereço
    int errcode = getaddrinfo(NULL, port, &hints, &res);
    if (errcode != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(errcode) << std::endl;
        exit(1);
    }

    // Associar socket ao endereço
    if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind");
        exit(1);
    }

    while (true) {
        addrlen = sizeof(addr);
        // Esperar por um datagrama
        n = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addrlen);
        if (n == -1) {
            perror("recvfrom");
            exit(1);
        }

        // Exibir a mensagem recebida
        write(1, "received: ", 10);
        write(1, buffer, n);

        // Enviar resposta para o cliente
        if (sendto(fd, buffer, n, 0, (struct sockaddr*)&addr, addrlen) == -1) {
            perror("sendto");
            exit(1);
        }
    }

    // Limpar recursos
    freeaddrinfo(res);
    close(fd);

    return 0;
}
