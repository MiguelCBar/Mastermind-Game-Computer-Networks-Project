#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>

bool verifyPLID(const std::string& input);
bool verifyMaxPlaytime(const std::string& input);
bool parseFileHeader(const std::string& response_buffer, ssize_t* file_size, char* cmd, char* status, char* file_name, ssize_t* headersize);
bool verifyColor(const std::string& color);



#endif