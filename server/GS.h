#ifndef _GS_H_
#define _GS_H_

int endGame(const char* plid, const char finisher_mode);
int handlerQuitCommand(const char* plid, char* response_buffer);
int handlerStartCommand(const char* plid, const char* max_playtime, const char* game_mode, const char* c1, const char* c2, const char* c3, const char* c4, char* response_buffer);
int makeNewTrial (const char* file_name, const char* header, const char* color_code, const char* guess, int trial_number, int* nB, int* nW);
int handlerTryCommand(const char* plid, const char* c1, const char* c2, const char* c3, const char* c4, const char* new_trial_number, char* response_buffer);
int handlerShowTrialsCommand(char* plid, char* response_buffer);
int handlerScoreboardCommand(char* response_buffer);
int resolveUDPCommands(const char* input, char* response_buffer);
int resolveTCPCommands(const char* input, char* response_buffer);




#endif