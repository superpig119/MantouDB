#include "user.h"

void User::incUseCount()
{
	++m_iUseCount;
}
	
bool User::decUseCount()
{
	return --m_iUseCount;
}

int User::getUseCount()
{
	return m_iUseCount;
}

bool User::hasLoggedOut()
{
	return m_bLoggedOut;
}

void User::setLogOut(bool logout)
{
	m_bLoggedOut = logout;
}

UserManager::UserManager()
{
}
UserManager::~UserManager()
{
}
	
int UserManager::insert(User* u)
{
	m_mActiveUsers[u->m_iKey] = u;
	return 0;
}

int UserManager::checkInactiveUsers(vector<User*>& iu, int timeout)
{
	for (map<int, User*>::iterator i = m_mActiveUsers.begin(); i != m_mActiveUsers.end(); ++ i)
	{
	 // slave and master are special users and they should never timeout
		if (0 == i->first)
			continue;
		 
		//if (0 >= i->second->getUseCount() && CTimer::getTime() - i->second->m_llLastRefreshTime > timeout * 1000000ULL)
		//	iu.push_back(i->second);
	}
 
	for (vector<User*>::iterator i = iu.begin(); i != iu.end(); ++ i)
		m_mActiveUsers.erase((*i)->m_iKey);

	return iu.size();
}
	
User* UserManager::acquire(int key)
{
	map<int, User*>::iterator i = m_mActiveUsers.find(key);
	if (i == m_mActiveUsers.end())
		return NULL;
 
    i->second->incUseCount();
	    return i->second;
}
	
void UserManager::release(User* user)
{
	if( !user->decUseCount() && user->hasLoggedOut() )
    {
		map<int,User*>::iterator i = m_mActiveUsers.find( user->m_iKey );
	    if( i != m_mActiveUsers.end() )
			m_mActiveUsers.erase(i);
			delete user;
	}
}
	
int UserManager::remove(int key)
{
	map<int, User*>::iterator i = m_mActiveUsers.find(key);
    if (i == m_mActiveUsers.end())
		return -1;

	delete i->second;
	m_mActiveUsers.erase(i);
		return 0;
}
