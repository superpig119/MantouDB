#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <list>

using namespace std;

typedef struct SMALLFILE	//存放所有小文件的内容
{
	string d;			
	//vector<string> *content;
	list<string> content;
}smallfile;

typedef struct FILEINFO	
{
	string d;	//每一个小文件的坐标
	int size;	//每一个小文件的大小
}fileinfo;

typedef struct BIGFILE	//存放大文件的信息，其中小文件的坐标及个数
{
	string d;			//大文件的坐标
	vector<fileinfo> vfileinfo;	//小文件的集合
	list<smallfile> lsmallfile;
}bigfile;

class MTFSop
{
public:
	vector<bigfile> vbigfile;
//	vector<smallfile> vsmallfile;
	void dataInit(void* s);
};
