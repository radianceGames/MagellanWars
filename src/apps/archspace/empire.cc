#include "empire.h"
#include <libintl.h>
#include "archspace.h"
#include <cstdlib>
#include <cstdio>
#include "tech.h"
#include "game.h"
#include "council.h"

TZone gEmpireAdmiralInfo =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CEmpireAdmiralInfo),
	0,
	0,
	NULL,
	"Zone CEmpireAdmiralInfo"
};

TZone gEmpireFleetInfo =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CEmpireFleetInfo),
	0,
	0,
	NULL,
	"Zone CEmpireFleetInfo"
};

TZone gEmpirePlanetInfo =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CEmpirePlanetInfo),
	0,
	0,
	NULL,
	"Zone CEmpirePlanetInfo"
};

int CEmpire::mInitialNumberOfMagistratePlanet = 50;
int CEmpire::mInitialNumberOfEmpirePlanet = 5000;
int CEmpire::mFleetRegenCycleInTurn = 1;
int CEmpire::mAmountOfMagistrateShipRegen = 1;
int CEmpire::mAmountOfFortress1ShipRegen = 1;
int CEmpire::mAmountOfFortress2ShipRegen = 1;
int CEmpire::mAmountOfFortress3ShipRegen = 2;
int CEmpire::mAmountOfFortress4ShipRegen = 3;
int CEmpire::mAmountOfCapitalPlanetShipRegen = 3;
int CEmpire::mFortress1AdmiralFleetCommandingBonus = 0;
int CEmpire::mFortress2AdmiralFleetCommandingBonus = 0;
int CEmpire::mFortress3AdmiralFleetCommandingBonus = 0;
int CEmpire::mFortress4AdmiralFleetCommandingBonus = 0;
int CEmpire::mCapitalPlanetAdmiralFleetCommandingBonus = 0;
int CEmpire::mEmpireInvasionLimit = 0;
int CEmpire::mEmpireInvasionLimitDuration = 1;
int CEmpire::mMaxEmpireFleetID = 0;

bool
CEmpire::update_max_empire_fleet_id(CFleet *aFleet)
{
	if (aFleet->get_id() > mMaxEmpireFleetID) mMaxEmpireFleetID = aFleet->get_id();

	return true;
}

CEmpireCapitalPlanet::CEmpireCapitalPlanet()
{
	mOwnerID = EMPIRE_GAME_ID;
	mName = GETTEXT("Empire Capital Planet");
}

CEmpireCapitalPlanet::~CEmpireCapitalPlanet()
{
}

CString &
CEmpireCapitalPlanet::query()
{
	static CString
		Query;
	Query.clear();

	switch (mQueryType)
	{
		case QUERY_UPDATE :
			Query.format("UPDATE empire_capital_planet SET owner_id = %d",
							mOwnerID);
			break;

		default :
			break;
	}

	mStoreFlag.clear();

	return Query;
}

bool
CEmpireCapitalPlanet::set_owner_id(int aOwnerID)
{
	CPlayer *
		Owner = PLAYER_TABLE->get_by_game_id(aOwnerID);
	if (Owner == NULL) return false;

	mOwnerID = aOwnerID;
	mStoreFlag += STORE_OWNER_ID;

	return true;
}

bool
CEmpireCapitalPlanet::add_admiral(CAdmiral *aAdmiral)
{
	if (aAdmiral == NULL) return false;

	return mAdmiralList.add_admiral(aAdmiral);
}

bool
CEmpireCapitalPlanet::remove_admiral(int aAdmiralID)
{
	return mAdmiralList.remove_admiral(aAdmiralID);
}

bool
CEmpireCapitalPlanet::add_fleet(CFleet *aFleet)
{
	if (aFleet == NULL) return false;

	return mFleetList.add_fleet(aFleet);
}

bool
CEmpireCapitalPlanet::remove_fleet(int aFleetID)
{
	return mFleetList.remove_fleet(aFleetID);
}

bool
CEmpireCapitalPlanet::is_dead()
{
	if (mOwnerID == EMPIRE_GAME_ID)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool
CEmpireCapitalPlanet::regen_empire_fleets()
{
	if (is_dead() == true) return true;

	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)mFleetList.get(i);
		int
			CurrentShip = Fleet->get_current_ship();
		Fleet->set_current_ship(CurrentShip + CEmpire::mAmountOfCapitalPlanetShipRegen);

		Fleet->type(QUERY_UPDATE);
		STORE_CENTER->store(*Fleet);
	}

	return true;
}

bool
CEmpireCapitalPlanet::update_DB()
{
	mAdmiralList.update_DB();
	mFleetList.update_DB();

	return true;
}

bool
CEmpireCapitalPlanet::initialize()
{
	SLOG("SYSTEM : Initialize Capital Empire Planet Fleet/Admiral");

	mFleetList.create_as_empire_fleet_group_static(&mAdmiralList, CEmpire::EMPIRE_FLEET_GROUP_TYPE_EMPIRE_CAPITAL_PLANET);

	/* Store Capital Planet Admiral and Capital Planet Fleet */
	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)mFleetList.get(i);
		CAdmiral *
			Admiral = mAdmiralList.get_by_id(Fleet->get_admiral_id());
		if (Admiral == NULL)
		{
			SLOG("ERROR : No empire planet admiral found in CEmpireCapitalPlanet::initialize(), Fleet->get_admiral_id() = %d", Fleet->get_admiral_id());
			continue;
		}

		Fleet->type(QUERY_INSERT);
		STORE_CENTER->store(*Fleet);

		Admiral->type(QUERY_INSERT);
		STORE_CENTER->store(*Admiral);
	}

	/* Store Capital Planet Admiral Info and Capital Planet Fleet Info */
	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)mFleetList.get(i);
		CAdmiral *
			Admiral = mAdmiralList.get_by_id(Fleet->get_admiral_id());

		CEmpireAdmiralInfo *
			EmpireAdmiralInfo = new CEmpireAdmiralInfo();
		EmpireAdmiralInfo->set_admiral_id(Admiral->get_id());
		EmpireAdmiralInfo->set_admiral_type(CEmpire::LAYER_EMPIRE_CAPITAL_PLANET);

		EmpireAdmiralInfo->type(QUERY_INSERT);
		STORE_CENTER->store(*EmpireAdmiralInfo);

		CEmpireAdmiralInfoList *
			EmpireAdmiralInfoList = EMPIRE->get_empire_admiral_info_list();
		EmpireAdmiralInfoList->add_empire_admiral_info(EmpireAdmiralInfo);

		CEmpireFleetInfo *
			EmpireFleetInfo = new CEmpireFleetInfo();
		EmpireFleetInfo->set_fleet_id(Fleet->get_id());
		EmpireFleetInfo->set_fleet_type(CEmpire::LAYER_EMPIRE_CAPITAL_PLANET);

		EmpireFleetInfo->type(QUERY_INSERT);
		STORE_CENTER->store(*EmpireFleetInfo);

		CEmpireFleetInfoList *
			EmpireFleetInfoList = EMPIRE->get_empire_fleet_info_list();
		EmpireFleetInfoList->add_empire_fleet_info(EmpireFleetInfo);
	}

	return true;
}

CEmpireAdmiralInfo::CEmpireAdmiralInfo()
{
	mAdmiralID = -1;
	mAdmiralType = -1;
	mPositionArg1 = -1;
	mPositionArg2 = -1;
	mPositionArg3 = -1;
}

CEmpireAdmiralInfo::CEmpireAdmiralInfo(MYSQL_ROW aRow)
{
	mAdmiralID = as_atoi(aRow[STORE_ADMIRAL_ID]);
	mAdmiralType = as_atoi(aRow[STORE_ADMIRAL_TYPE]);
	mPositionArg1 = as_atoi(aRow[STORE_POSITION_ARG1]);
	mPositionArg2 = as_atoi(aRow[STORE_POSITION_ARG2]);
	mPositionArg3 = as_atoi(aRow[STORE_POSITION_ARG3]);
}

CEmpireAdmiralInfo::~CEmpireAdmiralInfo()
{
}

CString &
CEmpireAdmiralInfo::query()
{
	static CString
		Query;
	Query.clear();

	switch (mQueryType)
	{
		case QUERY_INSERT :
			Query.format("INSERT INTO empire_admiral_info (admiral_id, admiral_type, position_arg1, position_arg2, position_arg3) VALUES (%d, %d, %d, %d, %d)", mAdmiralID, mAdmiralType, mPositionArg1, mPositionArg2, mPositionArg3);
			break;

		case QUERY_UPDATE :
		{
			if (mStoreFlag.is_empty()) break;

			Query.format("UPDATE empire_admiral_info SET admiral_id = %d", mAdmiralID);

#define STORE(x, y, z) \
			if (mStoreFlag.has(x)) \
				Query.format(y, z)

			STORE(STORE_ADMIRAL_TYPE, ", admiral_type = %d", mAdmiralType);

			STORE(STORE_POSITION_ARG1, ", position_arg1 = %d", mPositionArg1);

			STORE(STORE_POSITION_ARG2, ", position_arg2 = %d", mPositionArg2);

			STORE(STORE_POSITION_ARG3, ", position_arg3 = %d", mPositionArg3);

			Query.format(" WHERE id = %d", mAdmiralID);

			break;
		}

		case QUERY_DELETE :
			Query.format("DELETE FROM empire_admiral_info WHERE admiral_id = %d", mAdmiralID);
			break;
	}

	mStoreFlag.clear();

	return Query;
}

bool
CEmpireAdmiralInfo::set_admiral_id(int aAdmiralID)
{
	if (aAdmiralID < 1) return false;

	mAdmiralID = aAdmiralID;
	mStoreFlag += STORE_ADMIRAL_ID;
	return true;
}

bool
CEmpireAdmiralInfo::set_admiral_type(int aAdmiralType)
{
	if (aAdmiralType < CEmpire::LAYER_MAGISTRATE ||
		aAdmiralType > CEmpire::LAYER_EMPIRE_CAPITAL_PLANET) return false;

	mAdmiralType = aAdmiralType;
	mStoreFlag += STORE_ADMIRAL_TYPE;
	return true;
}

bool
CEmpireAdmiralInfo::set_position_arg1(int aPositionArg1)
{
	if (aPositionArg1 < 0) return false;

	mPositionArg1 = aPositionArg1;
	mStoreFlag += STORE_POSITION_ARG1;
	return true;
}

bool
CEmpireAdmiralInfo::set_position_arg2(int aPositionArg2)
{
	if (aPositionArg2 < 0) return false;

	mPositionArg2 = aPositionArg2;
	mStoreFlag += STORE_POSITION_ARG2;
	return true;
}

bool
CEmpireAdmiralInfo::set_position_arg3(int aPositionArg3)
{
	if (aPositionArg3 < 0) return false;

	mPositionArg3 = aPositionArg3;
	mStoreFlag += STORE_POSITION_ARG3;
	return true;
}

CEmpireAdmiralInfoList::CEmpireAdmiralInfoList()
{
}

CEmpireAdmiralInfoList::~CEmpireAdmiralInfoList()
{
}

bool
CEmpireAdmiralInfoList::free_item(TSomething aItem)
{
	CEmpireAdmiralInfo *
		EmpireAdmiralInfo = (CEmpireAdmiralInfo *)aItem;
	if (EmpireAdmiralInfo == NULL) return false;

	delete EmpireAdmiralInfo;
	return true;
}

int
CEmpireAdmiralInfoList::compare(TSomething aItem1, TSomething aItem2) const
{
	CEmpireAdmiralInfo
		*EmpireAdmiralInfo1 = (CEmpireAdmiralInfo *)aItem1,
		*EmpireAdmiralInfo2 = (CEmpireAdmiralInfo *)aItem2;

	if (EmpireAdmiralInfo1->get_admiral_id() > EmpireAdmiralInfo2->get_admiral_id()) return 1;
	if (EmpireAdmiralInfo1->get_admiral_id() < EmpireAdmiralInfo2->get_admiral_id()) return -1;
	return 0;
}

int
CEmpireAdmiralInfoList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CEmpireAdmiralInfo *
		EmpireAdmiralInfo = (CEmpireAdmiralInfo *)aItem;

	if (EmpireAdmiralInfo->get_admiral_id() > (int)aKey) return 1;
	if (EmpireAdmiralInfo->get_admiral_id() < (int)aKey) return -1;
	return 0;
}

bool
CEmpireAdmiralInfoList::add_empire_admiral_info(CEmpireAdmiralInfo *aEmpireAdmiralInfo)
{
	if (!aEmpireAdmiralInfo) return false;

	if (find_sorted_key((TConstSomething)aEmpireAdmiralInfo->get_admiral_id()) >= 0)
		return false;
	insert_sorted(aEmpireAdmiralInfo);

	return true;
}

bool
CEmpireAdmiralInfoList::remove_empire_admiral_info(int aAdmiralID)
{
	int
		Index = find_sorted_key((void *)aAdmiralID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

CEmpireAdmiralInfo *
CEmpireAdmiralInfoList::get_by_admiral_id(int aAdmiralID)
{
	if (length() == 0) return NULL;

	int
		Index = find_sorted_key((void *)aAdmiralID);

	if (Index < 0) return NULL;

	return (CEmpireAdmiralInfo *)get(Index);
}

bool
CEmpireAdmiralInfoList::initialize(CMySQL *aMySQL)
{
	//aMySQL->query("LOCK TABLE empire_admiral_info READ");
	aMySQL->query("SELECT * FROM empire_admiral_info");
	aMySQL->use_result();

	while (aMySQL->fetch_row())
	{
		CEmpireAdmiralInfo *
			EmpireAdmiralInfo = new CEmpireAdmiralInfo(aMySQL->row());
		add_empire_admiral_info(EmpireAdmiralInfo);
	}

	aMySQL->free_result();
	//aMySQL->query("UNLOCK TABLES");

	return true;
}

bool
CEmpireAdmiralInfoList::remove_all_from_db()
{
	for (int i=0 ; i<length() ; i++)
	{
		CEmpireAdmiralInfo *
			EmpireAdmiralInfo = (CEmpireAdmiralInfo *)get(i);

		EmpireAdmiralInfo->type(QUERY_DELETE);
		STORE_CENTER->store(*EmpireAdmiralInfo);
	}

	return true;
}

bool
CEmpireAdmiralInfoList::update_DB()
{
	for (int i=0 ; i<length() ; i++)
	{
		CEmpireAdmiralInfo *
			EmpireAdmiralInfo = (CEmpireAdmiralInfo *)get(i);
		if (EmpireAdmiralInfo->is_changed() == false) continue;

		EmpireAdmiralInfo->type(QUERY_UPDATE);
		STORE_CENTER->store(*EmpireAdmiralInfo);
	}

	return true;
}

CEmpireFleetInfo::CEmpireFleetInfo()
{
	mFleetID = -1;
	mFleetType = -1;
	mPositionArg1 = -1;
	mPositionArg2 = -1;
	mPositionArg3 = -1;
}

CEmpireFleetInfo::CEmpireFleetInfo(MYSQL_ROW aRow)
{
	mFleetID = as_atoi(aRow[STORE_FLEET_ID]);
	mFleetType = as_atoi(aRow[STORE_FLEET_TYPE]);
	mPositionArg1 = as_atoi(aRow[STORE_POSITION_ARG1]);
	mPositionArg2 = as_atoi(aRow[STORE_POSITION_ARG2]);
	mPositionArg3 = as_atoi(aRow[STORE_POSITION_ARG3]);
}

CEmpireFleetInfo::~CEmpireFleetInfo()
{
}

CString &
CEmpireFleetInfo::query()
{
	static CString
		Query;
	Query.clear();

	switch (mQueryType)
	{
		case QUERY_INSERT :
		{
			Query.format("INSERT INTO empire_fleet_info (fleet_id, fleet_type, position_arg1, position_arg2, position_arg3) VALUES (%d, %d, %d, %d, %d)", mFleetID, mFleetType, mPositionArg1, mPositionArg2, mPositionArg3);
		}
			break;

		case QUERY_UPDATE :
		{
			if (mStoreFlag.is_empty()) break;

			Query.format("UPDATE empire_fleet_info SET fleet_id = %d", mFleetID);

#define STORE(x, y, z) \
			if (mStoreFlag.has(x)) \
				Query.format(y, z)

			STORE(STORE_FLEET_TYPE, ", fleet_type = %d", mFleetType);

			STORE(STORE_POSITION_ARG1, ", position_arg1 = %d", mPositionArg1);

			STORE(STORE_POSITION_ARG2, ", position_arg2 = %d", mPositionArg2);

			STORE(STORE_POSITION_ARG3, ", position_arg3 = %d", mPositionArg3);

			Query.format(" WHERE id = %d", mFleetID);
		}

			break;

		case QUERY_DELETE :
			Query.format("DELETE FROM empire_fleet_info WHERE fleet_id = %d", mFleetID);
			break;
	}

	mStoreFlag.clear();

	return Query;
}

bool
CEmpireFleetInfo::set_fleet_id(int aFleetID)
{
	if (aFleetID < 1) return false;

	mFleetID = aFleetID;
	mStoreFlag += STORE_FLEET_ID;
	return true;
}

bool
CEmpireFleetInfo::set_fleet_type(int aFleetType)
{
	if (aFleetType < CEmpire::LAYER_MAGISTRATE ||
		aFleetType > CEmpire::LAYER_EMPIRE_CAPITAL_PLANET) return false;

	mFleetType = aFleetType;
	mStoreFlag += STORE_FLEET_TYPE;
	return true;
}

bool
CEmpireFleetInfo::set_position_arg1(int aPositionArg1)
{
	if (aPositionArg1 < 0) return false;

	mPositionArg1 = aPositionArg1;
	mStoreFlag += STORE_POSITION_ARG1;
	return true;
}

bool
CEmpireFleetInfo::set_position_arg2(int aPositionArg2)
{
	if (aPositionArg2 < 0) return false;

	mPositionArg2 = aPositionArg2;
	mStoreFlag += STORE_POSITION_ARG2;
	return true;
}

bool
CEmpireFleetInfo::set_position_arg3(int aPositionArg3)
{
	if (aPositionArg3 < 0) return false;

	mPositionArg3 = aPositionArg3;
	mStoreFlag += STORE_POSITION_ARG3;
	return true;
}

CEmpireFleetInfoList::CEmpireFleetInfoList()
{
}

CEmpireFleetInfoList::~CEmpireFleetInfoList()
{
}

bool
CEmpireFleetInfoList::free_item(TSomething aItem)
{
	CEmpireFleetInfo *
		EmpireFleetInfo = (CEmpireFleetInfo *)aItem;
	if (EmpireFleetInfo == NULL) return false;

	delete EmpireFleetInfo;
	return true;
}

int
CEmpireFleetInfoList::compare(TSomething aItem1, TSomething aItem2) const
{
	CEmpireFleetInfo
		*EmpireFleetInfo1 = (CEmpireFleetInfo *)aItem1,
		*EmpireFleetInfo2 = (CEmpireFleetInfo *)aItem2;

	if (EmpireFleetInfo1->get_fleet_id() > EmpireFleetInfo2->get_fleet_id()) return 1;
	if (EmpireFleetInfo1->get_fleet_id() < EmpireFleetInfo2->get_fleet_id()) return -1;
	return 0;
}

int
CEmpireFleetInfoList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CEmpireFleetInfo *
		EmpireFleetInfo = (CEmpireFleetInfo *)aItem;

	if (EmpireFleetInfo->get_fleet_id() > (int)aKey) return 1;
	if (EmpireFleetInfo->get_fleet_id() < (int)aKey) return -1;
	return 0;
}

bool
CEmpireFleetInfoList::add_empire_fleet_info(CEmpireFleetInfo *aEmpireFleetInfo)
{
	if (!aEmpireFleetInfo) return false;

	if (find_sorted_key((TConstSomething)aEmpireFleetInfo->get_fleet_id()) >= 0)
		return false;
	insert_sorted(aEmpireFleetInfo);

	return true;
}

bool
CEmpireFleetInfoList::remove_empire_fleet_info(int aFleetID)
{
	int
		Index = find_sorted_key((void *)aFleetID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

CEmpireFleetInfo *
CEmpireFleetInfoList::get_by_fleet_id(int aFleetID)
{
	if (length() == 0) return NULL;

	int
		Index = find_sorted_key((void *)aFleetID);

	if (Index < 0) return NULL;

	return (CEmpireFleetInfo *)get(Index);
}

bool
CEmpireFleetInfoList::initialize(CMySQL *aMySQL)
{
	//aMySQL->query("LOCK TABLE empire_fleet_info READ");
	aMySQL->query("SELECT * FROM empire_fleet_info");
	aMySQL->use_result();

	while (aMySQL->fetch_row())
	{
		CEmpireFleetInfo *
			EmpireFleetInfo = new CEmpireFleetInfo(aMySQL->row());
		add_empire_fleet_info(EmpireFleetInfo);
	}

	aMySQL->free_result();
	//aMySQL->query("UNLOCK TABLES");

	return true;
}

bool
CEmpireFleetInfoList::remove_all_from_db()
{
	for (int i=0 ; i<length() ; i++)
	{
		CEmpireFleetInfo *
			EmpireFleetInfo = (CEmpireFleetInfo *)get(i);

		EmpireFleetInfo->type(QUERY_DELETE);
		STORE_CENTER->store(*EmpireFleetInfo);
	}

	return true;
}

bool
CEmpireFleetInfoList::update_DB()
{
	for (int i=0 ; i<length() ; i++)
	{
		CEmpireFleetInfo *
			EmpireFleetInfo = (CEmpireFleetInfo *)get(i);
		if (EmpireFleetInfo->is_changed() == false) continue;

		EmpireFleetInfo->type(QUERY_UPDATE);
		STORE_CENTER->store(*EmpireFleetInfo);
	}

	return true;
}

CEmpirePlanetInfo::CEmpirePlanetInfo()
{
	mPlanetID = 0;
	mOwnerID = 0;
	mPlanetType = -1;
	mPositionArg = -1;
}

CEmpirePlanetInfo::CEmpirePlanetInfo(MYSQL_ROW aRow)
{
	mPlanetID = as_atoi(aRow[STORE_PLANET_ID]);
	mOwnerID = as_atoi(aRow[STORE_OWNER_ID]);
	mPlanetType = as_atoi(aRow[STORE_PLANET_TYPE]);
	mPositionArg = as_atoi(aRow[STORE_POSITION_ARG]);
}

CEmpirePlanetInfo::~CEmpirePlanetInfo()
{
}

CString &
CEmpirePlanetInfo::query()
{
	static CString
		Query;
	Query.clear();

	switch (mQueryType)
	{
		case QUERY_INSERT :
			Query.format("INSERT INTO empire_planet_info (planet_id, owner_id, planet_type, position_arg) VALUES (%d, %d, %d, %d)", mPlanetID, mOwnerID, mPlanetType, mPositionArg);
			break;

		case QUERY_UPDATE :
		{
			if (mStoreFlag.is_empty()) break;

			Query.format("UPDATE empire_planet_info SET planet_id = %d", mPlanetID);

#define STORE(x, y, z) \
			if (mStoreFlag.has(x)) \
				Query.format(y, z)

			STORE(STORE_OWNER_ID, ", owner_id = %d", mOwnerID);

			STORE(STORE_PLANET_TYPE, ", planet_type = %d", mPlanetType);

			STORE(STORE_POSITION_ARG, ", position_arg = %d", mPositionArg);

			Query.format(" WHERE planet_id = %d", mPlanetID);

			break;
		}

		case QUERY_DELETE :
			Query.format("DELETE FROM empire_planet_info WHERE planet_id = %d", mPlanetID);
			break;
	}

	mStoreFlag.clear();

	return Query;
}

bool
CEmpirePlanetInfo::set_planet_id(int aPlanetID)
{
	if (aPlanetID < 1) return false;

	mPlanetID = aPlanetID;
	mStoreFlag += STORE_PLANET_ID;
	return true;
}

bool
CEmpirePlanetInfo::set_owner_id(int aOwnerID)
{
	mOwnerID = aOwnerID;
	mStoreFlag += STORE_OWNER_ID;
	return true;
}

bool
CEmpirePlanetInfo::set_planet_type(int aPlanetType)
{
	if (aPlanetType != CEmpire::LAYER_MAGISTRATE &&
		aPlanetType != CEmpire::LAYER_EMPIRE_PLANET) return false;

	mPlanetType = aPlanetType;
	mStoreFlag += STORE_PLANET_TYPE;
	return true;
}

bool
CEmpirePlanetInfo::set_position_arg(int aPositionArg)
{
	if (aPositionArg< 0) return false;

	mPositionArg = aPositionArg;
	mStoreFlag += STORE_POSITION_ARG;
	return true;
}

CEmpirePlanetInfoList::CEmpirePlanetInfoList()
{
}

CEmpirePlanetInfoList::~CEmpirePlanetInfoList()
{
}

bool
CEmpirePlanetInfoList::free_item(TSomething aItem)
{
	CEmpirePlanetInfo *
		EmpirePlanetInfo = (CEmpirePlanetInfo *)aItem;
	if (EmpirePlanetInfo == NULL) return false;

	delete EmpirePlanetInfo;
	return true;
}

int
CEmpirePlanetInfoList::compare(TSomething aItem1, TSomething aItem2) const
{
	CEmpirePlanetInfo
		*EmpirePlanetInfo1 = (CEmpirePlanetInfo *)aItem1,
		*EmpirePlanetInfo2 = (CEmpirePlanetInfo *)aItem2;

	if (EmpirePlanetInfo1->get_planet_id() > EmpirePlanetInfo2->get_planet_id()) return 1;
	if (EmpirePlanetInfo1->get_planet_id() < EmpirePlanetInfo2->get_planet_id()) return -1;
	return 0;
}

int
CEmpirePlanetInfoList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CEmpirePlanetInfo *
		EmpirePlanetInfo = (CEmpirePlanetInfo *)aItem;

	if (EmpirePlanetInfo->get_planet_id() > (int)aKey) return 1;
	if (EmpirePlanetInfo->get_planet_id() < (int)aKey) return -1;
	return 0;
}

bool
CEmpirePlanetInfoList::add_empire_planet_info(CEmpirePlanetInfo *aEmpirePlanetInfo)
{
	if (!aEmpirePlanetInfo) return false;

	if (find_sorted_key((TConstSomething)aEmpirePlanetInfo->get_planet_id()) >= 0)
		return false;
	insert_sorted(aEmpirePlanetInfo);

	return true;
}

bool
CEmpirePlanetInfoList::remove_empire_planet_info(int aPlanetID)
{
	int
		Index = find_sorted_key((void *)aPlanetID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

CEmpirePlanetInfo *
CEmpirePlanetInfoList::get_by_planet_id(int aPlanetID)
{
	if (length() == 0) return NULL;

	int
		Index = find_sorted_key((void *)aPlanetID);

	if (Index < 0) return NULL;

	return (CEmpirePlanetInfo *)get(Index);
}

bool
CEmpirePlanetInfoList::initialize(CMySQL *aMySQL)
{
	//aMySQL->query("LOCK TABLE empire_planet_info READ");
	aMySQL->query("SELECT * FROM empire_planet_info");
	aMySQL->use_result();

	while (aMySQL->fetch_row())
	{
		CEmpirePlanetInfo *
			EmpirePlanetInfo = new CEmpirePlanetInfo(aMySQL->row());
		add_empire_planet_info(EmpirePlanetInfo);
	}

	aMySQL->free_result();
	//aMySQL->query("UNLOCK TABLES");

	return true;
}

bool
CEmpirePlanetInfoList::remove_all_from_db()
{
	for (int i=0 ; i<length() ; i++)
	{
		CEmpirePlanetInfo *
			EmpirePlanetInfo = (CEmpirePlanetInfo *)get(i);

		EmpirePlanetInfo->type(QUERY_DELETE);
		STORE_CENTER->store(*EmpirePlanetInfo);
	}

	return true;
}

bool
CEmpirePlanetInfoList::update_DB()
{
	for (int i=0 ; i<length() ; i++)
	{
		CEmpirePlanetInfo *
			EmpirePlanetInfo = (CEmpirePlanetInfo *)get(i);
		if (EmpirePlanetInfo->is_changed() == false) continue;

		EmpirePlanetInfo->type(QUERY_UPDATE);
		STORE_CENTER->store(*EmpirePlanetInfo);
	}

	return true;
}

int
CEmpirePlanetInfoList::get_number_of_planets_by_owner(CPlayer *aPlayer, int aPlanetType)
{
	if (aPlayer == NULL) return -1;
	if (aPlanetType != CEmpire::LAYER_MAGISTRATE &&
		aPlanetType != CEmpire::LAYER_EMPIRE_PLANET) return -1;

	int
		Number = 0;
	for (int i=0 ; i<length() ; i++)
	{
		CEmpirePlanetInfo *
			EmpirePlanetInfo = (CEmpirePlanetInfo *)get(i);
		if (EmpirePlanetInfo->get_planet_type() != aPlanetType) continue;
		if (EmpirePlanetInfo->get_owner_id() == aPlayer->get_game_id()) Number++;
	}

	return Number;
}

TZone gEmpireActionZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CEmpireAction),
	0,
	0,
	NULL,
	"Zone CEmpireAction"
};

CEmpireAction::CEmpireAction()
{
	mID = mOwner = mAmount = mAction = mTarget = mAnswer = mTime = 0;
}

CEmpireAction::CEmpireAction( MYSQL_ROW aRow )
{
	mID = atoi(aRow[STORE_ID]);
	mOwner = atoi(aRow[STORE_OWNER]);
	mAction = atoi(aRow[STORE_ACTION]);
	mAmount = atoi(aRow[STORE_AMOUNT]);
	mTarget = atoi(aRow[STORE_TARGET]);
	mAnswer = atoi(aRow[STORE_ANSWER]);
	mTime = atoi(aRow[STORE_TIME]) + CGameStatus::get_down_time();
	mStoreFlag += STORE_TIME;
	type(QUERY_UPDATE);
	STORE_CENTER->store(*this);
}

CEmpireAction::~CEmpireAction()
{

}

CString &
CEmpireAction::query()
{
	static CString
		Query;
	Query.clear();

	switch( type() ){
		case QUERY_INSERT :
			Query.format( "INSERT INTO empire_action ( id, owner, action, amount, target, answer, time ) VALUES ( %d, %d, %d, %d, %d, %d, %d )", mID, mOwner, mAction, mAmount, mTarget, mAnswer, mTime );
			break;
		case QUERY_DELETE :
			Query.format( "DELETE FROM empire_action WHERE owner = %d AND id = %d", mOwner, mID );
			break;
		case QUERY_UPDATE :
			Query.format( "UPDATE empire_action SET amount = %d, answer = %d, target = %d WHERE owner = %d AND id = %d", mAmount, mAnswer, mTarget, mOwner, mID );
			break;
	}

	mStoreFlag.clear();

	return Query;
}

bool
CEmpireAction::set_id(int aID)
{
	mID = aID;
	mStoreFlag += STORE_ID;

	return true;
}

bool
CEmpireAction::set_owner(int aOwner)
{
	mOwner = aOwner;
	mStoreFlag += STORE_OWNER;

	return true;
}

bool
CEmpireAction::set_action(int aAction)
{
	mAction = aAction;
	mStoreFlag += STORE_ACTION;

	return true;
}

const char *
CEmpireAction::get_action_name()
{
	switch( mAction ){
		case EA_ACTION_BRIBE :
			return GETTEXT("Bribe");
			break;
		case EA_ACTION_REQUEST_BOON :
			return GETTEXT("Request Boon");
			break;
		case EA_ACTION_REQUEST_MEDIATION :
			return GETTEXT("Request Mediation");
			break;
		case EA_ACTION_INVADE_MAGISTRATE_DOMINION :
			return GETTEXT("Invade Magistrate Dominion");
			break;
		case EA_ACTION_INVADE_EMPIRE :
			return GETTEXT("Invade Empire");
			break;
		case ED_DEMAND_TRIBUTE :
			return GETTEXT("Tribute Demanded");
			break;
		case ED_DEMAND_LEAVE_COUNCIL :
			return GETTEXT("Leave Council Demanded");
			break;
		case ED_DEMAND_DECLARE_WAR :
			return GETTEXT("Declare War Demanded");
			break;
		case ED_DEMAND_MAKE_TRUCE :
			return GETTEXT("Make Truce Demanded");
			break;
		case ED_DEMAND_FREE_SUBORDINARY_COUNCIL :
			return GETTEXT("Free Subordinary Council Demanded");
			break;
		case ED_DEMAND_ARMAMENT_REDUCTION :
			return GETTEXT("Armament Reduction Demanded");
			break;
		case ED_GRANT_RANK :
			return GETTEXT("Rank Granted");
			break;
		case ED_GRANT_BOON :
			return GETTEXT("Boon Granted");
			break;
		default :
			return NULL;
			break;
	}
}

bool
CEmpireAction::set_target(int aTarget)
{
	mTarget = aTarget;
	mStoreFlag += STORE_TARGET;

	return true;
}

bool
CEmpireAction::set_amount(int aAmount)
{
	mAmount = aAmount;
	mStoreFlag += STORE_AMOUNT;

	return true;
}

const char *
CEmpireAction::get_target_amount_str(CPlayer *aPlayer)
{
	static CString
		Message;
	Message.clear();

	switch( get_action() ){
		case EA_ACTION_BRIBE :
		case ED_DEMAND_TRIBUTE :
			Message.format( "%d PP", get_amount() );
			break;
		case EA_ACTION_REQUEST_BOON :
		case ED_GRANT_BOON :
			switch( get_target() ){
				case EA_REWARD_LOW_PRODUCTION :
				case EA_REWARD_MEDIUM_PRODUCTION :
				case EA_REWARD_HIGH_PRODUCTION :
					Message.format( "%d PP", get_amount() );
					break;
				case EA_REWARD_LEVEL2_TECH :
				case EA_REWARD_LEVEL34_TECH :
				case EA_REWARD_LEVEL56_TECH :
				case EA_REWARD_LEVEL78_TECH:
					{
						CTech
							*Tech = TECH_TABLE->get_by_id(get_amount());
						if(Tech)
							Message.format( "Tech: %s", Tech->get_name() );
						else
							Message.format( "Tech: (null)" );
					}
					break;
				case EA_REWARD_LEVEL13_COMMANDER :
				case EA_REWARD_LEVEL46_COMMANDER :
				case EA_REWARD_LEVEL710_COMMANDER :
				case EA_REWARD_LEVEL1115_COMMANDER :
					{
						CAdmiral
							*Admiral = aPlayer->get_admiral_pool()->get_by_id(get_amount());
						if( Admiral == NULL )
							Admiral = aPlayer->get_admiral_list()->get_by_id(get_amount());
						if( Admiral )
							Message.format( "Commander: %s", Admiral->get_name() );
						else
							Message += "Commander: R.I.P.";
					}
					break;
				case EA_REWARD_CLASS1_FLEET :
				case EA_REWARD_CLASS2_FLEET :
				case EA_REWARD_CLASS3_FLEET :
				case EA_REWARD_CLASS4_FLEET :
				case EA_REWARD_CLASS5_FLEET :
					{
						CFleet
							*Fleet = aPlayer->get_fleet_list()->get_by_id(get_amount());
						if( Fleet )
							Message.format( "Fleet: %s", Fleet->get_name() );
						else
							Message.format( "Fleet: (null)" );
					}
					break;
				case EA_REWARD_PLANET :
					{
						CPlanet
							*Planet = PLANET_TABLE->get_by_id(get_amount());
						if( Planet )
							Message.format( "Planet: %s", Planet->get_name() );
						else
							Message.format( "Planet: (null)" );
					}
					break;
				case EA_REWARD_DENIED :
				default :
					Message += GETTEXT("Denied");
					break;
			}
			break;
		case EA_ACTION_REQUEST_MEDIATION :
			if( get_target() ){
				Message += GETTEXT("Mediated");
			} else {
				Message += GETTEXT("Denied");
			}
			break;
		case ED_GRANT_RANK :
			Message.format( "%s/%d", aPlayer->get_court_rank_name(get_target()), get_amount() );
			break;

		case ED_DEMAND_DECLARE_WAR :
		case ED_DEMAND_MAKE_TRUCE :
			if( get_answer() == EA_ANSWER_YES ){
				Message = GETTEXT("Accepted");
			} else if( get_answer() == EA_ANSWER_NO ){
				Message = GETTEXT("Rejected");
			} else if( get_answer() == EA_ANSWER_WAITING ){
				if( get_target() ){
					CCouncil
						*Foe = COUNCIL_TABLE->get_by_id( get_amount() );
					if( Foe == NULL )
						Message = "-";
					else
						Message = Foe->get_nick();
				} else {
					CPlayer
						*Foe = PLAYER_TABLE->get_by_game_id( get_amount() );
					if( Foe == NULL )
						Message = "-";
					else
						Message = Foe->get_nick();
				}
			} else {
				Message = "-";
			}
			break;
		case ED_DEMAND_FREE_SUBORDINARY_COUNCIL :
			{
				CCouncil
					*Foe = COUNCIL_TABLE->get_by_id( get_amount() );
				if( Foe == NULL )
					Message = "-";
				else
					Message = Foe->get_nick();
			}
			break;
		case EA_ACTION_INVADE_MAGISTRATE_DOMINION :
		case EA_ACTION_INVADE_EMPIRE :

		case ED_DEMAND_LEAVE_COUNCIL :
		case ED_DEMAND_ARMAMENT_REDUCTION :
		default :
			Message = "-";
			break;
	}

	return (char*)Message;
}

bool
CEmpireAction::set_answer(int aAnswer)
{
	mAnswer = aAnswer;
	mStoreFlag += STORE_ANSWER;

	return true;
}

const char *
CEmpireAction::get_answer_name()
{
	static CString
		Answer;
	Answer.clear();

	switch( mAnswer ){
		case EA_ANSWER_WAITING :
			Answer = GETTEXT("Waiting");
			break;
		case EA_ANSWER_YES :
			Answer = GETTEXT("Accepted");
			break;
		case EA_ANSWER_NO :
			Answer = GETTEXT("Rejected");
			break;
		case EA_ANSWER_COMPLETE :
		default :
			Answer = GETTEXT("Completed");
			break;
	}

	return (char*)Answer;
}

bool
CEmpireAction::set_time()
{
	mTime = time(0);
	mStoreFlag += STORE_TIME;

	return true;
}

const char *
CEmpireAction::get_time_str()
{
	static char str[100];
	struct tm result;

	localtime_r( &mTime, &result );
	sprintf( str, "%d/%d/%d", result.tm_mon+1, result.tm_mday, result.tm_year+1900 );

	return str;
}

void
CEmpireAction::save_new_action()
{
	type( QUERY_INSERT );
	STORE_CENTER->store(*this);
}

const char *
CEmpireAction::accept_demand( CPlayer *aPlayer )
{
	static CString
		Message;
	Message.clear();

	switch (get_action())
	{
		case ED_DEMAND_TRIBUTE :
			if (mAmount < 0)
			{
				SLOG("ERROR : mAmount is less than 0 in CEmpireAction::accept_demand(), mID = %d, mOwner = %d, mAction = %d", mID, mOwner, mAction);
				break;
			}

			if (aPlayer->get_production() > mAmount)
			{
				set_answer( EA_ANSWER_YES );
				aPlayer->change_reserved_production( -get_amount() );
				aPlayer->change_honor_with_news( -1 );
				aPlayer->change_empire_relation( 7 );
				Message.format(
						GETTEXT("Dear %1$s,<BR>"
								"This missive is an official receipt of the Treasury having received your tax payment of %2$s PP on time.<BR>"
								"<BR>"
								"Lord Treasurer Constatiream,<BR>"
								"Imperial Treasurer"),
						aPlayer->get_nick(),
						dec2unit(get_amount()));
				Message += "<BR>\n";
			}
			else
			{
				Message.format(GETTEXT("You don't have %1$d PP to pay."),
								get_amount());
			}
			break;

		case ED_DEMAND_LEAVE_COUNCIL :
			Message.format(
					GETTEXT("Dear %1$s,<BR>"
							"This missive is to declare our pleasure that you have followed to our demands. If continue to prove your loyalty, your rewards will be mighty.<BR>"
							"<BR>"
							"His Supreme Emperor,<BR>"
							"Jo'Keric"),
					aPlayer->get_nick());
			Message += "<BR>\n";
			if (aPlayer->get_council()->get_number_of_members() > 1)
			{
				set_answer(EA_ANSWER_YES);
				CString
					News;
				CCouncil
					*Council = aPlayer->get_council();
				News.format( GETTEXT("%1$s declared independence and created %2$s's own council by the Empire demand."), aPlayer->get_nick(), aPlayer->get_nick() );
				aPlayer->independence_declaration( GETTEXT("Servant of Empire"), GETTEXT("Truthful Servant") );
				Council->send_time_news_to_all_member( (char*)News );
				aPlayer->change_honor_with_news( -10 );
				aPlayer->change_empire_relation( 15 );
			}
			break;
		case ED_DEMAND_DECLARE_WAR :
			Message.format(
					GETTEXT("Dear %1$s,<BR>"
							"This missive is to declare our pleasure that you have followed to our demands. If you continue to prove your loyalty, your rewards will be mighty.<BR>"
							"<BR>"
							"His Supreme Emperor,<BR>"
							"Jo'Keric"),
					aPlayer->get_nick());
			Message += "<BR>\n";
			if( get_target() ){
				CCouncil
					*Council = aPlayer->get_council(),
					*Foe = aPlayer->get_council()->relation_council(get_amount());
				if( Foe ){
					CCouncilRelation
						*Relation = Council->get_relation(Foe);
					if (Relation == NULL || !( Relation->get_relation() == CRelation::RELATION_WAR || Relation->get_relation() == CRelation::RELATION_TOTAL_WAR ))
					{
						set_answer(EA_ANSWER_YES);
						Council->declare_war(Foe);
						CString
							News;
						News.format( GETTEXT("By the demand of Empire, %1$s of %2$s declared war on your council."), aPlayer->get_nick(), Council->get_nick() );
						Foe->send_time_news_to_all_member( (char*)News );
						News.clear();
						News.format( GETTEXT("By the demand of Empire, %1$s declared war on %2$s."), aPlayer->get_nick(), Foe->get_nick() );
						Council->send_time_news_to_all_member( (char*)News );
						Council->change_honor_with_news( -5 );
						aPlayer->change_honor_with_news( -5 );
						aPlayer->change_empire_relation( 10 );
					}
				}
			} else {
				CPlayer
					*Foe = aPlayer->get_council()->get_member_by_game_id(get_amount());
				if( Foe ){
					CPlayerRelation
						*Relation = aPlayer->get_relation(Foe);
					if( Relation == NULL || Relation->get_relation() != CRelation::RELATION_WAR ){
						set_answer(EA_ANSWER_YES);
						aPlayer->declare_war(Foe);
						CString
							News;
						News.format( GETTEXT("By the demand of Empire, %1$s declared war on you."), aPlayer->get_nick() );
						Foe->time_news( (char*)News );
						News.clear();
						News.format( GETTEXT("By the demand of Empire, you declared war on %1$s."), Foe->get_nick() );
						Foe->time_news( (char*)News );
						aPlayer->change_honor_with_news( -5 );
						aPlayer->change_empire_relation( 10 );
					}
				}
			}
			break;
		case ED_DEMAND_MAKE_TRUCE :
			Message.format(
					GETTEXT("Dear %1$s,<BR>"
							"This missive is to declare our pleasure that you have followed to our demands. If you continue to prove your loyalty, your rewards will be mighty.<BR>"
							"<BR>"
							"His Supreme Emperor,<BR>"
							"Jo'Keric"),
					aPlayer->get_nick());
			Message += "<BR>\n";
			aPlayer->change_empire_relation( 5 );
			aPlayer->change_honor_with_news( -1 );
			if( get_target() )
            {
				CCouncil
					*Council = aPlayer->get_council(),
					*Foe = aPlayer->get_council()->relation_council(get_amount());
				if( Foe )
                {
					CCouncilRelation
						*Relation = Council->get_relation(Foe);
					if( Relation && ( Relation->get_relation() == CRelation::RELATION_WAR || Relation->get_relation() == CRelation::RELATION_TOTAL_WAR ) )
                    {
						Council->improve_relationship( CCouncilMessage::TYPE_SUGGEST_TRUCE, Foe );
						CString
							News;
						News.format( GETTEXT("By the demand of Empire, %1$s signed the truce with you."), Council->get_nick() );
						Foe->send_time_news_to_all_member( (char*)News );
						News.clear();
						News.format( GETTEXT("By the demand of Empire, %1$s signed the truce with %2$s."), aPlayer->get_nick(), Foe->get_nick() );
						Council->send_time_news_to_all_member( (char*)News );
					}
				}
			}
            else
            {
				CPlayer
					*Foe = aPlayer->get_council()->get_member_by_game_id(get_amount());
				if( Foe )
                {
					CPlayerRelation
						*Relation = aPlayer->get_relation(Foe);
					if( Relation && Relation->get_relation() == CRelation::RELATION_WAR )
                    {
						aPlayer->improve_relationship( CDiplomaticMessage::TYPE_SUGGEST_TRUCE, Foe );
						CString
							News;
						News.format( GETTEXT("By the demand of Empire, %1$s signed the truth with you."), aPlayer->get_nick() );
						Foe->time_news( (char*)News );
					}
				}
			}
			set_answer(EA_ANSWER_YES); // no matter what set it to yes in case a player CANT truce. TODO: make a better solution
			break;
		case ED_DEMAND_FREE_SUBORDINARY_COUNCIL :
			Message.format(
					GETTEXT("Dear %1$s,<BR>"
							"This missive is to declare our pleasure that you have followed to our demands. If you continue to prove your loyalty, your rewards will be mighty.<BR>"
							"<BR>"
							"His Supreme Emperor,<BR>"
							"Jo'Keric"),
					aPlayer->get_nick());
			Message += "<BR>\n";
			{
				CCouncil
					*Council = aPlayer->get_council(),
					*Foe = aPlayer->get_council()->relation_council(get_amount());
				if( Foe ){
					CCouncilRelation
						*Relation = Council->get_relation(Foe);
					if( Relation && Relation->get_relation() == CRelation::RELATION_SUBORDINARY && Relation->get_council1() == Council ){
						set_answer(EA_ANSWER_YES);
						Relation->set_relation(CRelation::RELATION_PEACE);
						Relation->type(QUERY_UPDATE);
						STORE_CENTER->store(*Relation);
						CString
							News;
						News.format( GETTEXT("%1$s of %2$s set free your council by Empire Demand."), aPlayer->get_nick(), Council->get_nick() );
						Foe->send_time_news_to_all_member( (char*)News );
						News.clear();
						News.format( GETTEXT("%1$s of your council set free your subordinary %2$s by Empire Demand."), aPlayer->get_nick(), Foe->get_nick() );
						Council->send_time_news_to_all_member( (char*)News );
					}
				}
			}
			break;
		case ED_DEMAND_ARMAMENT_REDUCTION :
			Message.format(
					GETTEXT("Dear %1$s,<BR>"
							"This missive is to declare our pleasure that you have followed to our demands. If you continue to prove your loyalty, your rewards will be mighty.<BR>"
							"<BR>"
							"His Supreme Emperor,<BR>"
							"Jo'Keric"),
					aPlayer->get_nick());
			Message += "<BR>\n";
			set_answer(EA_ANSWER_YES);
			for (int i=aPlayer->get_dock()->length()-1 ; i>= 0 ; i--)
			{
				CDockedShip *
					Ship = (CDockedShip *)(aPlayer->get_dock()->get(i));
				int
					DestructNum = 1+Ship->get_number()*get_amount()/100;
				CShipDesign
					*Design = aPlayer->get_ship_design_list()->get_by_id(Ship->get_design_id());
				aPlayer->get_dock()->change_ship( Design, -DestructNum );
			}
			for (int i=0 ; i<aPlayer->get_fleet_list()->length() ; i++)
			{
				CFleet
					*Fleet = (CFleet*)(aPlayer->get_fleet_list()->get(i));
				int
					DestructNum = Fleet->get_current_ship()*get_amount()/100;
				if (DestructNum == Fleet->get_current_ship()) DestructNum--;
				if (DestructNum < 0) DestructNum = 0;

				Fleet->set_current_ship( Fleet->get_current_ship()-DestructNum );
			}
			break;
		case ED_GRANT_RANK :
			if (mAmount < 0)
			{
				SLOG("ERROR : mAmount is less than 0 in CEmpireAction::accept_demand(), mID = %d, mOwner = %d, mAction = %d", mID, mOwner, mAction);
				break;
			}

			if (aPlayer->get_production() > mAmount)
			{
				set_answer( EA_ANSWER_YES );
				aPlayer->change_reserved_production(-mAmount);
				aPlayer->set_court_rank( get_target() );
				aPlayer->change_empire_relation( 10 );
				Message.format(
						GETTEXT("Welcome to the Ranks of Nobility, %1$s!!!<BR>"
								"Now, your home planet has been named after you in order to commemorate this grand occasion.<BR>"
								"All hail Noble %2$s!!!"),
						aPlayer->get_nick(),
						aPlayer->get_nick());
				Message += "<BR>\n";
				CPlanet *
					HomePlanet = (CPlanet *)(aPlayer->get_planet_list()->get(0));
				HomePlanet->set_name( aPlayer->get_name() );
			}
			else
			{
				Message.format(GETTEXT("You don't have %1$d PP to pay."),
								get_amount() );
			}
			break;
	}

	return (char*)Message;
}

const char *
CEmpireAction::reject_demand( CPlayer *aPlayer )
{
	static CString
		Message;
	Message.clear();

	set_answer( EA_ANSWER_NO );
	switch( get_action() ){
		case ED_DEMAND_TRIBUTE :
			Message = GETTEXT("This missive is to declare our extreme displeasure that you have failed to pay the tax that is required of all Imperial inhabitants. If such unsubject-like behavior continues, you shall incur our wrath.");
			aPlayer->change_empire_relation(-10);
			break;
		case ED_DEMAND_LEAVE_COUNCIL :
		case ED_DEMAND_DECLARE_WAR :
		case ED_DEMAND_MAKE_TRUCE :
		case ED_DEMAND_FREE_SUBORDINARY_COUNCIL :
		case ED_DEMAND_ARMAMENT_REDUCTION :
			Message.format(
					GETTEXT("Dear %1$s,<BR>"
							"This missive is to declare our extreme displeasure that you rejected the Empire Request. If such unsubject-like behavior continues, you shall incur our wrath.<BR>"
							"Consider yourself warned.<BR>"
							"<BR>"
							"His Supreme Emperor<BR>"
							"Jo'Keric"),
					aPlayer->get_nick());
			aPlayer->change_empire_relation(-10);
			break;
		case ED_GRANT_RANK :
			Message = GETTEXT("The Empire wondered why you would think yourself not to be important to deserve the title. You denied Empire Request.");
			aPlayer->change_empire_relation(-10);
			break;
	}

// there can be empire invasion - later
	return (char*)Message;
}

CEmpireActionList::CEmpireActionList()
{
}

CEmpireActionList::~CEmpireActionList()
{
	remove_all();
}

bool
CEmpireActionList::free_item(TSomething aItem)
{
	CEmpireAction
		*Action = (CEmpireAction *)aItem;

	if (!Action) return false;

	delete Action;

	return true;
}

int
CEmpireActionList::compare(TSomething aItem1, TSomething aItem2) const
{
	CEmpireAction
		*Item1 = (CEmpireAction*)aItem1,
		*Item2 = (CEmpireAction*)aItem2;

	if (Item1->get_id() > Item2->get_id()) return 1;
	if (Item1->get_id() < Item2->get_id()) return -1;
	return 0;
}

int
CEmpireActionList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CEmpireAction
		*Item = (CEmpireAction *)aItem;

	if (Item->get_id() > (int)aKey) return 1;
	if (Item->get_id() < (int)aKey) return -1;
	return 0;
}

CEmpireAction *
CEmpireActionList::get_by_id( int aID )
{
	if( length() == 0 ) return NULL;

	int
		index = find_sorted_key( (void*)aID );

	if( index < 0 ) return NULL;

	return (CEmpireAction*)get(index);
}

bool
CEmpireActionList::add_empire_action(CEmpireAction *aEmpireAction)
{
	if (!aEmpireAction) return false;

	insert_sorted(aEmpireAction);

	return true;
}

bool
CEmpireActionList::remove_empire_action(CEmpireAction *aEmpireAction)
{
	if (!aEmpireAction) return false;

	int
		index = find_sorted_key( (void*)(aEmpireAction->get_id()) );
	if( index < 0 ) return false;

	return CSortedList::remove(index);
}

void
CEmpireActionList::flush_db()
{
	time_t
		now = time(0);
	const int
		one_week = 7*24*60*60;

	for( int i = length()-1; i >= 0; i-- ){
		CEmpireAction
			*Action = (CEmpireAction*)get(i);

		if( Action->get_time() < now-one_week ){
			Action->type(QUERY_DELETE);
			STORE_CENTER->store(*Action);
			remove_empire_action(Action);
		} else if( Action->is_changed() ){
			Action->type(QUERY_UPDATE);
			STORE_CENTER->store(*Action);
		}
	}
}

int
CEmpireActionList::get_new_id()
{
	if( length() == 0 ) return 1;

	return ((CEmpireAction*)get(length()-1))->get_id()+1;
}
