// Client part for Server-Client chat. Developed by Mr_Dezz

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <inaddr.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

void heapify(vector<char> & clientBuff, int listLength, int root)
{
	int largest = root;
	int l = 2 * root + 1;
	int r = 2 * root + 2;

	if (l < listLength && clientBuff[l] < clientBuff[largest])
		largest = l;

	if (r < listLength && clientBuff[r] < clientBuff[largest])
		largest = r;

	if (largest != root)
	{
		swap(clientBuff[root], clientBuff[largest]);
		heapify(clientBuff, listLength, largest);
	}
}
void heapSort(vector<char> & clientBuff, int listLength)
{
	for (int i = listLength / 2 - 1; i >= 0; i--)
		heapify(clientBuff, listLength, i);

	for (int i = listLength - 1; i >= 0; i--)
	{
		swap(clientBuff[0], clientBuff[i]);
		heapify(clientBuff, i, 0);
	}
}

int main(void)
{
//#pragma region Connect
	//Key constants
	const char SERVER_IP[] = "127.0.0.1";			// Enter IPv4 address of Server
	const short SERVER_PORT_NUM = 1;				// Enter Listening port on Server side
	const short BUFF_SIZE = 1024;					// Maximum size of buffer for exchange info between server and client

	// Key variables for all program
	int erStat;										// For checking errors in sockets functions


	//IP in string format to numeric format for socket functions. Data is in "ip_to_num"
	in_addr ip_to_num;
	inet_pton(AF_INET, SERVER_IP, &ip_to_num);

	while (true)
	{
		// WinSock initialization
		WSADATA wsData;
		erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

		if (erStat != 0) {
			cout << "Error WinSock version initializaion #";
			cout << WSAGetLastError();
			continue;
		}
		else
			cout << "WinSock initialization is OK" << endl;



		// Socket initialization
		SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);

		if (ClientSock == INVALID_SOCKET) {
			cout << "Error initialization socket # " << WSAGetLastError() << endl;
			closesocket(ClientSock);
			WSACleanup();
			continue;
		}
		else
			cout << "Client socket initialization is OK" << endl;
		//#pragma endregion


				// Establishing a connection to Server
		sockaddr_in servInfo;

		ZeroMemory(&servInfo, sizeof(servInfo));

		servInfo.sin_family = AF_INET;
		servInfo.sin_addr = ip_to_num;
		servInfo.sin_port = htons(SERVER_PORT_NUM);

		erStat = connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo));

		if (erStat != 0) {
			cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << endl;
			closesocket(ClientSock);
			WSACleanup();
			//Sleep(3000);
			continue;
			//return 1;
		}
		else
			cout << "Connection established SUCCESSFULLY. Ready to send a message to Server" << endl;

		//Sending messages from program #1 to program #2

		while (true) {
			vector <char> clientBuff(BUFF_SIZE);					// Buffers for sending and receiving data
			short packet_size = 0;									// The size of sending packet in bytes

			bool FlagErrorMSG = true;		// Checking the correctness of the entered message
			int kol = 0;					// Checking that the message consists of only 64 characters
			int Sum = 0;					// The amount to be sent to program #2

			cout << "\nEnter the number you want to send to the server: ";
			fgets(clientBuff.data(), clientBuff.size(), stdin);

			// I check whether only numbers are written and not more than 64
			for (int i : clientBuff)
			{
				if (((i != '0') && (i != '1') && (i != '2') && (i != '3') && (i != '4') && (i != '5') &&
					(i != '6') && (i != '7') && (i != '8') && (i != '9') && (i != '\n')) || (kol > 64))
				{
					FlagErrorMSG = false;
					break;
				}

				if (i == '\n') break;

				kol++;
			}


			if (!FlagErrorMSG)
			{
				cout << "\nTry again to enter only digits no longer than 64";
				FlagErrorMSG = true;
				continue;
			}

			heapSort(clientBuff, clientBuff.size());

			for (int i = 0; clientBuff[i] != '\n'; i++)
				if ((clientBuff[i] - 48) / 2 == round(float(clientBuff[i] - 48) / 2))
				{
					clientBuff[i] = 'K';
					i++;
					clientBuff.insert(clientBuff.begin() + i, 'B');
				}
				else
					Sum += clientBuff[i] - 48;


			cout << "Client's message: " << clientBuff.data() << endl;

			//clientBuff.clear();

			if (Sum > 0)
			{
				for (int i = 1; Sum > 0; i++, Sum /= 10)
				{
					clientBuff[i] = char(Sum % 10 + 48);
					clientBuff[0] = i;
				}
			}
			else
			{
				clientBuff[0] = 1;
				clientBuff[1] = Sum + 48;
			}


			// Check whether client like to stop chatting !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
			//	shutdown(ClientSock, SD_BOTH);
			//	closesocket(ClientSock);
			//	WSACleanup();
			//	return 0;
			//}


			packet_size = send(ClientSock, clientBuff.data(), 4, 0);

			if (packet_size == SOCKET_ERROR) {
				cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
				closesocket(ClientSock);
				WSACleanup();
				break;
			}
		}
		closesocket(ClientSock);
	}

	WSACleanup();

	return 0;
}