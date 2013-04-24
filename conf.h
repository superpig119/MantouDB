#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>

using namespace std;

typedef struct relationStyle
{
    int DNum;               //维数
    vector<int> DClass;     //每一维的种类
}relStyle;

typedef struct slaveList
{
	string ip;
	int num;
}slavelist;

class ConfReader
{
public:
	static int locate(string& loc);
	bool ReadConfFile(string FileName, relStyle &rel);
	bool getSlaveList(char* Filename, string& base, string& username, vector<slavelist> &slave_list);
	bool getMasterAddress(char* Filename, string &addr);
	string GetLocalIP();
};


