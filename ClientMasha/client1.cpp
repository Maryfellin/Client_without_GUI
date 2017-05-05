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
string strmessage;
char address[256];
short iport;
string nickname;

enum Packet
{
	Message
};

BOOL WINAPI SetConsoleOutputCP(__in UINT wCodePageID); //установка кодовой страницы в поток вывода
BOOL WINAPI SetConsoleCP(__in UINT wCodePageID); //установка кодовой страницы в поток вывода

bool Send(int _int)
{
	int RetnCheck = send(Connection, (char*)&_int, sizeof(int), NULL);
	if (RetnCheck == SOCKET_ERROR) 
		return false;
	return true;
}

bool Get(int & _int)
{
	int RetnCheck = recv(Connection, (char*)&_int, sizeof(int), NULL); //приём
	if (RetnCheck == SOCKET_ERROR) //Если есть проблема с подключением
		return false;
	return true;
}

bool SendPacket(Packet _packettype)
{
	int RetnCheck = send(Connection, (char*)&_packettype, sizeof(Packet), NULL); //отправить пакет
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true; 
}

bool GetPacket(Packet & _packettype)
{
	int RetnCheck = recv(Connection, (char*)&_packettype, sizeof(Packet), NULL); //получение пакета
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool SendString(std::string & _string)
{
	if (!SendPacket(Message))
		return false; 
	int bufferlength = _string.size();
	if (!Send(bufferlength))
		return false;
	int RetnCheck = send(Connection, _string.c_str(), bufferlength, NULL);
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool GetString(std::string & _string)
{
	int bufferlength; //длина сообщения
	if (!Get(bufferlength)) //получить длину буфера и сохранить его в переменной: bufferlength
		return false;
	char * buffer = new char[bufferlength + 1]; //выделение буфера
	buffer[bufferlength] = '\0'; //устанавливаем последний символ буфера нулевым
	int RetnCheck = recv(Connection, buffer, bufferlength, NULL); //получаем сообщение и сохраняем в массиве буферов
	_string = buffer; //установить строку в полученное сообщение буфера
	delete[] buffer;
	if (RetnCheck == SOCKET_ERROR) //если соединение потеряно во время получения сообщения
		return false;
	return true;
}

bool ProcessPacket(Packet _packettype)
{
	switch (_packettype)
	{
	case Message:
	{
		std::string Message; //Строка для хранения полученного нами сообщения
		if (!GetString(Message)) //Получить сообщение чата и сохранить его в переменной
			return false;
		std::cout << Message << std::endl; //Отображение сообщения пользователю
		break;
	}
	default:
		std::cout << "Нераспознанный пакет" << _packettype << std::endl;
		break;
	}
	return true;
}

void ClientThread()
{
	Packet PacketType;
	while (true)
	{
		if (!GetPacket(PacketType))
			break;
		if (!ProcessPacket(PacketType))
			break;
	}
	std::cout << "Потеряна связь с сервером." << std::endl;
	closesocket(Connection);
}

int main()
{
	SetConsoleCP(1251); //установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); //установка кодовой страницы win-cp 1251 в поток вывода

	cout << "--------------------------------------CHAT--------------------------------------\n";

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_family = AF_INET;

	cout << "Введите IP-адрес: ";
	cin.getline(address, 256);
	cout << "--------------------------------------------------------------------------------\n";

	addr.sin_addr.s_addr = inet_addr(address);
	cout << "Введите порт: ";
	cin >> iport;
	addr.sin_port = htons(iport); // server port

	cout << "--------------------------------------------------------------------------------\n";

	cout << "Введите имя: ";
	cin >> nickname;
	cout << "--------------------------------------------------------------------------------\n";
	std::cout << "Приветствую, " << nickname << "!!! \n";
	cout << "--------------------------------------------------------------------------------\n";

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0)
	{
		MessageBoxA(NULL, "Ошибка подключения", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	std::cout << "Подключён к серверу!" << std::endl;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL);

	std::string name;
	std::string userinput; //содержит сообщение чата пользователя

	while (true)
	{
		std::getline(std::cin, userinput);
		name = nickname + ": " + userinput;
		userinput = name;
		if (!SendString(userinput))
			break;
		Sleep(10);
	}
	return 0;
}