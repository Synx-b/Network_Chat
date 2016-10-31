/* Client CPP */

#include "Client.h"

Client::Client(std::string IP, int PORT)
{

	// WSA Initalization
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	addr.sin_addr.s_addr = inet_addr(IP.c_str());  // Address to Connect To
	addr.sin_port = htons(PORT);  // Port
	addr.sin_family = AF_INET;  // IPv4
	clientptr = this;
}

bool Client::Connect()
{
	Connection = socket(AF_INET, SOCK_STREAM, NULL);  // Set Socket we will connect using
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0)  // If we cannot connect
	{
		MessageBoxA(NULL, "[ Failed To Connect ]", "Error", MB_OK | MB_ICONERROR);
		return false;  // Failed to Connect
	}
	std::cout << "[ Connected ]" << std::endl;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL);  // Create Thread for handling the clients connection to the server 

}

bool Client::CloseConnection()
{
	if (closesocket(Connection) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAENOTSOCK)  // If the socket is already closed
			return true;  // Close the Connection
		std::string ErrorMessage = "[ Failed to Close the Socket : " + std::to_string(WSAGetLastError()) + " ]";
		MessageBoxA(NULL, ErrorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
	}
	return true;
}

bool Client::ProcessPacket(Packet packettype)
{
	switch (packettype)
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
		std::cout << "[ Unrecognized Packet : " << packettype << " ]" << std::endl;
		break;
	}
	return true;
}

void Client::ClientThread()
{
	Packet PacketType;
	while (true)
	{
		if (!clientptr->GetPacketType(PacketType))  // Get the Packet Type
			break;  // If there is an issue with getting the packet type then exit the loop
		if (!clientptr->ProcessPacket(PacketType))  // If the packet if not properly processed
			break;  // Break out of our client handler loop
	}
	std::cout << "[ Connection Lost To Server ]" << std::endl;
	if (clientptr->CloseConnection())
	{
		std::cout << "[ Socket To Server Closed Correctly ]" << std::endl;
	}
	else 
	{
		std::cout << "[ Socket To Server Was Not Closed Correctly ]" << std::endl;
	}
}

bool Client::SendInt(int _int)
{
	int RetnCheck = send(Connection, (char*)&_int, sizeof(int), NULL);  // Send int: _int
	if (RetnCheck == SOCKET_ERROR)  // If in failed due to connection error
		return false;  // Connection Issue
	return true;  // Int successfully sent
}

bool Client::SendPacketType(Packet _packettype)
{
	int RetnCheck = send(Connection, (char*)&_packettype, sizeof(int), NULL);  // Send packet
	if (RetnCheck == SOCKET_ERROR)  // If it failed due to connection error
		return false;  // Connection Issue
	return true;  // Packet sent correctly
}

bool Client::SendString(std::string _string)
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

bool Client::GetInt(int & _int)
{
	int RetnCheck = recv(Connection, (char*)&_int, sizeof(int), NULL);  // Recieve interger
	if (RetnCheck == SOCKET_ERROR)  // If it failed due to connection error
		return false;  // Connection Issue
	return true;  // If we were successful in retrieving the int
}

bool Client::GetPacketType(Packet & _packettype)
{
	int RetnCheck = recv(Connection, (char*)&_packettype, sizeof(Packet), NULL);  // Recieve packet
	if (RetnCheck == SOCKET_ERROR)  // If packettype failed to send due to connection issue
		return false;  // Return false : Connection issue
	return true;  // Packet successfully recieved
}

bool Client::GetString(std::string & _string)
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
