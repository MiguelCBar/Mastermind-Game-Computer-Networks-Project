#include <iostream>
#include <cstring>

#include "commands.h"

#define SV_IP "127.0.0.1"
#define PORT "58058" //58000 + GN, where GN is 58
#define PLID_SIZE 6
#define INACTIVE_GAME 0
#define ACTIVE_GAME 1

#include "commands.h"

int main(int argc, char* argv[]) {

    int num_args, nT;
    const char *sv_ip, *port;

    if (argc == 1) {
        sv_ip = SV_IP;
        port = PORT;
    }
    else if(argc == 3) {
        if(std::string(argv[1]) == "-n")
            sv_ip = argv[2];
        else if(std::string(argv[1]) == "-p")
            port = argv[2];
        else {
            std::cerr << "Error in writing arguments, the correct structure is as follows: ./player [-n GSIP] [-p GSport]\n";
            exit(1);
        }
    }
    else if(argc == 5) {
        if(std::string(argv[1]) == "-n" && std::string(argv[3]) == "-p") {
            sv_ip = argv[2];
            port = argv[4];
        }
        else {
            std::cerr << "Error in writing arguments, the correct structure is as follows: ./player [-n GSIP] [-p GSport]\n";
            exit(1); 
        }
    }
    else {
        std::cerr << "Error in writing arguments, the correct structure is as follows: ./player [-n GSIP] [-p GSport]\n";
        exit(1);
    }

    //No while, falta verificar mensagens de ERR, não sei se é suposto acabar o programa ou só dizer que foi um return invalido
    while(true) {

        int game_state = INACTIVE_GAME;

        std::string input;
        char plid[32], cmd[32], arg1[32], arg2[32], arg3[32], arg4[32], arg5[32], arg6[32];
        //le comando
        std::cout << "Command: ";
        std::getline(std::cin, input);

        num_args = sscanf(input.c_str(), "%s %s %s %s %s %s %s\n", cmd, arg1, arg2, arg3, arg4, arg5, arg6);
        switch (num_args) {
            case 1:

                if (!strcmp(cmd, "quit")){
                    if(end_game(sv_ip, port, plid)); {
                        game_state = INACTIVE_GAME;
                        nT = 1;
                    }
                }
                else if (!strcmp(cmd, "exit")){
                    end_game(sv_ip, port, plid);
                    return 0; //sair do programa
                }
                else if (!strcmp(cmd, "st") || !strcmp(cmd, "show_trials")) {
                    show_trials(sv_ip, port);
                }
                else if (!strcmp(cmd, "sb") || !strcmp(cmd, "scoreboard")) {
                    scoreboard(sv_ip, port);
                }
                else {
                    std::cout << "Invalid command. Check cmd or number of arguments\n";
                }
                break;

            case 3:
                if(!strcmp(cmd, "start")) {
                    if(start_game(sv_ip, port, arg1, arg2)){ 
                        nT = 1;
                        strncpy(plid, arg1, PLID_SIZE);
                    }
                }
                else {
                    std::cout << "Invalid command. Check cmd or number of arguments\n";
                }
                break;

            case 5:
                if(!strcmp(cmd, "try")) {
                    try_command(sv_ip, port, arg1, arg2, arg3, arg4, &nT, plid);
                }
                else {
                    std::cout << "Invalid command. Check cmd or number of arguments\n";
                }
                break;

            case 7:
                if(!strcmp(cmd, "debug")) {
                    if(debug_command(sv_ip, port, arg1, arg2, arg3, arg4, arg5, arg6, game_state)) {
                        nT = 1;
                        strncpy(plid, arg1, PLID_SIZE);
                    }
                }   
                else {
                    std::cout << "Invalid command. Check cmd or number of arguments\n";
                } 
                break;

            default:
                std::cout << "Invalid command. Check cmd or number of arguments\n";
        }
    }
    return 0;
}