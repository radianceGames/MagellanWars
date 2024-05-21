#include "common.h"
#include "util.h"
#include "universe.h"
#include "planet.h"
#include "player.h"
#include "council.h"
#include <libintl.h>
#include <cstdlib>
#include "archspace.h"

TZone gClusterZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CCluster),
	0,
	0,
	NULL,
	"Zone CCluster"
};

TZone gSortedPlanetListZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CSortedPlanetList),
	0,
	0,
	NULL,
	"Zone CSortPlanetList"
};

TZone gPlayerListZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CPlayerList),
	0,
	0,
	NULL,
	"Zone CPlayerList"
};

TZone gCouncilListZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CCouncilList),
	0,
	0,
	NULL,
	"Zone CCouncilList"
};

CCluster::CCluster()
{
	mID = 0;
	mName = "No name";
	mAcceptNewPlayer = true;
	mWeight = 0;

	mPlanetList = new CSortedPlanetList;
	mPlayerList = new CPlayerList;
	mCouncilList = new CCouncilList;
	mEventList = new CEventInstanceList;

	refresh_nick();
}

CCluster::CCluster(int aClusterID)
{
	mID = aClusterID;
	mName = GETTEXT("Trabus");
	refresh_nick();
	mAcceptNewPlayer = false;
	mWeight = 9999;

	mPlayerList = new CPlayerList();
	mCouncilList = new CCouncilList();
	mPlanetList = new CSortedPlanetList();
	mEventList = NULL;
}

CCluster::CCluster(MYSQL_ROW aRow)
{
	enum 
	{	
		FIELD_ID = 0,
		FIELD_NAME
	};

	mID = atoi(aRow[FIELD_ID]);
	mName = (char*)aRow[FIELD_NAME];
	mAcceptNewPlayer = true;
	mWeight = 0;

	mPlanetList = new CSortedPlanetList;
	mPlayerList = new CPlayerList;
	mCouncilList = new CCouncilList;
	mEventList = new CEventInstanceList;

	refresh_nick();
}

CCluster::~CCluster()
{
	if (mPlanetList)
		delete mPlanetList;
	if (mPlayerList)
		delete mPlayerList;		
	if (mCouncilList)
		delete mCouncilList;
	if (mEventList)
		delete mEventList;
}

void
CCluster::refresh_nick()
{
	mNick.clear();
	mNick.format("%s (#%s)", (char *)mName, dec2unit(mID));
}

CString&
CCluster::query()
{
	static CString Query;

	Query.clear();

	switch(type())
	{
		case QUERY_INSERT:
			Query.format("INSERT INTO cluster (id, name) VALUES ( %d, '%s' )",
							mID, (char*)mName); 
			break;
		case QUERY_UPDATE:
			break;
		case QUERY_DELETE:
			Query.format("DELETE cluster WHERE id = %d", mID);
			break;
	}

	mStoreFlag.clear();

	return Query;
}

int
CCluster::get_planet_count()
{
	return mPlanetList->length();
}

int
CCluster::get_player_count()
{
	return mPlayerList->length();
}

int
CCluster::get_council_count()
{
	return mCouncilList->length();
}	

void 
CCluster::add_planet(CPlanet* aPlanet)
{
	assert(aPlanet);

	int 
		Index = mPlanetList->find_sorted_key(
						(TConstSomething)aPlanet->get_id());

	if (Index >= 0) return;

	mPlanetList->insert_sorted(aPlanet);
}

void 
CCluster::add_player(CPlayer* aPlayer)
{
	assert(aPlayer);

	int 
		Index = mPlayerList->find_sorted_key(
						(TConstSomething)aPlayer->get_game_id());

	if (Index >= 0) return;

	mPlayerList->insert_sorted(aPlayer);
}

void 
CCluster::add_council(CCouncil* aCouncil)
{
	assert(aCouncil);

	int 
		Index = mCouncilList->find_sorted_key(
						(TConstSomething)aCouncil->get_id());

	if (Index >= 0) return;

	mCouncilList->insert_sorted(aCouncil);
}

CPlanet*
CCluster::find_planet(int aPlanetID)
{
	int 
		Index = mPlanetList->find_sorted_key(
						(TConstSomething)aPlanetID);
	
	if (Index < 0) return NULL;

	return (CPlanet*)mPlanetList->get(Index);
}

CPlayer *
CCluster::find_player(int aPlayerID)
{
	int 
		Index = mPlayerList->find_sorted_key((TConstSomething)aPlayerID);
	
	if (Index < 0) return NULL;

	return (CPlayer *)mPlayerList->get(Index);
}

CCouncil*
CCluster::find_council(int aCouncilID)
{
	int 
		Index = mCouncilList->find_sorted_key(
						(TConstSomething)aCouncilID);
	
	if (Index < 0) return NULL;

	return (CCouncil*)mCouncilList->get(Index);
}

bool
CCluster::remove_planet(int aPlanetID)
{
	return mPlanetList->remove_planet(aPlanetID);
}

bool
CCluster::remove_player(int aPlayerID)
{
	return mPlayerList->remove_player(aPlayerID);
}

bool
CCluster::remove_council(int aCouncilID)
{
	return mCouncilList->remove_council(aCouncilID);
}


const char*
CCluster::get_new_planet_name()
{
	static CString Buffer;

	Buffer.clear();

	Buffer.format("%s-%d", (char *)mName, mPlanetList->length() + 1);

	return (char*)Buffer;
}

CCouncilList* 
CCluster::get_council_list()
{
	return mCouncilList;
}

CEventInstanceList*
CCluster::get_event_list()
{
	return mEventList;
}

char *
CCluster::info_council_list_html()
{
	static CString
		List;
	List.clear();

	List = "<TABLE WIDTH=\"510\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";

	int
		Column = 3;

	for (int i=0 ; i<mCouncilList->length() ; i++)
	{
		if (Column == 3) List += "<TR>\n";

		CCouncil *
			Council = (CCouncil *)mCouncilList->get(i);

		List += "<TD CLASS=\"tabletxt\" WIDTH=\"170\">";
		List += "&nbsp;";
		List.format("<A HREF=\"council_search_result.as?COUNCIL_ID=%d\">%s</A>",
					Council->get_id(), Council->get_nick());
		List += "</TD>\n";

		if (Column == 3) Column = 1;
		else Column++;

		if (Column == 3) List += "</TR>\n";
	}

	for (int i=Column ; i < 3 ; i++)
	{
		List += "<TD CLASS=\"tabletxt\" WIDTH=\"170\">&nbsp;</TD>\n";
		if (Column == 2) List += "</TR>\n";
	}

	List += "</TABLE>\n";

	return (char *)List;
}

