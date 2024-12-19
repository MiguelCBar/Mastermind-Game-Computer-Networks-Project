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


size_t containsChar(const char* buffer, size_t size, char target) {
    for (size_t i = 0; i < size; ++i) {
        if (buffer[i] == target) {
            return (i + 1);
        }
    }
    return 0;
}
bool validColor(const std::string& color) {
    std::string validColors = "RGBYOP";
    return color.length() == 1 && validColors.find(color) != std::string::npos;
}

bool gameOn(const char* plid) {

        char file_name[64];
        sprintf(file_name, "GAMES/GAME_%s.txt", plid);
        FILE* file = fopen(file_name, "r"); // Opens the file in read mode

        if (file) {
            fclose(file); // Closes file
            return true;  // File exists --> Game exists
        } else {
            return false; // O ficheiro não existe
        }
}


int getTimePassed(const char* plid) {

    FILE* file;
    char first_file_line[256], file_name[64];

    memset(file_name, 0, sizeof(file_name));
    sprintf(file_name, "GAMES/GAME_%s.txt", plid);

    file = fopen(file_name, "r");
    if(!file) {return ERROR;}
    
    memset(first_file_line, 0, sizeof(first_file_line));
    if(fgets(first_file_line, sizeof(first_file_line), file) == NULL) {return ERROR;}
    fclose(file);

    int max_game_time, game_duration;
    time_t start_time;
    sscanf(first_file_line + 14, "%03d %*04d-%*02d-%*02d %*02d:%*02d:%*02d %ld", &max_game_time, &start_time);

    time_t current_time = time(NULL);
    game_duration = (int)difftime(current_time, start_time); // VERIFICAR SE É PRECISO PASSAR O START TIME PARA TIME PORQUE FOI LIDO COM %ld

    if (max_game_time < game_duration) {
        game_duration = max_game_time;
    }
    return game_duration;
}


int timeExceeded(const char* plid) {

    FILE* file;
    char first_file_line[256], file_name[64];

    memset(file_name, 0, sizeof(file_name));
    sprintf(file_name, "GAMES/GAME_%s.txt", plid);

    file = fopen(file_name, "r");
    if(!file) {return ERROR;}
    
    memset(first_file_line, 0, sizeof(first_file_line));
    if(fgets(first_file_line, sizeof(first_file_line), file) == NULL) {return ERROR;}
    fclose(file);

    int max_game_time, game_duration;
    time_t start_time;
    sscanf(first_file_line + 14, "%03d %*04d-%*02d-%*02d %*02d:%*02d:%*02d %ld", &max_game_time, &start_time);

    time_t current_time = time(NULL);
    game_duration = (int)difftime(current_time, start_time); // VERIFICAR SE É PRECISO PASSAR O START TIME PARA TIME PORQUE FOI LIDO COM %ld~

    std::cout << "game_duration: " << game_duration << "\n" << "max_game_time: " << max_game_time << "\n\n\n";

    return max_game_time < game_duration;
}


void generateColorCode(char* color_code) {
    const char colors[] = "RGBYOP";
    for (int i = 0; i < 4; i++) {
        color_code[i] = colors[rand() % COLOR_NUMBER];
    }
    color_code[4] = '\0'; // Add the '\0' character
}

/* int transcriptShowTrialsFile(const char* file_path, char* response_buffer) {
    
    FILE* file;
    char file_name[32];
    int file_size;


    char 
    file = fopen(file_path, "r");
    if(!file) {
        return ERROR;
    }
    file_name = strrchr(file_path, '/'); 
    file_name++;



    sprintf(response_buffer, "RST ACT ");
    response_buffer
    strcpy(response_buffer, file_name);


    return SUCCESS;
} */
    
    




