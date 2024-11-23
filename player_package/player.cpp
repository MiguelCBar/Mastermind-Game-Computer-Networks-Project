#include "commands.cpp"

int main(int argc, char* argv[]) {

    int num_args, nT;
    const char *sv_ip, *port;
    char plid[32], cmd[32], arg1[32], arg2[32], arg3[32], arg4[32], arg5[32], arg6[32];
    std::string input;

    /* if (argc > 1 && argv[1] != NULL) {
        sv_ip = argv[1];
    } else {
        sv_ip = SV_IP;
    }
    if (argc > 2 && argv[2] != NULL) {
        port = argv[2];
    } else {
        port = PORT;
    } */

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