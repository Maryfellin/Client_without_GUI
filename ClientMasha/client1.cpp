#pragma comment(lib,"ws2_32.lib") 
#include <WinSock2.h> 
#include <string> 
#include <iostream> 

SOCKET Connection;

char ServerIP[] = "127.0.0.1";
int Port = 1509;
std::string nickname;

enum Packet
{
	P_ChatMessage,
	P_Test
};

bool ProcessPacket(Packet packettype)
{
	switch (packettype)
	{
	case P_ChatMessage:
	{
		int bufferlength; //����� ���������
		recv(Connection, (char*)&bufferlength, sizeof(int), NULL); //����� ������
		char * buffer = new char[bufferlength + 1]; //�������� �����
		buffer[bufferlength] = '\0';
		recv(Connection, buffer, bufferlength, NULL);
		printf(buffer); //��������
		delete[] buffer; //������� �����
		break;
	}
	case P_Test:
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

		if (!ProcessPacket(packettype)) //���� ����� ����������� ���������
			break; //����� �� �����
	}
}

int main()
{

	setlocale(LC_ALL, "russian");
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKADDR_IN addr; 
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(ServerIP);
	addr.sin_port = htons(Port);
	addr.sin_family = AF_INET;



	std::cout << "������� ���: ";
	std::getline(std::cin, nickname);

	std::cout << "�����������, " << nickname << "!!! \n";

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) 
	{
		MessageBoxA(NULL, "������ �����������", "Error", MB_OK | MB_ICONERROR);
		return 0; 

	}
	else printf ("��������� � �������!\n");


	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL); 


	std::string userinput; //�������� ��������� ���� ������������
	while (true)
	{
		std::getline(std::cin, userinput); //�������� ������, ���� ������������ �������� ������� enter � ��������� �����
		int bufferlength = userinput.size(); //����� ����� ������
		Packet chatmessagepacket = P_ChatMessage; //������� ��� ������: ��������� ����, ������������ �� ������
		send(Connection, (char*)&chatmessagepacket, sizeof(Packet), NULL); //��������� ��� ������: ��������� ����
		send(Connection, (char*)&bufferlength, sizeof(int), NULL); //��������� ����� ������
		send(Connection, userinput.c_str(), bufferlength, NULL); //��������� �����
		Sleep(10);
	}
	return 0;
}

