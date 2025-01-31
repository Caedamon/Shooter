#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "file_io.h"

extern HighScoreEntry highScore;

// Screen dimensions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Game constants
#define PADDLE_SPEED 650.0f
#define BLOCK_ROWS 5
#define BLOCK_COLUMNS 10
#define MAX_BALL_SPEED 1000.0f
#define MAX_EXPLODING_BALLS 10

// Custom colors
#undef PURPLE
#define PURPLE (Color){128, 0, 128, 255} // RGBA for Purple
#define TEAL (Color){0, 128, 128, 255}   // RGBA for Teal

// Structs
typedef struct {
    int number;
    float radius;
    Color color;
    bool active;
    Vector2 position;
    int score;
} Block;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float radius;
    int hitsRemaining;
    bool active;
    int scoreMultiplier;
} ExplodingBall;

typedef struct ExplodingBallNode {
    ExplodingBall ball;
    struct ExplodingBallNode *next;
} ExplodingBallNode;

extern ExplodingBallNode *explodingBallList;

// Extern global variables (shared across files)
extern Block blocks[BLOCK_ROWS][BLOCK_COLUMNS];
extern Rectangle paddle;
extern Vector2 ballPosition;
extern Vector2 ballVelocity;
extern float ballRadius;
extern float ballSpeed;
extern int lives;

// Function prototypes
void InitGame(void);
void UpdateGame(bool *gameOver, bool *win, bool *ballStuck);
void ResetGame(void);
bool ColorsEqual(Color c1, Color c2);

#endif
