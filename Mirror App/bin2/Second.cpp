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

    Ball1.x = 150;
    Ball1.y = 150;

    Ball2.x = 150;
    Ball2.y = 150;

    while(!WindowShouldClose())
    {
        char buffer [50];

        Net.Receive(buffer);

        Net.unpackData(buffer, Ball2, Net.unpackSize(buffer));

        Log(Ball2.x << " " << Ball2.y );

        BeginDrawing();
        ClearBackground(WHITE);

        DrawCircleV(Ball2, 10, BLUE);

        EndDrawing();
    }

    Net.Disconnect();

}
