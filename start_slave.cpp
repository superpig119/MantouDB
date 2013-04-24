#include <string>
#include <iostream>
#include "slave.h"

using namespace std;

int main()
{
	Slave s;
	s.init();
	s.run();
//	system("./skillport");
	return 0;
}
