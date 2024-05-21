#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "data.h"

CGameServer::CGameServer()
{
	mID = -1;
	mName = "";
	mPort = -1;
	mPopulation = -1;
	mLimitPopulation = -1;
}

CGameServer::~CGameServer()
{
	system_log("remove server info %s", (char*)mName);
}

void
CGameServer::initialize(int aID, const char* aName, int aPort, 
		int aPopulation, int aLimitPopulation)
{
	mID = aID;
	mName = aName;
	mPort = aPort;
	mPopulation = aPopulation;
	mLimitPopulation = aLimitPopulation;
}
