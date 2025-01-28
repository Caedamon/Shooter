#ifndef FILE_IO_H
#define FILE_IO_H

#include "raylib.h"

// Maximum size for a player's name
#define MAX_NAME_LENGTH 32

// Struct for a high score entry
typedef struct {
    char playerName[MAX_NAME_LENGTH];
    int score;
} HighScoreEntry;

// Function prototypes for high score saving/loading
void SaveHighScore(HighScoreEntry entry);
HighScoreEntry LoadHighScore(void);
int ReadHighScore(void);
void WriteHighScore(int score, const char *playerName);

#endif
