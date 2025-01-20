#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PADDLE_SPEED 500.0f

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pixel Breaker");
    SetTargetFPS(120);

    Rectangle paddle = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 30, 100, 20};
    Vector2 ballPosition = {SCREEN_WIDTH / 2, SCREEN_HEIGHT /2};
    Vector2 ballVelocity = {200.0f, -200.0f};
    float ballRadius = 10.0f;

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

        BeginDrawing();
        ClearBackground(BLACK);

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