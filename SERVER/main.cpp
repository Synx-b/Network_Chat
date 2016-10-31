/* Server Solution*/

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>

SOCKET Connections[100];
int ConnectionCounter = 0;

enum Packet
{
	P_ChatMessage
};

bool SendInt(int ID, int _int)
{
	int RetnCheck = send(Connections[ID], (char*)&_int, sizeof(int), NULL);  // Send int: _int
	if (RetnCheck == SOCKET_ERROR)  // If in failed due to connection error
		return false;  // Connection Issue
	return true;  // Int successfully sent
}

bool GetInt(int ID, int & _int)
{
	int RetnCheck = recv(Connections[ID], (char*)&_int, sizeof(int), NULL);  // Recieve interger
	if (RetnCheck == SOCKET_ERROR)  // If it failed due to connection error
		return false;  // Connection Issue
	return true;  // If we were successful in retrieving the int
}

bool SendPacketType(int ID, Packet _packettype)
{
	int RetnCheck = send(Connections[ID], (char*)&_packettype, sizeof(int), NULL);  // Send packet
	if (RetnCheck == SOCKET_ERROR)  // If it failed due to connection error
		return false;  // Connection Issue
	return true;  // Packet sent correctly
}

bool GetPacketType(int ID, Packet _packettype)
{
	int RetnCheck = recv(Connections[ID], (char*)&_packettype, sizeof(Packet), NULL);  // Recieve packet
	if (RetnCheck == SOCKET_ERROR)  // If packettype failed to send due to connection issue
		return false;  // Return false : Connection issue
	return true;  // Packet successfully recieved
}

bool SendString(int ID, std::string & _string)
{
	if (!SendPacketType(ID, P_ChatMessage))  // Send packet type; Chat message
		return false;  // Failed to send String
	int bufferLength = _string.size();  // Get string buffer length
	if (!SendInt(ID, bufferLength))  // Send length of the string buffer
		return false;  // Failed to send the length of the string buffer
	int RetnCheck = send(Connections[ID], _string.c_str(), bufferLength, NULL);  // Send string buffer
	if (RetnCheck == SOCKET_ERROR)  // If connnection failed
		return false;  // String not sent
	return true;  // String successfully sent
}

bool GetString(int ID, std::string & _string)
{
	int bufferLength;  // Integer to hold the length of the message
	if (!GetInt(ID, bufferLength))  // Get length of buffer
		return false;  // Failed to get buffer length
	char* buffer = new char[bufferLength + 1];  // Create a buffer of the size of the data
	buffer[bufferLength] = '\0';  // Set last character to a null terminating so we dont print info we dont want
	int RetnCheck = recv(Connections[ID], buffer, bufferLength, NULL);  // Recieve the message itself
	_string = buffer;  // Set the string to the buffer
	delete[] buffer;  // Deallocate buffer
	if (RetnCheck == SOCKET_ERROR)  // If connection is lost while getting message
		return false;  // If there is an issue with the connection
	return true;  // String was successfuly recieved
}

bool ProcessPacket(int ID, Packet packetType)
{
	switch (packetType)
	{
	case P_ChatMessage:  // Packet type : Chat Message
	{
		std::string message;
		if (!GetString(ID, message))  // Get the clients message
			return false;  // If we cannot get the message then return false

		for (int i = 0; i < ConnectionCounter; i++)
		{
			if (i == ID)  // If connection is the user who sent the message
				continue;  // Skip to the next user
			if (!SendString(i, message))
			{
				std::cout << "[ Message not sent from client {" << ID << "} ]" << std::endl;
			}
		}
		std::cout << "[ Processed Message From Client {" << ID << "} ]" << std::endl;
		break;
 	}
	default:
		std::cout << "[ Unrecognized Packet : " << packetType << " ]" << std::endl;
		break;
	}
	return true;
}

void ClientHandlerThread(int ID)  // ID = the index in SOCKET Connections Array
{
	Packet PacketType = P_ChatMessage;  // Getting the Packet Type
	while (true)
	{
		if (!GetPacketType(ID, PacketType))  // Getting the packet type
			break;  // If packet is not got, then exit the loop
		if (!ProcessPacket(ID, PacketType))  // Process the Packet
			break;  // Break out if the packet is not properly process
	}
	std::cout << "[ Lost Connection to Client {" << ID << "} ]" << std::endl;
	closesocket(Connections[ID]);
}

int main()
{
	// WSA Initalization
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	SOCKADDR_IN addr;  // Address we willbind our listening socket to
	int addrlen = sizeof(addr);  // Length of the address
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");   // LocalHost
	addr.sin_port = htons(1111);  // Port
	addr.sin_family = AF_INET;  // IPv4

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);  // Create socket to listen for new connections
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));  // Bind address to the socket
	listen(sListen, SOMAXCONN);  // Place sListen socket in a state where it is listening for new connections

	std::cout << "[ Waiting For Connections ]" << std::endl;

	SOCKET newConnection;  // Socket to hold the connecting users connection

	for (int i = 0; i < 100; i++)
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen);  // Accept the new Connection
		if (newConnection == 0)
			std::cout << "[ Failed accepting Clients Connection ]" << std::endl;
		else
		{
			std::cout << "[ Client Connected ]" << std::endl;
			Connections[i] = newConnection;  // Add the current connection to the array of connections
			ConnectionCounter++;  // Increment total # of clients that have connected
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL);  // Create a thread to handle all the incoming connections
			std::string MOTD = "[ MOTD ] : Zebra's are Black and White";  // Create buffer with a MOTD
			SendString(i, MOTD);
		}
	}
	system("pause");
	return 0;
}