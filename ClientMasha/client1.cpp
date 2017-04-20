#pragma comment(lib,"ws2_32.lib") 
#include <WinSock2.h> 
#include <string> 
#include <iostream> 

#include <windows.h>

using namespace std;

SOCKET Connection;

char ServerIP[] = "127.0.0.1";
int Port = 1509;
std::string nickname;

enum Packet
{
	Message,
	Test
};

BOOL WINAPI SetConsoleOutputCP(__in UINT wCodePageID); //установка кодовой страницы в поток вывода
BOOL WINAPI SetConsoleCP(__in UINT wCodePageID); //установка кодовой страницы в поток вывода


bool ProcessPacket(Packet packettype)
{
	switch (packettype)
	{
	case Message:
	{
		int bufferlength; //длина сообщения
		recv(Connection, (char*)&bufferlength, sizeof(int), NULL); //Буфер приема
		char * buffer = new char[bufferlength + 1]; //Выделяем буфер

		buffer[bufferlength] = '\0';
		recv(Connection, buffer, bufferlength, NULL);
		printf(buffer); //печатаем
		printf("\n");

		delete[] buffer; //удаляем буфер
		break;
	}
	case Test:
		printf("Тестовое сообщение отправлено!\n");
		break;
	default:
		printf("Неизвестный пакет: ", packettype);
		break;
	}
	return true;
}

void ClientThread()
{
	Packet packettype;
	while (true)
	{
		//Сначала получаем тип пакета
		recv(Connection, (char*)&packettype, sizeof(Packet), NULL); //Получение типа пакета с сервера
		printf("\n");


		if (!ProcessPacket(packettype)) //Если пакет неправильно обработан
			break; //выход из цикла
	}
}

int main()
{
	//setlocale(LC_ALL, "russian");

	SetConsoleCP(1251); //установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); //установка кодовой страницы win-cp 1251 в поток вывода

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(ServerIP);
	addr.sin_port = htons(Port);
	addr.sin_family = AF_INET;


	std::cout << "Введите имя: ";
	std::getline(std::cin, nickname);

	std::cout << "Приветствую, " << nickname << "!!! \n";

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0)
	{
		MessageBoxA(NULL, "Ошибка подключения", "Error", MB_OK | MB_ICONERROR);
		return 0;

	}
	else printf("Подключён к серверу!\n");


	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL);


	std::string name;
	std::string userinput; //содержит сообщение чата пользователя

	while (true)
	{
		std::getline(std::cin, userinput); //Получить строку, если пользователь нажимает клавишу enter и заполняет буфер
		name = nickname + ": " + userinput;
		int bufferlength = name.size(); //Найти длину буфера
		Packet chatmessagepacket = Message; //Создать тип пакета: сообщение чата, отправляемое на сервер

		send(Connection, (char*)&chatmessagepacket, sizeof(Packet), NULL); //Отправить тип пакета: сообщение чата
		send(Connection, (char*)&bufferlength, sizeof(int), NULL); //Отправить длину буфера
		send(Connection, name.c_str(), bufferlength, NULL); //Отправить буфер 
		Sleep(10);
	}
	return 0;
}

