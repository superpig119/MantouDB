#include <stdlib.h>
#include "conf.h"
#include "user.h"
#include "message.h"
#include "tcptransport.h"
#include <vector>
#include <string>
#include <pthread.h>
#include <queue>
#include "simplesocket/ServerSocket.h"
#include "simplesocket/SocketException.h"

using namespace std;

class Master;

struct SlaveNode
{
	string ip;
	int num;
	long long llTotalMemUser;
	long long llCurrMemUsed;
	long long llCurrCPUUsed;
	enum Status {INIT, RUNNING, STOPPED} m_Status;   // system status
};

typedef struct TCPInfo0
{
	ServerSocket *server;
//	TCPTransport tcp;
	string ip;
	int inport;
	int outport;
	Master *m;
}TCPInfo;

struct addrStruct
{
	string addr;
	string base;
};

typedef struct jobO
{
	int jobtype;
	int j_Status; //0为waiting,1为sending，2为running，3为finish
}job;

typedef struct slavejob0
{
	string slave;
	queue<job> slaveJobList;
}slaveJob;

typedef struct MAPNODE
{
	string dname;
	int maptype; //作为选择下面三种映射的依据
	multimap<string, int> msi;
	multimap<int, int> mii;
	multimap<float, int> mfi;
}mapnode;

typedef struct TABLEINFO
{
	string tableName;
	vector<string> dindex;
	vector<mapnode> dlist;
	vector<int> dcnum; //每一维每一slave上的划分数
}tableinfo;


class Master
{
public:
	Master();
	~Master();

public:
	int init();
	int	run();
	int	stop();
	int processSysCmd(const string& ip, const int port,  const User* user, const int32_t key, int id, MantouMsg* msg);
	void startSlave(const std::string& addr, const std::string& base, const std::string& option, const std::string& log = "");
	static void* getActiveSlaves(void *s);
	static void* System(void *s);
	static void* Shell(void *s);
	static void* SlaveCom(void* s);
	void getSlaveStat();
	static void* getSysinfo(void* s);
	static void* jobDealer(void* s);
	static void* start(void *s);
	static void* jobChecker(void *s);
	static void* getPartition(void *s);
	static void* test(void *s);
	//static void* dataInit(void* s);
	void dataInit();
public:
	enum Status {INIT, RUNNING, STOPPED} m_Status;   // system status
	ConfReader cr;
	vector<slavelist> slave_list;
	vector<struct SlaveNode> activeSlaves;
	bool listening;
	vector<slaveJob> m_jobList;
	queue<job> m_qmasterjobList;
	vector<tableinfo> m_vtableInfo;
	vector<ServerSocket> m_vsock;
	vector<TCPInfo> m_vTCPInfo;
private:
	static void* get(void* s);
//	void timefunc(int sig);
private:
	int inCmdPort;
	int outCmdPort;
	string tmp1;
	string tmp2;
	int m_itasknum;
	int m_isockcount;
};


void timefunc(int sig);      /* 定时事件代码 */
void clock(int second);
