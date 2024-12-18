#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

const char SV_IP[10] = "127.0.0.1";
const char PORT[6] = "58058"; //58000 + GN, where GN is 58
const int NOT_VERBOSE = 2;
const int VERBOSE = 3;
const int COLOR_NUMBER = 6;
const int ERROR = -1;

const int PLID_SIZE = 6;
const int GAME_WON = 1;
const int HEADER_SIZE = 64;
const int MAX_FILE_SIZE = 2048;

const char WIN = 'W';
const char FAIL = 'F';
const char QUIT = 'Q';
const char TIMEOUT = 'T';

#endif