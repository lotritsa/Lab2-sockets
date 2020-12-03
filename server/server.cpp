#include <winsock2.h>
#include <iostream>

#include <string>
#include <vector>
#include <time.h>

#pragma warning(disable:4996) 

using namespace std;

int main()
{
	WSADATA WSA_Socket;
	SOCKET s;
	//ініціалізуємо роботу програми із сокетами
	if (WSAStartup(0x0202, &WSA_Socket) != 0) {
		printf("%s" , "\nSocket_Server() > open WSAStartup is invalid!");
		getchar(); 
		return 1;
	}

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == 0) { 
		
	}

	sockaddr_in service;
	service.sin_family = PF_INET; //може бути також AF_INET
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(1055);

	if (bind(s, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) 
	{
		closesocket(s);
		return 0;
	}

	if (listen(s, 1) == SOCKET_ERROR) //можливе лише одне підключення
		printf("Error listening on socket.\n");

	SOCKET AcceptSocket;
	printf("Waiting for client to connect...\n");
	string who_answer = " |Tsybick Anastasiya K-26| |Option 30 (Synchronization of sets of numbers.)| \n";
	string info_answer = "\n Available commands for server: \n info|help|? (shows list of available commands) \n who|Who (the server provides information about the author and the number and a short name of the option) \n abort|Abort (shuts down the server) \n sync (synchronizes sets of client and server) \n";

	while (1) {
		AcceptSocket = SOCKET_ERROR;
		while (AcceptSocket == SOCKET_ERROR) {
			AcceptSocket = accept(s, NULL, NULL);
		}
		printf("Client connected.\n");

		vector <int> vec;
		srand(time(NULL));
		int num = rand() % 10 + 3;
		for (int i = 0; i < num; i++)
		{
			int n = (rand() % 7 + 6) * 2 + 1;
			bool prov = true;
			for (int j = 0; j < vec.size(); j++)
			{
				if (vec[j] == n) prov = false;
			}
			if (prov)vec.push_back(n);
		}
		cout << "Set of numbers generated: ";
		for (int i = 0; i < vec.size(); i++)
		{
			cout << vec[i] << ' ';
		}
		cout << "\n";

		s = AcceptSocket;
		while(1)
		{
			int bytesSent;
			int bytesRecv = SOCKET_ERROR;
			char sendbuf[512] = "";
			char recvbuf[512] = "";
			while (bytesRecv == SOCKET_ERROR)
			{
				bytesRecv = recv(s, recvbuf, 512, 0);
				if (bytesRecv == 0 || bytesRecv == WSAECONNRESET)
				{
					cout << "Connection Closed. \n";
					getchar();
					return 0;
				}
			}
			cout << "Bytes Recv: " << bytesRecv << "\n";
			cout << "Client message: " << recvbuf << "\n";
			string kek = "";
			for (int i = 0; i < 512; i++)
			{
				 if (recvbuf[i] != '\0') kek = kek + recvbuf[i];
			} 
		//	cout << '@' << kek << '@' << "\n";
			string answer = "Invalid command. Try 'help' for more information. \n";
			if (kek == "info" || kek == "help" || kek == "?")
			{
				answer = info_answer;
			}
			else if (kek == "Who" || kek == "who")
			{
				answer = who_answer;
			}
			else if (kek.size()>=6 && kek[0] == 's' && kek[1] == 'y' && kek[2] == 'n' && kek[3] == 'c' && kek[4] == '(' && kek[kek.size()-1] == ')')
			{
				vector <int> vec1;
				bool prov = true;
				int prev = 0;
				for (int i = 5; i < kek.size() - 2; i++)
				{
					if (kek[i] >= '0' && kek[i] <= '9')
					{
						string cur_num = "";
						while (i < kek.size() && kek[i] >= '0' && kek[i] <= '9')
						{
							cur_num += kek[i];
							i++;
						}
						int cur_ans = 0;
						int mult = 1;
						for (int j = cur_num.size() - 1; j >= 0; j--)
						{
							cur_ans += (cur_num[j] - '0') * mult;
							mult *= 10;
						}
						vec1.push_back(cur_ans);
						prev = 0;
					}
					else if (kek[i] == ',' && prev == 0)
					{
						prev = 1;
					}
					else
					{
						prov = false;
					}
				}
				if (prov)
				{
					answer = "Wrong numbers: ";
					bool wr = false;
					for (int i = 0; i < vec1.size(); i++)
					{
						if (vec1[i] > 27)
						{
							wr = true;
							answer += to_string(vec1[i]) + "(>) ";
						}
						else if (vec1[i] < 13)
						{
							wr = true;
							answer += to_string(vec1[i]) + "(<) ";
						}
						else if (vec1[i] % 2 == 0)
						{
							wr = true;
							answer += to_string(vec1[i]) + "(even) ";
						}
						else
						{
							bool prov = true;
							for (int j = 0; j < vec.size(); j++)
							{
								if (vec[j] == vec1[i]) prov = false;
							}
							if (prov) vec.push_back(vec1[i]);
						}
					}
					if (wr == false)
					{
						answer += "none \n";
					}
					else
					{
						answer += "\n";
					}
					answer += "Missing numbers: ";
					wr = false;
					for (int i = 0; i < vec.size(); i++)
					{
						bool prov = false;
						for (int j = 0; j < vec1.size(); j++)
						{
							if (vec[i] == vec1[j]) prov = true;
						}
						if (prov == false)
						{
							wr = true;
							answer += to_string(vec[i]) + " ";
						}
					}
					if (wr == false)
					{
						answer += "none\n";
					} 
					cout << "Sync succeeded! \n";
					cout << "Server's set changed: ";
					for (int i = 0; i < vec.size(); i++)
					{
						cout << vec[i] << ' ';
					}
					cout << "\n";
				}
				else
				{
					answer = "Sync failed. \n";
				}

			}
			else if (kek == "abort")
			{
				answer = "Server aborted.";
			}
			for (int i = 0; i < answer.size(); i++)
			{
				sendbuf[i] = answer[i];
			}
			bytesSent = send(s, sendbuf, strlen(sendbuf), 0);
			cout << "Bytes Sent: " << bytesSent << "\n";
			if (kek == "abort") return 0;
		}
		
	}

	WSACleanup();
}
