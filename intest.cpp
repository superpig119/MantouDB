#include <iostream>
#include <fstream>

using namespace std;

int main()
{
	ifstream ifile("masterlist");
	if(!ifile)
	{
		cout  << "error" << endl;
		return -1;
	}
	char str[40];
	ifile.getline(str, 40);
	cout << str;
	return 0;
}
