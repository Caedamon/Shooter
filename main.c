#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PADDLE_SPEED 650.0f

#define BLOCK_ROWS 5
#define BLOCK_COLUMNS 10
float ballSpeed = 200.0f;
#define BLOCK_RADIUS (ballRadius * 3)
#define MAX_BALL_SPEED 1000.0f

typedef struct {
    int number;
    float radius;
    Color color;
    bool active;
    Vector2 position;
} Block;

int main() {
    bool gameOver = false;
    bool ballStuck = true;
    bool gameStarted = false;
    bool win = false;
    int lives = 3;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pixel Breaker");
    SetTargetFPS(120);

    Rectangle paddle = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 30, 100, 20};
    Vector2 ballPosition = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    Vector2 ballVelocity = Vector2Scale(Vector2Normalize((Vector2){200.0f, -200.0f}), ballSpeed);
    float ballRadius = 10.0f;

    // create blocks
    Block blocks[BLOCK_ROWS][BLOCK_COLUMNS];
    for (int row = 0; row < BLOCK_ROWS; row++) {
        for (int col = 0; col < BLOCK_COLUMNS; col++) {
            blocks[row][col].number = 4;
            blocks[row][col].radius = BLOCK_RADIUS;
            blocks[row][col].color = BLUE;
            blocks[row][col].active = true;

            // Randomized positioning for blocks
            blocks[row][col].position.x = GetRandomValue(BLOCK_RADIUS, SCREEN_WIDTH - BLOCK_RADIUS);
            blocks[row][col].position.y = GetRandomValue(BLOCK_RADIUS, SCREEN_HEIGHT / 2 - BLOCK_RADIUS);

            //Purple Blocks = +1 life
            if (GetRandomValue(0,9) == 0) {
                blocks[row][col].color = PURPLE;
                blocks[row][col].number = 1;
            }
        }
    }

    // setup key's to play
    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);

        if (gameOver) {
            DrawText("GAME OVER", SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 50) / 2, SCREEN_HEIGHT / 2 - 50, 50, RED);
            DrawText("Press R to restart", SCREEN_WIDTH / 2 - MeasureText("Press R to restart", 20) / 2, SCREEN_HEIGHT / 2 + 20, 20, WHITE);
            DrawText("Press ESC to quit", SCREEN_WIDTH / 2 - MeasureText("Press ESC to quit", 20) / 2, SCREEN_HEIGHT / 2 + 60, 20, WHITE);

            if (IsKeyPressed(KEY_R)) {
                // Reset the game
                gameOver = false;
                gameStarted = false;
                ballStuck = true;
                ballPosition = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
                ballVelocity = Vector2Scale(Vector2Normalize((Vector2){200.0f, -200.0f}), ballSpeed);
                ballSpeed = 200.0f;

                // Reactivate all blocks
                for (int row = 0; row < BLOCK_ROWS; row++) {
                    for (int col = 0; col < BLOCK_COLUMNS; col++) {
                        blocks[row][col].number = 4;
                        blocks[row][col].radius = BLOCK_RADIUS;
                        blocks[row][col].color = BLUE;
                        blocks[row][col].active = true;

                        // Randomized positioning for blocks
                        blocks[row][col].position.x = GetRandomValue(BLOCK_RADIUS, SCREEN_WIDTH - BLOCK_RADIUS);
                        blocks[row][col].position.y = GetRandomValue(BLOCK_RADIUS, SCREEN_HEIGHT / 2 - BLOCK_RADIUS);
                    }
                }
            } else if (IsKeyPressed(KEY_ESCAPE)) {
                CloseWindow();
                return 0;
            }

            EndDrawing();
            continue;
        }

        //setting up WIN conditions
        win = true;
        for (int row = 0; row < BLOCK_ROWS; row++) {
            for (int col = 0; col < BLOCK_COLUMNS; col++) {
                if (blocks[row][col].active) {
                    win = false;
                    break;
                }
            }
            if (!win) break;
        }

        if (win) {
            DrawText("YOU WIN!", SCREEN_WIDTH / 2 - MeasureText("YOU WIN!", 50) / 2, SCREEN_HEIGHT / 2 - 50, 50, GREEN);
            DrawText("Press R to restart", SCREEN_WIDTH / 2 - MeasureText("Press R to restart", 20) / 2, SCREEN_HEIGHT / 2 + 20, 20, WHITE);
            DrawText("Press ESC to quit", SCREEN_WIDTH / 2 - MeasureText("Press ESC to quit", 20) / 2, SCREEN_HEIGHT / 2 + 60, 20, WHITE);

            if (IsKeyPressed(KEY_R)) {
                gameOver = false;
                gameStarted = false;
                win = false;
                ballStuck = true;
                ballSpeed = 200.0f;
                ballPosition = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
                ballVelocity = Vector2Zero();

                for (int row = 0; row < BLOCK_ROWS; row++) {
                    for (int col = 0; col < BLOCK_COLUMNS; col++) {
                        blocks[row][col].number = 4;
                        blocks[row][col].radius = BLOCK_RADIUS;
                        blocks[row][col].color = BLUE;
                        blocks[row][col].active = true;

                        // Randomized positioning for blocks
                        blocks[row][col].position.x = GetRandomValue(BLOCK_RADIUS, SCREEN_WIDTH - BLOCK_RADIUS);
                        blocks[row][col].position.y = GetRandomValue(BLOCK_RADIUS, SCREEN_HEIGHT / 2 - BLOCK_RADIUS);
                    }
                }
            } else if (IsKeyPressed(KEY_ESCAPE)) {
                CloseWindow();
                return 0;
            }

            EndDrawing();
            continue;
        }

        if (!gameStarted) {
            // Show the welcome screen
            DrawText("Welcome!", SCREEN_WIDTH / 2 - MeasureText("Welcome!", 40) / 2, SCREEN_HEIGHT / 2 - 60, 40, WHITE);
            DrawText("To", SCREEN_WIDTH / 2 - MeasureText("To", 30) / 2, SCREEN_HEIGHT / 2, 30, WHITE);
            DrawText("PIXEL BREAKER!!!", SCREEN_WIDTH / 2 - MeasureText("PIXEL BREAKER!!!", 50) / 2, SCREEN_HEIGHT / 2 + 40, 50, RED);
            DrawText("Press SPACE to start", SCREEN_WIDTH / 2 - MeasureText("Press SPACE to start", 20) / 2, SCREEN_HEIGHT / 2 + 120, 20, WHITE);

            if (IsKeyPressed(KEY_SPACE)) {
                gameStarted = true;
            }
        } else {
            // Paddle movement
            if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) && paddle.x > 0) {
                paddle.x -= PADDLE_SPEED * delta_time;
            }
            if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) && paddle.x < SCREEN_WIDTH - paddle.width) {
                paddle.x += PADDLE_SPEED * delta_time;
            }

            //Make sure the ball is stuck to the paddle at the start of the game
            if (ballStuck) {
                ballPosition.x = paddle.x + paddle.width / 2;
                ballPosition.y = paddle.y - ballRadius;

                if (IsKeyPressed(KEY_SPACE)) {
                    ballStuck = false;
                    ballVelocity = Vector2Scale(Vector2Normalize((Vector2){200.0f, -200.0f}), ballSpeed);
                }
            } else {
                // Ball movement
                ballPosition.x += ballVelocity.x * delta_time;
                ballPosition.y += ballVelocity.y * delta_time;

                // Wall collisions
                if (ballPosition.x - ballRadius <= 0 || ballPosition.x + ballRadius >= SCREEN_WIDTH) {
                    ballVelocity.x *= -1;
                }

                if (ballPosition.y - ballRadius <= 0) {
                    ballVelocity.y *= -1;
                }
                //lives and logic for removing lives
                if (ballPosition.y + ballRadius > SCREEN_HEIGHT) {
                    lives--;
                    if (lives <= 0) {
                        gameOver = true;
                    } else {
                        ballStuck = true;
                        ballPosition = (Vector2){paddle.x + paddle.width / 2,  paddle.y - ballRadius};
                        ballSpeed = 200.0f;
                        ballVelocity = Vector2Zero();
                    }
                }

                // Paddle collision
                if (CheckCollisionCircleRec(ballPosition, ballRadius, paddle)) {
                    ballVelocity.y *= -1;

                    // Divide paddle into 4 segments and change angle based on hit position
                    float segmentWidth = paddle.width / 4;
                    float hitPosition = ballPosition.x - paddle.x;

                    if (hitPosition < segmentWidth) {
                        ballVelocity.x = -300.0f;
                    } else if (hitPosition < segmentWidth * 2) {
                        ballVelocity.x = -150.0f;
                    } else if (hitPosition < segmentWidth * 3) {
                        ballVelocity.x = 150.0f;
                    } else {
                        ballVelocity.x = 300.0f;
                    }

                    // Normalize and scale the velocity to maintain speed
                    ballVelocity = Vector2Scale(Vector2Normalize(ballVelocity), ballSpeed);
                }

                // Block collisions
                for (int row = 0; row < BLOCK_ROWS; row++) {
                    for (int col = 0; col < BLOCK_COLUMNS; col++) {
                        if (blocks[row][col].active) {
                            // Check for collision with the ball
                            if (CheckCollisionCircles(ballPosition, ballRadius, blocks[row][col].position, blocks[row][col].radius)) {
                                blocks[row][col].number--;
                                blocks[row][col].radius *= 0.75f;
                                ballVelocity.y *= -1;

                                // this changes the color of the blocks depending on which number it is
                                switch (blocks[row][col].number) {
                                    case 3: blocks[row][col].color = GREEN; break;
                                    case 2: blocks[row][col].color = YELLOW; break;
                                    case 1: blocks[row][col].color = RED; break;
                                }

                                //handling the logic for Life (Purple) block
                                if (blocks[row][col].color.r == PURPLE.r &&
                                    blocks[row][col].color.g == PURPLE.g &&
                                    blocks[row][col].color.b == PURPLE.b &&
                                    blocks[row][col].color.a == PURPLE.a) {
                                    lives++;
                                    blocks[row][col].active = false;
                                }

                                // delete block after number reaches 0
                                if (blocks[row][col].number <= 0) {
                                    blocks[row][col].active = false;
                                }

                                // increase ball-speed by 10% when "block" is hit
                                ballSpeed *= 1.1f;
                                if (ballSpeed > MAX_BALL_SPEED) {
                                    ballSpeed = MAX_BALL_SPEED;
                                }
                                ballVelocity = Vector2Scale(Vector2Normalize(ballVelocity), ballSpeed);
                                break;
                            }
                        }
                    }
                }
            }
            // Draw block circles
            for (int row = 0; row < BLOCK_ROWS; row++) {
                for (int col = 0; col < BLOCK_COLUMNS; col++) {
                    if (blocks[row][col].active) {
                        DrawCircle(blocks[row][col].position.x, blocks[row][col].position.y, blocks[row][col].radius, blocks[row][col].color);
                        DrawText(TextFormat("%d", blocks[row][col].number),
                                 blocks[row][col].position.x - MeasureText(TextFormat("%d", blocks[row][col].number), 20) / 2,
                                 blocks[row][col].position.y - 10, 20, WHITE);
                    }
                }
            }

            //Draw Life Counter
            DrawText(TextFormat("Lives: %d", lives), 10,10,20, WHITE);

            // Draw Paddle
            DrawRectangleRec(paddle, WHITE);

            // Draw Ball
            DrawCircleV(ballPosition, ballRadius, WHITE);
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}