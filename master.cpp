#include <iostream>
#include <string>
#include "master.h"
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sstream>
#include <fstream>

using namespace std;
int n = 0;

Master::Master()
{
}

Master::~Master()
{
}

int Master::init()
{
    relStyle rel;
    string filename = "conf.xml";
    cr.ReadConfFile(filename, rel); //获取masterconf数据
    cout << rel.DNum << endl;
    vector<int>::iterator irel;
    for(irel = rel.DClass.begin(); irel != rel.DClass.end(); irel++)
        cout << *irel << endl;
	m_Status = RUNNING;
	inCmdPort = 53000;
	outCmdPort = 53001;
	m_itasknum = 0;
	m_isockcount = 0;
	
	//MTFS mtfs;
	mt.MasterDataInit(this); //调用MTFS来做数据划分
	vector<tableinfo>::iterator im_vtableInfo, im_vtableInfo2;
	map<string, mapnode>::iterator idlist;
	vector<pair<string, int> >::iterator ivsi;
	vector<pair<int, int> >::iterator ivii;
	vector<pair<float, int> >::iterator ivfi;
	for(im_vtableInfo = mt.m_vtableInfo.begin(); im_vtableInfo != mt.m_vtableInfo.end(); im_vtableInfo++)
	{
		cout << "tablename: " << (*im_vtableInfo).tableName << endl;
		cout << "dnum: " << (*im_vtableInfo).dlist.size() << endl;
		for(idlist = (*im_vtableInfo).dlist.begin(); idlist != (*im_vtableInfo).dlist.end(); idlist++)
		{
			cout << (*idlist).first << " ";
			switch((*idlist).second.maptype)
			{
				case 10:
				case 11:
				case 12:
				{
					for(ivii = (*idlist).second.vii.begin(); ivii != (*idlist).second.vii.end(); ivii++)
					{
						cout << (*ivii).first << " " << (*ivii).second << " ";
					}
	//				cout << endl;
					break;
				}
				case 20:
				{
					for(ivfi = (*idlist).second.vfi.begin(); ivfi != (*idlist).second.vfi.end(); ivfi++)
					{
						cout << (*ivfi).first << " " << (*ivfi).second << " ";
					}
	//				cout << endl;
					break;
				}
				case 30:
				{
					for(ivsi = (*idlist).second.vsi.begin(); ivsi != (*idlist).second.vsi.end(); ivsi++)
					{
						cout << (*ivsi).first << " " << (*ivsi).second << " ";
					}
	//				cout << endl;
					break;
				}
				default:
					break;
			}
			cout << " " <<  (*idlist).second.splitnum << endl;
		}
	}

    return 0;
}

int Master::run()
{
	pthread_t startserver;
	if(pthread_create(&startserver, NULL, start, this))		//start线程，启动每个slave，对joblist做初始化
		cout << "create thread error" << endl;
	void *exit = (int*)malloc(4);
//	pthread_detach(startserver);
	pthread_join(startserver, &exit);
}

void* Master::start(void *s)
{
	Master* self = (Master*)s;
	vector<ServerSocket> vsock;
	ServerSocket initServer(self->inCmdPort);
	self->m_vsock.push_back(initServer);
	self->m_isockcount++;
	string base, username;
	self->cr.getSlaveList("slaves", base, username, self->slave_list);
	cout << base << " " << username<<endl;
	vector<slavelist>::iterator islave;
	for(islave = self->slave_list.begin(); islave != self->slave_list.end(); islave++)	//根据slave文件启动每一个节点的slave
	{
		ServerSocket slaveCom;
		vsock.push_back(slaveCom);
		self->startSlave((*islave).ip, base, "", "");		
	}
	int count = 0;
	vector<TCPInfo> vTCPInfo;
	while(self->m_Status == RUNNING)
	{
		string data;
		initServer.accept(vsock[count]);
		cout << "init sock:" <<  initServer.m_sock << endl;
		try
		{
			vsock[count] >> data;
		}
		catch(SocketException& e)
		{
			cout << "error:" << e.description() << endl;
		}
		int i;
		string str;
		for(i = 0; i < 16; i++)
		{
			if(data[i] == ' ')
			{
				break;
			}
			str += data[i];
		}
		TCPInfo tinfo;		//把建立起来的t给保存下来
		tinfo.server = &(vsock[count]);
		tinfo.ip = str;
		tinfo.outport = self->outCmdPort;
		tinfo.inport = self->inCmdPort;
		tinfo.m = self;
 		vTCPInfo.push_back(tinfo);
	
		struct SlaveNode sn;
		sn.ip = str;
		int num;
		for(islave = self->slave_list.begin(); islave != self->slave_list.end(); islave++)	//根据slave文件启动每一个节点的slave
		{
			int p1 = (*islave).ip.find("@");
			int p2 = (*islave).ip.find(" ");
			string ip = (*islave).ip.substr(p1 + 1, p2);
			if(ip == str)
			{
				num = (*islave).num;
				break;
			}
		}
		sn.num = num;
		self->activeSlaves.push_back(sn);

		slaveJob sj;
		sj.slave = str;
		job j;
		j.jobtype = 1;
		j.j_Status = 0;
		sj.slaveJobList.push(j);
		self->m_jobList.push_back(sj);
		count++;
		if(count == self->slave_list.size())
			break;
	}
	job j;
	j.jobtype = 1;
	j.j_Status = 0;
	self->m_qmasterjobList.push(j);
	self->m_itasknum++;	
	vector<struct SlaveNode>::iterator islavenode;
	for(islavenode = self->activeSlaves.begin(); islavenode != self->activeSlaves.end(); islavenode++)	//根据slave文件启动每一个节点的slave
	{
		cout << "active slaves:" << (*islavenode).num << " " << (*islavenode).ip << endl;
	}

	vector<slaveJob>::iterator islavejob;
/*	for(islavejob = self->jobList.begin(); islavejob != self->jobList.end(); islavejob++)	//根据slave文件启动每一个节点的slave
	{
		cout << "slave job:" << (*islavejob).slave << endl;
		cout << "slave job type:" << (*islavejob).slaveJobList.front().jobtype << endl;
		cout << "slave job status:" << (*islavejob).slaveJobList.front().j_Status << endl;
	}
*/
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	pthread_attr_setscope(&tattr, PTHREAD_SCOPE_SYSTEM);

	pthread_t pjobdealer;
	void* exit = (int*)malloc(4);
	pthread_create(&pjobdealer, &tattr, jobDealer, self);
	pthread_detach(pjobdealer);
	pthread_join(pjobdealer, &exit);		
	
	pthread_t pjobchecker;
	pthread_create(&pjobchecker, &tattr, jobChecker, self);
	pthread_detach(pjobchecker);
	pthread_join(pjobchecker, &exit);		

	pthread_mutex_t lock;
	pthread_mutex_init(&lock, NULL);
	vector<TCPInfo>::iterator iTCPInfo;
	int k;
	for(iTCPInfo = vTCPInfo.begin(), k = 0 ; iTCPInfo != vTCPInfo.end(); k++)
	{
		pthread_mutex_lock(&lock);
		TCPInfo tinfo = *iTCPInfo;
		cout << (*iTCPInfo).ip << endl;
		pthread_t pslavecom;
//		pthread_create(&pslavecom, NULL, SlaveCom, &tinfo);
		pthread_create(&pslavecom, &tattr, SlaveCom, &vTCPInfo[k]);
		iTCPInfo++;
		pthread_detach(pslavecom);
		pthread_mutex_unlock(&lock);
		pthread_join(pslavecom, &exit);		
	//	sleep(2);
	}
	pthread_mutex_destroy(&lock);
	pthread_t pshell;		//启动shell
	if(pthread_create(&pshell, &tattr, Shell, self))
		cout << "create thread error" << endl;
	//void *exit = (int*)malloc(4);
//	pthread_detach(pshell);
	pthread_join(pshell, &exit);
}

//向各个slave传递命令的线程
void* Master::SlaveCom(void *s)
{
	TCPInfo* self = (TCPInfo*)s;
	vector<slaveJob>::iterator ij;
	cout << "sending cmd to" <<  self->ip <<  ":" << self->inport<<endl;
	queue<job>* pslaveJob = (queue<job>*)malloc(16);
	for(ij = self->m->m_jobList.begin(); ij != self->m->m_jobList.end(); ij++)
	{
		if((*ij).slave == self->ip)		//找到该slave的job队列
		{
			pslaveJob = &(*ij).slaveJobList;
			break;
		}
	}	
	string ip;
	int port;
	ServerSocket socket = *(self->server);
	while(self->m->m_Status == RUNNING)
	{

		if(self->m->m_qmasterjobList.front().j_Status == 0)
			self->m->m_qmasterjobList.front().j_Status = 1;
		if(pslaveJob->size() != 0 && pslaveJob->front().j_Status == 0) //如果job列表不空且第一个job状态为等待处理，则将其提交给slave
		{
		
			job j = pslaveJob->front();
			int jobtype = j.jobtype;
			string cmd;
			stringstream ss;
			ss << jobtype;
			ss >> cmd;
			try
			{
				socket << cmd;		//发送当前任务
				int tasknum = self->m->inCmdPort + self->m->m_itasknum;
				cout << "sendpornum: " << tasknum << endl; 
				ss.clear();
				ss << tasknum;
				string num;
				ss >> num;
				socket << num;
			}
			catch(SocketException& e)
			{
				cout << "error:" << e.description() << endl;
			}
			pslaveJob->front().j_Status = 1;
		}
	}
}

void* Master::jobDealer(void*s)
{
	Master* self = (Master*)s;
	while(self->m_Status == RUNNING)
	{
		if(self->m_qmasterjobList.size() != 0 && self->m_qmasterjobList.front().j_Status == 1) //如果job列表不空且第一个job状态为等待处理，则将其提交给slave
		{
			int count = 0;
			job j = self->m_qmasterjobList.front();
			int jobtype = j.jobtype;
					
			vector<TCPInfo> vTCPInfo;
			vector<ServerSocket> vsock;
			ServerSocket socket(self->inCmdPort + self->m_itasknum);		
			vector<struct SlaveNode>::iterator islavenode;
			for(islavenode = self->activeSlaves.begin(); islavenode != self->activeSlaves.end(); islavenode++)	
			{
				ServerSocket jobCom;
				vsock.push_back(jobCom);
			}
			count = 0;
			cout << "port: " << self->inCmdPort + self->m_itasknum << endl;
			for(islavenode = self->activeSlaves.begin(); islavenode != self->activeSlaves.end(); islavenode++)	//根据activeslaves获取每一个新的连接
			{	
				socket.accept(vsock[count]);
				
				TCPInfo tinfo;		//把建立起来的t给保存下来
				cout << "task server " << endl;
				tinfo.server = &vsock[count];
				cout << "server sock:"<< tinfo.server->m_sock << endl;
				tinfo.inport = self->inCmdPort + self->m_itasknum;
				tinfo.ip = (*islavenode).ip;
				tinfo.m = self;
				vTCPInfo.push_back(tinfo);
				count++;
			}		
			vector<TCPInfo>::iterator iTCPInfo;
		
			switch(jobtype)
			{
				case 1:
				{
					int k;
					//for(iTCPInfo = vTCPInfo.begin(); iTCPInfo != vTCPInfo.end(); iTCPInfo++)
					//for(k = 0; k != vTCPInfo.size();  k++)
				//	pthread_mutex_t lock;
				//	pthread_mutex_init(&lock, NULL);
					for(k = 0, iTCPInfo = vTCPInfo.begin(); iTCPInfo != vTCPInfo.end(); k++)
					{
					//	pthread_mutex_lock(&lock);
						TCPInfo tinfo = *iTCPInfo;
						pthread_t pgetSysinfo;
						pthread_create(&pgetSysinfo, NULL, getSysinfo, &vTCPInfo[k]);
						void* exit = (int*)malloc(4);
						iTCPInfo++;
					//	pthread_detach(pgetSysinfo);
					//	pthread_mutex_unlock(&lock);
						pthread_join(pgetSysinfo, &exit);		
					//	sleep(1);
					}
				//	pthread_mutex_destroy(&lock);
					break;
				}
				case 3:
				{
					for(iTCPInfo = vTCPInfo.begin(); iTCPInfo != vTCPInfo.end(); iTCPInfo++)
					{
						cout << "in 3" << endl;
						cout << (*iTCPInfo).server->m_sock << endl;
						TCPInfo tinfo = *iTCPInfo;
						pthread_t pgetPartition;
						pthread_create(&pgetPartition, NULL, getPartition, &tinfo);
						void* exit = (int*)malloc(4);
					//	pthread_detach(pgetSysinfo);
						pthread_join(pgetPartition, &exit);		
						sleep(1);
					}
					pthread_t pdatasplit;
					pthread_create(&pdatasplit, NULL, dataSplit, self);
					void* exit = (int*)malloc(4);
					//	pthread_detach(pgetSysinfo);
					pthread_join(pdatasplit, &exit);		
				
					break;
				}
				case 4:
				{
					for(iTCPInfo = vTCPInfo.begin(); iTCPInfo != vTCPInfo.end(); iTCPInfo++)
					{
						cout <<  "in 4" << endl;
						cout << (*iTCPInfo).server->m_sock << endl;
						TCPInfo tinfo = *iTCPInfo;
						pthread_t ptest;
						pthread_create(&ptest, NULL, test, &tinfo);
						void* exit = (int*)malloc(4);
					//	pthread_detach(pgetSysinfo);
						pthread_join(ptest, &exit);		
						sleep(1);
					}
					break;
				}
				case 5:
				{
					cout << "in 5" << endl;
					latertcptype ltt;
					ltt.master = self;
					ltt.vTCPInfo = vTCPInfo;
					pthread_t ptestget;
					pthread_create(&ptestget, NULL, getData, &ltt);
					void* exit = (int*)malloc(4);
					//	pthread_detach(pgetSysinfo);
					pthread_join(ptestget, &exit);		
					sleep(1);
					break;
				}
				default:
					break;
			}
			
			if(self->m_qmasterjobList.front().j_Status == 1)
				self->m_qmasterjobList.front().j_Status = 2;
			vector<slaveJob>::iterator islavejob;
			for(islavejob = self->m_jobList.begin(); islavejob != self->m_jobList.end(); islavejob++)
			{
				if((*islavejob).slaveJobList.front().j_Status == 1)
					(*islavejob).slaveJobList.front().j_Status = 2;

			}
			::close(socket.m_sock);
			::shutdown(socket.m_sock,2);
		}
	}
}

//检查masterjoblist中已经完成的任务，将其删除
void* Master::jobChecker(void* s)
{
	Master* self = (Master*)s;
	int count;
	vector<slaveJob>::iterator islavejob;
	while(self->m_Status == RUNNING)
	{
		count = 0;
		for(islavejob = self->m_jobList.begin(); islavejob != self->m_jobList.end(); islavejob++)
		{
			if((*islavejob).slaveJobList.front().j_Status != 3)
				break;
			count++;
		}
		if(count == self->m_jobList.size())
		{
			self->m_qmasterjobList.front().j_Status = 3;
			for(islavejob = self->m_jobList.begin(); islavejob != self->m_jobList.end(); islavejob++)
				(*islavejob).slaveJobList.pop();
			self->m_qmasterjobList.pop();
			cout << "JOB " << self->m_itasknum << " is finished!" << endl; 
		}
	}
}

//Master的shell线程
void* Master::Shell(void *s)
{
	Master* self = (Master*)s;
	string line, word;
	cout << ">>";
	while(getline(cin,line))
	{
		istringstream stream(line);
		cout << "line:" << line << endl;
		cin.clear();
		while(stream >> word)
		{	
			cout << "word:" << word << endl;
			if("getstat" == word)
			{
				stream.clear();
				self->m_itasknum++;
				job j;
				vector<slaveJob>::iterator ij;
				for(ij = self->m_jobList.begin(); ij != self->m_jobList.end(); ij++)
				{
					j.jobtype = 1;
					j.j_Status = 0;
					(*ij).slaveJobList.push(j);
				}
				self->m_qmasterjobList.push(j);
				break;
			}
			else if("q" == word)
			{
				self->m_itasknum++;
				job j;
				vector<slaveJob>::iterator ij;
				for(ij = self->m_jobList.begin(); ij != self->m_jobList.end(); ij++)
				{
					j.jobtype = 2;
					j.j_Status = 0;
					(*ij).slaveJobList.push(j);
					cout << "sending to slaves:" << (*ij).slave << endl;
				}
				self->m_qmasterjobList.push(j);
				sleep(2);
				self->m_Status = STOPPED;
				cout << "bye" << endl;
				exit(0);
			}
			else if("test" == word)
			{
				self->m_itasknum++;
				job j;
				vector<slaveJob>::iterator ij;
				for(ij = self->m_jobList.begin(); ij != self->m_jobList.end(); ij++)
				{
					j.jobtype = 4;
					j.j_Status = 0;
					(*ij).slaveJobList.push(j);
					cout << "sending to slaves:" << (*ij).slave << endl;
				}
				self->m_qmasterjobList.push(j);
			}
			else if("gettest" == word)
			{
				self->m_itasknum++;
				job j;
				vector<slaveJob>::iterator ij;
				for(ij = self->m_jobList.begin(); ij != self->m_jobList.end(); ij++)
				{
					j.jobtype = 5;
					j.j_Status = 0;
					(*ij).slaveJobList.push(j);
					cout << "sending to slaves:" << (*ij).slave << endl;
				}
				self->m_qmasterjobList.push(j);
			}
			else if("showjob" == word)
			{
				vector<slaveJob>::iterator islavejob;
				for(islavejob = self->m_jobList.begin(); islavejob != self->m_jobList.end(); islavejob++)
				{
					cout << (*islavejob).slave << " jobtype:" << (*islavejob).slaveJobList.front().jobtype << endl;
					cout << (*islavejob).slave << " jobstatus:" << (*islavejob).slaveJobList.front().j_Status << endl;
				}
				cout << "master list is empty?: " << self->m_qmasterjobList.empty() << endl;
				cout << "master list front type: " << self->m_qmasterjobList.front().jobtype << endl;
				cout << "master list front status: " << self->m_qmasterjobList.front().j_Status << endl;
			}
			else if("createnew" == word)
			{
				self->m_itasknum++;
				string tablename, filename, fileconf;
				stream >> tablename;
				stream >> filename;
				stream >> fileconf;
				tableinfo table;
				
				bool gout = false;
				vector<tableinfo>::iterator im_vtableInfo;
				for(im_vtableInfo = self->mt.m_vtableInfo.begin(); im_vtableInfo != self->mt.m_vtableInfo.end(); im_vtableInfo++)
				{
					if((*im_vtableInfo).tableName == tablename)
					{
						cout << tablename << " is used!" << endl;
						gout = true;
						break;
					}
				}
				if(gout)
					continue;

				table.filename = filename;
				table.confname = fileconf;
				table.tableName = tablename;
				ifstream  infile, inconf;
				string filepath = "../data/";
				infile.open((filepath + filename).c_str());
				if(!infile)
				{
					cout << "cannot open file " << filename << "!" << endl;
					continue;
				}
				inconf.open((filepath + fileconf).c_str());
				if(!inconf)
				{
					cout << "cannot open fileconf " << fileconf << "!" << endl;
					continue;
				}
				string vline;
				int linenum, dnum;
				inconf >> linenum >> dnum;
				cout << "linenum:" << linenum << endl << "dnum:" << dnum << endl;
				getline(inconf, vline);
				getline(inconf, vline);
				int i;
				string dname;
				int dtype;
				vector<ofstream*> vtmpfile;
				vector<ofstream*>::iterator ivtmpfile;
				int dcnum;	
				
				for(i = 0; i < dnum; i++)
				{
					inconf >> dname; 
					inconf >> dtype;
					inconf >> dcnum;
					dcnum *= self->activeSlaves.size();
					table.dindex.push_back(dname);
					table.dcnum.push_back(dcnum);
					mapnode mn;
					int idtype;
					stringstream s;
					s << dtype;
					s >> idtype;
					mn.maptype = idtype;
					table.dlist.insert(make_pair(dname, mn));

					stringstream ss;
					string index;
					string tmpname;
					tmpname = filepath + "tmp/" + filename + "tmp";
					ss << i;
					ss >> index;
					tmpname += index;
					ofstream *of = new ofstream();
					of->open(tmpname.c_str(), fstream::out);
					if(!of)
					{
						cout << "tmp file create error!" << endl;
						break;
					}
					*of << tablename << endl;
					*of << dname << endl;
					*of << dtype << endl;
					*of << dcnum << endl;
					*of << linenum/100 << endl;
					vtmpfile.push_back(of);
				}
				
				self->mt.m_vtableInfo.push_back(table);
				
				string line;
				int count = 0;
				getline(infile,line);
				for(i = 0; i < linenum; i++)
				{
					line = "";
					getline(infile,line);
					if((count % 100) == 0)
					{
						int comapos1 = 0, comapos2;
						//getline(infile, line);
						for(ivtmpfile = vtmpfile.begin(); ivtmpfile != vtmpfile.end(); ivtmpfile++)
						{
							comapos2 = line.find(',',comapos1);
							if(comapos2 == comapos1)
							{
								*(*ivtmpfile) << endl;
								comapos1 = comapos2 + 1;
								continue;
							}
							*(*ivtmpfile)  << line.substr(comapos1, comapos2 - comapos1) << endl;
							comapos1 = comapos2 + 1;
						}
					}
					count++;
				}
				string base, username;
				self->cr.getSlaveList("slaves", base, username, self->slave_list);
				cout << "base:" << base << endl;
				string tmpname,index;
				vector<struct SlaveNode>::iterator iactiveSlaves = self->activeSlaves.begin();
				for(i = 0; i < dnum; i++)
				{
					stringstream ss;
					index = "";
					tmpname = filepath + "tmp/" + filename + "tmp";
					ss << i;
					ss >> index;
					tmpname += index;
					if(iactiveSlaves == self->activeSlaves.end())
						iactiveSlaves = self->activeSlaves.begin();
					string cmd = (string("scp -P 13022 -r ") + tmpname + " " + username + "@" + (*iactiveSlaves).ip + ":" + base  + "/data/tmp/" + filename + "tmp"+ index);
					system(cmd.c_str());
					iactiveSlaves++;
				}
				string cmd = "rm ../data/tmp/*";
				system(cmd.c_str());
				for(ivtmpfile = vtmpfile.begin(); ivtmpfile != vtmpfile.end(); ivtmpfile++)
				{
					delete *ivtmpfile;
				}

				job j;
				vector<slaveJob>::iterator ij;
				for(ij = self->m_jobList.begin(); ij != self->m_jobList.end(); ij++)
				{
					j.jobtype = 3;
					j.j_Status = 0;
					(*ij).slaveJobList.push(j);
					cout << "sending to slaves:" << (*ij).slave << endl;
				}
				cout << "dcnumsize:" << (*((self->mt.m_vtableInfo.end())-1)).dcnum.size() << endl;
				self->m_qmasterjobList.push(j);
				//	self->m_itasknum++;
				
			}
			else 
			{
				cout << "wrong command!" << endl;
				break;
			}
		}
		cout << ">>";
	}
}

void Master::startSlave(const std::string& addr, const std::string& base, const std::string& option, const std::string& log)	//启动slave节点
{
	pthread_mutex_t lock;
	pthread_mutex_init(&lock, NULL);
	pthread_mutex_lock(&lock);
	
	tmp1 = addr;
	tmp2 = base;
	struct addrStruct t;
	t.addr = addr;
	t.base = base;
	cout << t.addr << endl;
	pthread_t pstartSlave;
	if(pthread_create(&pstartSlave, NULL, System, &(t)))
		cout << "create thread error" << endl;
	void *exit = (int*)malloc(4);
	pthread_detach(pstartSlave);
	pthread_mutex_unlock(&lock);
	pthread_join(pstartSlave, &exit);
	pthread_mutex_destroy(&lock);
	sleep(1);
}

void* Master::System(void *s)
{
	struct addrStruct *self;
	self = (struct addrStruct*)s;
	string start_slave = self->base + "/mantouDB/slave/";
	string cmd = (string("ssh -p 13022 -o StrictHostKeychecking=no ") + self->addr + " " + self->base  + "/mantouDB/start_slave");
	cout << "starting " << self->addr << endl;
	system(cmd.c_str());
}

//任务一，获取slave节点的系统信息
void* Master::getSysinfo(void* s)
{
	TCPInfo* self = (TCPInfo*)s;
	string sysinfo,s2;
	ServerSocket socket = *(self->server);
	socket >> sysinfo; 
	socket << "???";
	socket >> s2; 
	cout << "master " << self->ip << " sysinfo:" << sysinfo << endl;
	queue<job>* pslaveJob = (queue<job>*)malloc(16);
	vector<slaveJob>::iterator ij;
	::close((*(self->server)).m_sock);
	::shutdown((*(self->server)).m_sock,2);
	for(ij = self->m->m_jobList.begin(); ij != self->m->m_jobList.end(); ij++)
	{
		if((*ij).slave == self->ip)		//找到该slave的job队列
		{
			pslaveJob = &(*ij).slaveJobList;
			pslaveJob->front().j_Status = 3;
			break;
		}
	}
}

void* Master::test(void* s)
{
	TCPInfo* self = (TCPInfo*)s;
	ServerSocket server = (*self->server);
	string text, t;
	server >> text;
	server << text;
	
	cout << "test:" << text << endl;
	queue<job>* pslaveJob = (queue<job>*)malloc(16);
	vector<slaveJob>::iterator ij;
	::close((*(self->server)).m_sock);
	::shutdown((*(self->server)).m_sock,2);
	for(ij = self->m->m_jobList.begin(); ij != self->m->m_jobList.end(); ij++)
	{
		if((*ij).slave == self->ip)		//找到该slave的job队列
		{
			pslaveJob = &(*ij).slaveJobList;
			pslaveJob->front().j_Status = 3;
			break;
		}
	}
}

void* Master::getPartition(void* s)
{
	TCPInfo* self = (TCPInfo*)s;
	string tablename, recvtimes;
	int recvt, i;
	tableinfo ti;
	ServerSocket server = (*self->server);
//	cout << "getpart sock:" << (*(self->server)).m_sock << endl;
	server >> tablename;	//接收的表头
	server << "?";	//接收的表头

	server >> recvtimes;	//得到应有的接收次数，发来的元组个数
	server << "?";
	cout << "tablename:" << tablename << endl;
	cout << "recvtimes:" << recvtimes << endl;
	stringstream ss;
	ss << recvtimes;
	ss >> recvt;
	ss.clear();

	vector<tableinfo>::iterator im_vtableInfo;
	for(im_vtableInfo = self->m->mt.m_vtableInfo.begin(); im_vtableInfo != self->m->mt.m_vtableInfo.end(); im_vtableInfo++)
	{
		if((*im_vtableInfo).tableName == tablename)
			break;
	}
	
	map<string, mapnode>::iterator idlist;
	for(i = 0; i < recvt; i++)
	{
		string dname;
		server >> dname;
		server << "?";
		int count = 0;
	/*	for(idlist = (*im_vtableInfo).dlist.begin(); idlist != (*im_vtableInfo).dlist.end(); idlist++)
		{
			count++;
			if((*idlist).dname == dname)
				break;
		}
*/
		string smaptype;
		int imaptype;
		ss.clear();
		server >> smaptype;
		server << "?";
		ss << smaptype;
		ss >> imaptype;
		(*im_vtableInfo).dlist[dname].maptype = imaptype;
	//	(*idlist).maptype = imaptype;
		switch(imaptype)
		{
			case 10:
			case 11:
			case 12:
			{
				ss.clear();
				string sdcnum;
				int dcnum;
				server >> sdcnum;
				server << ">";
				ss << sdcnum;
				ss >> dcnum;
				int j;
				string srec;
				int rec;
				for(j = 0; j < dcnum; j++)
				{
					ss.clear();
					server >> srec;
					server << ">>";
					ss << srec;
					ss >> rec;
					//(*idlist).mii.insert(make_pair(rec, 0));
					(*im_vtableInfo).dlist[dname].vii.push_back(make_pair(rec, 0));
				}
				cout << "int finish" << endl;
	//			(*im_vtableInfo).dlist.push_back(mn);
				break;
			}		
			case 20:
			{
				ss.clear();
				string sdcnum;
				int dcnum;
				server >> sdcnum;
				server << ">";
				ss << sdcnum;
				ss >> dcnum;
				int j;
				string srec;
				float rec;
				for(j = 0; j < dcnum; j++)
				{
					ss.clear();
					server >> srec;
					server << ">>";
					ss << srec;
					ss >> rec;
				//	(*idlist).mfi.insert(make_pair(rec, 0));
					(*im_vtableInfo).dlist[dname].vfi.push_back(make_pair(rec, 0));
				}
	//			(*im_vtableInfo).dlist.push_back(mn);
				break;
			}
			case 30:
			{
				ss.clear();
				string sdcnum;
				int dcnum;
				server >> sdcnum;
				server << ">";
				ss << sdcnum;
				ss >> dcnum;
				int j;
				string srec;
				float rec;
				cout << "pos:" << count << endl;
				for(j = 0; j < dcnum; j++)
				{
			//		ss.clear();
					server >> srec;
					server << ">>";
			//		ss << srec;
			//		cout << srec << endl;
					//(*idlist).msi.insert(make_pair(srec, 0));
					(*im_vtableInfo).dlist[dname].vsi.push_back(make_pair(srec, 0));
				}
	//			(*im_vtableInfo).dlist.push_back(mn);
				break;
			}
			default:
				break;
		}
	}

	::close((*(self->server)).m_sock);
	::shutdown((*(self->server)).m_sock,2);
	queue<job>* pslaveJob = (queue<job>*)malloc(16);
	vector<slaveJob>::iterator ij;
	for(ij = self->m->m_jobList.begin(); ij != self->m->m_jobList.end(); ij++)
	{
		if((*ij).slave == self->ip)		//找到该slave的job队列
		{
			pslaveJob = &(*ij).slaveJobList;
			pslaveJob->front().j_Status = 3;
			break;
		}
	}
//	free(pslaveJob);
}

void* Master::dataSplit(void* s)
{
	Master* self = (Master*)s;
	self->mt.dataSplit(self); //调用MTFS来做数据划分
	
	vector<struct SlaveNode>::iterator iactiveSlaves;
	vector<string> filename;
	vector<string>::iterator ifilename;
	map<string, vector<string> > splitfilename;
	map<string, vector<string> >::iterator isplitfilename;
	self->trave_dir("../data/tmp", filename);
	
	int p1, p2;
	string tablename = *(filename.begin());
	p1 = tablename.find(".");
	tablename = tablename.substr(0, p1);

	for(ifilename = filename.begin(); ifilename != filename.end(); ifilename++)
	{
	//	cout << *ifilename << endl;
		string slaveno = *ifilename;
		p1 = slaveno.find(".");
		p2 = slaveno.find(".", p1 + 1);
		slaveno = slaveno.substr(p1 + 1, p2 - p1 - 1);
		cout << slaveno << endl;
		splitfilename[slaveno].push_back(*ifilename);
	}
	
	string base, username;
	self->cr.getSlaveList("slaves", base, username, self->slave_list);
	string no;
	int i = 1;
	stringstream ss;
	for(iactiveSlaves = self->activeSlaves.begin(); iactiveSlaves != self->activeSlaves.end(); iactiveSlaves++, i++)
	{
		ss.clear();
		ss << i;
		ss >> no;
		sendfileinfo sf;
		sf.ip = (*iactiveSlaves).ip;
		sf.num = no;
		sf.base = base;
		sf.tablename = tablename;
		sf.username = username;
		sf.filename = splitfilename[no];
//		for(ifilename = sf.filename.begin(); ifilename != sf.filename.end(); ifilename++)
//			cout << *ifilename << endl;
		pthread_t psenddata;
		pthread_create(&psenddata, NULL, senddata, &sf);
		void* exit = (int*)malloc(4);
		pthread_join(psenddata, &exit);
	}
}

void* Master::senddata(void* s)
{
	sendfileinfo *self = (sendfileinfo*)s;
	vector<string>::iterator ifilename;
	string filepath = "../data/tmp/";
	string cmd;
	for(ifilename = self->filename.begin(); ifilename != self->filename.end(); ifilename++)
	{
		cmd = (string("scp -P 13022 -r ") + filepath + (*ifilename) + " " + self->username + "@" + self->ip + ":" + self->base  + "/data/" + self->tablename +  "/" + *ifilename);
		system(cmd.c_str());
		system(("rm " + filepath + (*ifilename)).c_str());
	}
	string confpath = "../data/meta/";
	cmd = (string("scp -P 13022 -r ") + confpath + self->tablename + ".meta" + " " + self->username + "@" + self->ip + ":" + self->base  + "/data/meta/" + self->tablename + ".meta");
	system(cmd.c_str());

}

int Master::trave_dir(char* path, vector<string> &filename)
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

void Master::decode(vector<string> c, vector<tablecorrdinate> &vtablecorrdinate)	//将查询语句转化成查询坐标
{
	vector<string>::iterator ic;
	vector<tableinfo>::iterator im_vtableInfo;
	vector<pair<string, int> >::iterator ivsi1, ivsi2;
	vector<pair<int, int> >::iterator ivii1, ivii2;
	vector<pair<float, int> >::iterator ivfi1, ivfi2;
	string tablename, command, attrname, relation, value, connect;
	int type, ivalue;
	int p1, p2;		//坐标上下界
	float fvalue;
	stringstream ss;
	for(ic = c.begin(); ic != c.end(); ic++)
	{
		tablecorrdinate tc;
		istringstream stream(*ic);
		stream >> tablename;
		tc.tablename = tablename;
		for(im_vtableInfo = mt.m_vtableInfo.begin(); im_vtableInfo != mt.m_vtableInfo.end(); im_vtableInfo++)
		{
			if(tablename == (*im_vtableInfo).tableName)
			{
				break;
			}
		}
		if(im_vtableInfo == mt.m_vtableInfo.end())
			cout << "no such table!" << endl;
		
		stream >> command;
		do
		{
			corrdinate corrdset;
			stream >> attrname;
			stream >> relation;
			stream >> value;
			corrdset.attrname = attrname;
			type = (*im_vtableInfo).dlist[attrname].maptype;
			switch(type)
			{
				case 10:
				case 11:
				case 12:
				{
					ss.clear();
					ss << value;
					ss >> ivalue;
					ivii1 = (*im_vtableInfo).dlist[attrname].vii.begin();
					ivii2 = (*im_vtableInfo).dlist[attrname].vii.begin() + 1; 
					while((*ivii1).first == (*ivii2).first)	//初始，让1和2指向不同的值 
						ivii2++;
					if(relation == "=")
					{
						while(1)
						{
							if(ivii2 == (*im_vtableInfo).dlist[attrname].vii.end())
								break;
							while(ivalue >= (*ivii2).first && (*ivii1).first == (*ivii2).first && ivii2 != (*im_vtableInfo).dlist[attrname].vii.end())
							{
								ivii2++;
							}
							if(ivalue > (*ivii2).first && (*ivii1).first != (*ivii2).first)
							{
								ivii1 = ivii2;
								ivii2++;
								continue;
							}
							if(ivalue < (*ivii2).first)
							{
								ivii2--;
								break;
							}

						}
					}
					else if(relation == ">" || relation == ">=")
					{
						while(1)
						{
							while((*ivii2).first < ivalue)
							{
								ivii1 = ivii2;
								while((*ivii1).first == (*ivii2).first && ivii2 != (*im_vtableInfo).dlist[attrname].vii.end())	//让1和2指向不同的值 
									ivii2++;
							}
							if(ivii2 == (*im_vtableInfo).dlist[attrname].vii.end())	//返回的是最后一组
								break;
	
							if((*ivii2).first == ivalue)
							{
								ivii1 = ivii2;
							}

							ivii2 = (*im_vtableInfo).dlist[attrname].vii.end();
							break;
						}
					}
					else if(relation == "<")
					{
						while((*ivii2).first < ivalue && ivii2 != (*im_vtableInfo).dlist[attrname].vii.end())
						{
							ivii2++;
						}
						ivii2--;
					}
					else if(relation == "<=")
					{
						while((*ivii2).first <= ivalue && ivii2 != (*im_vtableInfo).dlist[attrname].vii.end())
						{
							ivii2++;
						}
						ivii2--;
					}
					p1 = distance((*im_vtableInfo).dlist[attrname].vii.begin(), ivii1) + 1;
					if(ivii2 == (*im_vtableInfo).dlist[attrname].vii.end())
						ivii2 = ivii2 - 1;
					p2 = distance((*im_vtableInfo).dlist[attrname].vii.begin(), ivii2) + 1;
					ss.clear();
					ss << p1;
					ss >> corrdset.p1;
					ss.clear();
					ss << p2;
					ss >> corrdset.p2;
					break;
				}
				case 20:
				{
					ss.clear();
					ss << value;
					ss >> fvalue;
					cout << fvalue<< endl;
					ivfi1 = (*im_vtableInfo).dlist[attrname].vfi.begin();
					ivfi2 = (*im_vtableInfo).dlist[attrname].vfi.begin() + 1;
					while((*ivfi1).first == (*ivfi2).first)	//初始，让1和2指向不同的值 
						ivfi2++;
					if(relation == "=")
					{
						while(1)
						{
							if(ivfi2 == (*im_vtableInfo).dlist[attrname].vfi.end())
								break;
							if(fvalue >= (*ivfi2).first && (*ivfi1).first == (*ivfi2).first)
							{
								ivfi2++;
								continue;
							}
							if(fvalue > (*ivfi2).first && (*ivfi1).first != (*ivfi2).first)
							{
								ivfi1 = ivfi2;
								ivfi2++;
								continue;
							}
							if(fvalue < (*ivfi2).first)
							{
								ivfi2--;
								break;
							}
						}
					}
					else if(relation == ">" || relation == ">=")
					{
						while(1)
						{
							while((*ivfi2).first < fvalue)
							{
								ivfi1 = ivfi2;
								while((*ivfi1).first == (*ivfi2).first && ivfi2 != (*im_vtableInfo).dlist[attrname].vfi.end())	//让1和2指向不同的值 
									ivfi2++;
							}
							if(ivfi2 == (*im_vtableInfo).dlist[attrname].vfi.end())	//返回的是最后一组
								break;
	
							if((*ivfi2).first == fvalue)
							{
								ivfi1 = ivfi2;
							}

							ivfi2 = (*im_vtableInfo).dlist[attrname].vfi.end();
							break;
						}
					}
					else if(relation == "<")
					{
						while((*ivfi2).first < fvalue && ivfi2 != (*im_vtableInfo).dlist[attrname].vfi.end())
						{
							cout << (*ivfi2).first << endl;
							ivfi2++;
						}
						ivfi2--;
					}
					else if(relation == "<=")
					{
						while((*ivfi2).first <= fvalue && ivfi2 != (*im_vtableInfo).dlist[attrname].vfi.end())
						{
							cout << (*ivfi2).first << endl;
							ivfi2++;
						}
						ivfi2--;
					}
					p1 = distance((*im_vtableInfo).dlist[attrname].vfi.begin(), ivfi1) + 1;
					if(ivfi2 == (*im_vtableInfo).dlist[attrname].vfi.end())
						ivfi2 = ivfi2 - 1;
					p2 = distance((*im_vtableInfo).dlist[attrname].vfi.begin(), ivfi2) + 1;
					ss.clear();
					ss << p1;
					ss >> corrdset.p1;
					ss.clear();
					ss << p2;
					ss >> corrdset.p2;
					break;
				}
				case 30:
				{
					ivsi1 = (*im_vtableInfo).dlist[attrname].vsi.begin();
					ivsi2 = (*im_vtableInfo).dlist[attrname].vsi.begin() + 1;
					while((*ivsi1).first == (*ivsi2).first)	//初始，让1和2指向不同的值 
						ivsi2++;
					value = "\"" + value +  "\"";
					cout << "value " <<  value << endl;
					if(relation == "=")
					{
						while((*ivsi2).first <= value)
						{
							if(ivsi2 + 1 == (*im_vtableInfo).dlist[attrname].vsi.end())
								break;
							ivsi1 = ivsi2;
							while((*ivsi1).first == (*ivsi2).first && ivsi2 + 1 != (*im_vtableInfo).dlist[attrname].vsi.end())	//让1和2指向不同的值 
								ivsi2++;
						}
						if((*ivsi2).first > value)
							ivsi2--;
					}
					else if(relation == ">" || relation == ">=")
					{
						while(1)
						{	
							while((*ivsi2).first < value)
							{
								ivsi1 = ivsi2;
								while((*ivsi1).first == (*ivsi2).first && ivsi2 != (*im_vtableInfo).dlist[attrname].vsi.end())	//让1和2指向不同的值 
									ivsi2++;
							}
							if(ivsi2 == (*im_vtableInfo).dlist[attrname].vsi.end())	//返回的是最后一组
								break;
	
							if((*ivsi2).first == value)
							{
								ivsi1 = ivsi2;
							}

							ivsi2 = (*im_vtableInfo).dlist[attrname].vsi.end();
							break;
						}
					}
					else if(relation == "<")
					{
						while((*ivsi2).first < value && ivsi2 != (*im_vtableInfo).dlist[attrname].vsi.end())
						{
							ivsi2++;
						}
						ivsi2--;
					}
					else if(relation == "<=")
					{
						while((*ivsi2).first <= value && ivsi2 != (*im_vtableInfo).dlist[attrname].vsi.end())
						{
							ivsi2++;
						}
						ivsi2--;
					}
					p1 = distance((*im_vtableInfo).dlist[attrname].vsi.begin(), ivsi1) + 1;
					p1 = distance((*im_vtableInfo).dlist[attrname].vsi.begin(), ivsi1) + 1;
					if(ivsi2 == (*im_vtableInfo).dlist[attrname].vsi.end())
						ivsi2 = ivsi2 - 1;
					p2 = distance((*im_vtableInfo).dlist[attrname].vsi.begin(), ivsi2) + 1;
					ss.clear();
					ss << p1;
					ss >> corrdset.p1;
					ss.clear();
					ss << p2;
					ss >> corrdset.p2;
					break;
				}
				default:
					break;
			}
//			cout << "p1:" << p1 << endl;
//			cout << "p2:" << p2 << endl;
			tc.vcorrdset.push_back(corrdset);
		}while(stream >> connect);
		vtablecorrdinate.push_back(tc);
	}
}

void* Master::getData(void *s)
{
	latertcptype* self = (latertcptype*)s;
	Master* m = self->master;
	vector<tablecorrdinate> vtablecorrdinate;
	vector<tablecorrdinate>::iterator ivtablecorrdinate;
	vector<string> c;
	vector<corrdinate> vcorrdset;
	vector<corrdinate>::iterator ivcorrdset;
	string testcmd = "f get COUNTRY = IT and APPYEAR <= 1975 and BCKGLAG < 2 and FWDAPLAG > 15";
	//string testcmd = "f get COUNTRY = US and APPYEAR <= 1975";
	cout << testcmd << endl;
	c.push_back(testcmd);
	m->decode(c, vtablecorrdinate);
	cout << "decode finish" << endl;
	
	vector<string> sendstr;
	vector<string>::iterator isendstr;
	for(ivtablecorrdinate = vtablecorrdinate.begin(); ivtablecorrdinate != vtablecorrdinate.end(); ivtablecorrdinate++)
	{
		string s = (*ivtablecorrdinate).tablename;
		for(ivcorrdset = (*ivtablecorrdinate).vcorrdset.begin(); ivcorrdset != (*ivtablecorrdinate).vcorrdset.end(); ivcorrdset++)
		{	
			s += " " + (*ivcorrdset).attrname + 
	" " + (*ivcorrdset).p1 + " " + (*ivcorrdset).p2;
		}
		sendstr.push_back(s);
	}
	
	for(isendstr = sendstr.begin(); isendstr != sendstr.end(); isendstr++)
		cout << *isendstr << endl;

	vector<getRecordTcp> vgetRecordTcp;
	vector<getRecordTcp>::iterator ivgetRecordTcp;
	string tmp;
	vector<TCPInfo>::iterator iTCPInfo;
	for(iTCPInfo = self->vTCPInfo.begin(); iTCPInfo != self->vTCPInfo.end(); iTCPInfo++)
	{
		getRecordTcp grt;
		grt.tcpinfo = *iTCPInfo;
		grt.vscmd = c;
		grt.vscorrd = sendstr;
		grt.m = m;
		vgetRecordTcp.push_back(grt);
	}
	
	for(ivgetRecordTcp = vgetRecordTcp.begin(); ivgetRecordTcp != vgetRecordTcp.end(); ivgetRecordTcp++)
	{
		getRecordTcp grt = *ivgetRecordTcp;
		pthread_t pslavegetrecord;
		pthread_create(&pslavegetrecord, NULL, slaveGetRecord, &grt);
		void* exit = (int*)malloc(4);
		pthread_join(pslavegetrecord, &exit);		
	}
}

void* Master::slaveGetRecord(void *s)
{
	getRecordTcp* self = (getRecordTcp*) s;
	Master* m = self->m;
	ServerSocket socket = *(self->tcpinfo).server;
	cout << socket.m_sock<<endl;
	int time = self->vscmd.size();
	string stime;
	stringstream ss;
	ss.clear();
	ss << time;
	ss >> stime;
	string tmp;
	cout << "stime:" << stime << endl;
	socket << stime;
	socket >> tmp;

	vector<string>::iterator isendstr;
	vector<string>::iterator ic;
	for(ic = self->vscmd.begin(), isendstr = self->vscorrd.begin(); ic != self->vscmd.end(); ic++, isendstr++)
	{
		socket << *ic;
	//	cout << "ic " << *ic << endl;
		socket >> tmp;
		socket << *isendstr;
		socket >> tmp;
	}

	queue<job>* pslaveJob = (queue<job>*)malloc(16);
	vector<slaveJob>::iterator ij;
	for(ij = self->m->m_jobList.begin(); ij != m->m_jobList.end(); ij++)
	{
		if((*ij).slave == self->tcpinfo.ip)		//找到该slave的job队列
		{
			pslaveJob = &(*ij).slaveJobList;
			pslaveJob->front().j_Status = 3;
			break;
		}
	}
}
