#include <iostream>
#include <cstring>

#include "commands.h"
#include "../constants.h"

int main(int argc, char* argv[]) {

    int num_args, nT, active_game = false;
    const char *sv_ip, *port;

    switch (argc) {
        case 1:
            sv_ip = SV_IP;
            port = PORT;
            break;

        case 3:
            if (std::string(argv[1]) == "-n") {
                sv_ip = argv[2];
            } else if (std::string(argv[1]) == "-p") {
                port = argv[2];
            } else {
                std::cerr << "ERROR: failed to write arguments, the correct structure is as follows: ./player [-n GSIP] [-p GSport]\n";
                exit(1);
            }
            break;

        case 5:
            if (std::string(argv[1]) == "-n" && std::string(argv[3]) == "-p") {
                sv_ip = argv[2];
                port = argv[4];
            } else {
                std::cerr << "ERROR: failed to write arguments, the correct structure is as follows: ./player [-n GSIP] [-p GSport]\n";
                exit(1);
            }
            break;

        default:
            std::cerr << "ERROR: failed to write arguments, the correct structure is as follows: ./player [-n GSIP] [-p GSport]\n";
            exit(1);
    }


    //No while, falta verificar mensagens de ERR, não sei se é suposto acabar o programa ou só dizer que foi um return invalido
    std::cout << "\nHELLO Good sir! I see you want to play the MASTERMIND game.\nLets hop right into it!\n";
    std::cout << "----------------------------------\n";
    while(true) {

        std::string input;
        char plid[32], cmd[32], arg1[32], arg2[32], arg3[32], arg4[32], arg5[32], arg6[32], extra[32];
        std::cout << "\nCOMMAND -> ";
        std::getline(std::cin, input);

        num_args = sscanf(input.c_str(), "%s %s %s %s %s %s %s %s\n", cmd, arg1, arg2, arg3, arg4, arg5, arg6, extra);
        switch (num_args) {
            case 1:

                if (!strcmp(cmd, "quit")){
                    if (active_game) {
                        if(commandEndGame(sv_ip, port, plid) == SUCCESS) {
                            active_game = false;
                            nT = 1;
                        }
                        else {
                            exit(1);
                        }
                    }
                    else {
                        std::cout << "You do not have an ongoing game\n";
                    }
                
                }
                else if (!strcmp(cmd, "exit")){
                    if(active_game) {
                        commandEndGame(sv_ip, port, plid);
                    }
                    return 0;
                }
                else if (!strcmp(cmd, "st") || !strcmp(cmd, "show_trials")) {
                    if (strlen(plid) > 0) {
                        commandShowTrials(sv_ip, port, plid);
                    } else {
                        std::cout << "You do not have an ongoing game\n";
                    }
                }
                else if (!strcmp(cmd, "sb") || !strcmp(cmd, "scoreboard")) {
                    commandScoreboard(sv_ip, port);
                }
                else {
                    std::cout << "ERROR: Invalid command. Check cmd or number of arguments\n";
                }
                break;

            case 3:
                if(!strcmp(cmd, "start")) {
                    if(commandStartGame(sv_ip, port, arg1, arg2) == SUCCESS){ 
                        active_game = true;
                        nT = 1;
                        strncpy(plid, arg1, PLID_SIZE);
                    }
                }
                else {
                    std::cerr << "ERROR: Invalid command. Check cmd or number of arguments\n";
                }
                break;

            case 5:
                if(!strcmp(cmd, "try")) {
                    if (active_game) {
                        int try_status = commandTry(sv_ip, port, arg1, arg2, arg3, arg4, &nT, plid);
                        if (try_status == GAME_WON || try_status == GAME_END) {          
                            active_game = false;
                        }

                    } else {
                        std::cout << "You do not have an ongoing game\n";
                    }
                }
                else {
                    std::cerr << "ERROR: Invalid command. Check cmd or number of arguments\n";
                }
                break;

            case 7:
                if(!strcmp(cmd, "debug")) {
                    if(commandDebug(sv_ip, port, arg1, arg2, arg3, arg4, arg5, arg6)) {
                        active_game = true;
                        nT = 1;
                        strncpy(plid, arg1, PLID_SIZE);
                    }
                }   
                else {
                    std::cerr << "ERROR: Invalid command. Check cmd or number of arguments\n";
                } 
                break;

            default:
                std::cerr << "ERROR: Invalid command. Check cmd or number of arguments\n";
        }
    }
    return 0;
}