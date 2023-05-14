class Network
{
private:
	// Class variables
	unsigned int port = 1313;
	int maxPacketSize = 4096;
	SOCKET serverSocket;

	// Class data structures
	struct ClientInfo;
	std::vector<ClientInfo> clients;
	std::mutex clientsMutex;

	// Class Functions
	char * ReceivePacket(int index);
	bool SendPacket(SOCKET clientSocket, char* buffer, int size);
	void Disconnect(int index, std::string reason);
	void HandleClient(SOCKET clientSocket);

	//Packer
	#include <ServerPacker.h>

public:
	// Constructer and deconstructer
	Network(unsigned int userPort);
	~Network();

};