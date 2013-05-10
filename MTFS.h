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
	vector<bigfile> vbigfile;
	void dataSplit(void* s);
	void MasterDataInit(void *s);
public:
	int trave_dir(char* path, vector<string> &filename);
};
