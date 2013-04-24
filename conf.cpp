#include "conf.h"

int ConfReader::locate(string& loc)
{
   return 0;   
}

bool ConfReader::getSlaveList(char* Filename, string &base, string &username, vector<slavelist> &slave_list)
{
	ifstream ifile(Filename);
	char baseline[40];
	string slavename;
	char un[30];
	ifile.getline(un, 40);
	ifile.getline(baseline, 40);
	base = baseline;
	username = un;
	while(ifile >> slavename)
	{
		int num;
		ifile >> num;
		slavelist sl;
		sl.ip = slavename;
		sl.num = num;
		cout << slavename << endl;
		slave_list.push_back(sl);
	//	memset(slavename, 0, 30);
	}
	ifile.close();
	return true;
}

bool ConfReader::getMasterAddress(char* Filename, string &addr)
{
	ifstream ifile(Filename);
	char address[20];
	if(!ifile)
	{
		cout << "cannot find master file!" << endl;
		return false;
	}
	ifile.getline(address, 20);
	addr = address;
	//cout << address << endl;
	ifile.close();
	if(addr.length() != 0)
		return true;
	else
	{
		cout  << "empty!" << endl;
		return false;
	}
}

bool ConfReader::ReadConfFile(string FileName, relStyle &rel)
{
    try
    {
        TiXmlDocument *myDocument = new TiXmlDocument(FileName.c_str());
        myDocument->LoadFile();
        TiXmlElement *RootElement = myDocument->RootElement();
        TiXmlElement *DNumxml = RootElement->FirstChildElement();

        int DNum = atoi(DNumxml->GetText());
        rel.DNum = DNum;

        int i;
        TiXmlElement *D = DNumxml->NextSiblingElement();
        for(i = 0; i < DNum; i++)
        {
            TiXmlElement *DClass = D->FirstChildElement();
            rel.DClass.push_back(atoi(DClass->GetText()));
            D = D->NextSiblingElement();
        }
    }
    catch (string& e)
    {
        return false;
    }
	return true;
}

string ConfReader::GetLocalIP()
{
    int MAXINTERFACES=16;
    string ip = "";
    int fd, intrface, retn = 0;
    struct ifreq buf[MAXINTERFACES];
    struct ifconf ifc;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t)buf;
        if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
        {
            intrface = ifc.ifc_len / sizeof(struct ifreq);

            while (intrface-- > 0)
            {
                if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
                {
                    ip=(inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));
                    break;
                }
            }
        }
        //close (fd);
        return ip;
    }
}

