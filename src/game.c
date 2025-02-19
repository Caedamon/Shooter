#include "game.h"
#include "raylib.h"
#include "vector_math.h"
#include <math.h>
#include "file_io.h"
#include <stdlib.h>

// Definitions for paddle, ball, blocks, etc.
#define PADDLE_SPEED 650.0f
#define BLOCK_ROWS 5
#define BLOCK_COLUMNS 10
#define MAX_BALL_SPEED 1000.0f
#define BLOCK_RADIUS (ballRadius * 3)
#define MAX_EXPLODING_BALLS 10


// Global variables for game state
Block blocks[BLOCK_ROWS][BLOCK_COLUMNS];
Rectangle paddle;
Vector2 ballPosition;
Vector2 ballVelocity;
float ballRadius;
float ballSpeed;
int lives;
ExplodingBallNode *explodingBallList = NULL;
int playerScore = 0;

// Initializes the game state
void InitGame(void) {
    explodingBallList = NULL;
    paddle = (Rectangle){SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 30, 100, 20};
    ballPosition = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    ballVelocity = ScaleVector2(NormalizeVector2((Vector2){200.0f, -200.0f}), 200.0f);
    ballRadius = 10.0f;
    ballSpeed = 200.0f;
    lives = 3;

    // Initialize blocks
    for (int row = 0; row < BLOCK_ROWS; row++) {
        for (int col = 0; col < BLOCK_COLUMNS; col++) {
            blocks[row][col].number = 4;
            blocks[row][col].radius = BLOCK_RADIUS;
            blocks[row][col].color = BLUE;
            blocks[row][col].active = true;
            blocks[row][col].score = 10;

            // Randomized block positioning
            blocks[row][col].position.x = GetRandomValue(BLOCK_RADIUS, SCREEN_WIDTH - BLOCK_RADIUS);
            blocks[row][col].position.y = GetRandomValue(BLOCK_RADIUS, SCREEN_HEIGHT / 2 - BLOCK_RADIUS);

            // Special blocks (Purple = +1 life, Teal = exploding)
            int randomValue = GetRandomValue(0, 19);
            if (randomValue == 0) {
                blocks[row][col].color = PURPLE;
                blocks[row][col].number = 1;
                blocks[row][col].score = 50;
            } else if (randomValue == 1) {
                blocks[row][col].color = TEAL;
                blocks[row][col].number = -1;
                blocks[row][col].score = 0;
            }
        }
    }
}

// Updates the game state
void UpdateGame(bool *gameOver, bool *win, bool *ballStuck) {
    float delta_time = GetFrameTime();

    // Paddle movement
    if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) && paddle.x > 0) {
        paddle.x -= PADDLE_SPEED * delta_time;
    }
    if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) && paddle.x < SCREEN_WIDTH - paddle.width) {
        paddle.x += PADDLE_SPEED * delta_time;
    }

    // Ball stuck logic
    if (*ballStuck) {
        ballPosition.x = paddle.x + paddle.width / 2;
        ballPosition.y = paddle.y - ballRadius;

        if (IsKeyPressed(KEY_SPACE)) {
            *ballStuck = false;
            ballVelocity = ScaleVector2(NormalizeVector2((Vector2){200.0f, -200.0f}), ballSpeed);
        }
        return;
    }

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

    // Ball falls below screen
    if (ballPosition.y + ballRadius > SCREEN_HEIGHT) {
        lives--;
        if (lives <= 0) {
            *gameOver = true;
        } else {
            *ballStuck = true; // Reset ball to paddle
            ballSpeed = 200.0f;
        }
        return;
    }

    // Paddle collision
    if (CheckCollisionCircleRec(ballPosition, ballRadius, paddle)) {
        ballVelocity.y *= -1;

        // Adjust ball velocity based on paddle hit position
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

        ballVelocity = ScaleVector2(NormalizeVector2(ballVelocity), ballSpeed);
    }

    // Block collisions
    for (int row = 0; row < BLOCK_ROWS; row++) {
        for (int col = 0; col < BLOCK_COLUMNS; col++) {
            if (blocks[row][col].active) {
                if (CheckCollisionCircles(ballPosition, ballRadius, blocks[row][col].position, blocks[row][col].radius)) {
                    if (ColorsEqual(blocks[row][col].color, TEAL)) {
                        // Always spawn MAX_EXPLODING_BALLS, regardless of existing ones
                        for (int i = 0; i < MAX_EXPLODING_BALLS; i++) {
                            ExplodingBallNode *newNode = (ExplodingBallNode *)malloc(sizeof(ExplodingBallNode));
                            if (!newNode) break;

                            newNode->ball.active = true;
                            newNode->ball.position = blocks[row][col].position;
                            newNode->ball.radius = ballRadius / 2;
                            newNode->ball.hitsRemaining = 10;
                            newNode->ball.scoreMultiplier = 3;

                            float angle = (2 * PI / MAX_EXPLODING_BALLS) * i;
                            newNode->ball.velocity = ScaleVector2(
                                (Vector2){cosf(angle), sinf(angle)},
                                ballSpeed * 2.0f
                            );

                            newNode->next = explodingBallList;
                            explodingBallList = newNode;
                        }

                        blocks[row][col].active = false;
                        break;
                    }

                    // Update block state
                    blocks[row][col].number--;
                    blocks[row][col].radius *= 0.75f;
                    ballVelocity.y *= -1;

                    if (ColorsEqual(blocks[row][col].color, PURPLE)) {
                        lives++;
                        blocks[row][col].active = false;
                    }

                    if (blocks[row][col].number <= 0) {
                        playerScore += blocks[row][col].score;
                        blocks[row][col].active = false;
                    }

                    // Increase ball speed
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

    // Exploding ball logic
ExplodingBallNode *prev = NULL;
ExplodingBallNode *ballNode = explodingBallList;

while (ballNode != NULL) {
    ballNode->ball.position.x += ballNode->ball.velocity.x * delta_time;
    ballNode->ball.position.y += ballNode->ball.velocity.y * delta_time;

    // Wall collisions for exploding balls
    if (ballNode->ball.position.x - ballNode->ball.radius <= 0 ||
        ballNode->ball.position.x + ballNode->ball.radius >= SCREEN_WIDTH) {
        ballNode->ball.velocity.x *= -1;
    }
    if (ballNode->ball.position.y - ballNode->ball.radius <= 0 ||
        ballNode->ball.position.y + ballNode->ball.radius >= SCREEN_HEIGHT) {
        ballNode->ball.velocity.y *= -1;
    }

    // Block collisions for exploding balls
    for (int row = 0; row < BLOCK_ROWS; row++) {
        for (int col = 0; col < BLOCK_COLUMNS; col++) {
            if (blocks[row][col].active && CheckCollisionCircles(
                    ballNode->ball.position,
                    ballNode->ball.radius,
                    blocks[row][col].position,
                    blocks[row][col].radius)) {

                // Handle block collision only once per hit
                blocks[row][col].number--;
                blocks[row][col].radius *= 0.75f;
                ballNode->ball.hitsRemaining--;

                // Multiply score when an exploding ball hits
                playerScore += blocks[row][col].score * ballNode->ball.scoreMultiplier;

                // Change block color
                switch (blocks[row][col].number) {
                    case 3: blocks[row][col].color = GREEN; break;
                    case 2: blocks[row][col].color = YELLOW; break;
                    case 1: blocks[row][col].color = RED; break;
                }

                // Deactivate block if health is 0
                if (blocks[row][col].number <= 0) {
                    blocks[row][col].active = false;
                }

                // Bounce logic for exploding balls
                Vector2 collisionNormal = NormalizeVector2(
                    (Vector2){ballNode->ball.position.x - blocks[row][col].position.x,
                              ballNode->ball.position.y - blocks[row][col].position.y});

                // Prevent extreme bouncing
                Vector2 newVelocity = ReflectVector2(ballNode->ball.velocity, collisionNormal);
                if (MagnitudeVector2(newVelocity) > ballSpeed * 2.0f) {
                    newVelocity = ScaleVector2(NormalizeVector2(newVelocity), ballSpeed * 2.0f);
                }
                ballNode->ball.velocity = newVelocity;

                break;
            }
        }
    }

    // Remove the ball if it is inactive
    if (ballNode->ball.hitsRemaining <= 0) {
        ExplodingBallNode *toDelete = ballNode;
        if (prev == NULL) {
            explodingBallList = ballNode->next;
        } else {
            prev->next = ballNode->next;
        }
        ballNode = ballNode->next;
        free(toDelete);
    } else {
        prev = ballNode;
        ballNode = ballNode->next;
    }
}

    // Check win condition
    *win = true;
    for (int row = 0; row < BLOCK_ROWS; row++) {
        for (int col = 0; col < BLOCK_COLUMNS; col++) {
            if (blocks[row][col].active && blocks[row][col].number != -1) {
                *win = false;
                break;
            }
        }
        if (!*win) break;
    }
}

bool ColorsEqual(Color c1, Color c2) {
    return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a);
}

// Resets the game state
void ResetGame(void) {
    ExplodingBallNode *ballNode = explodingBallList;
    while (ballNode != NULL) {
        ExplodingBallNode *toDelete = ballNode;
        ballNode = ballNode->next;
        free(toDelete);
    }
    explodingBallList = NULL;

    InitGame();
}