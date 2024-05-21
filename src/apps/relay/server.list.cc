#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "data.h"

CGameServerList::CGameServerList()
{
}

CGameServerList::~CGameServerList()
{
	remove_all();
}

bool
CGameServerList::free_item(TSomething aItem)
{
	CGameServer
		*Server = (CGameServer*)aItem;

	if (!Server) return false;

	delete Server;

	return true;
}

int
CGameServerList::compare(TSomething aItem1, TSomething aItem2, 
			int aMode) const
{
	(void)aMode;
	CGameServer
		*Server1 = (CGameServer*)aItem1,
		*Server2 = (CGameServer*)aItem2;

	if (Server1->get_id() > Server2->get_id()) return 1;
	if (Server1->get_id() < Server2->get_id()) return -1;

	return 0;
}

int
CGameServerList::compare_key(TSomething aItem, TConstSomething aKey,
			int aMode) const
{
	(void)aMode;
	CGameServer
		*Server = (CGameServer*)aItem;

	if (Server->get_id() > (int)aKey) return 1;
	if (Server->get_id() < (int)aKey) return -1;

	return 0;
}

bool
CGameServerList::remove(int aGameServerID)
{
	int 
		Pos = find_sorted_key((TSomething)aGameServerID);
	if (Pos < 0) return false;
	
	CList::remove(Pos);
	return true;
}

int
CGameServerList::get_available_server()
{
	int
		ServerID = -1,
		LowPop = 999999999;
	for(int i=0; i<length(); i++)
	{
		CGameServer
			*GameServer = (CGameServer*)get(i);
		if (GameServer->get_population() 
				>= GameServer->get_limit_population()) continue;
		if (GameServer->get_population() < LowPop)
		{
			ServerID = GameServer->get_id();
			LowPop = GameServer->get_population();
		}
	}
	return ServerID;
}
