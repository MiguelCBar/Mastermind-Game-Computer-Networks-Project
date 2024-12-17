#include "utils.h"
#include "constants.h"
#include <fstream>

bool verifyPLID(const std::string& input) {
    // Verifica se tem exatamente 6 caracteres e todos são dígitos
    if (input.length() == 6) {
        for (char c : input) {
            if (!std::isdigit(c)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool verifyMaxPlaytime(const std::string& input) {
    if(input.length() <= 3) {
        for(char c : input) {
            if(!std::isdigit(c)) {
                return false;
            }
        }
        if(std::stoi(input) > 600)
            return false;
        return true;
    }
    return false;
}


bool containsChar(const char* buffer, size_t size, char target) {
    for (size_t i = 0; i < size; ++i) {
        if (buffer[i] == target) {
            return true;
        }
    }
    return false;
}

bool parseFileHeader(const std::string& response_buffer, ssize_t* file_size, char* cmd, char* status, char* file_name, ssize_t* headersize) {
    char f_size[32];
    int num_args;
    printf("response_buffer: %s\n", response_buffer);
    (*headersize) = response_buffer.find('\n');
    if(*headersize != std::string::npos) {
        sscanf(response_buffer.c_str(), "%s %s %s %s", cmd, status, file_name, f_size);
        /* if(num_args != 4 ) {
            return false;
        } */
        *file_size = std::strtol(f_size, nullptr, 10);
        (*headersize)++;
        return true;
    }
    //printf("ta mal: %s\n", c);
    return false;
}

bool verifyColor(const std::string& color) {
    std::string validColors = "RGBYOP";
    return color.length() == 1 && validColors.find(color) != std::string::npos;
}

bool gameOn(const char* plid) {

        char file_name[64];
        sprintf(file_name, "./server/GAMES/GAME_%s.txt", plid);
        FILE* file = fopen(file_name, "r"); // Opens the file in read mode

        if (file) {
            fclose(file); // Closes file
            return true;  // File exists --> Game exists
        } else {
            return false; // O ficheiro não existe
        }
}


void generateColorCode(char* color_code) {
    const char colors[] = "RGBYOP";
    for (int i = 0; i < 4; i++) {
        color_code[i] = colors[rand() % COLOR_NUMBER];
    }
    color_code[4] = '\0'; // Add the '\0' character
}

