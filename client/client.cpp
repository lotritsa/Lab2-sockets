#include <winsock2.h>
#include <iostream>

#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>

#pragma warning(disable:4996) 

using namespace std;

int main()
{
	WSADATA WSA_Socket;
	SOCKET s;
	//ініціалізуємо роботу програми із сокетами
	if (WSAStartup(0x0202, &WSA_Socket) != 0) {
		printf("%s", "\nSocket_Server() > open WSAStartup is invalid!");
		getchar();
		return 1;
	}

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == 0) {
		printf("%s", "Create socket failed. \n");
		getchar();
		return 1;
	}

	sockaddr_in service;
	service.sin_family = PF_INET; 
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(1055);

	if (connect(s, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
		printf("Failed to connect.\n");
		WSACleanup();
		return 0;
	}
	printf("Connected to server.\n");
	vector <int> vec;
	srand(time(NULL));
	int num = rand() % 10 + 3;
	for (int i = 0; i < num; i++)
	{
		int n = rand() % 100;
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

	string kek = "";
	string info_answer = "Available commands for client: \n clear (removes all numbers from the client set) \n add(int) (adds a given number to the client's set)  \n";
	cout << info_answer << "\n Try *help* for more information. \n";
	while (cin >> kek)
	{
		if (kek.size() >= 512)
		{
			cout << "The length of the message cannot exceed 32 characters. Try 'help' for more information. \n";
		}
		else if (kek == "clear")
		{
			vec.clear();
			cout << "Client's set cleared \n";
		}
		else if (kek.size() >= 6 && kek[0] == 'a' && kek[1] == 'd' && kek[2] == 'd' && kek[3] == '(')
		{
			int i = 4;
			string add_num = "";
			bool prov = true;
			while (i < kek.size() && kek[i] >= '0' && kek[i]<='9')
			{
				add_num = add_num + kek[i];
				i++;
			}
			if (add_num != "" && prov == true && i<kek.size() && kek[i] == ')')
			{
				int ans = 0;
				int mult = 1;
				for (int j = add_num.size() - 1; j >= 0; j--)
				{
					ans += (add_num[j] - '0') * mult;
					mult *= 10;
				}
				bool prov = true;
				for (int i = 0; i < vec.size(); i++)
				{
					if (vec[i] == ans) prov = false;
				}
				if (prov == false)
				{
					cout << "The client's set already contains this number.\n";
				}
				else
				{
					vec.push_back(ans);
					cout << "Client's set of numbers changed: ";
					for (int j = 0; j < vec.size(); j++)
					{
						cout << vec[j] << ' ';
					}
					cout << "\n";
				}
			}
			else
			{
				cout << "Invalid command. Try 'help' for more information. \n";
			}
		}
		else
		{
			if (kek == "sync")
			{
				kek += '(';
				for (int i = 0; i < vec.size(); i++)
				{
					kek += to_string(vec[i]);
					if (i < vec.size() - 1) kek += ',';
				}
				kek += ')';
			}
			int bytesSent;
			int bytesRecv = SOCKET_ERROR;
			char sendbuf[512] = "";
			for (int i = 0; i < kek.size(); i++)
			{
				sendbuf[i] = kek[i];
			}
			char recvbuf[512] = "";
			bytesSent = send(s, sendbuf, strlen(sendbuf), 0);
			cout << "Bytes Sent: " << bytesSent << "\n";
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
			cout << "Server response: " <<  recvbuf << "\n";
			string server_answer = "";
			for (int i = 0; i < 512; i++)
			{
				if (recvbuf[i] != '\0') server_answer = server_answer + recvbuf[i];
			}
			if (server_answer[0] == 'W')
			{
				vector <int> wrong_nums;
				vector <int> missing_nums;
				bool prov = false;
				for (int i = 0; i < server_answer.size(); i++)
				{
					if (server_answer[i] >= '0' && server_answer[i] <= '9')
					{
						string cur_num = "";
						while (i < server_answer.size() && server_answer[i] >= '0' && server_answer[i] <= '9')
						{
							cur_num += server_answer[i];
							i++;
						}
						int cur_ans = 0;
						int mult = 1;
						for (int j = cur_num.size() - 1; j >= 0; j--)
						{
							cur_ans += (cur_num[j] - '0') * mult;
							mult *= 10;
						}
						if (prov == false) wrong_nums.push_back(cur_ans);
						else missing_nums.push_back(cur_ans);
					}
					else if (server_answer[i] == 'M')
					{
						prov = true;
					}

				}
				vector <int> new_vec;
				for (int i = 0; i < vec.size(); i++)
				{
					bool prov = true;
					for (int j = 0; j < wrong_nums.size(); j++)
					{
						if (vec[i] == wrong_nums[j]) prov = false;
					}
					if (prov) new_vec.push_back(vec[i]);
				}
				vec.clear();
				for (int i = 0; i < new_vec.size(); i++)
				{
					vec.push_back(new_vec[i]);
				}
				for (int i = 0; i < missing_nums.size(); i++)
				{
					vec.push_back(missing_nums[i]);
				}
				cout << "Sync succeeded! \n";
				cout << "Client's set changed: ";
				for (int i = 0; i < vec.size(); i++)
				{
					cout << vec[i] << ' ';
				}
				cout << "\n";
			}
		}
	}

	WSACleanup();
}
