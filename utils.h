#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include "constants.h"

struct FileInfo {
    int score;
    char filepath[MAX_FILE_NAME];
};

bool validPLID(const std::string& input);
bool verifyMaxPlaytime(const std::string& input);
bool validColor(const std::string& color);
void generateColorCode(char* color_code);
int getTimePassed(const char* header);
int timeExceeded(const char* header);
size_t containsChar(const char* buffer, size_t size, char target);
int transcriptOngoingGameFile(const char* file_path, const char* header, char* response_buffer);
int transcriptFinishedGameFile(const char* file_path, const char* header, char* response_buffer);
int getGameHeader(const char* file_path, char* header);
void getKeyColorCode(const char* header, char* color_code);
void displayColorCode(const char* color_code, char* spaced_color_code);
int FindLastGame(char *PLID, char *fname);
int processScores(const char* directory, char* file_data);
int calculateGameScore(int game_duration, int tries_count);
void transcriptGameScoreFile(const char* line, char* file_data);

#endif