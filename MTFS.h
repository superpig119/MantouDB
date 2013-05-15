#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <list>
#include <map>

using namespace std;

typedef struct MAPNODE
{
//	string dname;
	int maptype;			//作为选择下面三种映射的依据
	int splitnum;
	vector<pair<int, int> > vii;
	vector<pair<string, int> > vsi;
	vector<pair<float, int> > vfi;
}mapnode;

typedef struct TABLESLAVE
{
	string ip;
	bool alive;
}tableslave;

typedef struct TABLEINFO
{
	vector<tableslave> vtableslave;
	string filename;
	string confname;
	string tableName;
	vector<string> dindex;
	map<string, mapnode> dlist;
	//vector<mapnode> dlist;
	vector<int> dcnum; //每一维每一slave上的划分数
}tableinfo;

typedef struct SLAVEMAPNODE
{
	string dname;
	int maptype;
	int splitnum;
	int p1;
	int p2;
	string relation;
	string value;
	bool hasRelation;
}slavemapnode;

typedef struct SLAVETABLEINFO
{
	vector<tableslave> vtableslave;
	string filename;
	string confname;
	string tableName;
	vector<slavemapnode> dlist;
	vector<int> dcnum; //每一维每一slave上的划分数
}slavetableinfo;

typedef struct SMALLFILE	//存放所有小文件的内容
{
	string d;			
	list<string> content;
}smallfile;

typedef struct BIGFILE	//存放大文件的信息，其中小文件的坐标及个数
{
	string d;			//大文件的坐标
	map<string, int> fileinfo;
	map<string, list<string> > msmallfile;
}bigfile;

class MTFS
{
public:
	vector<tableinfo> m_vtableInfo;
	vector<slavetableinfo> m_vslavetableInfo;
	vector<bigfile> vbigfile;
	void dataSplit(void* s);
	void MasterDataInit(void *s);
	void SlaveDataInit();
	void readData(string tablename, vector<slavemapnode> &dlist);
public:
	int trave_dir(char* path, vector<string> &filename);
};
