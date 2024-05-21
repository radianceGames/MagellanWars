#include "magistrate.h"
#include "archspace.h"
#include <libintl.h>
#include "empire.h"
#include "game.h"

TZone gMagistrateZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CMagistrate),
	0,
	0,
	NULL,
	"Zone CMagistrate"
};

CMagistrate::CMagistrate()
{
}

CMagistrate::~CMagistrate()
{
}

bool
CMagistrate::set_cluster_id(int aClusterID)
{
	if (aClusterID < 1) return false;

	mClusterID = aClusterID;

	refresh_name();

	return true;
}

bool
CMagistrate::add_admiral(CAdmiral *aAdmiral)
{
	return mAdmiralList.add_admiral(aAdmiral);
}

bool
CMagistrate::remove_admiral(int aAdmiralID)
{
	return mAdmiralList.remove_admiral(aAdmiralID);
}

bool
CMagistrate::add_fleet(CFleet *aFleet)
{
	return mFleetList.add_fleet(aFleet);
}

bool
CMagistrate::remove_fleet(int aID)
{
	return mFleetList.remove_fleet(aID);
}

bool
CMagistrate::add_planet(CPlanet *aPlanet)
{
	return mPlanetList.add_planet(aPlanet);
}

bool
CMagistrate::remove_planet(int aPlanetID)
{
	return mPlanetList.remove_planet(aPlanetID);
}

int
CMagistrate::get_number_of_planets()
{
	return mPlanetList.length();
}

CSortedPlanetList *
CMagistrate::get_lost_planet_list()
{
	static CSortedPlanetList
		LostPlanetList;
	LostPlanetList.remove_all();

	CEmpirePlanetInfoList *
		EmpirePlanetInfoList = EMPIRE->get_empire_planet_info_list();

	for (int i=0 ; i<EmpirePlanetInfoList->length() ; i++)
	{
		CEmpirePlanetInfo *
			EmpirePlanetInfo = (CEmpirePlanetInfo *)EmpirePlanetInfoList->get(i);
		if (EmpirePlanetInfo->get_planet_type() != CEmpire::LAYER_MAGISTRATE) continue;
		if (EmpirePlanetInfo->get_position_arg() != mClusterID) continue;
		if (EmpirePlanetInfo->get_owner_id() == EMPIRE_GAME_ID) continue;

		CPlanet *
			Planet = PLANET_TABLE->get_by_id(EmpirePlanetInfo->get_planet_id());
		if (Planet == NULL)
		{
			SLOG("ERROR : Planet is NULL in CMagistrate::get_lost_planet_list(), EmpirePlanetInfo->get_planet_id() = %d", EmpirePlanetInfo->get_planet_id());
			continue;
		}

		LostPlanetList.add_planet(Planet);
	}

	return &LostPlanetList;
}

int
CMagistrate::get_number_of_lost_planets()
{
	return CEmpire::mInitialNumberOfMagistratePlanet - mPlanetList.length();
}

bool
CMagistrate::is_dead()
{
	if (mPlanetList.length() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

CPlanet *
CMagistrate::get_target_planet()
{
	int
		NumberOfPlanet = mPlanetList.length();
	if (NumberOfPlanet == 0) return NULL;

	return (CPlanet *)mPlanetList.get(number(NumberOfPlanet) - 1);
}

bool
CMagistrate::regen_empire_fleets()
{
	if (is_dead() == true) return true;

	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)mFleetList.get(i);
		int
			CurrentShip = Fleet->get_current_ship();
		Fleet->set_current_ship(CurrentShip + CEmpire::mAmountOfMagistrateShipRegen);

		Fleet->type(QUERY_UPDATE);
		STORE_CENTER->store(*Fleet);
	}

	return true;
}

bool
CMagistrate::update_DB()
{
	mAdmiralList.update_DB();
	mFleetList.update_DB();

	mPlanetList.update_DB();

	return true;
}

bool
CMagistrate::initialize(int aClusterID)
{
	SLOG("SYSTEM : Initialize Magistrate(#%d) Fleet/Admiral/Planet", aClusterID);

	mClusterID = aClusterID;

	refresh_name();

	mFleetList.create_as_empire_fleet_group_static(&mAdmiralList, CEmpire::EMPIRE_FLEET_GROUP_TYPE_MAGISTRATE, mClusterID);

	/* Store Magistrate Admiral and Magistrate Fleet */
	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)mFleetList.get(i);
		CAdmiral *
			Admiral = mAdmiralList.get_by_id(Fleet->get_admiral_id());
		if (Admiral == NULL)
		{
			SLOG("ERROR : No magistrate(#%d) admiral found in CMagistrate::initialize(), Fleet->get_admiral_id() = %d", mClusterID, Fleet->get_admiral_id());
			continue;
		}

		Admiral->type(QUERY_INSERT);
		STORE_CENTER->store(*Admiral);

		Fleet->type(QUERY_INSERT);
		STORE_CENTER->store(*Fleet);
	}

	/* Store Magistrate Admiral Info and Magistrate Fleet Info */
	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)mFleetList.get(i);
		CAdmiral *
			Admiral = mAdmiralList.get_by_id(Fleet->get_admiral_id());
		if (Admiral == NULL)
		{
			SLOG("ERROR : No magistrate(#%d) admiral found in CMagistrate::initialize(), Fleet->get_admiral_id() = %d", mClusterID, Fleet->get_admiral_id());
			continue;
		}

		CEmpireAdmiralInfo *
			EmpireAdmiralInfo = new CEmpireAdmiralInfo();
		EmpireAdmiralInfo->set_admiral_id(Admiral->get_id());
		EmpireAdmiralInfo->set_admiral_type(CEmpire::LAYER_MAGISTRATE);
		EmpireAdmiralInfo->set_position_arg1(mClusterID);

		EmpireAdmiralInfo->type(QUERY_INSERT);
		STORE_CENTER->store(*EmpireAdmiralInfo);

		CEmpireAdmiralInfoList *
			EmpireAdmiralInfoList = EMPIRE->get_empire_admiral_info_list();
		EmpireAdmiralInfoList->add_empire_admiral_info(EmpireAdmiralInfo);

		CEmpireFleetInfo *
			EmpireFleetInfo = new CEmpireFleetInfo();
		EmpireFleetInfo->set_fleet_id(Fleet->get_id());
		EmpireFleetInfo->set_fleet_type(CEmpire::LAYER_MAGISTRATE);
		EmpireFleetInfo->set_position_arg1(mClusterID);

		EmpireFleetInfo->type(QUERY_INSERT);
		STORE_CENTER->store(*EmpireFleetInfo);

		CEmpireFleetInfoList *
			EmpireFleetInfoList = EMPIRE->get_empire_fleet_info_list();
		EmpireFleetInfoList->add_empire_fleet_info(EmpireFleetInfo);
	}

	CCluster *
		Cluster = UNIVERSE->get_by_id(mClusterID);

	/* Store Magistrate Planet */
	for (int i=0 ; i<CEmpire::mInitialNumberOfMagistratePlanet ; i++)
	{
		CPlanet *
			Planet = new CPlanet();
		Planet->initialize();
		Planet->set_owner(EMPIRE_GAME_ID);
		Planet->set_order(i);
		Planet->set_cluster(Cluster);
		Planet->set_name(Cluster->get_new_planet_name());
		Planet->set_max_population();
		Planet->init_planet_news_center();

		PLANET_TABLE->add_planet(Planet);
		mPlanetList.add_planet(Planet);

		Planet->type(QUERY_INSERT);
		STORE_CENTER->store(*Planet);
	}

	/* Store Magistrate Planet Info */
	for (int i=0 ; i<mPlanetList.length() ; i++)
	{
		CPlanet *
			Planet = (CPlanet *)mPlanetList.get(i);
		CEmpirePlanetInfo *
			EmpirePlanetInfo = new CEmpirePlanetInfo();
		EmpirePlanetInfo->set_planet_id(Planet->get_id());
		EmpirePlanetInfo->set_owner_id(EMPIRE_GAME_ID);
		EmpirePlanetInfo->set_planet_type(CEmpire::LAYER_MAGISTRATE);
		EmpirePlanetInfo->set_position_arg(mClusterID);

		EmpirePlanetInfo->type(QUERY_INSERT);
		STORE_CENTER->store(*EmpirePlanetInfo);

		CEmpirePlanetInfoList *
			EmpirePlanetInfoList = EMPIRE->get_empire_planet_info_list();
		EmpirePlanetInfoList->add_empire_planet_info(EmpirePlanetInfo);
	}

	return true;
}

void
CMagistrate::refresh_name()
{
	mName.clear();
	mName.format(GETTEXT("Magistrate (#%1$s)"), dec2unit(mClusterID));
}

CMagistrateList::CMagistrateList()
{
}

CMagistrateList::~CMagistrateList()
{
}

bool
CMagistrateList::free_item(TSomething aItem)
{
	CMagistrate *
		Magistrate = (CMagistrate *)aItem;
	if (Magistrate == NULL) return false;

	delete Magistrate;
	return true;
}

int
CMagistrateList::compare(TSomething aItem1, TSomething aItem2) const
{
	CMagistrate
		*Magistrate1 = (CMagistrate *)aItem1,
		*Magistrate2 = (CMagistrate *)aItem2;

	if (Magistrate1->get_cluster_id() > Magistrate2->get_cluster_id()) return 1;
	if (Magistrate1->get_cluster_id() < Magistrate2->get_cluster_id()) return -1;
	return 0;
}

int
CMagistrateList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CMagistrate *
		Magistrate = (CMagistrate *)aItem;

	if (Magistrate->get_cluster_id() > (int)aKey) return 1;
	if (Magistrate->get_cluster_id() < (int)aKey) return -1;
	return 0;
}

bool
CMagistrateList::add_magistrate(CMagistrate *aMagistrate)
{
	if (!aMagistrate) return false;

	if (find_sorted_key((TConstSomething)aMagistrate->get_cluster_id()) >= 0) 
		return false;
	insert_sorted(aMagistrate);

	return true;
}

bool
CMagistrateList::remove_magistrate(int aClusterID)
{
	int 
		Index = find_sorted_key((void *)aClusterID);	
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

CMagistrate *
CMagistrateList::get_by_cluster_id(int aClusterID)
{
	if (length() == 0) return NULL;

	int 
		Index = find_sorted_key((void *)aClusterID);

	if (Index < 0) return NULL;

	return (CMagistrate *)get(Index);
}

bool
CMagistrateList::update_DB()
{
	for (int i=0 ; i<length() ; i++)
	{
		CMagistrate *
			Magistrate = (CMagistrate *)get(i);
		Magistrate->update_DB();
	}

	return true;
}

