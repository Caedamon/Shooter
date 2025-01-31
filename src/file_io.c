#include "file_io.h"
#include <stdio.h>
#include <string.h>

// Saves the high score (player name and score) to a file
void SaveHighScore(HighScoreEntry entry) {
    FILE *file = fopen("highscore.dat", "w");
    if (file) {
        // Save player name and score in a text format
        fprintf(file, "%s %d\n", entry.playerName, entry.score);
        fclose(file);
    } else {
        TraceLog(LOG_WARNING, "Could not save high score: Unable to open file.");
    }
}

// Loads the high score (player name and score) from a file
HighScoreEntry LoadHighScore(void) {
    HighScoreEntry entry = { "", 0 };
    FILE *file = fopen("highscore.dat", "r");
    if (file) {
        if (fscanf(file, "%31s %d", entry.playerName, &entry.score) != 2) {
            entry.playerName[0] = '\0'; // Ensure empty name on error
            TraceLog(LOG_WARNING, "Invalid high score format in file.");
        }
        entry.playerName[MAX_NAME_LENGTH - 1] = '\0'; // Always null-terminate
        fclose(file);
    } else {
        TraceLog(LOG_WARNING, "Could not load high score: File does not exist.");
    }
    return entry;
}

// Reads the highest score directly (score only, without name)
int ReadHighScore(void) {
    HighScoreEntry entry = LoadHighScore();
    return entry.score;
}

// Writes a new high score to the file
void WriteHighScore(int score, const char *playerName) {
    HighScoreEntry entry = { "", score };
    strncpy(entry.playerName, playerName, MAX_NAME_LENGTH - 1);
    entry.playerName[MAX_NAME_LENGTH - 1] = '\0'; // Null-terminate for safety
    SaveHighScore(entry);
}