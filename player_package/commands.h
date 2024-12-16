#ifndef _COMMANDS_H_
#define _COMMANDS_H_

int end_game(const char* arg1, const char* arg2, const char* arg3);
int show_trials(const char* sv_ip, const char* port, const char* plid);
int scoreboard(const char* arg1, const char* arg2);
int start_game(const char* arg1, const char* arg2, const char* arg3, const char* arg4);
int try_command(const char* arg1, const char* arg2, const char* arg3, const char* arg4, const char* arg5, const char* arg6, int* arg7, const char* arg8);
int debug_command(const char* arg1, const char* arg2, const char* arg3, const char* arg4, const char* arg5, const char* arg6, const char* arg7, const char* arg8);

#endif
