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
#include <dirent.h>
#include <sys/stat.h>
#include "MTFS.h"

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


typedef struct SPLITFILE
{
	string line;
	vector<int> xi;
	string dnum;
	int x;
}splitfile;

typedef struct SENDFILEINFO	//将划分完成的文件传送给slave时用到的结构
{
	Master *m;
	string ip;
	string num;
	string username;
	string tablename;
	string base;
	vector<string> filename;
}sendfileinfo;

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
	static void* dataSplit(void* s);
	static void* senddata(void* s);
	int trave_dir(char* path, vector<string> &filename);
//	void dataInit();
public:
	enum Status {INIT, RUNNING, STOPPED} m_Status;   // system status
	ConfReader cr;
	MTFS mt;
	vector<slavelist> slave_list;
	vector<struct SlaveNode> activeSlaves;
	bool listening;
	vector<slaveJob> m_jobList;
	queue<job> m_qmasterjobList;
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
