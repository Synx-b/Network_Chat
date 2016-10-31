/* Client Header */

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string>

enum Packet
{
	P_ChatMessage
};

class Client
{
public:
	Client(std::string, int);
	bool Connect();
	bool CloseConnection();
private:
	bool ProcessPacket(Packet);
	static void ClientThread();

	// Data Sending Methods
	bool SendInt(int);
	bool SendPacketType(Packet);
	bool SendString(std::string);

	// Data Recieving Methods
	bool GetInt(int&);
	bool GetPacketType(Packet&);
	bool GetString(std::string&);

	// Data Members
	SOCKET Connection;  // Clients Connection
	SOCKADDR_IN addr;  // Address to be binded to the Socket
	int sizeofaddr = sizeof(addr);  // Size of the Address
};

static Client * clientptr;
