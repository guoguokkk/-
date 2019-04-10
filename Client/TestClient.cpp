#include"Client.h"
#include<thread>
#include<iostream>
using std::cout;
using std::endl;
using std::cin;

//绑定一个线程负责发送请求
void cmd_thread(Client* client_p)
{
	while (true)
	{
		char send_buf[128];
		cin >> send_buf;
		if (strcmp(send_buf, "exit") == 0)
		{
			cout << "Client exit." << endl;
			client_p->CloseClient();
			break;
		}
		else if (strcmp(send_buf, "login")==0)
		{
			Login login;
			strcpy(login.name, "kzj");
			strcpy(login.password, "12345");
			client_p->SendData(&login);
		}
		else if (strcmp(send_buf, "logout") == 0)
		{
			Logout logout;
			strcpy(logout.name, "kzj");
			client_p->SendData(&logout);
		}
		else
		{
			cout << "Invalid input, please re-enter." << endl;
		}
	}
}

int main()
{
	Client client;
	client.InitClient();
	client.Connect(SERVER_IP, PORT);

	//启动线程
	std::thread t(cmd_thread, &client);
	t.detach();

	while (client.IsRun())
	{
		client.OnRun();
	}
	client.CloseClient();
	return 0;
}