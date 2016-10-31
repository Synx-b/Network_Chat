/* Client Main */

#include "Client.h"

int main()
{

	Client client("127.0.0.1", 1111);

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