#include "master.h"
#include <algorithm>

bool operator==(const smallfile &s1, const smallfile &s2)
{
	return s1.d == s2.d;
}

bool operator!=(const smallfile &s1, const smallfile &s2)
{
	return !(s1.d == s2.d);
}

bool operator>(const smallfile &s1, const smallfile &s2)
{
	return s1.d > s2.d;
}

bool operator<(const smallfile &s1, const smallfile &s2)
{
	return s1.d < s2.d;
}

bool operator>=(const smallfile &s1, const smallfile &s2)
{
	return s1.d >= s2.d;
}

bool operator<=(const smallfile &s1, const smallfile &s2)
{
	return s1.d <= s2.d;
}

void MTFS::dataSplit(void *s)
{
	Master* self = (Master*)s;
	vector<tableinfo>::iterator im_vtableInfo, im_vtableInfo2;
	im_vtableInfo = (self->mt.m_vtableInfo.end()) - 1;
	vector<mapnode>::iterator idlist;
	cout << "in datainit!!!" << endl;
	stringstream ss;
	string strx;
	vector<bigfile>::iterator ivbigfile;
	int c= 0;
	while(1)
	{
		if((*im_vtableInfo).dlist.size() == (*im_vtableInfo).dcnum.size())
		{
			ifstream  infile, inconf;
			string filepath = "../data/";
			infile.open((filepath + (*im_vtableInfo).filename).c_str());
			if(!infile)
			{
				cout << "cannot open file " << (*im_vtableInfo).filename << "!" << endl;
				continue;
			}
			inconf.open((filepath + (*im_vtableInfo).confname).c_str());
			if(!inconf)
			{
				cout << "cannot open fileconf " << (*im_vtableInfo).confname << "!" << endl;
				continue;
			}
			int linenum, dnum, addrnum, i;
			string sline, sdnum, saddrnum;
			
			getline(inconf, sline);
			getline(inconf, sdnum);
			getline(inconf, saddrnum);
			ss.clear();
			ss << sline;
			ss >> linenum;
			ss.clear();
			ss << sdnum;
			ss >> dnum;
			ss.clear();
			ss << saddrnum;
			ss >> addrnum;

			string line;
			line = "";
			getline(inconf,line);
			
			string linetmp;
			line = "";
			getline(infile,line);
			vector<int> d;	//数据的保存坐标信息
			vector<int>::iterator ivd;
			int dcount;
			string key;
			vector<string>::iterator idindex;
			for(i = 0; i < linenum; i++)
			{
				line = "";
				getline(infile,line);
				int comapos1 = 0, comapos2 = 0;
				int x = 0;
				dcount = 0;
				for(idindex = (*im_vtableInfo).dindex.begin(); idindex != (*im_vtableInfo).dindex.end(); idindex++) //对每行中的每个属性
				{	
					comapos2 = line.find(',',comapos1);
					linetmp = line.substr(comapos1, comapos2 - comapos1);			
					int type = (*im_vtableInfo).dlist[*idindex].maptype;
					comapos1 = comapos2 + 1;
					switch(type)
					{
						case 10:
						case 11:
						case 12:
						{
							int n, min, xi;
							vector<pair<int, int> >::iterator ii1, ii2, ii3;
							ss.clear();
							ss << linetmp;
							ss >> n;

							if(comapos1 == comapos2)
							{
								n = 0;
							}
							ii1 = (*im_vtableInfo).dlist[*idindex].vii.begin(); 
							ii2 = (*im_vtableInfo).dlist[*idindex].vii.begin() + 1; 
							while(1)
							{
								while(n >= (*ii1).first && (*ii2).first == (*ii1).first && (ii2 + 1) != (*im_vtableInfo).dlist[*idindex].vii.end())
								{
									ii2 += 1;
								}
								if(n >= (*ii2).first && (ii2 + 1) != (*im_vtableInfo).dlist[*idindex].vii.end())
								{
									ii1 = ii2;
									ii2 += 1;
								}
								else 
									break;
							}	
							min = (*ii1).second;
							ii3 = ii1;
							for(; ii1 != ii2 + 1; ii1++)
							{
								if(min > (*ii1).second)
								{
									min = (*ii1).second;
									ii3 = ii1;
								}
							}
							for(xi = 0, ii1 = (*im_vtableInfo).dlist[*idindex].vii.begin(); ii1 != ii3;xi++, ii1++)
							{
							}
							xi++;
							(*ii3).second += 1;
				//			cout << "count:" << (*ii3).second << endl;
							x += xi;
							d.push_back(xi);
							break;
						}
						case 20:
						{
							float n;
							int xi, min;
							vector<pair<float, int> >::iterator fi1, fi2, fi3;
							ss.clear();
							ss << linetmp;
							ss >> n;
							if(comapos1 == comapos2)
							{
								n = 0;
							}
							fi1 = (*im_vtableInfo).dlist[*idindex].vfi.begin(); 
							fi2 = (*im_vtableInfo).dlist[*idindex].vfi.begin() + 1; 
							while(1)
							{
								while(n >= (*fi1).first && (*fi2).first == (*fi1).first && (fi2 + 1) != (*im_vtableInfo).dlist[*idindex].vfi.end())
								{
									fi2 += 1;
								}
								if(n >= (*fi2).first && (fi2 + 1) != (*im_vtableInfo).dlist[*idindex].vfi.end())
								{
									fi1 = fi2;
									fi2 += 1;
								}
								else 
									break;
							}	
							min = (*fi1).second;
							fi3 = fi1;
							for(; fi1 != fi2 + 1; fi1++)
							{
								if(min > (*fi1).second)
								{
									min = (*fi1).second;
									fi3 = fi1;
								}
							}
							for(xi = 0, fi1 = (*im_vtableInfo).dlist[*idindex].vfi.begin(); fi1 != fi3;xi++, fi1++)
							{
							}
							xi++;
							(*fi3).second += 1;
							x += xi;
							d.push_back(xi);
							break;
						}
						case 30:
						{
							string n = linetmp;
							int xi, min;
							vector<pair<string, int> >::iterator si1, si2, si3;
							if(comapos1 == comapos2)
							{
								n = "";
							}
				//			cout << "n:" << n << endl;
							si1 = (*im_vtableInfo).dlist[*idindex].vsi.begin(); 
							si2 = (*im_vtableInfo).dlist[*idindex].vsi.begin() + 1; 
							while(1)
							{
								if(n == "")
								{
									while((*si2).first == "")
										si2++;
									break;
								}

								while(n >= (*si1).first && (*si2).first == (*si1).first && (si2 + 1) != (*im_vtableInfo).dlist[*idindex].vsi.end())
								{
									si2 += 1;
								}
								if(n >= (*si2).first && (si2 + 1) != (*im_vtableInfo).dlist[*idindex].vsi.end())
								{
									si1 = si2;
									si2 += 1;
								}
								else 
									break;
							}	
							min = (*si1).second;
							si3 = si1;
							for(; si1 != si2 + 1; si1++)
							{
								if(min > (*si1).second)
								{
									min = (*si1).second;
									si3 = si1;
								}
							}
							for(xi = 0, si1 = (*im_vtableInfo).dlist[*idindex].vsi.begin(); si1 != si3;xi++, si1++)
							{
							}
							xi++;
							(*si3).second += 1;
							x += xi;
							d.push_back(xi);
							break;
						}
						default:
							break;
					}
				}
//				cout << x << endl;
				
				string strpartnum;
				int cmd, partnum;
				cmd = x % self->activeSlaves.size() + 1;
				partnum = x / self->activeSlaves.size();
				ss.clear();
				ss << cmd;
				ss >> strx;
				ss.clear();
				ss << x;
				ss >> strpartnum;
				string fname;
				string fd,tmp;	//fd为这一行数据的坐标，也是小文件坐标
				string bigfileD;//大文件坐标，即小文件坐标的前几位
				bigfileD = strx + ".";
				fd = strx + ".";
				int count = 0;
				for(ivd = d.begin(); ivd != d.end(); ivd++)
				{
					ss.clear();
					ss << *ivd;
					ss >> tmp;
					if(count < addrnum)
					{
						bigfileD += tmp;	//计算bigfile的坐标
						count++;
						if(count != addrnum)
							bigfileD += ".";
					}
					fd += tmp;
					if(ivd + 1 == d.end())
						break;
					fd += ".";
				}

				for(ivbigfile = vbigfile.begin(); ivbigfile != vbigfile.end(); ivbigfile++)		//修改vbigfile
				{
					if((*ivbigfile).d == bigfileD)	//找到相同的bigfile
					{
						(*ivbigfile).msmallfile[fd].push_back(line);    
						(*ivbigfile).fileinfo[fd]++;    
						break;
					}
				}
				if(ivbigfile == vbigfile.end())	//将新的bigfile加入到bigfile
				{
					bigfile bf;
					bf.d = bigfileD;
					bf.fileinfo[fd] = 1;
					list<string> c;
					c.push_back(line);
					bf.msmallfile[fd] = c;
					vbigfile.push_back(bf);
					cout << "new big file:" << bigfileD << endl;
				}
				c++;
				if(c % 5000 == 0)
					cout << c << ":" << bigfileD << "    " << fd <<  endl;
				d.clear();	//清空坐标信息，迎接新的一行
			}
			string fname;
			string fpath = "../data/tmp/";
			im_vtableInfo2 = (self->mt.m_vtableInfo.end()) - 1;
			map<string, int>::iterator ifileinfo;
			map<string, list<string> >::iterator imsmallfile;
			list<string>::iterator icontent;
			for(ivbigfile = vbigfile.begin(); ivbigfile != vbigfile.end(); ivbigfile++)
			{
				fname = fpath + (*im_vtableInfo2).tableName + "." + (*ivbigfile).d;
				fstream of;
				of.open(fname.c_str(), ofstream::out | ofstream::app);
				of << (*ivbigfile).fileinfo.size() << endl;
	/*			for(ifileinfo = (*ivbigfile).fileinfo.begin(); ifileinfo != (*ivbigfile).fileinfo.end(); ifileinfo++)
				{
					of << (*ifileinfo).first << " " << (*ifileinfo).second << endl;
				}*/
				for(imsmallfile = (*ivbigfile).msmallfile.begin(), ifileinfo = (*ivbigfile).fileinfo.begin(); imsmallfile != (*ivbigfile).msmallfile.end(); imsmallfile++, ifileinfo++)
				{
					of << (*imsmallfile).first << " " << (*ifileinfo).second << endl;
					for(icontent = (*imsmallfile).second.begin(); icontent != (*imsmallfile).second.end(); icontent++)
					{
						of << *icontent << endl;
					}
				}
			//	cout << fname << endl;
				of.close();
			}
			vbigfile.clear();
			cout << "FINISH!" << endl;
	/*		for(idlist = (*im_vtableInfo).dlist.begin(); idlist != (*im_vtableInfo).dlist.end(); idlist++)
			{
				cout << (*idlist).dname << endl;
			}
	*/		
			vector<tableinfo>::iterator im_vtableInfo;	//存储该表的元信息
			map<string, mapnode>::iterator idlist;
		    stringstream ss;
	        im_vtableInfo = (self->mt.m_vtableInfo.end()) - 1;
	        cout << "tablename:" << (*im_vtableInfo).tableName << endl;
	        cout << "m_vtableInfosize:" << self->mt.m_vtableInfo.size()<< endl;
	        ofstream ofmeta;
			filepath = "../data/";
	        filepath = filepath + "meta/" + (*im_vtableInfo).tableName + ".meta";
	        cout << filepath << endl;
	        ofmeta.open(filepath.c_str());
	        ofmeta << (*im_vtableInfo).confname << endl;	//写入配置文件的名字
	        ofmeta << (*im_vtableInfo).dlist.size() << endl;//写入维数
			ofmeta << self->activeSlaves.size() << endl;
			vector<struct SlaveNode>::iterator iactiveSlaves;
			int splitnum;
			string ssplitnum;
			for(iactiveSlaves = self->activeSlaves.begin(); iactiveSlaves != self->activeSlaves.end(); iactiveSlaves++)
			{
				tableslave ts;
				ts.ip = (*iactiveSlaves).ip;
				ofmeta << (*iactiveSlaves).ip << endl;
				ts.alive = true;
				(*im_vtableInfo).vtableslave.push_back(ts);
			}
			
			string stype;
			
	        for(idindex = (*im_vtableInfo).dindex.begin(); idindex != (*im_vtableInfo).dindex.end(); idindex++)
	        {
				splitnum = 0;
		        line = *idindex;
				ss.clear();
				ss << (*im_vtableInfo).dlist[*idindex].maptype;
				ss >> stype;
				line = line + " " + stype;
				string contentline;
		        switch((*im_vtableInfo).dlist[*idindex].maptype)
		        {
			        case 10:
			        case 11:
			        case 12:
			        {
				        vector<pair<int, int> >::iterator ivii;
				        int num1,num2;
				        string snum1, snum2;
				        for(ivii = (*im_vtableInfo).dlist[*idindex].vii.begin(); ivii != (*im_vtableInfo).dlist[*idindex].vii.end(); ivii++)
				        {
							splitnum++;
					        num1 = (*ivii).first;
							ss.clear();
					        ss << num1;
					        ss >> snum1;
					        ss.clear();
					        num2 = (*ivii).second;
					        ss << num2;
					        ss >> snum2;
					        contentline = contentline + " " + snum1 + " " + snum2;
				        }
					    ss.clear();
						ss << splitnum;
						ss >> ssplitnum;
						line = line + " " + ssplitnum + " " + contentline;
				        ofmeta << line << endl;
				        break;
			        }
			        case 20:
			        {
				        vector<pair<float, int> >::iterator ivfi;
				        float fnum;
				        int inum;
				        string sfnum, sinum;
				        for(ivfi = (*im_vtableInfo).dlist[*idindex].vfi.begin(); ivfi != (*im_vtableInfo).dlist[*idindex].vfi.end(); ivfi++)
				        {
							splitnum++;
					        fnum = (*ivfi).first;
							ss.clear();
					        ss << fnum;
					        ss >> sfnum;
					        ss.clear();
					        inum = (*ivfi).second;
					        ss << inum;
					        ss >> sinum;
					        contentline = contentline + " " + sfnum + " " + sinum;
				        }
					    ss.clear();
						ss << splitnum;
						ss >> ssplitnum;
						line = line + " " + ssplitnum + " " + contentline;
				        ofmeta << line << endl;
				        break;
			        }
			        case 30:
			        {
				        vector<pair<string, int> >::iterator ivsi;
				        int inum;
				        string sinum;
				        for(ivsi = (*im_vtableInfo).dlist[*idindex].vsi.begin(); ivsi != (*im_vtableInfo).dlist[*idindex].vsi.end(); ivsi++)
				        {
							splitnum++;
					        inum = (*ivsi).second;
							ss.clear();
					        ss << inum;
					        ss >> sinum;
					        contentline = contentline + " " + (*ivsi).first + " " + sinum;
				        }
					    ss.clear();
						ss << splitnum;
						ss >> ssplitnum;
						line = line + " " + ssplitnum + " " + contentline;
				        ofmeta << line << endl;
				        break;
			        }
			        default:
				        break;
			    }
	        }
	        ofmeta.close();
			break;
		}
	}
}

void MTFS::MasterDataInit(void *s)
{
	Master* self = (Master*)s;
	vector<string> filename;
	trave_dir("../data/meta", filename);
	
	vector<string>::iterator ifilename;
	vector<tableinfo>::iterator im_vtableInfo, im_vtableInfo2;
	map<string, mapnode>::iterator idlist;
	vector<pair<string, int> >::iterator ivsi;
	vector<pair<int, int> >::iterator ivii;
	vector<pair<float, int> >::iterator ivfi;
	stringstream ss;
	int i, slavenum;
	string line, word, stype;
	for(ifilename = filename.begin(); ifilename != filename.end(); ifilename++)
	{
		tableinfo table;
		ifstream ifile;
		string path = "../data/meta/" + *ifilename;
		string confname, snum;
		int dnum, lnum;
		ifile.open(path.c_str());
		table.tableName = (*ifilename).substr(0, (*ifilename).find("."));
		ifile >> confname;
		ifile >> snum;
		ss.clear();
		ss << snum;
		ss >> dnum;
		table.confname = confname;

		ifile >> slavenum;
		for(i = 0; i < slavenum; i++)
		{
			tableslave ts;
			ifile >> ts.ip;
			ts.alive = false;
			table.vtableslave.push_back(ts);
		}

		for(i = 0; i < dnum; i++)
		{
		    string tablename;
			mapnode mn;
			getline(ifile, line);
			istringstream stream(line);
			stream >> tablename;
			stream >> stype;
			stream >> mn.splitnum;
			ss.clear();
			ss << stype;
			ss >> mn.maptype;
			switch(mn.maptype)
			{
				case 10:	
				case 11:	
				case 12:
				{
					while(stream >> word)
					{
						int n;
						stream >> snum;
						ss.clear();
						ss << word;
						ss >> n;
						ss.clear();
						ss << snum;
						ss >> lnum;
						mn.vii.push_back(make_pair(n, lnum));
					}
					break;
				}
				case 20:
				{
					while(stream >> word)
					{
						float f;
						stream >> snum;
						ss.clear();
						ss << word;
						ss >> f;
						ss.clear();
						ss << snum;
						ss >> lnum;
						mn.vfi.push_back(make_pair(f, lnum));
					}
					break;
				}
				case 30:
				{
					while(stream >> word)
					{
						stream >> snum;
						ss.clear();
						ss << snum;
						ss >> lnum;
						mn.vsi.push_back(make_pair(word, lnum));
					}
					break;
				}
				default:
					break;
			}
			table.dlist.insert(make_pair(tablename, mn));
		}
		self->mt.m_vtableInfo.push_back(table);
	}
}

void MTFS::SlaveDataInit()
{
	vector<string> filename;
	trave_dir("./mantouDB/data/meta", filename);
	
	vector<string>::iterator ifilename;
	vector<slavetableinfo>::iterator im_vtableInfo;
	vector<slavemapnode>::iterator idlist;

	stringstream ss;
	int i, slavenum;
	string line, word, stype;
	for(ifilename = filename.begin(); ifilename != filename.end(); ifilename++)
	{
		slavetableinfo table;
		ifstream ifile;
		string path = "./mantouDB/data/meta/" + *ifilename;
		string confname, snum;
		int dnum, lnum;
		ifile.open(path.c_str());
		table.tableName = (*ifilename).substr(0, (*ifilename).find("."));
		ifile >> confname;
		ifile >> dnum;
		table.confname = confname;
        
		ifile >> slavenum;
		for(i = 0; i < slavenum; i++)
		{
			tableslave ts;
			ifile >> ts.ip;
			cout << ts.ip << endl;
			ts.alive = false;
			table.vtableslave.push_back(ts);
		}

		getline(ifile, line);
		for(i = 0; i < dnum; i++)
		{
			int p2;
			slavemapnode mn;
			mn.p1 = 1;
			getline(ifile, line);
			istringstream stream(line);
			stream >> mn.dname;
			stream >> stype;
			stream >> p2;
			ss.clear();
			ss << p2;
			ss >> mn.p2;
			ss.clear();
			ss << stype;
			ss >> mn.maptype;
			mn.hasRelation = false;
			table.dlist.push_back(mn);
		}
		m_vslavetableInfo.push_back(table);
	}
}


int MTFS::trave_dir(char* path, vector<string> &filename)
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


void MTFS::readData(string tablename, vector<slavemapnode> &dlist)
{
	vector<string> filename;
	vector<string>::iterator ifilename;
	vector<slavemapnode>::iterator idlist;
	string path = "./mantouDB/data/";
	path += tablename;
	char cpath[40];
	strcpy(cpath, path.c_str());
	trave_dir(cpath, filename);
	
	stringstream ss;
	bool flag;
	int count, num, n1 = 0, n2 = 0, ntmp;
	string corrd, c, dtmp;
	for(ifilename = filename.begin(); ifilename != filename.end(); ifilename++)
	{
		bigfile bf;
		bf.d = (*ifilename).substr(tablename.size() + 1, (*ifilename).size() - tablename.size());
	//	cout << bf.d << endl;
		ifstream infile;
		infile.open((path + "/" + *ifilename).c_str());
//		cout << *ifilename << endl;
		infile >> count;
//		cout << "count" << count << endl;
		while(count)
		{
			infile >> corrd >> num;
			n1 = n2 = 0;
			flag = true;
		//	cout << corrd << endl;
			n1 = corrd.find(".", n2 + 1);
			for(idlist = dlist.begin(); idlist != dlist.end(); idlist++)
			{
				n2 = corrd.find(".", n1 + 1);
				dtmp = corrd.substr(n1 + 1, n2 - n1 -1);
				n1 = n2;
			//	cout  << dtmp << endl;
				ss.clear();
				ss << dtmp;
				ss >> ntmp;
		//		cout << dtmp << " " << (*idlist).p1 << " " << (*idlist).p2 << endl;
				if(ntmp < (*idlist).p1 || ntmp > (*idlist).p2)
				{
					flag = false;
				//	cout << dtmp << " " << (*idlist).p1 << " " << (*idlist).p2 << endl;
				//	cout << "out!" << endl;
					count--;
					while(num)
					{
						infile >> c;
						num--;
					}
					break;
				}
	//			cout  << dtmp << endl;
			}
	//		cout << endl;
			
			if(!flag)
				continue;
	//		cout << corrd << endl;
			list<string> content;
			while(num)
			{
				flag = true;
				infile >> c;
				n1 = n2 = 0;
				for(idlist = dlist.begin(); idlist != dlist.end(); idlist++)
				{
					//cout << c << endl;
					n1 = c.find(",", n2);
					if((*idlist).hasRelation)
					{
						cout << (*idlist).dname << " " << (*idlist).relation << " " << (*idlist).value << endl;

						if((*idlist).relation == "=")
						{	
							dtmp = c.substr(n2, n1 - n2);
							cout << dtmp << endl;
							if(dtmp != (*idlist).value)
							{
								flag = false;
								num--;
								break;
							}
						}
						else if((*idlist).relation == ">")
						{	
							dtmp = c.substr(n2, n1 - n2);
							if(dtmp <= (*idlist).value)
							{
								flag = false;
								num--;
								break;
							}
						}
						else if((*idlist).relation == ">=")
						{	
							dtmp = c.substr(n2, n1 - n2);
							if(dtmp < (*idlist).value)
							{
								flag = false;
								num--;
								break;
							}
						}
						else if((*idlist).relation == "<")
						{	
							dtmp = c.substr(n2, n1 - n2);
							if(dtmp >= (*idlist).value)
							{
								flag = false;
								num--;
								break;
							}
						}
						else if((*idlist).relation == "<=")
						{	
							dtmp = c.substr(n2, n1 - n2);
							if(dtmp > (*idlist).value)
							{
								flag = false;
								num--;
								break;
							}
						}
					}
					n2 = n1 + 1;
				}
				if(num == 0)
					break;
				if(flag)
				{
					cout << c << endl;
					content.push_back(c);
				}
				num--;
			}
			bf.msmallfile[corrd] = content;
			count--;
		}
		vbigfile.push_back(bf);
	}
	cout << "read finsih" <<endl;
	cout << vbigfile.size() << endl;
	
}
	















