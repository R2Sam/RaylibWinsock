class Network
{
private:
	// Class variables
	unsigned int port;
	int maxPacketSize = 4096;
	std::string ip;
	SOCKET clientSocket;

	// Class data structures


	// Class Functions
		// Packets
	bool SendPacket(char* buffer, int size);
	char* ReceivePacket();

public:
	// Constructer and deconstructer
	Network();
	~Network();

	// Connection
	bool Connect(std::string ip, unsigned int port);
	void Disconnect();

	// Transmission buffer
	bool Send(char* buffer, bool deleteFlag);
	bool Receive(char* buffer);

	//Other
	int GetMaxPacketSize();

	// Packer
	#include "ClientPacker.h"
};
