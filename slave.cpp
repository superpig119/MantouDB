#include "slave.h"
#include <sstream>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fstream>
#include <algorithm>

using namespace std;

int n = 0;
Slave::Slave()
{
}
Slave::~Slave()
{
}

int Slave::init()
{
	inCmdPort = 53001;
	outCmdPort = 53000;
	if(!cr.getMasterAddress("./mantouDB/mantouDB/masterlist", masterAddress))
		return 0;
	ClientSocket c(masterAddress, outCmdPort);
	m_client = c;
	cout << "m_client sock:" << m_client.m_sock << endl;
	localAddress = cr.GetLocalIP();
	string report = localAddress + " is started!";
	cout << localAddress << " clisock" <<m_client.m_sock<<endl;
	cout << "init connect success" << endl;
	try
	{
		m_client << report;
		string s,s1;
	}

	catch(SocketException& e)
	{
		cout << "error:" << e.description() << endl;
	}
	
	m_Status = RUNNING;	

	mt.SlaveDataInit();
/*	vector<slavetableinfo>::iterator im_vtableInfo;
	vector<slavemapnode>::iterator idlist;
	for(im_vtableInfo = mt.m_vslavetableInfo.begin(); im_vtableInfo != mt.m_vslavetableInfo.end(); im_vtableInfo++)
	{
		cout << "tablename: " << (*im_vtableInfo).tableName << endl;
		cout << "dnum: " << (*im_vtableInfo).dlist.size() << endl;
		for(idlist = (*im_vtableInfo).dlist.begin(); idlist != (*im_vtableInfo).dlist.end(); idlist++)
		{
			cout << (*idlist).dname << " " << (*idlist).maptype << " " << (*idlist).splitnum << endl;
		}
	}
*/
	pthread_t getTaskFunc;
	if(pthread_create(&getTaskFunc, NULL, getTask, this))
		cout << "create thread error" << endl;
	void *exit = (int*)malloc(4);
	pthread_detach(getTaskFunc);
	pthread_join(getTaskFunc, &exit);
					
	pthread_t pjobAssign;
	pthread_create(&pjobAssign, NULL, jobAssign, this);
	pthread_join(pjobAssign,&exit);
}


int Slave::run()
{
/*	pthread_t pgetTask;
	pthread_create(&pgetTask, NULL, getTask, this);
	void *exit = (int*)malloc(4);
	pthread_join(pgetTask,&exit);*/
}


void Slave::reportAlive()
{
}

void *Slave::getTask(void *s)	//负责接收任务，将job插入到job队列中
{
	Slave* self = (Slave*)s;
	ClientSocket client;
	client = self->m_client;
	while(self->m_Status == RUNNING)
	{
		string data;
		string port;
		int portnum;
		client >> data;
		client >> port;
		int taskCode;
		stringstream ss,ss2;
		ss << data;
		ss >> taskCode;
		ss2 << port;
		ss2 >> portnum;
		switch(taskCode)
		{
			case 1:
			{
				job j;
				j.jobtype = 1;
				j.j_Status = 0;
				j.portnum = portnum;
				self->m_jobList.push(j);
				break;
			}
			case 2:
				cout << self->localAddress <<"is shutting down!" << endl;
				self->m_Status = STOPPED;
				exit(0);
				break;
			case 3:
			{
				job j;
				j.jobtype = 3;
				j.j_Status = 0;
				j.portnum = portnum;
				self->m_jobList.push(j);
				break;
			}
			case 4:
			{
				job j;
				j.jobtype = 4;
				j.j_Status = 0;
				j.portnum = portnum;
				self->m_jobList.push(j);
				break;
			}
			case 5:
			{
				job j;
				j.jobtype = 5;
				j.j_Status = 0;
				j.portnum = portnum;
				self->m_jobList.push(j);
				break;
			}
			default:
				break;
	
		}
	}
}

void* Slave::jobAssign(void* s)	//负责根据job列表来分派任务
{
	Slave* self = (Slave*)s;
	while(self->m_Status == RUNNING)
	{
		if(self->m_jobList.size() != 0 && self->m_jobList.front().j_Status == 0)
		{
			job j = self->m_jobList.front();
			switch(j.jobtype)
			{
				case 1:
				{
					pthread_t psendSysinfo;
					pthread_create(&psendSysinfo, NULL, sendSysinfo, self);
					void *exit = (int*)malloc(4);
					pthread_join(psendSysinfo,&exit);
					j.j_Status = 1;
					break;
				}
				case 3:
				{
					pthread_t ppartition;
					pthread_create(&ppartition, NULL, partition, self);
					void *exit = (int*)malloc(4);
					pthread_join(ppartition,&exit);
					j.j_Status = 1;
					break;
				}
				case 4:
				{
					pthread_t ptest;
					pthread_create(&ptest, NULL, test, self);
					void *exit = (int*)malloc(4);
					pthread_join(ptest,&exit);
					j.j_Status = 1;
					break;
				}
				case 5:
				{
					pthread_t pread;
					pthread_create(&pread, NULL, readData, self);
					void *exit = (int*)malloc(4);
					pthread_join(pread,&exit);
					j.j_Status = 1;
					break;
				}
				default:
					break;
			}
		}
		if(self->m_jobList.size() != 0 && self->m_jobList.front().j_Status == 2)
		{
			self->m_jobList.pop();
		}

	}
}

void* Slave::sendSysinfo(void* s)
{
	Slave* self = (Slave*)s;
	job j;
	if(self->m_jobList.front().jobtype != 1)
	{
		cout << "send sysinfo job error" << endl;
		exit(0);
	}
	int pid = getpid();
	
	ClientSocket client(self->masterAddress, self->m_jobList.front().portnum);
	cout << self->localAddress << " getsysinfo sock: " << client.m_sock << endl;
	char memfile[64];
	sprintf(memfile, "/proc/%d/status", pid);
	ifstream ifs;
	ifs.open(memfile, ios::in);
	if (!ifs.fail())
	{
	   // this looks for the VmSize line in the process status file
	    char buf[1024];
		buf[0] = '\0';
		while (!ifs.eof())
		{
			ifs.getline(buf, 1024);
			if (string(buf).substr(0, 6) == "VmSize")
				break;
		}
		string tmp;
		stringstream ss;
		ss.str(buf);
		ss >> tmp >> self->m_llCurrMemUsed;
		self->m_llCurrMemUsed *= 1024;
		ifs.close();
	}
	string msg;
	msg = "current mem used:";
	string sllCurrMemUsed;
	stringstream ss;
	ss << self->m_llCurrMemUsed;
	ss >> sllCurrMemUsed;
	msg += sllCurrMemUsed;
	client << msg;
	string rec;
	client >> rec;
	cout << rec << endl;
	client << "2";
	::close(client.m_sock);
	::shutdown(client.m_sock, 2);
	self->m_jobList.front().j_Status = 2;
}

void* Slave::test(void* s)
{
	Slave* self = (Slave*)s;
	ClientSocket client(self->masterAddress, self->m_jobList.front().portnum);
	string  str = "testsuccess!";
	str += self->localAddress;
	client << str;
	client >> str;
	::close(client.m_sock);
	::shutdown(client.m_sock, 2);
	self->m_jobList.front().j_Status = 2;
}
/*
void* Slave::partition(void* s)
{
	Slave* self = (Slave*)s;
	ClientSocket client(self->masterAddress, self->m_jobList.front().portnum);
	string  str = "testsuccess!";
	str += self->localAddress;
	client << str;
	client >> str;
	client << str;
	::close(client.m_sock);
	::shutdown(client.m_sock, 2);
	self->m_jobList.front().j_Status = 2;

}*/

void* Slave::partition(void* s)
{
	Slave* self = (Slave*)s;
	ClientSocket client(self->masterAddress, self->m_jobList.front().portnum);
	vector<string> filename;
	vector<string>::iterator ifile;
	self->trave_dir("./mantouDB/data/tmp", filename);
	string path;
	string t;
	ifile = filename.begin();
	string tn = *ifile;
	string p = "./mantouDB/data/tmp/";
	ifstream infileFormer;
	string str;
	infileFormer.open((p + tn).c_str());
	infileFormer >> t;
	//getline(infileFormer, t);
	client  << t;	//传回表名
	system(("mkdir ./mantouDB/data/" + t).c_str());
	
	client >> str;
	string fnum;
	stringstream ss;
	ss << filename.size();
	ss >> fnum;
	cout << "cfnum: " << fnum << endl;
	client << fnum;	//传回需要处理的文件的个数
	client >> str;
	for(ifile = filename.begin(); ifile != filename.end(); ifile++)
	{
		int dtype, linenum, dcnum;
		string tablename, dnum;
		string dname, line;
		path = "./mantouDB/data/tmp/";
		cout << self->localAddress << " " << *ifile << endl;
		path += *ifile;
		int p = (*ifile).find("tmp");
		dnum = (*ifile).substr(p + 3, (*ifile).length() - p + 3);
		cout << dnum << endl;	//维度编号
		
		ifstream infile;
		infile.open(path.c_str());
		infile >> tablename;
		infile >> dname;
		infile >> dtype;
		infile >> dcnum;
		stringstream s;
		string sdcnum;
		s << dcnum;
		s >> sdcnum;
		infile >> linenum;
		
		client << dname;
		client >> str;
//		cout << "dname:" << dname << endl;
//		cout << "dtype:" << dtype <<  endl;
//		cout << "linenum:" << linenum << endl;

		switch(dtype)
		{
			case 10:	//ID型int 不可重复
			{
				client << "10";
				client >> str;
				vector<long long> vID;
				vector<long long>::iterator ivID;
				long long id;
				int i;
				for(i = 0; i < linenum; i++)
				{
					infile >> id;
					vID.push_back(id);
				}
				sort(vID.begin(), vID.end());
				
				int step = linenum / dcnum;
				client << sdcnum;
				client >> str;
				int index = 0;
				for(i = 0; i < dcnum; i++)
				{
					string out;
					stringstream s;
					s <<  vID[index];
					s >> out;
					client << out;
					client >> str;
					index += step;
				}
				ofstream of;
				of.open(("./mantouDB/mantouDB/data/" + dname).c_str(), fstream::out);
				for(ivID = vID.begin(); ivID != vID.end(); ivID++)
				{
					of << *ivID << endl;
				}
				
				break;
			}
			case 11:	//大int 可重复
			{
				client << "11";
				client >> str;
				vector<long long> vLONG;
				vector<long long>::iterator ivLONG;
				long long LONG;
				int i;
				for(i = 0; i < linenum; i++)
				{
					infile >> LONG;
					vLONG.push_back(LONG);
				}
				sort(vLONG.begin(), vLONG.end());
				client << sdcnum;
				client >> str;
				int step = linenum / dcnum;
				int index = 0;
				for(i = 0; i < dcnum; i++)
				{
					string out;
					stringstream s;
					s <<  vLONG[index];
					s >> out;

					client <<  out;
					client >> str;
					index += step;
				}
	/*			ofstream of;
				of.open(("./mantouDB/mantouDB/data/" + dname).c_str(), fstream::out);
				for(ivLONG = vLONG.begin(); ivLONG != vLONG.end(); ivLONG++)
				{
					of << *ivLONG << endl;
				}
*/

				break;
			}
			case 12:	//小范围可重复int
			{
				client << "12";
				client >> str;
				vector<int> vINT;
				vector<int>::iterator ivINT;
				int INT;
				int i;
				for(i = 0; i < linenum; i++)
				{
					infile >> INT;
					vINT.push_back(INT);
				}
				sort(vINT.begin(), vINT.end());
				client << sdcnum;
				client >> str;
				int step = linenum / dcnum;
				int index = 0;
				for(i = 0; i < dcnum; i++)
				{
					string out;
					stringstream s;
					s <<  vINT[index];
					s >> out;
					client <<  out;
					client >> str;
					index += step;
				}
/*				ofstream of;
				of.open(("./mantouDB/mantouDB/data/" + dname).c_str(), fstream::out);
				for(ivINT = vINT.begin(); ivINT != vINT.end(); ivINT++)
				{
					of << *ivINT << endl;
				}
*/
				break;
			}
			case 20:	//float
			{
				client << "20";
				client >> str;
				vector<float> vFLOAT;
				vector<float>::iterator ivFLOAT;
				float FLOAT;
				int i;
				for(i = 0; i < linenum; i++)
				{
					infile >> FLOAT;
					vFLOAT.push_back(FLOAT);
				}
				sort(vFLOAT.begin(), vFLOAT.end());
				client << sdcnum;
				client >> str;
				int step = linenum / dcnum;
				int index = 0;
				for(i = 0; i < dcnum; i++)
				{
					string out;
					stringstream s;
					s <<  vFLOAT[index];
					s >> out;
					client <<  out;
					client >> str;
					index += step;
				}
/*				ofstream of;
				of.open(("./mantouDB/mantouDB/data/" + dname).c_str(), fstream::out);
				for(ivFLOAT = vFLOAT.begin(); ivFLOAT != vFLOAT.end(); ivFLOAT++)
				{
					of << *ivFLOAT << endl;
				}
*/
				break;
			} 
			case 30:	//小范围str
			{
				client << "30";
				client >> str;
				vector<string> vSTR;
				vector<string>::iterator ivSTR;
				string STR;
				int i;
				for(i = 0; i < linenum; i++)
				{
					infile >> STR;
					vSTR.push_back(STR);
				}
				sort(vSTR.begin(), vSTR.end());
				client << sdcnum;
				client >> str;
				int step = linenum / dcnum;
				int index = 0;
				for(i = 0; i < dcnum; i++)
				{
					string out;
					stringstream s;
					s <<  vSTR[index];
					s >> out;
					client <<  out;
					client >> str;
					index += step;
				}
/*				ofstream of;
				of.open(("./mantouDB/mantouDB/data/" + dname).c_str(), fstream::out);
				for(ivSTR = vSTR.begin(); ivSTR != vSTR.end(); ivSTR++)
				{
					of << *ivSTR << endl;
				}
*/
				break;
			}
			default: 
				break;
		}
		system(("rm " + path).c_str());
	}
	self->m_jobList.front().j_Status = 2;
	::close(client.m_sock);
	::shutdown(client.m_sock, 2);
}

int Slave::trave_dir(char* path, vector<string> &filename)
{
    DIR *d = opendir(path); //声明一个句柄
    struct dirent *file = NULL; //readdir函数的返回值就存放在这个结构体中
    struct stat sb;    
    string f;
    if(!d)
    {
        printf("error opendir %s!!!\n",path);
        return -1;
    }
    while((file = readdir(d)) != NULL)
    {
        //把当前目录.，上一级目录..及隐藏文件都去掉，避免死循环遍历目录
        if(strncmp(file->d_name, ".", 1) == 0)
            continue;
		f = file->d_name;
		filename.push_back(f);      
    }
    closedir(d);
    return 0;
}

void* Slave::readData(void *s)
{
	Slave *self = (Slave*)s;
	ClientSocket client(self->masterAddress, self->m_jobList.front().portnum);
	string srecvtime;
	int recvtime;
	stringstream ss;
	client >> srecvtime;
	client << "~~";
	ss << srecvtime;
	ss >> recvtime;
	
	vector<corrdinate> vcorrd;
	string scmd, scorrd, attrname;
	vector<string> vcmd;
	vector<string>::iterator ivcmd;
	vector<string> corrd;
	vector<string>::iterator icorrd;
	vector<slavetableinfo>:: iterator im_vslavetableInfo;
	vector<slavemapnode>::iterator idlist;
	while(recvtime)
	{
		client >> scmd;
		client << "~~";
		cout << "slave:" << scmd << endl;
		client >> scorrd;
		cout << "slave:" << scorrd << endl;
		client << "~~";
		vcmd.push_back(scmd);
		corrd.push_back(scorrd);
		recvtime--;
	}
	
	vector<slavemapnode> dlisttmp;
	string tablename, command, relation, cat;
	for(ivcmd = vcmd.begin(), icorrd = corrd.begin(); ivcmd != vcmd.end(); ivcmd++, icorrd++)
	{
		istringstream cmdstream(*ivcmd);
		istringstream corrdstream(*icorrd);
		cmdstream >> tablename;
		cmdstream >> command;
		corrdstream >> tablename;	
		for(im_vslavetableInfo = self->mt.m_vslavetableInfo.begin(); im_vslavetableInfo != self->mt.m_vslavetableInfo.end(); im_vslavetableInfo++)
		{
			if((*im_vslavetableInfo).tableName == tablename)
				break;
		}
		dlisttmp = (*im_vslavetableInfo).dlist;
		while(corrdstream >> attrname)
		{
			for(idlist = dlisttmp.begin(); idlist != dlisttmp.end(); idlist++)
			{
				if(attrname == (*idlist).dname)
					break;
			}
			corrdstream >> (*idlist).p1;	
			corrdstream >> (*idlist).p2;	
		}	
		
		do
		{	cmdstream >> attrname;		
			for(idlist = dlisttmp.begin(); idlist != dlisttmp.end(); idlist++)
			{
				if(attrname == (*idlist).dname)
					break;
			}

			cmdstream >> (*idlist).relation;
			cmdstream >> (*idlist).value;
			if((*idlist).maptype == 30)
			{
				if((*idlist).value.find("\"") == string::npos)
					(*idlist).value = "\"" + (*idlist).value + "\"";
			}
			(*idlist).hasRelation = true;
		}while(cmdstream >> cat);
		self->mt.readData(tablename, dlisttmp);
	}

	for(idlist = dlisttmp.begin(); idlist != dlisttmp.end(); idlist++)
	{
		cout << (*idlist).dname << " " << (*idlist).p1 << " " << (*idlist).p2 << endl;
	}
	
	self->m_jobList.front().j_Status = 2;
}
