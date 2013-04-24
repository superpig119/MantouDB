#include <string>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <iostream>
#include "tcptransport.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include "conf.h"
#include <queue>
#include "simplesocket/ClientSocket.h"
#include "simplesocket/SocketException.h"
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

class Slave;

class SlaveNode
{
public:
   SlaveNode();

public:
   int m_iNodeID;					// unique slave node ID

   //Address m_Addr;
   std::string m_strIP;					// ip address (used for internal communication)
   std::string m_strPublicIP;				// public ip address, used for client communication. currently NOT used
   int m_iPort;						// GMP control port number
   int m_iDataPort;					// UDT data channel port number

   std::string m_strStoragePath;			// data storage path on the local file system
   std::string m_strAddr;               		// username@hostname/ip
   std::string m_strBase;               		// $SECTOR_HOME location on the slave
   std::string m_strOption;             		// slave options

   long long m_llAvailDiskSpace;				// available disk space
   long long m_llTotalFileSize;				// total data size

   long long m_llTimeStamp;				// last statistics refresh time
   long long m_llCurrMemUsed;				// physical memory used by the slave
   long long m_llCurrCPUUsed;				// CPU percentage used by the slave
   long long m_llTotalInputData;				// total network input
   long long m_llTotalOutputData;				// total network output
   std::map<std::string, long long> m_mSysIndInput;	// network input from each other slave
   std::map<std::string, long long> m_mSysIndOutput;	// network outout to each other slave
   std::map<std::string, long long> m_mCliIndInput;	// network input from each client
   std::map<std::string, long long> m_mCliIndOutput;	// network output to each client

   long long m_llLastUpdateTime;				// last update time
   int m_iStatus;					// 0: inactive 1: active-normal 2: active-disk full/read only 3: bad
   bool m_bDiskLowWarning;                              // if the disk low warning info has been updated to the master

   std::set<int> m_sBadVote;				// set of bad votes by other slaves
   long long m_llLastVoteTime;				// timestamp of last vote

   std::vector<int> m_viPath;				// topology path, from root to this node on the tree structure

   int m_iActiveTrans;					// number of active transactions

public:
   int deserialize(const char* buf, int size);
};

struct sysinfo
{
	long long m_llTotalMemUser;
	long long m_llCurrMemUsed;
	long long m_llCurrCPUUsed;
};

struct task
{
	string taskClass;
};

typedef struct TCPInfo0
{
	TCPTransport tcp;
	string ip;
	string inport;
	string outport;
	Slave *s;
}TCPInfo;

struct addrStruct
{
	string addr;
	string base;
};

typedef struct jobO
{
	int jobtype;
	int j_Status; //job工作状态，0为等待,1为运行中，2为结束
	int portnum;
}job;


class Slave
{
public:
	Slave();
	~Slave();

private:
	int inCmdPort;
	int outCmdPort;
	int dataPort;
	string localAddress;
	string masterAddress;
	TCPTransport tcp;
	ClientSocket m_client;
public:
	ConfReader cr;
	long long m_llTotalMemUser;
	long long m_llCurrMemUsed;
	long long m_llCurrCPUUsed;
	enum Status {INIT, RUNNING, STOPPED} m_Status;   // system status
	queue<job> m_jobList;
	static void* getTask(void* s);
	int trave_dir(char* path, vector<string> &filename);
public:
	int init();
	int run();
	int stop();
	void reportAlive();
	
private:
	static void* jobAssign(void* s);
	static void* sendSysinfo(void* s);
	static void* partition(void* s);
	static void* test(void* s);
};
