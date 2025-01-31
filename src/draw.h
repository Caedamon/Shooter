#ifndef DRAW_H
#define DRAW_H

#include "raylib.h"
#include "game.h"

// Function prototypes for rendering
void DrawGame(void);
void DrawGameOverScreen(void);
void DrawWinScreen(void);
void DrawStartScreen(void);

extern Block blocks[BLOCK_ROWS][BLOCK_COLUMNS];
bool ColorsEqual(Color c1, Color c2);
#endif
