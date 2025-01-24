#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
using std::cin;
using std::cout;
using std::endl;
//Server
#define DEFAULT_PORT	"27015"

#define BUFFER_SIZE		1500

#pragma comment(lib, "Ws2_32.lib")

void main()
{
	setlocale(LC_ALL, "");
	//1. 
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		cout << "WinSock initialization fauled with error #" << iResult << endl;
	}
	//2. Create socket:
	addrinfo* result = NULL;
	addrinfo* ptr = NULL;
	addrinfo hInst;
	//2.1 �������� ����� �������� ����
	ZeroMemory(&hInst, sizeof(hInst));
	hInst.ai_family = AF_UNSPEC;
	hInst.ai_socktype = SOCK_STREAM;
	hInst.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo("192.168.43.28", DEFAULT_PORT, &hInst, &result);
	if (iResult != 0)
	{
		cout << "getaddrinfo() failed with error #" << WSAGetLastError() << endl;
		WSACleanup();
	}

	//2.2. Create a Socket
	SOCKET ConnectSocket = socket(hInst.ai_family, hInst.ai_socktype, hInst.ai_protocol);
	if (ConnectSocket == INVALID_SOCKET)
	{
		cout << "Socket creation failed with error #" << WSAGetLastError() << endl;
		freeaddrinfo(result);
		return;
	}

	//3. Connecting to Server:
	iResult = connect(ConnectSocket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}
	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET)
	{
		cout << "Unable to connect to Server" << endl;
		WSACleanup();
		return;
	}

	//4. Send & Receive data:
	const char sendbuffer[] = "������ Server!";
	char recvbuffer[BUFFER_SIZE]{};

	iResult = send(ConnectSocket, sendbuffer, strlen(sendbuffer), 0);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Send field with error #" << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
	cout << "Bytes sent: " << iResult << endl;

	//Close connection:
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		cout << "shutdown failed with error #" << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}

	int recived;
	do
	{
		recived = recv(ConnectSocket, recvbuffer, BUFFER_SIZE, 0);
		if (recived > 0)
		{
			cout << "Bytes received:  \t" << recived << endl;
			cout <<"Received message: \t" << recvbuffer << endl;
		}
		else if (recived == 0)
		{
			cout << "Connection closed" << endl;
		}
		else cout << "Receive failed with error #" << WSAGetLastError() << endl;
	} while (recived > 0);

	//5. Disconection:
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		cout << "shutdown failed with error #" << WSAGetLastError() << endl;
		WSACleanup();
		closesocket(ConnectSocket);
	}
	system("PAUSE");
}