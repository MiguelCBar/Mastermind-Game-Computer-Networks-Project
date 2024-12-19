#include "utils.h"
#include "constants.h"
#include <fstream>
#include <string.h>
#include <iostream>

bool validPLID(const std::string& input) {
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

bool validColor(const std::string& color) {
    std::string validColors = "RGBYOP";
    return color.length() == 1 && validColors.find(color) != std::string::npos;
}

void generateColorCode(char* color_code) {
    const char colors[] = "RGBYOP";
    for (int i = 0; i < 4; i++) {
        color_code[i] = colors[rand() % COLOR_NUMBER];
    }
    color_code[4] = '\0'; // Add the '\0' character
}

/* bool gameOn(const char* plid) {

        char file_name[64];
        sprintf(file_name, "GAMES/GAME_%s.txt", plid);
        FILE* file = fopen(file_name, "r"); // Opens the file in read mode

        if (file) {
            fclose(file); // Closes file
            return true;  // File exists --> Game exists
        } else {
            return false; // O ficheiro não existe
        }
} */


int getTimePassed(const char* header) {

    int max_game_time, game_duration;
    time_t start_time;
    sscanf(header + 14, "%03d %*04d-%*02d-%*02d %*02d:%*02d:%*02d %ld", &max_game_time, &start_time);

    time_t current_time = time(NULL);
    game_duration = (int)difftime(current_time, start_time); // VERIFICAR SE É PRECISO PASSAR O START TIME PARA TIME PORQUE FOI LIDO COM %ld

    if (max_game_time < game_duration) {
        game_duration = max_game_time;
    }
    return game_duration;
}


int timeExceeded(const char* header) {

    int max_game_time, game_duration;
    sscanf(header + 14, "%03d", &max_game_time);

    game_duration = getTimePassed(header);

    return max_game_time <= game_duration;
}



int getGameHeader(const char* file_name, char* header) {

    FILE* file;
    file = fopen(file_name, "r");
    if(!file) {return ERROR;}

    if(fgets(header, HEADER_SIZE, file) == NULL) {return ERROR;}
    fclose(file);

    return SUCCESS;
}

void getKeyColorCode(const char* header, char* color_code){
    sscanf(header + 9, "%s", color_code);
}

void displayColorCode(const char* color_code, char* spaced_color_code) {
    size_t length = strlen(color_code);
    size_t j = 0;
    for (size_t i = 0; i < length; i++) {
        spaced_color_code[j++] = color_code[i];      // Copy character
        if (i < length - 1) {        // Add space if not the last character
            spaced_color_code[j++] = ' ';
        }
    }
    spaced_color_code[j] = '\0';                // Null-terminate the output string
}

/* int transcriptShowTrialsFile(const char* file_name, const char* header, char* response_buffer) {
    
    FILE* file;
    ssize_t file_size;
    char cmd[32], status[32], file_name[32];

    sscanf
    file = fopen(file_name, "r");
    if(!file) {
        return ERROR;
    }



    sprintf(response_buffer, "RST ACT ");
    response_buffer
    strcpy(response_buffer, file_name);


    return SUCCESS;
} */
    
    




