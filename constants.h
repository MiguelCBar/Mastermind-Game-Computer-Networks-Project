#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

const int ERROR = -1;

// DEFAULT CONNECTION
const char SV_IP[10] = "127.0.0.1";
const char PORT[6] = "58058"; //58000 + GN, where GN is 58

// SERVER RELATED
const int NOT_VERBOSE = 2;
const int VERBOSE = 3;

// GAME RELATED
const int SUCCESS = 1;
const int GAME_WON = 2;
const int GAME_END = 3;
const int GAME_ON = 4;

const int EMPTY = 5;

// SIZES
const int COLOR_CODE_SIZE = 4 + 1;
const int COLOR_NUMBER = 6;
const int PLID_SIZE = 6;
const int HEADER_SIZE = 64;
const int MAX_FILE_SIZE = 2048;
const int MAX_FILE_NAME = 128;

// GAME END STATUS
const char WIN = 'W';
const char FAIL = 'F';
const char QUIT = 'Q';
const char TIMEOUT = 'T';

#endif