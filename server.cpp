#include <iostream>
#include "tcptransport.h"
#include <string>
#include <stdlib.h>
#include <cstring>

using namespace std;

int main()
{
	TCPTransport tcp;
	int port = 40000;
	int port2;
	tcp.open(port);

	
	tcp.listen();
	string ip;
	int i = 0;
	TCPTransport *t = tcp.accept(ip, port2);
	cout << ip << port2 << endl;
	while(1)
	{
	//	if(NULL == t)
	//		continue;
		int size = 50;
		char* data = (char*)malloc(50);
	//	memset(data, 0, size);
		cout << t->recv(data,size);
		cout << size << endl;
		string d;
		d = *data;
		if(d != "")	
		{		
			cout << d << endl;
			cout << "data: " << *data << endl;
			i++;
		}
		if(3 == i)
			break;
	}
	return 0;
}
