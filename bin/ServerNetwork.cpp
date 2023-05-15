#include "declarations.h"

Network::Network(unsigned int userPort)
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
        WSACleanup();
        return;
    }
    else
    {
        Log("Winsock ddl found");
        Log("Status: " << wsaData.szSystemStatus);
    }


    // Init Socket
    Log("--Socket setup--");

    serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        Log("Error at socket: " << WSAGetLastError());
        WSACleanup();
        return;
    }
    else
        Log("Socket is OK");


    // Init Bind
    Log("--Bind setup--");

    // If input port is 0 use default port(1313)
    if(userPort)
        port = userPort;

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = htonl(INADDR_ANY);
    service.sin_port = htons(port);
    if(bind(serverSocket, (SOCKADDR*)&service, sizeof(service))== SOCKET_ERROR)
    {
        Log("Bind failed: " << WSAGetLastError());
        closesocket(serverSocket);
        Log("Stopping winsock");
        return;
    }
    else
        Log("Bind is on port " << port << " is OK");

    // Listen
    Log("--Listen--");

    if(listen(serverSocket, 1)== SOCKET_ERROR)
    {
        Log("Listen error: " << WSAGetLastError());
        Log("Stopping winsock");
        return;
    }
    else
        Log("Listen is OK, waiting for connections...");

    // Client connect loop
    while (true)
    {
        // Accept Socket
        Log("--Waiting for new connection--");

        SOCKET acceptSocket;
        acceptSocket = accept(serverSocket, NULL, NULL);
        if (acceptSocket == INVALID_SOCKET)
        {
            Log("Accept failed: " << WSAGetLastError());
            closesocket(acceptSocket);
            Log("Stopping winsock");
            return;
        }
        Log("Accpeted connection: " << acceptSocket);

        // Create thread for client handling
        std::thread clientT(&Network::HandleClient,this , acceptSocket);
        clientT.detach();
    }
}

Network::~Network()
{
    // Clean up winsock
    WSACleanup();
}

struct Network::ClientInfo
{
    SOCKET Socket;
};

char * Network::ReceivePacket(int index)
{
    // Receive
    //Log("--Receive--");

    char* buffer = new char[maxPacketSize];

    int bytesRecv;

    bytesRecv = recv(clients[index].Socket, buffer, maxPacketSize, 0);
    
    if (bytesRecv > 0)
    {
        // Check if the received packet exceeds the buffer size
        if (bytesRecv > maxPacketSize)
        {
            // Handle client
            Disconnect(index, "Packet exceeds buffer size");
            delete[] buffer;
            return nullptr;
        }

        Log("Bytes received: "<< bytesRecv << " Client: " << clients[index].Socket);

        return buffer;
    }
    else
    {
        Disconnect(index, "Received empty");

        // Delete buffer received
        delete[] buffer;
        return nullptr;
    }
}

bool Network::SendPacket(SOCKET clientSocket, char* buffer, int size)
{
    // Send
    //Log("--Send--");

    int tryCount = 0;
    int bytesSent = 0;

    // Check if buffer exists and try 5 times if fail
    if (buffer != nullptr)
        while (tryCount < 5)
        {
            int bytesSent;

            bytesSent = send(clientSocket, buffer, size, 0);

            if (bytesSent > 0)
            {
                Log("Bytes sent: "<< bytesSent << " to client: " << clientSocket);

                // Delete buffer received
                delete[] buffer;
                return true;
            }
            else
            {
                Log("Error sending packet to: " << clientSocket << " Error: " << WSAGetLastError());
                tryCount ++;
            }
        }

    // Delete buffer received
    delete[] buffer;
    return false;
}

void Network::Disconnect(int index, std::string reason)
{
    Log("Client disconnected: " << clients[index].Socket << " - " << reason << std::endl);

    // Client info and socket cleanup
    closesocket(clients[index].Socket);
    clients.erase(clients.begin() + index);
}


void Network::HandleClient(SOCKET clientSocket)
{
    // Create instance to store info
    ClientInfo Client;
    int clientIndex;

    clients.push_back(Client);
    clientIndex = clients.size() - 1;

    // Store client info
    clients[clientIndex].Socket = clientSocket;

    // Send MaxPacketSize
    std::string temp = std::to_string(maxPacketSize);
    char* maxSize = new char [temp.size()];
    memcpy(maxSize, temp.data(), temp.size());
    SendPacket(clientSocket, maxSize, temp.size());
    
    // Send recieved packet to all users except client
    while (true)
    {
        // Listen for packet
        char* message = ReceivePacket(clientIndex);
        if (message == nullptr)
            return;

        // Check if header is ping
        char* headerDescription = unpackDescription(message);
        
        if (headerDescription[11] == '9')
        {
            Log("Client: " << clientSocket << " requested ping");
            SendPacket(clientSocket, message, unpackSize(message));
            continue;
        }

        // Send packet to all except client if multiple clients
        if(clients.size() > 1)
        {
            for (int i = 0; i <= clients.size() -1; i++)
                if (i != clientIndex)
                    if (!SendPacket(clients[i].Socket, message, unpackSize(message)))
                    {
                        Disconnect(i, "Client not found");
                        return;
                    }
        }
    }

    Disconnect(clientIndex, "End of handle");
}