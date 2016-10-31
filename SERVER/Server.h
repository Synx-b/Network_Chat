/* Server Header */

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include "Packets.h"

class Server
{
public:
	Server(int, bool);
	bool ListenForConnection();

	// Recieve Data
	bool GetInt(int, int&);
	bool GetPacketType(int, Packet&);
	bool GetString(int, std::string&);

	// Send Data
	bool SendInt(int, int&);
	bool SendPacketType(int, Packet&);
	bool SendString(int, std::string&);

	// Processing
	bool ProcessPacket(int, Packet);
	static void ClientHandlerThread(int);

private:
	SOCKET Connections[100];
	int ConnectionsCounter = 0;
};
Server * serverptr;