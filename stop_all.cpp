#include <iostream>
#include <stdlib.h>

int main()
{
	system("killall -9 start_master");
	system("./mkillport");
	return 0;
}
