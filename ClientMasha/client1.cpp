#pragma comment(lib,"ws2_32.lib") 
#include <WinSock2.h> 
#include <string> 
#include <iostream> 
#include <windows.h>

using namespace std;

SOCKET Connection;

char ServerIP[] = "127.0.0.1";
int Port = 1509;
string confirm;
char message[200];
string strmessage;
char address[256];
short iport;

string nickname;

enum Packet
{
	Message,
	Test
};

BOOL WINAPI SetConsoleOutputCP(__in UINT wCodePageID); //��������� ������� �������� � ����� ������
BOOL WINAPI SetConsoleCP(__in UINT wCodePageID); //��������� ������� �������� � ����� ������


bool ProcessPacket(Packet packettype)
{
	switch (packettype)
	{
	case Message:
	{
		int bufferlength; //����� ���������
		recv(Connection, (char*)&bufferlength, sizeof(int), NULL); //����� ������
		char * buffer = new char[bufferlength + 1]; //�������� �����

		buffer[bufferlength] = '\0';
		recv(Connection, buffer, bufferlength, NULL);
		printf(buffer); //��������
		printf("\n");

		delete[] buffer; //������� �����
		break;
	}
	case Test:
		printf("�������� ��������� ����������!\n");
		break;
	default:
		printf("����������� �����: ", packettype);
		break;
	}
	return true;
}

void ClientThread()
{
	Packet packettype;
	while (true)
	{
		//������� �������� ��� ������
		recv(Connection, (char*)&packettype, sizeof(Packet), NULL); //��������� ���� ������ � �������
		printf("\n");


		if (!ProcessPacket(packettype)) //���� ����� ����������� ���������
			break; //����� �� �����
	}
}

int main()
{
	//setlocale(LC_ALL, "russian");


		SetConsoleCP(1251); //��������� ������� �������� win-cp 1251 � ����� �����
		SetConsoleOutputCP(1251); //��������� ������� �������� win-cp 1251 � ����� ������


		WSADATA WSAData;
		WSAStartup(MAKEWORD(2, 0), &WSAData);

		SOCKADDR_IN addr;
		int sizeofaddr = sizeof(addr);
		addr.sin_family = AF_INET;

		cout << "������� IP-�����: ";
		cin.getline(address, 256);

		addr.sin_addr.s_addr = inet_addr(address);
		cout << "������� ����: ";
		cin >> iport;
		addr.sin_port = htons(iport); // server port
		cout << endl;
		//addr.sin_port = htons(Port);


		cout << "������� ���: ";
		cin >> nickname;

		std::cout << "�����������, " << nickname << "!!! \n";

		Connection = socket(AF_INET, SOCK_STREAM, NULL);
		if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0)
		{
			MessageBoxA(NULL, "������ �����������", "Error", MB_OK | MB_ICONERROR);
			return 0;
			printf("������� ���");
			return 0;

		}
		else printf("��������� � �������!\n");

		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL);


		std::string name;
		std::string userinput; //�������� ��������� ���� ������������

		while (true)
		{
			std::getline(std::cin, userinput); //�������� ������, ���� ������������ �������� ������� enter � ��������� �����
			name = nickname + ": " + userinput;
			int bufferlength = name.size(); //����� ����� ������
			Packet chatmessagepacket = Message; //������� ��� ������: ��������� ����, ������������ �� ������

			send(Connection, (char*)&chatmessagepacket, sizeof(Packet), NULL); //��������� ��� ������: ��������� ����
			send(Connection, (char*)&bufferlength, sizeof(int), NULL); //��������� ����� ������
			send(Connection, name.c_str(), bufferlength, NULL); //��������� ����� 
			Sleep(10);
		}
		return 0;

}

