#include "declarations.h"

Network::Network()
{
	// Init Winsock
    Log("--Winsock ddl setup--");

    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2,2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaerr != 0)
    {
        Log("Winsock ddl not found");
        Log("Stopping winsock");
        return;
    }
    else
    {
        Log("Winsock ddl found");
        Log("Status: " << wsaData.szSystemStatus);
    }
}

Network::~Network()
{
    if (clientSocket != INVALID_SOCKET)
        Disconnect();

    // Clean up winsock
    closesocket(clientSocket);
    WSACleanup();
}

bool Network::Connect(std::string ip, unsigned int port)
{
    // Init Socket
    Log("--Socket setup--");

    clientSocket = INVALID_SOCKET;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        Log("Error at socket: " << WSAGetLastError());
        return false;
    }
    else
        Log("Socket is OK");


    // Connect
    Log("--Connect--");

    sockaddr_in service;
    service.sin_family = AF_INET;
    InetPton(AF_INET, _T(const_cast<char*>(ip.c_str())), &service.sin_addr.s_addr);
    service.sin_port = htons(port);
    if(connect(clientSocket, (SOCKADDR*)&service, sizeof(service))== SOCKET_ERROR)
    {
        Log("Connection failed: " << WSAGetLastError());
        return false;
    }
    else
    {
        Log("Connection to " << ip << " at " << port << " is OK");

        // Get packet size as first server msg
        maxPacketSize = std::stoi(ReceivePacket());

        Log("Max packet size from server: " << maxPacketSize);

        return true;
    }
}

void Network::Disconnect()
{
    // Disconnect 
    Log("--Disconnect--");

    int result = shutdown(clientSocket, SD_BOTH);
    if (result == SOCKET_ERROR)
        Log("Error disconnecting: " << WSAGetLastError());
    else
        Log("Disconnected");

    closesocket(clientSocket);
}

bool Network::SendPacket(char* buffer, int size)
{
    // Send
    //Log("--Send--");

    int tryCount = 0;
    int bytesSent = 0;

    if (size > maxPacketSize)
    {
        Log("Packet too large");
        return false;
    }

    // Check if buffer exists and try 5 times if fail
    if (buffer != nullptr)
        while (tryCount < 5)
        {
            bytesSent = send(clientSocket, buffer, size, 0);
            if (bytesSent > 0)
            {
                Log("Bytes sent: "<< bytesSent);
                return true;
            }
            else
            {
                Log("Error sending packet");
                tryCount ++;
            }
        }
    return false;
}

char* Network::ReceivePacket()
{
    // Receive
    Log("--Receive--");

	char* buffer = new char[maxPacketSize + 1];

	int bytesRecv = recv(clientSocket, buffer, maxPacketSize, 0);

	if (bytesRecv > 0)
    {
        // Check if the received packet exceeds the buffer size
        if (bytesRecv > maxPacketSize)
        {
            delete[] buffer;
            return nullptr;
        }

        // Add nullbyte
        buffer[bytesRecv] = '\0';
        return buffer;
    }
    else
    {
        Log("Received empty");
        Disconnect();

        // Delete buffer
        delete[] buffer;
        return nullptr;
    }
}

bool Network::Send(char* buffer, bool deleteFlag)
{
    bool result;

    // Input buffer size
    int size = unpackSize(buffer);

    result = SendPacket(buffer, size);

    if (deleteFlag)
        delete[] buffer;

    return result;
}

bool Network::Receive(char* buffer)
{
    // Receive and check if nullptr
    char* receiveBuffer = ReceivePacket();
    if (receiveBuffer == nullptr)
        return false;

    // Get size
    int size = unpackSize(receiveBuffer);

    // Check if input buffer is large enough
    if (strlen(buffer) <= size)
        {
        delete[] receiveBuffer;
        return false;
        }

    // Copy and delete
    strcpy_s(buffer, size, receiveBuffer);

    // Add nullbyte
    buffer[size] = '\0';

    delete[] receiveBuffer;
    return true;
}