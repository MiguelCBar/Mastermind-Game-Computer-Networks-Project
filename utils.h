#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>

bool validPLID(const std::string& input);
bool containsChar(const char* buffer, size_t size, char target);
bool verifyMaxPlaytime(const std::string& input);
bool parseFileHeader(const std::string& response_buffer, ssize_t* file_size, char* cmd, char* status, char* file_name, ssize_t* headersize);
bool validColor(const std::string& color);
void generateColorCode(char* color_code);
bool gameOn(const char* plid);

#endif