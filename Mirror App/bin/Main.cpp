#include "declarations.h"
#define RAYGUI_IMPLEMENTATION

void Listener(Network& Net, std::queue<char*>& packetQueue);

int main(int argc, char* argv[])
{
    Network Net;
    Net.Connect("127.0.0.1", 1313);

    const int screenWidth = 300;
    const int screenHeight = 300;
    InitWindow(screenWidth, screenHeight, "Main");
    SetWindowState(FLAG_VSYNC_HINT);
    SetExitKey(KEY_NULL);

    SetTargetFPS(30);

    std::queue<char*> packetQueue;
    std::thread Rec(Listener, std::ref(Net), std::ref(packetQueue));
    Rec.detach();

    Vec2 Ball1;
    Ball1.x = 50;
    Ball1.y = 50;

    Vec2 Ball2;
    Ball2.x = 250;
    Ball2.y = 250;

    while (!WindowShouldClose())
    {
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

        DrawCircleV(Ball2, 10, BLUE);
        DrawCircleV(Ball1, 10, RED);

        // Process received packets in the queue
        while (!packetQueue.empty())
        {
            char* receivedBuffer = packetQueue.front();
            // Process the received packet
            
            Net.unpackData(receivedBuffer, Ball2, Net.unpackSize(receivedBuffer));

            // Delete the received packet
            delete[] receivedBuffer;
            packetQueue.pop();
        }

        EndDrawing();
    }

    Net.Disconnect();
}

void Listener(Network& Net, std::queue<char*>& packetQueue)
{
    while (true)
    {
        char recBuffer[Net.GetMaxPacketSize()];
        if (Net.Receive(recBuffer))
        {
            // Add the received packet to the queue
            char* receivedPacket = new char[Net.GetMaxPacketSize()];
            memcpy(receivedPacket, recBuffer, Net.unpackSize(recBuffer));
            packetQueue.push(receivedPacket);
        }
    }
}
