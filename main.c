#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PADDLE_SPEED 500.0f

#define BLOCK_ROWS 5
#define BLOCK_COLUMNS 10
float ballSpeed = 200.0f;
#define BLOCK_RADIUS (ballRadius *3)
#define MAX_BALL_SPEED 1500.0f

typedef struct {
    int number;
    float radius;
    Color color;
    bool active;
    Vector2 position;
} Block;

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pixel Breaker");
    SetTargetFPS(120);

    Rectangle paddle = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 30, 100, 20};
    Vector2 ballPosition = {SCREEN_WIDTH / 2, SCREEN_HEIGHT /2};
    Vector2 ballVelocity = Vector2Scale(Vector2Normalize((Vector2){200.0f, -200.0f}), ballSpeed);
    float ballRadius = 10.0f;

    //create blocks
    Block blocks[BLOCK_ROWS][BLOCK_COLUMNS];
    for (int row = 0; row < BLOCK_ROWS; row++) {
        for (int col = 0; col < BLOCK_COLUMNS; col++) {
            blocks[row][col].number = 4;
            blocks[row][col].radius = BLOCK_RADIUS;
            blocks[row][col].color = BLUE;
            blocks[row][col].active = true;

            //random positioning of blocks to make it harder
            blocks[row][col].position.x = GetRandomValue(BLOCK_RADIUS, SCREEN_WIDTH - BLOCK_RADIUS);
            blocks[row][col].position.y = GetRandomValue(BLOCK_RADIUS, SCREEN_HEIGHT / 2 - BLOCK_RADIUS);
        }
    }

    //settup key's to play
    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();

        if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) && paddle.x > 0) {
            paddle.x -= PADDLE_SPEED * delta_time;
        }
        if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) && paddle.x < SCREEN_WIDTH - paddle.width) {
            paddle.x += PADDLE_SPEED * delta_time;
        }

        //Ball Velocity
        ballPosition.x += ballVelocity.x * delta_time;
        ballPosition.y += ballVelocity.y * delta_time;

        //Wall Collisions
        if (ballPosition.x - ballRadius <= 0 || ballPosition.x + ballRadius >= SCREEN_WIDTH) {
            ballVelocity.x *= -1;
        }
        //Reverse Vertical Velocity when hitting top or bottom
        if (ballPosition.y - ballRadius <= 0 || ballPosition.y + ballRadius >= SCREEN_HEIGHT) {
            ballVelocity.y *=-1;
        }

        //checks collision and reverses the balls direction when paddle collision occurs
        if (CheckCollisionCircleRec(ballPosition, ballRadius, paddle)) {
            ballVelocity.y *= -1;

            //ball hitposition on paddle
            float hitPosition = (ballPosition.x - (paddle.x + paddle.width / 2)) / paddle.width / 2;
            ballVelocity.x = hitPosition * 300.0f;
        }

        //this handles block collisions
        for (int row = 0; row < BLOCK_ROWS; row++) {
            for (int col = 0; col < BLOCK_COLUMNS; col++) {
                if (blocks[row][col].active) {
                    float blockX = col * (BLOCK_RADIUS * 2) + BLOCK_RADIUS;
                    float blockY = row * (BLOCK_RADIUS * 2) + BLOCK_RADIUS;

                    //delete block after number reaches 0
                    if (blocks[row][col].number <= 0) {
                        blocks[row][col].active = false;
                    }

                    if (CheckCollisionCircles(ballPosition, ballRadius, blocks[row][col].position, blocks[row][col].radius)) {
                        blocks[row][col].number--;
                        blocks[row][col].radius *=0.75f;
                        ballVelocity.y *= -1;

                        // this changes the color of the blocks depending on  wich number it is
                        switch (blocks[row][col].number) {
                            case 3: blocks[row][col].color = GREEN; break;
                            case 2: blocks[row][col].color = YELLOW; break;
                            case 1: blocks[row][col].color = RED; break;
                        }

                        //increase ballspeed by 10% when "block" is hit.
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

        BeginDrawing();
        ClearBackground(BLACK);

            // Draw block circles
            for (int row = 0; row < BLOCK_ROWS; row++) {
                for (int col = 0; col < BLOCK_COLUMNS; col++) {
                    if (blocks[row][col].active) {
                        float blockX = col * (BLOCK_RADIUS * 2) + BLOCK_RADIUS;
                        float blockY = row * (BLOCK_RADIUS * 2) + BLOCK_RADIUS;
                        if (blocks[row][col].active) {
                            DrawCircle(blocks[row][col].position.x, blocks[row][col].position.y, blocks[row][col].radius, blocks[row][col].color);
                            DrawText(TextFormat("%d", blocks[row][col].number),
                                     blocks[row][col].position.x - MeasureText(TextFormat("%d", blocks[row][col].number), 20) / 2,
                                     blocks[row][col].position.y - 10, 20, WHITE);
                        }
                    }
                }
            }

        //MeasureText calculated text width in pixels for the font size, used to center text.
        DrawText("Welcome!", SCREEN_WIDTH / 2 - MeasureText("Welcome!", 40) / 2, SCREEN_HEIGHT/2 - 60, 40, WHITE);
        DrawText("To", SCREEN_WIDTH / 2 - MeasureText("To", 30)/2, SCREEN_HEIGHT / 2,30,WHITE);
        DrawText("PIXEL BREAKER!!!", SCREEN_WIDTH/2 - MeasureText("PIXEL BREAKER!!!", 50)/2, SCREEN_HEIGHT /2 + 40,50,RED);

        //Draw Paddle
        DrawRectangleRec(paddle, WHITE);

        //Draw Ball
        DrawCircleV(ballPosition, ballRadius, WHITE);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
