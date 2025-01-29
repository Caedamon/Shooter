/*
 https://github.com/Caedamon/Shooter.git
 git link

 Created and finalized by Carl F.
*/


#include "raylib.h"
#include "game.h"
#include "draw.h"
#include "file_io.h"
#include "vector_math.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

HighScoreEntry highScore;

int main() {
    // Game state variables
    bool gameOver = false;
    bool ballStuck = true;
    bool gameStarted = false;
    bool win = false;

    // Load the high score when the game initializes
    highScore = LoadHighScore();

    // Initialize the game window and settings
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pixel Breaker");
    SetTargetFPS(120);

    // Initialize all game objects and variables
    InitGame();

    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);

        // Display the current high score on the screen during gameplay
        DrawText(TextFormat("High Score: %s - %d", highScore.playerName, highScore.score),
                 10, 50, 20, WHITE);

        if (gameOver) {
            // Draw game over screen
            DrawGameOverScreen();

            if (IsKeyPressed(KEY_R)) {
                ResetGame();  // Reset the game state
                gameOver = false;
                gameStarted = false;
                win = false;
                ballStuck = true; // Ensure ball resets correctly
            } else if (IsKeyPressed(KEY_ESCAPE)) {
                CloseWindow();
                return 0;
            }
        } else if (win) {
            // Draw win screen
            DrawWinScreen();

            if (IsKeyPressed(KEY_R)) {
                ResetGame();  // Reset the game state
                gameOver = false;
                gameStarted = false;
                win = false;
                ballStuck = true; // Ensure ball resets correctly
            } else if (IsKeyPressed(KEY_ESCAPE)) {
                CloseWindow();
                return 0;
            }
        } else if (!gameStarted) {
            // Draw start screen
            DrawStartScreen();

            if (IsKeyPressed(KEY_SPACE)) {
                gameStarted = true;
            }
        } else {
            // Handle paddle movement
            if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) && paddle.x > 0) {
                paddle.x -= PADDLE_SPEED * delta_time;
            }
            if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) && paddle.x < SCREEN_WIDTH - paddle.width) {
                paddle.x += PADDLE_SPEED * delta_time;
            }

            // Ball stuck logic
            if (ballStuck) {
                ballPosition.x = paddle.x + paddle.width / 2;
                ballPosition.y = paddle.y - ballRadius;

                if (IsKeyPressed(KEY_SPACE)) {
                    ballStuck = false;
                    ballVelocity = ScaleVector2(NormalizeVector2((Vector2){200.0f, -200.0f}), ballSpeed);
                }
            } else {
                // Update game state and check collisions
                UpdateGame(&gameOver, &win, &ballStuck);
            }

            // Draw all game elements (blocks, paddle, ball, etc.)
            DrawGame();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
