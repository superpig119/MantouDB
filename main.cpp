#include <iostream>
#include <string>
#include "tinyxml.h"
#include "tinystr.h"
#include <vector>

using namespace std;

typedef struct relationStyle
{
    int DNum;               //维数
    vector<int> DClass;     //每一维的种类
}relStyle;

bool ReadConfFile(string FileName, relStyle &rel);


int main()
{
    relStyle rel;
    string filename = "conf.xml";
    ReadConfFile(filename, rel);
    cout << rel.DNum << endl;
    vector<int>::iterator irel;
    for(irel = rel.DClass.begin(); irel != rel.DClass.end(); irel++)
        cout << *irel << endl;
    return 0;
}

bool ReadConfFile(string FileName, relStyle &rel)
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
