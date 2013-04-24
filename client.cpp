#include <stdlib.h>
#include <iostream>
#include "tcptransport.h"
#include <string>
#include <unistd.h>

using namespace std;

int main()
{
	TCPTransport tcp;
	int port = 40001;
	int port2 = 40000;
	tcp.open(port);
	tcp.connect("127.0.0.1", port2);
	string ip;
	if(tcp.isConnected())
	{
		string str1, str2, str3;
		str1 = "!!!!";
		char* a = "!!!!";
		cout << tcp.send(a, sizeof(*a));
		cout << "sent str1" << str1 <<str1.length() <<   endl;
		sleep(1);
		char* b = "@@@@";
		cout << tcp.send(b, sizeof(b));
		cout << "sent str2" << *b << endl;
		sleep(1);
		str3 = "####";
		cout << tcp.send(str3.c_str(), str3.length());
		cout << "sent str3" << str3 << endl;
		sleep(1);
		tcp.close();
	}
	return 0;
}
