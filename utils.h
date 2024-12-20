#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>

bool validPLID(const std::string& input);
bool verifyMaxPlaytime(const std::string& input);
bool validColor(const std::string& color);
void generateColorCode(char* color_code);
int getTimePassed(const char* header);
int timeExceeded(const char* header);
int transcriptOngoingGameFile(const char* file_path, const char* header, char* response_buffer);
int getGameHeader(const char* file_path, char* header);
void getKeyColorCode(const char* header, char* color_code);
void displayColorCode(const char* color_code, char* spaced_color_code);
int FindLastGame(char *PLID, char *fname);
#endif