/* Client Solution*/

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string>

SOCKET Connection;

enum Packet
{
	P_ChatMessage
};

bool SendInt(int _int)
{
	int RetnCheck = send(Connection, (char*)&_int, sizeof(int), NULL);  // Send int: _int
	if (RetnCheck == SOCKET_ERROR)  // If in failed due to connection error
		return false;  // Connection Issue
	return true;  // Int successfully sent
}

bool GetInt(int & _int)
{
	int RetnCheck = recv(Connection, (char*)&_int, sizeof(int), NULL);  // Recieve interger
	if (RetnCheck == SOCKET_ERROR)  // If it failed due to connection error
		return false;  // Connection Issue
	return true;  // If we were successful in retrieving the int
}

bool SendPacketType(Packet _packettype)
{
	int RetnCheck = send(Connection, (char*)&_packettype, sizeof(int), NULL);  // Send packet
	if (RetnCheck == SOCKET_ERROR)  // If it failed due to connection error
		return false;  // Connection Issue
	return true;  // Packet sent correctly
}

bool GetPacketType(Packet _packettype)
{
	int RetnCheck = recv(Connection, (char*)&_packettype, sizeof(Packet), NULL);  // Recieve packet
	if (RetnCheck == SOCKET_ERROR)  // If packettype failed to send due to connection issue
		return false;  // Return false : Connection issue
	return true;  // Packet successfully recieved
}

bool SendString(std::string & _string)
{
	if (!SendPacketType(P_ChatMessage))  // Send packet type; Chat message
		return false;  // Failed to send String
	int bufferLength = _string.size();  // Get string buffer length
	if (!SendInt(bufferLength))  // Send length of the string buffer
		return false;  // Failed to send the length of the string buffer
	int RetnCheck = send(Connection, _string.c_str(), bufferLength, NULL);  // Send string buffer
	if (RetnCheck == SOCKET_ERROR)  // If connnection failed
		return false;  // String not sent
	return true;  // String successfully sent
}

bool GetString(std::string & _string)
{
	int bufferLength;  // Integer to hold the length of the message
	if (!GetInt(bufferLength))  // Get length of buffer
		return false;  // Failed to get buffer length
	char* buffer = new char[bufferLength + 1];  // Create a buffer of the size of the data
	buffer[bufferLength] = '\0';  // Set last character to a null terminating so we dont print info we dont want
	int RetnCheck = recv(Connection, buffer, bufferLength, NULL);  // Recieve the message itself
	_string = buffer;  // Set the string to the buffer
	delete[] buffer;  // Deallocate buffer
	if (RetnCheck == SOCKET_ERROR)  // If connection is lost while getting message
		return false;  // If there is an issue with the connection
	return true;  // String was successfuly recieved
}

bool ProcessPacket(Packet packetType)
{
	switch (packetType)
	{
	case P_ChatMessage:
	{
		std::string Message;  // String to hold the message
		if (!GetString(Message))  // Get the chat message
			return false;  // If we cannot get the chat message
		std::cout << Message << std::endl;  // Display the Message to the user
		break;
	}
	default:
		std::cout << "[ Unrecognized Packet : " << packetType << " ]" << std::endl;
		break;
	}
	return true;
}

void ClientThread()
{
	Packet packetType = P_ChatMessage;
	while (true)
	{
		if (!GetPacketType(packetType))  // Get the Packet Type
			break;  // If there is an issue with getting the packet type then exit the loop
		if (!ProcessPacket(packetType))  // If the packet if not properly processed
			break;  // Break out of our client handler loop
	}
	std::cout << "[ Lost Connection to Server ]" << std::endl;
	closesocket(Connection);
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
	int sizeofaddr = sizeof(addr);  // Length of the address needed for connect method
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");   // LocalHost
	addr.sin_port = htons(1111);  // Port
	addr.sin_family = AF_INET;  // IPv4

	Connection = socket(AF_INET, SOCK_STREAM, NULL);  // Set Socket we will connect using
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0)  // If we cannot connect
	{
		MessageBoxA(NULL, "[ Failed To Connect ]", "Error", MB_OK | MB_ICONERROR);
		return 0;  // Failed to Connect
	}
	std::cout << "[ Connected ]" << std::endl;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL);  // Create Thread for handling the clients connection to the server 

	std::string buffer;
	while (true)
	{
		std::getline(std::cin, buffer);  // Get line if the user entered anything
		if (!SendString(buffer))  // Send the string to the server, if it fails to send then break out of the loop
			break;
		Sleep(10);
	}

	system("pause");
	return 0;
}