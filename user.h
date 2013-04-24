#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdint.h>

using namespace std;

class User
{
public:
	string m_strName;
	string m_strIP;
	int m_iPort;
	int m_iDataPort;
	int m_iKey;

	bool m_bLoggedOut;
	int m_iUseCount;
	int m_llLastRefreshTime;         // timestamp of last activity
public:
	void incUseCount();
	bool decUseCount();
	int getUseCount();
	bool hasLoggedOut();
	void setLogOut(bool logout);
};

class UserManager
{
public:
	UserManager();
	~UserManager();

public:
	int insert(User* u);
	int checkInactiveUsers(std::vector<User*>& iu, int timeout);
	User* acquire(int key);
	void release(User* user);
	int remove(int key);

	map<int, User*> m_mActiveUsers;
};
