#ifndef _COMMANDS_H_
#define _COMMANDS_H_

int commandEndGame(const char* arg1, const char* arg2, const char* arg3);
int commandShowTrials(const char* sv_ip, const char* port, const char* plid);
int commandScoreboard(const char* arg1, const char* arg2);
int commandStartGame(const char* arg1, const char* arg2, const char* arg3, const char* arg4);
int commandTry(const char* arg1, const char* arg2, const char* arg3, const char* arg4, const char* arg5, const char* arg6, int* arg7, const char* arg8);
int commandDebug(const char* arg1, const char* arg2, const char* arg3, const char* arg4, const char* arg5, const char* arg6, const char* arg7, const char* arg8);

#endif
