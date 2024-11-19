#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define GN "0"
#define PORT "58012"
#define SV_IP "lima.ist.utl.pt"

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


int main() {
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[512], command[128], istID[128], max_time[128];
    std::string input;

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

    std::cout << "Digite o comando, número de 6 dígitos e número de 3 dígitos: ";
    std::getline(std::cin, input); // Captura a entrada do terminal como string


    // Analisa a string
    if (sscanf(input.c_str(), "%s %s %s", command, istID, max_time) == 3) {
        std::cout << "Comando: " << command << "\n";
        std::cout << "Número 1: " << istID << "\n";
        std::cout << "Número 2: " << max_time << "\n";
    } else {
        std::cerr << "Erro ao analisar a entrada.\n";
    }
    
    // Get address info
    errcode = getaddrinfo(SV_IP, PORT, &hints, &res);
    if (errcode != 0) {
        std::cerr << "Error in getaddrinfo: " << gai_strerror(errcode) << "\n";
        close(fd);
        return 1;
    }

    sprintf(buffer,"%s %s %s\n", "SNG", istID, max_time);
    // Send data
    printf("input: %s\n", buffer);
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
    // Clean up
    freeaddrinfo(res);
    close(fd);

    return 0;
}
