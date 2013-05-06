#include "MTFS.h"
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

void MTFSop::dataInit(void *s)
{
	Master* self = (Master*)s;
	vector<tableinfo>::iterator im_vtableInfo, im_vtableInfo2;
	im_vtableInfo = (self->m_vtableInfo.end()) - 1;
	vector<mapnode>::iterator idlist;
	cout << "in datainit!!!" << endl;
	stringstream ss;
	string strx;
	vector<fileinfo>::iterator ivfileinfo;
	vector<bigfile>::iterator ivbigfile;
	list<smallfile>::iterator ilsmallfile;
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
			for(i = 0; i < linenum; i++)
			{
				line = "";
				getline(infile,line);
				int comapos1 = 0, comapos2 = 0;
				int x = 0;
				dcount = 0;
				for(idlist = (*im_vtableInfo).dlist.begin(); idlist != (*im_vtableInfo).dlist.end(); idlist++) //对每行中的每个属性
				{	
					
					comapos2 = line.find(',',comapos1);
					linetmp = line.substr(comapos1, comapos2 - comapos1);			
					int type = (*idlist).maptype;
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
							ii1 = (*idlist).vii.begin(); 
							ii2 = (*idlist).vii.begin() + 1;
							while(1)
							{
								while(n > (*ii1).first && (*ii2).first == (*ii1).first && (ii2 + 1) != (*idlist).vii.end())
								{
									ii2 += 1;
								}
								if(n > (*ii2).first && (ii2 + 1) != (*idlist).vii.end())
								{
									ii1 = ii2;
									ii2 += 1;
								}
								else 
									break;
							}	
							min = (*ii1).second;
							ii3 = ii1;
							for(; ii1 != ii2; ii1++)
							{
								if(min > (*ii1).second)
								{
									min = (*ii1).second;
									ii3 = ii1;
								}
							}
							for(xi = 0, ii1 = (*idlist).vii.begin(); ii1 != ii3;xi++, ii1++)
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
							fi1 = (*idlist).vfi.begin(); 
							fi2 = (*idlist).vfi.begin() + 1;
							while(1)
							{
								while(n > (*fi1).first && (*fi2).first == (*fi1).first && (fi2 + 1) != (*idlist).vfi.end())
								{
									fi2 += 1;
								}
								if(n > (*fi2).first && (fi2 + 1) != (*idlist).vfi.end())
								{
									fi1 = fi2;
									fi2 += 1;
								}
								else 
									break;
							}	
							min = (*fi1).second;
							fi3 = fi1;
							for(; fi1 != fi2; fi1++)
							{
								if(min > (*fi1).second)
								{
									min = (*fi1).second;
									fi3 = fi1;
								}
							}
							for(xi = 0, fi1 = (*idlist).vfi.begin(); fi1 != fi3;xi++, fi1++)
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
							si1 = (*idlist).vsi.begin(); 
							si2 = (*idlist).vsi.begin() + 1;
							while(1)
							{
								if(n == "")
								{
									while((*si2).first == "")
										si2++;
									break;
								}

								while(n > (*si1).first && (*si2).first == (*si1).first && (si2 + 1) != (*idlist).vsi.end())
								{
									si2 += 1;
								}
								if(n >= (*si2).first && (si2 + 1) != (*idlist).vsi.end())
								{
									si1 = si2;
									si2 += 1;
								}
								else 
									break;
							}	
							min = (*si1).second;
							si3 = si1;
							for(; si1 != si2; si1++)
							{
								if(min > (*si1).second)
								{
									min = (*si1).second;
									si3 = si1;
								}
							}
							for(xi = 0, si1 = (*idlist).vsi.begin(); si1 != si3;xi++, si1++)
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
		//		cout << "cmd:" << cmd << endl;
				ss.clear();
				ss << cmd;
				ss >> strx;
				ss.clear();
				ss << x;
				ss >> strpartnum;
				string fname;
				string fd,tmp;	//fd为这一行数据的坐标，也是小文件坐标
				string bigfileD;//大文件坐标，即小文件坐标的前几位
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
			//	cout << "bigD:" <<  bigfileD << endl;
		//		cout << "smalld" << fd << endl;
				for(ivbigfile = vbigfile.begin(); ivbigfile != vbigfile.end(); ivbigfile++)		//修改vbigfile
				{
					if((*ivbigfile).d == bigfileD)	//找到相同的bigfile
					{
					/*	for(ivfileinfo = (*ivbigfile).vfileinfo.begin(); ivfileinfo != (*ivbigfile).vfileinfo.end(); ivfileinfo++)	//大文件中的小文件信息
						{
							if((*ivfileinfo).d == fd)	//有相同小文件	
							{
								(*ivfileinfo).size++;
								break;
							}
						}
						if(ivfileinfo == (*ivbigfile).vfileinfo.end())	//无相同小文件
						{
							fileinfo fi;
							fi.d = fd;
							fi.size = 1;
							(*ivbigfile).vfileinfo.push_back(fi);
						}
					*/	
						smallfile sma;
						sma.d = fd;
						cout << "??" << endl;
						cout << "begin:" << (*(*ivbigfile).lsmallfile.begin()).d << endl;
						ilsmallfile = lower_bound((*ivbigfile).lsmallfile.begin(), (*ivbigfile).lsmallfile.end(), &sma);
						cout << (*ilsmallfile).d << endl;
						if((*ilsmallfile).d == fd)
						{
							cout << "append" << endl;
							(*ilsmallfile).content.push_back(line);
						}
						else if(ilsmallfile != (*ivbigfile).lsmallfile.end()) 
						{
						//	ilsmallfile = lower_bound((*ivbigfile).lsmallfile.begin(), (*ivbigfile).lsmallfile.end(), sma);
							smallfile sm;
							sm.d = fd;
							sm.content.push_back(line);
							cout << "insert" << endl;
						//	(*ivbigfile).lsmallfile.insert(ilsmallfile, sm);
							(*ivbigfile).lsmallfile.insert(ilsmallfile, sm);
							
						}
						else if(ilsmallfile == (*ivbigfile).lsmallfile.end()) 
						{
							smallfile sm;
							sm.d = fd;
							sm.content.push_back(line);
							cout << "insert" << endl;
						//	(*ivbigfile).lsmallfile.insert(ilsmallfile, sm);
							(*ivbigfile).lsmallfile.push_back(sm);
						}

			/*			for(ilsmallfile = (*ivbigfile).lsmallfile.begin(); ilsmallfile != (*ivbigfile).lsmallfile.end(); ilsmallfile++)	//大文件中的小文件列表
						{
							if((*ilsmallfile).d == fd)
							{
					//			cout << "pushback" << endl;
						//		(*((*ivsmallfile).content)).push_back(line);
								(*ilsmallfile).content.push_front(line);
								break;
							}
							if((*ilsmallfile).d > fd)	//按字典序插入
							{
					//			cout << "insert" << endl;
								smallfile sm;
								sm.d = fd;
						//		vector<string> *content = new vector<string>;
						//		sm.content = content;
						//		(*(sm.content)).push_back(line);
							//	(*ivbigfile).vsmallfile.insert(ivsmallfile, sm);
								sm.content.push_back(line);
								(*ivbigfile).lsmallfile.insert(ilsmallfile, sm);
								break;
							}
						}
						if(ilsmallfile == (*ivbigfile).lsmallfile.end())	//在末尾插入
						{
		//					cout << "push_back small" << endl;
							smallfile sm;
							sm.d = fd;
					//		vector<string> *content = new vector<string>;
					//		sm.content = content;
					//		(*(sm.content)).push_back(line);
							sm.content.push_back(line);
							(*ivbigfile).lsmallfile.push_back(sm);
						}*/
						break;
					}
				}
				if(ivbigfile == vbigfile.end())	//将新的bigfile加入到bigfile
				{
					bigfile bf;
					bf.d = bigfileD;
					fileinfo fi;
					fi.d = fd;
					fi.size = 1;
					smallfile sm;
					sm.d = fd;
					sm.content.push_back(line);
				//	vector<string> *content = new vector<string>;
				//	sm.content = content;
				//	(*(sm.content)).push_back(line);
		//			cout << "smcontent:" << (*(sm.content))[0] << endl;
	//				sm.content.push_back(line);
					bf.lsmallfile.push_back(sm);
					bf.vfileinfo.push_back(fi);
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
			im_vtableInfo2 = (self->m_vtableInfo.end()) - 1;
			for(ivbigfile = vbigfile.begin(); ivbigfile != vbigfile.end(); ivbigfile++)
			{
				fname = fpath + (*im_vtableInfo2).tableName + "." + strx + "." + (*ivbigfile).d;
				cout << fname << endl;
			}
		/*	vector<tableinfo>::iterator  im_vtableInfo2;
			im_vtableInfo2 = (self->m_vtableInfo.end()) - 1;
			vector<mapnode>::iterator idlist;
			fname = fpath + (*im_vtableInfo2).tableName + "." + strx + "." + fd;
			ofstream of;
			of.open(fname.c_str(), ofstream::out | ofstream::app);
			of << line << endl; 
			of.close();
		*/	
			
			cout << "FINISH!" << endl;
			for(idlist = (*im_vtableInfo).dlist.begin(); idlist != (*im_vtableInfo).dlist.end(); idlist++)
			{
				
				cout << (*idlist).dname << endl;
			}
			break;
		}
	}
}
