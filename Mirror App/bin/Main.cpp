#include "declarations.h"
#define RAYGUI_IMPLEMENTATION


int main(int argc, char *argv[])
{
    Network Net;
    Net.Connect("127.0.0.1", 1313);

    const int screenWidth = 300;
    const int screenHeight = 300;
    InitWindow(screenWidth, screenHeight, "Mirror App");
    SetWindowState(FLAG_VSYNC_HINT);
    SetExitKey(KEY_NULL);

    SetTargetFPS(30);

    Vec2 Ball1;
    Vec2 Ball2;

    Ball1.x = 50;
    Ball1.y = 50;

    Ball2.x = 150;
    Ball2.y = 150;

    while(!WindowShouldClose())
    {
        Log("Ball 1: " << Ball1.x << " " << Ball1.y );

        char* buffer = Net.packDataWithHeader(Ball1, "100000000000");
        
        Net.Send(buffer, 1);

        if (IsKeyDown(KEY_UP))
            Ball1.y -= 5;

        if (IsKeyDown(KEY_DOWN))
            Ball1.y += 5;

        if (IsKeyDown(KEY_RIGHT))
            Ball1.x += 5;

        if (IsKeyDown(KEY_LEFT))
            Ball1.x -= 5;

        BeginDrawing();
        ClearBackground(WHITE);

        DrawCircleV(Ball1, 10, RED);

        EndDrawing();
    }

    Net.Disconnect();

}
