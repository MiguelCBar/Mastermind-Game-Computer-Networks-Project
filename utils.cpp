#include "utils.h"
#include "constants.h"
#include <fstream>
#include <string.h>
#include <iostream>
#include <dirent.h>
#include <vector>
#include <filesystem>
#include <bits/algorithmfwd.h>

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
    srand(static_cast<unsigned>(time(0)));
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

    if(fgets(header, HEADER_SIZE, file) == NULL) {
        printf("entrou mal\n");
        return ERROR;}
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

size_t containsChar(const char* buffer, size_t size, char target) {
    for (size_t i = 0; i < size; ++i) {
        if (buffer[i] == target) {
            return (i + 1);
        }
    }
    return 0;
}

int transcriptOngoingGameFile(const char* file_path, const char* header, char* response_buffer) {
    
    FILE* file;
    size_t offset = 0;
    char file_name[32], file_data[MAX_FILE_SIZE], file_tries_data[MAX_FILE_SIZE/2];

    int plid, gameTime, trials_count = 0;
    char date[11], time[9];

    sscanf(header, "%d %*1s %*4s %03d %s %s", &plid, &gameTime, date, time);

    memset(file_name, 0, sizeof(file_name));
    sprintf(file_name, "STATE_%d.txt", plid);

    file = fopen(file_path, "r");
    if(!file) {
        return ERROR;
    }
    char line[64];
    memset(line, 0, sizeof(line));
    if(fgets(line, HEADER_SIZE, file) == NULL) {
        return ERROR;
    }
    char color_code[COLOR_CODE_SIZE];
    int nB, nW, try_time;
    memset(line, 0, sizeof(line));
    memset(file_tries_data, 0, sizeof(file_tries_data));
    while(fgets(line, HEADER_SIZE, file)) {
        sscanf(line, "T: %4s %d %d %d", color_code, &nB, &nW, &try_time);
        offset += sprintf(file_tries_data + offset, "Trial: %s, nB: %d\tnW: %d at %d sec\n", color_code, nB, nW, try_time);
        trials_count++;
    }

    fclose(file);
    int time_left = gameTime - getTimePassed(header);

    offset = 0;
    memset(file_data, 0, sizeof(file_data));
    offset += sprintf(file_data + offset, "--------------------------------\nActive game found: PLAYER %d\n\nGame initiated: %s %s\nTime to complete: %d seconds\n", plid, date, time, gameTime);
    if(trials_count == 0) {
        offset += sprintf(file_data + offset, "\n\tNO TRANSACTIONS FOUND\n");
    }
    else {
        offset += sprintf(file_data + offset, "\n\tTRANSACTIONS FOUND: %d\n\n", trials_count);
    }
    offset += sprintf(file_data + offset, "%s\n\n\tREMAINING TIME LEFT: %d\n--------------------------------", file_tries_data, time_left);

    sprintf(response_buffer, "RST ACT %s %ld %s\n", file_name, strlen(file_data), file_data);

    return SUCCESS;
}

int transcriptFinishedGameFile(const char* file_path, const char* header, char* response_buffer) {

    FILE* file;
    size_t offset = 0;
    char file_name[32], file_data[MAX_FILE_SIZE], file_tries_data[MAX_FILE_SIZE/2];

    int plid, gameTime, trials_count = 0;
    char mode, initial_date[11], initial_time[9], key_color_code[5], finisher_mode;

    sscanf(header, "%d %c %4s %03d %s %s", &plid, &mode, key_color_code, &gameTime, initial_date, initial_time);

    memset(file_name, 0, sizeof(file_name));
    sprintf(file_name, "STATE_%d.txt", plid);

    finisher_mode = file_path[strlen(file_path) - 5];

    file = fopen(file_path, "r");
    if(!file) {
        return ERROR;   
    }
    char line[64];
    memset(line, 0, sizeof(line));
    if(fgets(line, HEADER_SIZE, file) == NULL) {
        return ERROR;
    }
    char color_code[COLOR_CODE_SIZE];
    int nB, nW, try_time;
    memset(line, 0, sizeof(line));
    memset(file_tries_data, 0, sizeof(file_tries_data));
    while(fgets(line, HEADER_SIZE, file)) {
        printf("char: %c\n", line[0]);
        if (line[0] == 'T') {
            sscanf(line, "T: %4s %d %d %d", color_code, &nB, &nW, &try_time);
            offset += sprintf(file_tries_data + offset, "Trial: %s, nB: %d\tnW: %d at %d sec\n", color_code, nB, nW, try_time);
            trials_count++;
        }
        else {
            break;
        }
    }

    fclose(file);

    char final_date[11], final_time[9];
    int game_duration;

    sscanf(line, "%s %s %d\n", final_date, final_time, &game_duration);

    offset = 0;
    memset(file_data, 0, sizeof(file_data));
    offset += sprintf(file_data + offset, "--------------------------------\nLast finalized game found: ");
    offset += sprintf(file_data + offset, "PLAYER %d\n\nGame initiated: %s %s\nTime to complete: %d seconds\n", plid, initial_date, initial_time, gameTime);
    if(mode == 'P') {
        offset += sprintf(file_data + offset, "Mode: PLAY\nSecret code: %s\n\n", key_color_code);
    }
    else {
        offset += sprintf(file_data + offset, "Mode: DEBUG\nSecret code: %s\n\n", key_color_code);
    }
    if(trials_count == 0) {
        offset += sprintf(file_data + offset, "\n\tNO TRANSACTIONS FOUND\n");
    }
    else {
        offset += sprintf(file_data + offset, "\n\tTRANSACTIONS FOUND: %d\n\n", trials_count);
    }

    offset += sprintf(file_data + offset, "%s\n\n", file_tries_data);

    if(finisher_mode == 'W') {
        offset += sprintf(file_data + offset, "Termination: WIN ");
    }
    else if(finisher_mode == 'T') {
        offset += sprintf(file_data + offset, "Termination: TIMEOUT ");
    }
    else if(finisher_mode == 'Q') {
        offset += sprintf(file_data + offset, "Termination: QUIT ");
    }
    else if(finisher_mode == 'F')
        offset += sprintf(file_data + offset, "Termination: FAIL ");

    offset += sprintf(file_data + offset, "at %s %s\nGame Duration: %d seconds\n--------------------------------", final_date, final_time, game_duration);
    sprintf(response_buffer, "RST FIN %s %ld %s\n", file_name, strlen(file_data), file_data);

    return SUCCESS;
}


int FindLastGame(char *PLID, char *fname) {

    struct dirent **filelist;
    int n_entries, found;
    char dirname[20];

    sprintf(dirname, "GAMES/%s/", PLID);

    n_entries = scandir(dirname, &filelist, 0, alphasort);

    found = 0;

    if (n_entries <= 0)
        return (0);
    else
    {
        while (n_entries--)
        {
            if (filelist[n_entries]->d_name[0] != '.' && !found)
            {
                sprintf(fname, "GAMES/%s/%s", PLID, filelist[n_entries]->d_name);
                found = 1;
            }
            free(filelist[n_entries]);
        }
        free(filelist);
    }

    return (found);
}



int processScores(const char* directory, char* file_data) {
    
    DIR* dir;
    struct dirent* entry;
    FileInfo topFiles[10] = {};

    // Abrir a diretoria

    dir = opendir(directory);
    if (!dir) {
        std::cerr << "ERROR: failed to open directory\n";
        return ERROR;
    }

    // Iterar pelos ficheiros na diretoria
    while ((entry = readdir(dir)) != nullptr) {
        
        char* filename = entry->d_name;
        // Extract score
        int score = atoi(filename);

        char filepath[MAX_FILE_NAME];
        snprintf(filepath, sizeof(filepath), "%s/%s", directory, filename);

        // Verify if game should be on the Top 10 Scores
        for (int i = 0; i < 10; ++i) {
            if (score > topFiles[i].score) {
                // Push the rest of the files downwards
                for (int j = 9; j > i; --j) {
                    topFiles[j] = topFiles[j - 1];
                }
                // Insert new file
                topFiles[i].score = score;
                strncpy(topFiles[i].filepath, filepath, MAX_FILE_NAME);
                break;
            }
        }
    }

    closedir(dir);

    // Read and copy to buffer the top 10 best scores
    char line[128], transformed_line[128];
    int offset = 0;

    offset += sprintf(file_data + offset, "--------------------------------------------\n\t\tTOP 10 SCORES\n--------------------------------------------\n\n");
    offset += sprintf(file_data + offset, "     SCORE  PLAYER   CODE  TRIALS   MODE\n\n");    

    for (int i = 0; i < 10; ++i) {
        if (topFiles[i].score == 0) 
            continue; // No valid file was found

        FILE* input = fopen(topFiles[i].filepath, "r");
        if (!input) {
            return ERROR;
        }
        memset(line, 0, sizeof(line));
        if (fgets(line, sizeof(line), input)) {
            if(i == 9)  {
                offset += sprintf(file_data + offset, " %d - ", i + 1);
            }
            else {  
                offset += sprintf(file_data + offset, "  %d - ", i + 1);
            }
            transcriptGameScoreFile(line, transformed_line);
            offset += sprintf(file_data + offset, "%s", transformed_line);
        }
        fclose(input);
    }
    
    std::cout << "acabou de processar o buffer\n\n";

    return SUCCESS;

} 



int calculateGameScore(int game_duration, int tries_count) {
    return int((600 - game_duration) * 50/600 + (8 - tries_count) * 50 / (8-1));
}

void transcriptGameScoreFile(const char* line, char* transformed_line) {
    
    int score, plid, n_tries;
    char color_code[5], mode[8];
    sscanf(line, "%d %d %s %d %s\n", &score, &plid, color_code, &n_tries, mode);
    sprintf(transformed_line, "%03d   %06d   %s     %d     %s\n", score, plid, color_code, n_tries, mode);

}   

    
    




