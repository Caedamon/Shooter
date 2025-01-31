#include "draw.h"
#include "game.h"
#include "file_io.h"
#include <string.h>

extern int playerScore;
extern HighScoreEntry highScore;

// Draws the game elements (paddle, blocks, ball, etc.)
void DrawGame(void) {
    // Draw blocks
    for (int row = 0; row < BLOCK_ROWS; row++) {
        for (int col = 0; col < BLOCK_COLUMNS; col++) {
            if (blocks[row][col].active) {
                DrawCircle(blocks[row][col].position.x, blocks[row][col].position.y, blocks[row][col].radius, blocks[row][col].color);

                // Draw block number unless it's a Teal or Purple block
                if (!ColorsEqual(blocks[row][col].color, TEAL) && !ColorsEqual(blocks[row][col].color, PURPLE)) {
                    DrawText(TextFormat("%d", blocks[row][col].number),
                             blocks[row][col].position.x - MeasureText(TextFormat("%d", blocks[row][col].number), 20) / 2,
                             blocks[row][col].position.y - 10, 20, WHITE);
                }
            }
        }
    }

    // Draw life counter
    DrawText(TextFormat("Lives: %d", lives), 10, 10, 20, WHITE);

    //Draw Score
    DrawText(TextFormat("Score: %d", playerScore), 10, 30, 20, WHITE);

    // Draw paddle
    DrawRectangleRec(paddle, WHITE);

    // Draw ball
    DrawCircleV(ballPosition, ballRadius, WHITE);

    // Draw exploding balls
    for (int i = 0; i < MAX_EXPLODING_BALLS; i++) {
        if (explodingBalls[i].active) {
            DrawCircleV(explodingBalls[i].position, explodingBalls[i].radius, WHITE);
        }
    }
}

// Draws the start screen
void DrawStartScreen(void) {
    DrawText("Welcome!", SCREEN_WIDTH / 2 - MeasureText("Welcome!", 40) / 2, SCREEN_HEIGHT / 2 - 60, 40, WHITE);
    DrawText("To", SCREEN_WIDTH / 2 - MeasureText("To", 30) / 2, SCREEN_HEIGHT / 2, 30, WHITE);
    DrawText("PIXEL BREAKER!!!", SCREEN_WIDTH / 2 - MeasureText("PIXEL BREAKER!!!", 50) / 2, SCREEN_HEIGHT / 2 + 40, 50, RED);
    DrawText("Press SPACE to start", SCREEN_WIDTH / 2 - MeasureText("Press SPACE to start", 20) / 2, SCREEN_HEIGHT / 2 + 120, 20, WHITE);
}

void DrawGameOverScreen() {
    DrawText("GAME OVER", SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 50) / 2, SCREEN_HEIGHT / 2 - 50, 50, RED);
    DrawText("Press R to restart", SCREEN_WIDTH / 2 - MeasureText("Press R to restart", 20) / 2, SCREEN_HEIGHT / 2 + 20, 20, WHITE);
    DrawText("Press ESC to quit", SCREEN_WIDTH / 2 - MeasureText("Press ESC to quit", 20) / 2, SCREEN_HEIGHT / 2 + 60, 20, WHITE);

    // High score logic
    if (playerScore > highScore.score) {
        DrawText("NEW HIGH SCORE!", SCREEN_WIDTH / 2 - MeasureText("NEW HIGH SCORE!", 30) / 2, SCREEN_HEIGHT / 2 + 100, 30, YELLOW);

        // Ask for the player's name
        DrawText("Enter your name:", SCREEN_WIDTH / 2 - MeasureText("Enter your name:", 20) / 2, SCREEN_HEIGHT / 2 + 150, 20, WHITE);

        // Placeholder: Implement logic to capture player name input
        const char *playerName = "Player";

        // Save the high score
        highScore.score = playerScore;
        strncpy(highScore.playerName, playerName, sizeof(highScore.playerName) - 1);
        highScore.playerName[sizeof(highScore.playerName) - 1] = '\0';
        SaveHighScore(highScore);
    }
}

void DrawWinScreen() {
    DrawText("YOU WIN!", SCREEN_WIDTH / 2 - MeasureText("YOU WIN!", 50) / 2, SCREEN_HEIGHT / 2 - 50, 50, GREEN);
    DrawText("Press R to restart", SCREEN_WIDTH / 2 - MeasureText("Press R to restart", 20) / 2, SCREEN_HEIGHT / 2 + 20, 20, WHITE);
    DrawText("Press ESC to quit", SCREEN_WIDTH / 2 - MeasureText("Press ESC to quit", 20) / 2, SCREEN_HEIGHT / 2 + 60, 20, WHITE);

    // High score logic
    if (playerScore > highScore.score) {
        DrawText("NEW HIGH SCORE!", SCREEN_WIDTH / 2 - MeasureText("NEW HIGH SCORE!", 30) / 2, SCREEN_HEIGHT / 2 + 100, 30, YELLOW);

        // Ask for the player's name
        DrawText("Enter your name:", SCREEN_WIDTH / 2 - MeasureText("Enter your name:", 20) / 2, SCREEN_HEIGHT / 2 + 150, 20, WHITE);

        // Placeholder: Implement logic to capture player name input
        const char *playerName = "Player";

        // Save the high score
        highScore.score = playerScore;
        strncpy(highScore.playerName, playerName, sizeof(highScore.playerName) - 1);
        SaveHighScore(highScore);
    }
}