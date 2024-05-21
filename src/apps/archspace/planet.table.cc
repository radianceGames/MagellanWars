#include "planet.h"
#include "archspace.h"
#include "game.h"
#include <cstdlib>

CPlanetTable::CIDIndex::CIDIndex():
	CList(PLANET_TABLE_INDEX_LIMIT, PLANET_TABLE_INDEX_BLOCK)
{
}

CPlanetTable::CIDIndex::~CIDIndex()
{
	remove_all();
}

bool
CPlanetTable::CIDIndex::free_item(TSomething aItem)
{
	(void)aItem;
	return true;
}




CPlanetTable::CPlanetTable(): 
		CSortedList(PLANET_TABLE_LIMIT, PLANET_TABLE_BLOCK)
{
}

CPlanetTable::~CPlanetTable()
{
	remove_all();
}

bool
CPlanetTable::free_item(TSomething aItem)
{
	CPlanet
		*Planet = (CPlanet*)aItem;

	if (!Planet) return false;

	//SLOG("free planet(%d)", Planet->get_id());

	delete Planet;

	return true;
}

int
CPlanetTable::compare(TSomething aItem1, TSomething aItem2) const
{
	CPlanet
		*Planet1 = (CPlanet*)aItem1,
		*Planet2 = (CPlanet*)aItem2;

	if (Planet1->get_id() > Planet2->get_id()) return 1;
	if (Planet1->get_id() < Planet2->get_id()) return -1;
	return 0;
}

int
CPlanetTable::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CPlanet
		*Planet = (CPlanet*)aItem;

	if (Planet->get_id() > (int)aKey) return 1;
	if (Planet->get_id() < (int)aKey) return -1;
	return 0;
}

bool
CPlanetTable::load(CMySQL &aMySQL)
{
	int
		MaxID;

	if (PLAYER_TABLE->length() > 0)
		MaxID = ((CPlayer *)PLAYER_TABLE->get(PLAYER_TABLE->length()-1))->get_game_id();
	else
		return true;

	SLOG("Planet Loading");

	//aMySQL.query("LOCK TABLE planet READ");

	int PlayerID = -1;
	int PlanetOrder = 0;
	for (int i=0 ; i<MaxID+100 ; i += 100)
	{
		CString
			Query;
		Query.format("SELECT * FROM planet WHERE owner >= %d AND owner < %d ORDER BY owner, order_", i, i+100 );
		aMySQL.query((char *)Query);
		aMySQL.use_result();

		while(aMySQL.fetch_row())
		{
			if (atoi(aMySQL.row(CPlanet::STORE_OWNER)) == EMPIRE_GAME_ID) continue;

			if (atoi(aMySQL.row(CPlanet::STORE_OWNER)) != PlayerID)
			{
				PlayerID = atoi(aMySQL.row(CPlanet::STORE_OWNER));
				PlanetOrder = 0;
			} else
				PlanetOrder++;

			CPlanet *
				Planet = new CPlanet(aMySQL.row());
			if (Planet->get_order() != PlanetOrder) Planet->set_order(PlanetOrder);

			add_planet_when_loading(Planet);
		}

		aMySQL.free_result();
	}

	for (int i=0 ; i<PLAYER_TABLE->length() ; i++)
	{
		CPlayer *
			Player = (CPlayer *)PLAYER_TABLE->get(i);
		CPlanetList *
			PlanetList = Player->get_planet_list();
		for (int j=0 ; j<PlanetList->length() ; j++)
		{
			CPlanet *
				Planet = (CPlanet *)PlanetList->get(j);
			if (Planet->get_owner_id() != Player->get_game_id())
			{
				SLOG("ERROR : Wrong Planet owner in CPlayerTable::load(), Player->get_game_id() = %d, Planet->get_id() = %d, Planet->get_owner_id() = %d", Player->get_game_id(), Planet->get_id(), Planet->get_owner_id());
			}
		}
	}

	//aMySQL.query( "UNLOCK TABLES" );

	SLOG("%d planets are loaded", length());

	return true;
}

int
CPlanetTable::add_planet_when_loading(CPlanet *aPlanet)
{
	assert(aPlanet);
	if (!aPlanet) return -1;
	if (aPlanet->get_id() < 0) return -1;
	insert_sorted(aPlanet);

	mIDIndex.at(aPlanet->get_id(), aPlanet);

	CPlayer
		*Player = PLAYER_TABLE->get_by_game_id(aPlanet->get_owner_id());
	if (Player)
	{
		Player->add_planet_when_loading(aPlanet);	
	} else 
	{
		SLOG("Could not found owner(%d)", aPlanet->get_owner_id());
		aPlanet->type(QUERY_DELETE);
		STORE_CENTER->store(*aPlanet);
		delete aPlanet;
	}
	CCluster *Cluster = aPlanet->get_cluster();

	assert(Cluster);

	if (Cluster)
		Cluster->add_planet(aPlanet);
	else SLOG("WARNING:Could not found cluster");

	return aPlanet->get_id();
}

int
CPlanetTable::add_planet(CPlanet *aPlanet)
{
	if (aPlanet == NULL) return -1;
	if (aPlanet->get_id() < 0) return -1;
	insert_sorted(aPlanet);

	mIDIndex.at(aPlanet->get_id(), aPlanet);

	CCluster *Cluster = aPlanet->get_cluster();

	assert(Cluster);

	if (Cluster)
		Cluster->add_planet(aPlanet);
	else SLOG("WARNING:Could not found cluster");

	return aPlanet->get_id();
}

bool
CPlanetTable::remove_planet(int aPlanetID)
{
	mIDIndex.replace(aPlanetID, NULL);

	int 
		Index = find_sorted_key((TConstSomething)aPlanetID);

	if (Index < 0)
	{
		return false;
	}
	else
	{
		return CSortedList::remove(Index);
	}
}

CPlanet *
CPlanetTable::create_new_planet(CRace *aRace)
{
	CPlanet
		*Planet = new CPlanet;

	// Race should be considered after race design.
	Planet->initialize(aRace);

	return Planet;
}

