#include "raylib.h"
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PADDLE_SPEED 650.0f

#define BLOCK_ROWS 5
#define BLOCK_COLUMNS 10
float ballSpeed = 200.0f;
#define BLOCK_RADIUS (ballRadius * 3)
#define MAX_BALL_SPEED 1000.0f
#define PURPLE (Color){128, 0, 128, 255} // RGBA for Purple
#define TEAL (Color){0, 128, 128, 255} // RGB for Teal
#define PI 3.14159265358979323846

typedef struct {
    int number;
    float radius;
    Color color;
    bool active;
    Vector2 position;
} Block;

// struct for exploding bock
typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
    int hitsRemaining;
    bool active;
} ExplodingBall;

// Custom Vector Math Functions
float MagnitudeVector2(Vector2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

Vector2 NormalizeVector2(Vector2 v) {
    float magnitude = MagnitudeVector2(v);
    if (magnitude == 0) return (Vector2){0, 0}; // Handle zero vector
    return (Vector2){v.x / magnitude, v.y / magnitude};
}

Vector2 ScaleVector2(Vector2 v, float scale) {
    return (Vector2){v.x * scale, v.y * scale};
}

Vector2 ZeroVector2() {
    return (Vector2){0, 0};
}

// Custom function to compare Colors
bool ColorsEqual(Color c1, Color c2) {
    return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a);
}

int main() {
    bool gameOver = false;
    bool ballStuck = true;
    bool gameStarted = false;
    bool win = false;
    int lives = 3;

    #define MAX_EXPLODING_BALLS 10
    ExplodingBall explodingBalls[MAX_EXPLODING_BALLS];

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pixel Breaker");
    SetTargetFPS(120);

    Rectangle paddle = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 30, 100, 20};
    Vector2 ballPosition = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    Vector2 ballVelocity = ScaleVector2(NormalizeVector2((Vector2){200.0f, -200.0f}), ballSpeed);
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

            // Purple Blocks = +1 life
            int randomValue = GetRandomValue(0, 19);
            if (randomValue == 0) {
                blocks[row][col].color = PURPLE;
                blocks[row][col].number = 1;
            } else if (randomValue == 1) {
                blocks[row][col].color = TEAL;
                blocks[row][col].number = -1;
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
                ballVelocity = ScaleVector2(NormalizeVector2((Vector2){200.0f, -200.0f}), ballSpeed);
                ballSpeed = 200.0f;

                lives = 3; // Reset lives

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

                        // Purple Blocks = +1 life + higher chanse of purple//teal blocks
                        int randomValue = GetRandomValue(0, 19);
                        if (randomValue == 0) {
                            blocks[row][col].color = PURPLE;
                            blocks[row][col].number = 1;
                        } else if (randomValue <= 2) {
                            blocks[row][col].color = TEAL;
                            blocks[row][col].number = -1;
                        }
                    }
                }

                // Reset exploding balls
                for (int i = 0; i < MAX_EXPLODING_BALLS; i++) {
                    explodingBalls[i].position = ZeroVector2();
                    explodingBalls[i].velocity = ZeroVector2();
                    explodingBalls[i].radius = ballRadius / 2;
                    explodingBalls[i].hitsRemaining = 10;
                    explodingBalls[i].active = false;
                }
            } else if (IsKeyPressed(KEY_ESCAPE)) {
                CloseWindow();
                return 0;
            }

            EndDrawing();
            continue;
        }

        // setting up WIN conditions
        if (gameStarted) {
            win = true;
            for (int row = 0; row < BLOCK_ROWS; row++) {
                for (int col = 0; col < BLOCK_COLUMNS; col++) {
                    if (blocks[row][col].active && blocks[row][col].number != -1) {
                        win = false;
                        break;
                    }
                }
                if (!win) break;
            }
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
                ballVelocity = ZeroVector2();
                lives = 3;

                for (int row = 0; row < BLOCK_ROWS; row++) {
                    for (int col = 0; col < BLOCK_COLUMNS; col++) {
                        blocks[row][col].number = 4;
                        blocks[row][col].radius = BLOCK_RADIUS;
                        blocks[row][col].color = BLUE;
                        blocks[row][col].active = true;

                        // Randomized positioning for blocks
                        blocks[row][col].position.x = GetRandomValue(BLOCK_RADIUS, SCREEN_WIDTH - BLOCK_RADIUS);
                        blocks[row][col].position.y = GetRandomValue(BLOCK_RADIUS, SCREEN_HEIGHT / 2 - BLOCK_RADIUS);

                        if (GetRandomValue(0, 9) == 0) {
                            blocks[row][col].color = PURPLE;
                            blocks[row][col].number = 1;
                        }
                    }
                }

                for (int i = 0; i < MAX_EXPLODING_BALLS; i++) {
                    explodingBalls[i].position = ZeroVector2();
                    explodingBalls[i].velocity = ZeroVector2();
                    explodingBalls[i].radius = ballRadius / 2;
                    explodingBalls[i].hitsRemaining = 10;
                    explodingBalls[i].active = false;
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

            // Make sure the ball is stuck to the paddle at the start of the game
            if (ballStuck) {
                ballPosition.x = paddle.x + paddle.width / 2;
                ballPosition.y = paddle.y - ballRadius;

                if (IsKeyPressed(KEY_SPACE)) {
                    ballStuck = false;
                    ballVelocity = ScaleVector2(NormalizeVector2((Vector2){200.0f, -200.0f}), ballSpeed);
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

                // Lives and logic for removing lives
                if (ballPosition.y + ballRadius > SCREEN_HEIGHT) {
                    lives--;
                    if (lives <= 0) {
                        gameOver = true;
                    } else {
                        ballStuck = true;
                        ballPosition = (Vector2){paddle.x + paddle.width / 2, paddle.y - ballRadius};
                        ballSpeed = 200.0f;
                        ballVelocity = ZeroVector2();
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
                    ballVelocity = ScaleVector2(NormalizeVector2(ballVelocity), ballSpeed);
                }

                // Block collisions
                for (int row = 0; row < BLOCK_ROWS; row++) {
                    for (int col = 0; col < BLOCK_COLUMNS; col++) {
                        if (blocks[row][col].active) {
                            // Check for collision with the ball
                            if (CheckCollisionCircles(ballPosition, ballRadius, blocks[row][col].position, blocks[row][col].radius)) {

                                if (ColorsEqual(blocks[row][col].color, TEAL)) {
                                    // Trigger Teal block explosion
                                    int explodingBallsCreated = 0;

                                    for (int i = 0; i < MAX_EXPLODING_BALLS; i++) {
                                        if (!explodingBalls[i].active) {
                                            explodingBalls[i].active = true;
                                            explodingBalls[i].position = blocks[row][col].position;
                                            explodingBalls[i].radius = ballRadius / 2;
                                            explodingBalls[i].hitsRemaining = 10;

                                            // Assign velocity in a circular pattern
                                            float angle = (2 * PI / MAX_EXPLODING_BALLS) * explodingBallsCreated;
                                            explodingBalls[i].velocity = ScaleVector2(
                                                (Vector2){cosf(angle), sinf(angle)},
                                                ballSpeed * 0.5f
                                            );

                                            explodingBallsCreated++;
                                            if (explodingBallsCreated >= MAX_EXPLODING_BALLS) break;
                                        }
                                    }

                                    // Deactivate the Teal block
                                    blocks[row][col].active = false;
                                    break; // Prevent further collision checks for this block
                                }

                                blocks[row][col].number--;
                                blocks[row][col].radius *= 0.75f;
                                ballVelocity.y *= -1;

                                // this changes the color of the blocks depending on which number it is
                                switch (blocks[row][col].number) {
                                    case 3:
                                        blocks[row][col].color = GREEN;
                                        break;
                                    case 2:
                                        blocks[row][col].color = YELLOW;
                                        break;
                                    case 1:
                                        blocks[row][col].color = RED;
                                        break;
                                }

                                // handling the logic for Life (Purple) block
                                if (ColorsEqual(blocks[row][col].color, PURPLE)) {
                                    lives++;
                                    blocks[row][col].active = false;
                                }

                                // delete block after number reaches 0
                                if (blocks[row][col].number <= 0) {
                                    blocks[row][col].active = false;
                                }

                                // Teal block logic (Exploding Balls)
                                if (ColorsEqual(blocks[row][col].color, TEAL)) {
                                    int explodingBallsCreated = 0;

                                    for (int i = 0; i < MAX_EXPLODING_BALLS; i++) {
                                        if (!explodingBalls[i].active) {
                                            explodingBalls[i].active = true;
                                            explodingBalls[i].position = blocks[row][col].position;
                                            explodingBalls[i].radius = ballRadius / 2;
                                            explodingBalls[i].hitsRemaining = 10;

                                            // Assign velocity in a circular pattern
                                            float angle = (2 * PI / MAX_EXPLODING_BALLS) * explodingBallsCreated;
                                            explodingBalls[i].velocity = ScaleVector2(
                                                (Vector2){cosf(angle), sinf(angle)},
                                                ballSpeed * 0.5f
                                            );

                                            explodingBallsCreated++;
                                            if (explodingBallsCreated >= MAX_EXPLODING_BALLS) break;
                                        }
                                    }

                                    blocks[row][col].active = false;
                                    break;
                                }

                                // increase ball-speed by 10% when "block" is hit
                                ballSpeed *= 1.1f;
                                if (ballSpeed > MAX_BALL_SPEED) {
                                    ballSpeed = MAX_BALL_SPEED;
                                }
                                ballVelocity = ScaleVector2(NormalizeVector2(ballVelocity), ballSpeed);
                                break;
                            }
                        }
                    }
                }

                // Update exploding balls logic
                for (int i = 0; i < MAX_EXPLODING_BALLS; i++) {
                    if (explodingBalls[i].active) {
                        explodingBalls[i].position.x += explodingBalls[i].velocity.x * delta_time;
                        explodingBalls[i].position.y += explodingBalls[i].velocity.y * delta_time;

                        // Wall collisions for exploding balls
                        if (explodingBalls[i].position.x - explodingBalls[i].radius <= 0 || explodingBalls[i].position.x + explodingBalls[i].radius >= SCREEN_WIDTH) {
                            explodingBalls[i].velocity.x *= -1;
                        }
                        if (explodingBalls[i].position.y - explodingBalls[i].radius <= 0 || explodingBalls[i].position.y + explodingBalls[i].radius >= SCREEN_HEIGHT) {
                            explodingBalls[i].velocity.y *= -1;
                        }

                        // Block collisions for exploding balls
                        for (int row = 0; row < BLOCK_ROWS; row++) {
                            for (int col = 0; col < BLOCK_COLUMNS; col++) {
                                if (blocks[row][col].active) {
                                    if (CheckCollisionCircles(explodingBalls[i].position, explodingBalls[i].radius, blocks[row][col].position, blocks[row][col].radius)) {
                                        blocks[row][col].number--;
                                        blocks[row][col].radius *= 0.75f;
                                        explodingBalls[i].hitsRemaining--;

                                        // change the block's color
                                        switch (blocks[row][col].number) {
                                            case 3:
                                                blocks[row][col].color = GREEN;
                                                break;
                                            case 2:
                                                blocks[row][col].color = YELLOW;
                                                break;
                                            case 1:
                                                blocks[row][col].color = RED;
                                                break;
                                        }

                                        // delete block after number reaches 0
                                        if (blocks[row][col].number <= 0) {
                                            blocks[row][col].active = false;
                                        }

                                        if (explodingBalls[i].hitsRemaining <= 0) {
                                            explodingBalls[i].active = false;
                                        }
                                    }
                                }
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
                        if (!ColorsEqual(blocks[row][col].color, TEAL) && !ColorsEqual(blocks[row][col].color, PURPLE)) {
                            DrawText(TextFormat("%d", blocks[row][col].number),
                                blocks[row][col].position.x - MeasureText(TextFormat("%d", blocks[row][col].number), 20) / 2,
                                blocks[row][col].position.y - 10, 20, WHITE);
                        }
                    }
                }
            }

            // Draw Life Counter
            DrawText(TextFormat("Lives: %d", lives), 10, 10, 20, WHITE);

            // Draw Paddle
            DrawRectangleRec(paddle, WHITE);

            // Draw Ball
            DrawCircleV(ballPosition, ballRadius, WHITE);

            // Draw Exploding Balls
            for (int i = 0; i < MAX_EXPLODING_BALLS; i++) {
                if (explodingBalls[i].active) {
                    DrawCircleV(explodingBalls[i].position, explodingBalls[i].radius, WHITE);
                }
            }
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}