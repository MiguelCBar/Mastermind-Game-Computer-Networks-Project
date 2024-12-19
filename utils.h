#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>

bool validPLID(const std::string& input);
size_t containsChar(const char* buffer, size_t size, char target);
bool verifyMaxPlaytime(const std::string& input);
bool validColor(const std::string& color);
void generateColorCode(char* color_code);
bool gameOn(const char* plid);
int getTimePassed(const char* plid);
int timeExceeded(const char* plid);
int transcriptShowTrialsFile(const char* file_path, char* response_buffer);

#endif