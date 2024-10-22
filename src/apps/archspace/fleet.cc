#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <math.h>

#include "common.h"
#include "util.h"
#include "fleet.h"
#include "component.h"
#include "game.h"
#include "archspace.h"
#include "race.h"

TZone gFleetZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CFleet),
	0,
	0,
	NULL,
	"Zone CFleet"
};

TZone gFleetEffectZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CFleetEffect),
	0,
	0,
	NULL,
	"Zone CFleetEffect"
};

//---------------------------------------------------------- CMission

time_t CMission::mTrainMissionTime = 24*900;
time_t CMission::mPatrolMissionTime = 96*900;
time_t CMission::mDispatchToAllyMissionTime = 288*900;
time_t CMission::mExpeditionMissionTime = 24*900;
time_t CMission::mReturningWithPlanetMissionTime = 24*900;
int CMission::mAdmiralExpRaid = 0;
int CMission::mAdmiralExpRaidMultiplier = 20;
int CMission::mAdmiralExpPatrol = 10;
int CMission::mAdmiralExpStation = 5;
int CMission::mAdmiralExpExpedition = 1000;
int CMission::mAdmiralExpBattle = 0;
int CMission::mAdmiralExpDetect = 0;
int CMission::mTrainMaxLevel = 13;
int CMission::mFleetExpTrain = 2;
int CMission::mAdmiralExpTrain = 21;
int CMission::mAdmiralExpMinPrivateer = 0;
int CMission::mAdmiralExpMaxPrivateer = 1000;
int CMission::mMaxPrivateerCapacity = 70;

CMission::CMission()
{
	mMission = CMission::MISSION_NONE;
	mTarget = 0;
	mTerminateTime = 0;
}

CMission::~CMission()
{
}

CMission::EMissionType
	CMission::get_mission()
{
	return mMission;
}

time_t
	CMission::get_terminate_time()
{
	return mTerminateTime;
}



const char*
	CMission::get_mission_name(CMission::EMissionType aMission)
{
	switch(aMission)
	{
		case MISSION_NONE:
			return GETTEXT("None");
		case MISSION_TRAIN:
			return GETTEXT("Train");
		case MISSION_STATION_ON_PLANET:
			return GETTEXT("Station on Planet");
		case MISSION_PATROL:
			return GETTEXT("Patrol");
		case MISSION_EXPEDITION:
			return GETTEXT("Expedition");
		case MISSION_DISPATCH_TO_ALLY:
			return GETTEXT("Dispatched to Ally");
		case MISSION_RETURNING_WITH_PLANET :
			return GETTEXT("Returning with Planet");
		case MISSION_SORTIE:
			return GETTEXT("Sortie");
		case MISSION_RETURNING:
			return GETTEXT("Recharging Engines");
		case MISSION_PRIVATEER:
			return GETTEXT("Privateer");
	}
	return NULL;
}

const char*
	CMission::get_mission_name()
{
	return get_mission_name(mMission);
}

void
	CMission::set_mission(CMission::EMissionType aMission)
{
	mMission = aMission;

	switch(mMission)
	{
		case MISSION_TRAIN:
			mTerminateTime = CGame::get_game_time();
			break;
		case MISSION_PATROL:
			mTerminateTime = CGame::get_game_time() + mPatrolMissionTime;
			break;
		case MISSION_DISPATCH_TO_ALLY:
			mTerminateTime = CGame::get_game_time() + mDispatchToAllyMissionTime;
			break;
		case MISSION_EXPEDITION:
			if (CGame::mUpdateTurn)
			{
				mTerminateTime = CGame::get_game_time() + mExpeditionMissionTime;
			}
			else
			{
				mTerminateTime = mExpeditionMissionTime;
			}
			break;
		case MISSION_SORTIE: // spend one turn
			mTerminateTime = CGame::get_game_time()+1*CGame::mSecondPerTurn;
			break;
		case MISSION_RETURNING:
		case MISSION_NONE:
		case MISSION_STATION_ON_PLANET:
			mTerminateTime = CGame::get_game_time();
			break;
		case MISSION_RETURNING_WITH_PLANET :
			mTerminateTime = CGame::get_game_time() + mReturningWithPlanetMissionTime;
			break;
		case MISSION_PRIVATEER:
			mTerminateTime = CGame::get_game_time()
				+ TURNS_PER_6HOURS*CGame::mSecondPerTurn;
			break;
	}
}

void
	CMission::set_target(int aTarget)
{
	mTarget = aTarget;
}

void
	CMission::set_terminate_time(time_t aTime)
{
	mTerminateTime = aTime;
}

bool
	CMission::is_over()
{
	switch(mMission)
	{


		case MISSION_PATROL:
		case MISSION_DISPATCH_TO_ALLY:
		case MISSION_SORTIE:
		case MISSION_RETURNING:
		case MISSION_PRIVATEER:
		case MISSION_EXPEDITION:
			if (mTerminateTime < CGame::get_game_time()) return true;
			break;
		case MISSION_TRAIN:
		case MISSION_NONE:
		case MISSION_STATION_ON_PLANET:
			return false;
			break;
		case MISSION_RETURNING_WITH_PLANET :
			if (mTerminateTime < CGame::get_game_time()) return true;
	}

	return false;
}

char *
	CMission::select_mission_html()
{
	static CString
		Mission;
	Mission.clear();

	Mission = "<TD CLASS=\"tabletxt\" ALIGN=left>\n";

	Mission.format("<INPUT TYPE=radio NAME=\"MISSION\" VALUE=\"%d\">%s<BR>\n",
		MISSION_TRAIN, get_mission_name(MISSION_TRAIN));

	Mission.format("<INPUT TYPE=radio NAME=\"MISSION\" VALUE=\"%d\">%s<BR>\n",
		MISSION_STATION_ON_PLANET, get_mission_name(MISSION_STATION_ON_PLANET));

	Mission.format("<INPUT TYPE=radio NAME=\"MISSION\" VALUE=\"%d\">%s<BR>\n",
		MISSION_PATROL, get_mission_name(MISSION_PATROL));

	Mission.format("<INPUT TYPE=radio NAME=\"MISSION\" VALUE=\"%d\">%s<BR>\n",
		MISSION_DISPATCH_TO_ALLY, get_mission_name(MISSION_DISPATCH_TO_ALLY));

	Mission += "</TD>\n";

	return (char *)Mission;
}

const char*
	CMission::get_query()
{
	static CString Query;
	Query.clear();

	Query.format(", mission = %d, mission_target = %d, mission_terminate_time = %d",
		(int)mMission, mTarget, mTerminateTime);

	return (char*)Query;
}

//-------------------------------------------------------- CFleet

CFleet::CFleet()
{
	mKilledShip = mKilledFleet = mExp = 0;
	mStatus = CFleet::FLEET_STAND_BY;
}

CFleet::CFleet(CShipDesignList *aList, MYSQL_ROW aRow)
{
	CShipDesign::set_owner(atoi(aRow[STORE_OWNER]));
	mID = atoi(aRow[STORE_ID]);
	mName = aRow[STORE_NAME];
	mAdmiralID = atoi(aRow[STORE_ADMIRAL]);
	mExp = atoi(aRow[STORE_EXP]);
	mStatus = (EFleetStatus)atoi(aRow[STORE_STATUS]);
	mMaxShip = atoi(aRow[STORE_MAX_SHIP]);
	mCurrentShip = atoi(aRow[STORE_CURRENT_SHIP]);

	CShipDesign *Design = aList->get_by_id(atoi(aRow[STORE_SHIP_CLASS]));
	if (Design == NULL)
	{
		system_log("ERROR : Wrong design ID %s in CFleet %d(Owner ID : %d)",
			aRow[STORE_SHIP_CLASS], mID, get_owner() );
	}
	else
		set_ship_class(Design);

	mMission.set_mission((CMission::EMissionType)atoi(aRow[STORE_MISSION]));
	mMission.set_target(atoi(aRow[STORE_MISSION_TARGET]));
	mMission.set_terminate_time(atoi(aRow[STORE_MISSION_TERMINATE_TIME]));
	//mMission.set_terminate_time(atoi(aRow[STORE_MISSION_TERMINATE_TIME]) + CGameStatus::get_down_time());
	mStoreFlag += STORE_MISSION_TERMINATE_TIME;
	type(QUERY_UPDATE);
	STORE_CENTER->store(*this);


	mKilledShip = atoi(aRow[STORE_KILLED_SHIP]);
	mKilledFleet = atoi(aRow[STORE_KILLED_FLEET]);

	init_key();
}

int
	CFleet::get_id()
{
	return mID;
}

int
	CFleet::get_exp()
{
	return mExp;
}

int
	CFleet::get_status()
{
	return mStatus;
}

const char*
	CFleet::get_status_string(int aStatus)
{
	switch(aStatus)
	{
		case FLEET_STAND_BY: return GETTEXT("Stand-by");
		case FLEET_UNDER_OPERATION: return GETTEXT("Under Operation");
		case FLEET_UNDER_MISSION: return GETTEXT("Under Mission");
		case FLEET_DEACTIVATED: return GETTEXT("Deactivated");
		case FLEET_PRIVATEER: return GETTEXT("Privateer");
		case FLEET_COMMANDER_ABSENT: return GETTEXT("Commander Absent");
	}
	return NULL;
};

const char*
	CFleet::get_status_string()
{
	return get_status_string(mStatus);
}

int
	CFleet::get_max_ship()
{
	return mMaxShip;
}

int
	CFleet::get_current_ship()
{
	return mCurrentShip;
}

int
	CFleet::get_ship_class()
{
	return get_design_id();
}

int
	CFleet::get_killed_ship()
{
	return mKilledShip;
}

int
	CFleet::get_killed_fleet()
{
	return mKilledFleet;
}

CMission&
CFleet::get_mission()
{
	return mMission;
}

void
	CFleet::set_owner(int aOwner)
{
	CShipDesign::set_owner(aOwner);
	mStoreFlag += STORE_OWNER;

	init_key();
}

void
	CFleet::set_id(int aID)
{
	mID = aID;
	mStoreFlag += STORE_ID;

	init_key();
}

void
	CFleet::set_name(const char *aName)
{
	mName = aName;
	mStoreFlag += STORE_NAME;
}

void
	CFleet::set_admiral(int aAdmiral)
{
	mAdmiralID = aAdmiral;
	mStoreFlag += STORE_ADMIRAL;
}

void
	CFleet::set_exp(int aExp)
{
	mExp = aExp;
	mStoreFlag += STORE_EXP;
}

void
	CFleet::set_status(EFleetStatus aStatus)
{
	mStatus = aStatus;
	mStoreFlag += STORE_STATUS;
}

void
	CFleet::set_max_ship(int aNo)
{
	mMaxShip = aNo;
	mStoreFlag += STORE_MAX_SHIP;
}

void
	CFleet::set_current_ship(int aNo)
{
	if (aNo > mMaxShip) aNo = mMaxShip;
	if (aNo < 0) aNo = 0;

	mCurrentShip = aNo;
	mStoreFlag += STORE_CURRENT_SHIP;

	CPlayer *
	Owner = PLAYER_TABLE->get_by_game_id(CShipDesign::get_owner());
	if (Owner != NULL)
	{
		if (Owner->get_game_id() != EMPIRE_GAME_ID)
		{
			Owner->get_fleet_list()->refresh_power();
		}
	}
}

void
	CFleet::set_mission(CMission::EMissionType aMission)
{
	mMission.set_mission(aMission);
	mStoreFlag += STORE_MISSION;
}

void
	CFleet::set_target(int aTarget)
{
	mMission.set_target(aTarget);
	mStoreFlag += STORE_MISSION_TARGET;
}

bool
	CFleet::under_mission()
{
	if (mStatus == FLEET_UNDER_MISSION)
		return true;
	else
		return false;
}

bool
	CFleet::is_changed()
{
	return !mStoreFlag.is_empty();
}

const char *
	CFleet::get_name()
{
	return (char *)mName;
}

const char *
	CFleet::get_nick()
{
	static CString Nick;
	Nick.clear();

	if (get_owner() != EMPIRE_GAME_ID)
	{
		switch (mID)
		{
			case 1:
				Nick.format(GETTEXT("1st %1$s fleet"), (char *)mName);
				break;
			case 2:
				Nick.format(GETTEXT("2nd %1$s fleet"), (char *)mName);
				break;
			case 3:
				Nick.format(GETTEXT("3rd %1$s fleet"), (char *)mName);
				break;
			default:
				Nick.format(GETTEXT("%1$sth %2$s fleet"), dec2unit(mID), (char *)mName);
				break;
		}
	}
	else
	{
		Nick = mName;
	}

	return (char *)Nick;
}

int
	CFleet::get_power()
{


	float TotalPower = 0;

	int
		TotalLevel = 0,
	TotalComponents = 0;

	CComponent
		*Component;

	Component = COMPONENT_TABLE->get_by_id( get_armor() );
	if (Component) TotalLevel += Component->get_level();
	Component = COMPONENT_TABLE->get_by_id( get_engine() );
	if (Component) TotalLevel += Component->get_level();
	Component = COMPONENT_TABLE->get_by_id( get_computer() );
	if (Component) TotalLevel += Component->get_level();
	Component = COMPONENT_TABLE->get_by_id( get_shield() );
	if (Component) TotalLevel += Component->get_level();

	TotalComponents += 4;

    // ---Change---//
    for (int i=0 ; i < WEAPON_MAX_NUMBER; i++) {
    // ---End Change---//
        Component = COMPONENT_TABLE->get_by_id( get_weapon(i) );
        if( Component == NULL ) continue;

        TotalLevel += Component->get_level();
        TotalComponents++;
    }
    // ---Change---//
    for (int i=0 ; i < DEVICE_MAX_NUMBER; i++) {
    // ---End Change---//
        Component = COMPONENT_TABLE->get_by_id( get_device(i) );
        if( Component == NULL ) continue;

        TotalLevel += 5;//+= Component->get_level();
        TotalComponents++;
    }
	CShipSize *
	Ship = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(get_body());
	//ERROR
	
	SLOG("SHIP SIZE : %d", (CShipSize *)SHIP_SIZE_TABLE->get_by_id(get_body()));
	TotalPower = (float)((float)Ship->get_space() / 100.0f) * (2.5f + ((float)(((float)TotalLevel / (float)TotalComponents)) / 2.0f));
	//SLOG("POWER LOGING: %d, %d",(int)TotalPower, Ship->get_space());
	//SLOG("%d * %d = %d", (int) TotalPower, (int) get_current_ship(), ((int)TotalPower * (int) get_current_ship()));
	return ((int)TotalPower * (int)get_current_ship());
}

bool
	CFleet::delay_mission(time_t aDelayedTime)
{
	if (mMission.get_mission() == CMission::MISSION_NONE) return false;

	mMission.set_terminate_time(mMission.get_terminate_time() + aDelayedTime);
	mStoreFlag += STORE_MISSION;

	return true;
}

void
	CFleet::change_exp( int aExp )
{
	mExp += aExp;
	if( mExp < 0 ) mExp = 0;
	if( mExp > 100 ) mExp = 100;

	mStoreFlag += STORE_EXP;
}

void
	CFleet::change_killed_ship( int aKill )
{
	mKilledShip += aKill;

	mStoreFlag += STORE_KILLED_SHIP;
}

void
	CFleet::change_killed_fleet( int aKill )
{
	mKilledFleet += aKill;

	mStoreFlag += STORE_KILLED_FLEET;
}

bool
	CFleet::init_mission(CMission::EMissionType aMission, int aTarget, time_t aCostTime )
{
	if (aMission != CMission::MISSION_PRIVATEER)
		set_status(FLEET_UNDER_MISSION);
	else
		set_status(FLEET_PRIVATEER);

	set_mission( aMission );
	set_target( aTarget );
	if (aCostTime > 0)
		mMission.set_terminate_time(CGame::get_game_time()+aCostTime);

	if (aMission == CMission::MISSION_DISPATCH_TO_ALLY)
	{
		CPlayer *
		TargetPlayer = PLAYER_TABLE->get_by_game_id(aTarget);

		if (!TargetPlayer) return false;

		CAllyFleetList *
		TargetAllyFleetList = TargetPlayer->get_ally_fleet_list();

		TargetAllyFleetList->add_fleet(this);
	}

	return true;
}

void
	CFleet::end_mission()
{
	if (mMission.get_mission() == CMission::MISSION_DISPATCH_TO_ALLY)
	{
		CPlayer *
		Ally = PLAYER_TABLE->get_by_game_id(mMission.get_target());
		if (Ally)
		{
			CAllyFleetList *
			AllyFleetList = Ally->get_ally_fleet_list();
			CPlayer *
			Owner = PLAYER_TABLE->get_by_game_id(get_owner());

			AllyFleetList->remove_fleet(Owner, mID);
			Ally->refresh_power();
		}
	}

	if (mStatus != FLEET_DEACTIVATED) set_status(FLEET_STAND_BY);

	set_mission(CMission::MISSION_NONE);
}

const char *
	CFleet::get_detailed_status_string()
{
	static CString
		Buf;
	Buf.clear();

	if (get_status() == FLEET_UNDER_MISSION)
	{
		switch(mMission.get_mission())
		{
			case CMission::MISSION_SORTIE:
				Buf = mMission.get_mission_name();
				break;
			case CMission::MISSION_RETURNING:
			case CMission::MISSION_EXPEDITION:
			case CMission::MISSION_RETURNING_WITH_PLANET:
				Buf.format("%s - %s (%d)",
					get_status_string(), mMission.get_mission_name(), ((mMission.get_terminate_time() - CGame::get_game_time())/CGame::mSecondPerTurn) + 1);
				break;
			case CMission::MISSION_PRIVATEER:
				Buf = get_status_string();
				break;
			case CMission::MISSION_STATION_ON_PLANET :
			case CMission::MISSION_PATROL:
				{
					CPlanet *
					Planet = PLANET_TABLE->get_by_id(mMission.get_target());
					if (Planet == NULL)
					{
						Buf = get_status_string();
					}
					else
					{
						Buf.format("%s %s",
							mMission.get_mission_name(),
							Planet->get_name());
					}
				}
				break;
			case CMission::MISSION_DISPATCH_TO_ALLY:
				{
					CPlayer *
					Ally = PLAYER_TABLE->get_by_game_id(mMission.get_target());
					if (Ally)
					{
						Buf.format("%s %s",
							mMission.get_mission_name(), Ally->get_nick());
					}
					else
					{
						Buf.format("%s - %s",
							get_status_string(), mMission.get_mission_name());
					}
				}
				break;
			default :
				Buf.format("%s - %s",
					get_status_string(), mMission.get_mission_name());
				break;
		}
	} else {
		Buf = get_status_string();
	}

	return (char*)Buf;
}

void
	CFleet::set_ship_class( CShipDesign *aClass )
{
	*(CShipDesign*)this = *aClass;
	mStoreFlag += STORE_SHIP_CLASS;
}

// calc upkeep by MP
int
	CFleet::calc_upkeep()
{
	double
		Upkeep;

	CShipSize *
		Body = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(get_body());

	Upkeep = get_current_ship() * Body->get_upkeep();

	return (int)Upkeep;
}

const char *
	CFleet::get_size_name()
{
	return SHIP_SIZE_TABLE->get_by_id(get_body())->get_name();
}

// Returns the HTML format & strings associated with SELECT_REASSIGNMENT
// On reassignment.as
char *CFleet::select_reassignment_html() {
    static CString Reassignment;
    Reassignment.clear();

    Reassignment = "<TD CLASS=\"tabletxt\" ALIGN=left>\n";

    Reassignment += "<INPUT TYPE=radio NAME=\"REASSIGNMENT\" VALUE=\"1\">";
    Reassignment += GETTEXT("Commander Change");
    Reassignment += "<BR>\n";

    Reassignment += "<INPUT TYPE=radio NAME=\"REASSIGNMENT\" VALUE=\"2\">";
    Reassignment += GETTEXT("Ship Reassignment");
    Reassignment += "<BR>\n";

    Reassignment += "<INPUT TYPE=radio NAME=\"REASSIGNMENT\" VALUE=\"3\">";
    Reassignment += GETTEXT("Remove/Add Ships");
    Reassignment += "<BR>\n";

    Reassignment += "<INPUT TYPE=radio NAME=\"REASSIGNMENT\" VALUE=\"4\">";
    Reassignment += GETTEXT("Refill Fleet(s)");
    Reassignment += "<BR>\n";

    Reassignment += "<INPUT TYPE=radio NAME=\"REASSIGNMENT\" VALUE=\"5\">";
    Reassignment += GETTEXT("Maximize Fleet(s)");
    Reassignment += "<BR>\n";

    Reassignment += "</TD>\n";

    return (char *)Reassignment;
}

bool
	CFleet::create_as_empire_fleet(int aFleetRating, int aTechLevel, int aSizeRating, CAdmiral *aAdmiral, int aOrder)
{
	int
		ShipSize = int(aFleetRating/100) + 1;
	if (ShipSize < 1) ShipSize = 1;
	if (ShipSize > 10) ShipSize = 10;

	if (aTechLevel < 1) aTechLevel = 1;
	if (aTechLevel > 6) aTechLevel = 6;
	
	if (aTechLevel == 6) {
        ShipSize = 10;
    }       

	if (aAdmiral == NULL) return false;

	if (aOrder < 0) aOrder = 0;
	if (aOrder > 19) aOrder = 19;

	CShipDesignList
	PerfectShipDesignList;

	int
		Attempt = 0,
	CurrentTechLevel = aTechLevel;
	while (1)
	{
		for (int i=0 ; i<EMPIRE_SHIP_DESIGN_TABLE->length() ; i++)
		{
			CShipDesign *
			ShipDesign = (CShipDesign *)EMPIRE_SHIP_DESIGN_TABLE->get(i);
			if (((ShipDesign->get_design_id()/100)%10) != CurrentTechLevel) continue;

			if (ShipDesign->get_size() == ShipSize)
			{
				PerfectShipDesignList.add_ship_design(ShipDesign);
			}
		}

		Attempt++;

		if (PerfectShipDesignList.length() > 0) break;

		if (aTechLevel >= CurrentTechLevel)
		{
			CurrentTechLevel = aTechLevel + (aTechLevel - CurrentTechLevel) + 1;
		}
		else
		{
			CurrentTechLevel = aTechLevel - (CurrentTechLevel - aTechLevel);
		}

		if (Attempt > 5)
		{
			SLOG("ERROR : No available ship design in CFleet::create_as_empire_fleet(), aFleetRating = %d, aTechLevel = %d",
				aFleetRating, aTechLevel);
			return false;
		}
	}

	int
		NumberOfPerfectDesign = PerfectShipDesignList.length();
	CShipDesign *
	PerfectShipDesign = (CShipDesign *)PerfectShipDesignList.get(number(NumberOfPerfectDesign) - 1);
	*(CShipDesign *)this = *PerfectShipDesign;

	mID = CEmpire::mMaxEmpireFleetID + 1;
	mAdmiralID = aAdmiral->get_id();

	switch (get_size())
	{
		case 1 :
		case 2 :
		case 3 :
		case 4 :
		case 5 :
		case 6 :
			mMaxShip = aAdmiral->get_fleet_commanding(); // * (aSizeRating/100);
			if (mMaxShip > EMPIRE_FLEET_MAX_SHIP) mMaxShip = EMPIRE_FLEET_MAX_SHIP;
			if (mMaxShip < 8) mMaxShip = 8;
			set_current_ship(mMaxShip);
			break;

		case 7 :
			mMaxShip = (aAdmiral->get_fleet_commanding());// * 75 * (aSizeRating/100) / 100;
			if (mMaxShip > EMPIRE_FLEET_MAX_SHIP) mMaxShip = EMPIRE_FLEET_MAX_SHIP;
			if (mMaxShip < 8) mMaxShip = 8;
			set_current_ship(mMaxShip);
			break;

		case 8 :
			mMaxShip = (aAdmiral->get_fleet_commanding());// * 66 * (aSizeRating/100) / 100;
			if (mMaxShip > EMPIRE_FLEET_MAX_SHIP) mMaxShip = EMPIRE_FLEET_MAX_SHIP;
			if (mMaxShip < 8) mMaxShip = 8;
			set_current_ship(mMaxShip);
			break;

		case 9 :
			mMaxShip = (aAdmiral->get_fleet_commanding());// * 50 * (aSizeRating/100) / 100;
			if (mMaxShip > EMPIRE_FLEET_MAX_SHIP) mMaxShip = EMPIRE_FLEET_MAX_SHIP;
			if (mMaxShip < 8) mMaxShip = 8;
			set_current_ship(mMaxShip);
			break;

		case 10 :
			mMaxShip = (aAdmiral->get_fleet_commanding());// * 20 * (aSizeRating/100) / 100;
			if (mMaxShip > EMPIRE_FLEET_MAX_SHIP) mMaxShip = EMPIRE_FLEET_MAX_SHIP;
			if (mMaxShip < 8) mMaxShip = 8;
			set_current_ship(mMaxShip);
			break;

		default :
			mMaxShip = (aAdmiral->get_fleet_commanding());// * 20 * (aSizeRating/100) / 100;
			if (mMaxShip > EMPIRE_FLEET_MAX_SHIP) mMaxShip = EMPIRE_FLEET_MAX_SHIP;
			if (mMaxShip < 8) mMaxShip = 8;
			set_current_ship(mMaxShip);
			SLOG("ERROR : Invalid ship size return from get_size() in CFleet::create_as_empire_fleet()");
			break;
	}

	if (aOrder == 0)
	{
		mName = GETTEXT("1st IGF Fleet");
	}
	else if (aOrder == 1)
	{
		mName = GETTEXT("2nd IGF Fleet");
	}
	else if (aOrder == 2)
	{
		mName = GETTEXT("3rd IGF Fleet");
	}
	else
	{
		mName.format(GETTEXT("%1$sth IGF Fleet"), dec2unit(aOrder + 1));
	}

	init_key();

	mExp = 10 + int(sqrt((double)aFleetRating)) + aAdmiral->get_level()*2 + number(30);
	if (mExp > 100) mExp = 100;

	aAdmiral->set_fleet_number(mID);

	PerfectShipDesignList.remove_without_free_all();

	CEmpire::update_max_empire_fleet_id(this);

	return true;
}

void
	CFleet::init_key()
{
	mKey = unsigned_int_to_char(get_owner());
	mKey += " ";
	mKey += unsigned_int_to_char(mID);
}

CString &
CFleet::query()
{
	static CString
		query;
	query.clear();

	switch (type())
	{
		case QUERY_INSERT:
			query.format("INSERT INTO fleet (owner, id, name, admiral, exp, status, maxship, currentship, shipclass, mission, mission_target, mission_terminate_time, killed_ship, killed_fleet) VALUES (%d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
				get_owner(),
				mID,
				(char*)add_slashes((char*)mName),
				mAdmiralID,
				mExp,
				mStatus,
				mMaxShip,
				mCurrentShip,
				get_ship_class(),
				mMission.get_mission(),
				mMission.get_target(),
				mMission.get_terminate_time(),
				mKilledShip,
				mKilledFleet);
			break;

		case QUERY_UPDATE:
			query.format("UPDATE fleet SET exp = %d", get_exp());

#define STORE(x, y, z) \
			if (mStoreFlag.has(x)) \
				query.format(y, z)

			STORE(STORE_NAME, ", name = '%s'", (char*)add_slashes(get_name()));
			STORE(STORE_ADMIRAL, ", admiral = %d", get_admiral_id());
			STORE(STORE_STATUS, ", status = %d", get_status());
			STORE(STORE_MAX_SHIP, ", maxship = %d", get_max_ship());
			STORE(STORE_CURRENT_SHIP, ", currentship = %d", get_current_ship());
			STORE(STORE_SHIP_CLASS, ", shipclass = %d", get_ship_class());
			STORE(STORE_KILLED_SHIP, ", killed_ship = %d", get_killed_ship());
			STORE(STORE_KILLED_FLEET, ", killed_fleet = %d", get_killed_fleet());
			if (mStoreFlag.has(STORE_MISSION)) query += mMission.get_query();

			query.format(" WHERE owner = %d AND id = %d", get_owner(), get_id());

			break;

		case QUERY_DELETE:
			query.format( "DELETE FROM fleet WHERE owner = %d AND id = %d", get_owner(), get_id() );
			break;
	}

	mStoreFlag.clear();

	return query;
}

CFleetList::CFleetList()
{
	mPower = 0;
	mIsVolatile = false;
}

CFleetList::~CFleetList()
{
	remove_all();
}

bool
	CFleetList::free_item( TSomething aItem )
{
	CFleet *
	fleet = (CFleet *)aItem;

	if (!fleet) return false;

	delete fleet;

	return true;
}

int
	CFleetList::compare(TSomething aItem1, TSomething aItem2) const
{
	CFleet
		*Fleet1 = (CFleet *)aItem1,
	*Fleet2 = (CFleet *)aItem2;

	if (strcmp(Fleet1->get_key(), Fleet2->get_key()) > 0) return 1;
	if (strcmp(Fleet1->get_key(), Fleet2->get_key()) < 0) return -1;
	return 0;
}

int
	CFleetList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CFleet *
	Fleet = (CFleet *)aItem;

	if (strcmp(Fleet->get_key(), (char *)aKey) > 0) return 1;
	if (strcmp(Fleet->get_key(), (char *)aKey) < 0) return -1;
	return 0;
}

bool
	CFleetList::remove_fleet(int aFleetID)
{
	int
		Index;
	CFleet *
	Fleet = get_by_id(aFleetID, &Index);

	if (!Fleet)
	{
		SLOG("Could not found remove fleet %d", aFleetID);
		return false;
	}

	if (CSortedList::remove(Index) == true)
	{
		refresh_power();
		return true;
	}

	return false;
}

bool
	CFleetList::remove_without_free_fleet(int aFleetID)
{
	int
		Index;
	CFleet *
	Fleet = get_by_id(aFleetID, &Index);

	if (!Fleet)
	{
		SLOG("Could not found remove fleet %u", aFleetID);
		return false;
	}

	if (remove_without_free(Index) == true)
	{
		refresh_power();
		return true;
	}

	return false;
}

bool
	CFleetList::add_fleet(CFleet *aFleet)
{
	if (!aFleet) return false;

	if (find_sorted_key( (TConstSomething)aFleet->get_key() ) >= 0) return false;
	insert_sorted( aFleet );

	refresh_power();

	return true;
}

CFleet *
CFleetList::get_by_id(int aFleetID, int *aIndex)
{
	if (!length()) return NULL;

	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)get(i);
		if (Fleet->get_id() == aFleetID)
		{
			if (aIndex) *aIndex = i;
			return Fleet;
		}
	}

	return NULL;
}

int
	CFleetList::get_new_fleet_id()
{
	int
		ID = 1;

	while (get_by_id(ID)) ID++;

	return ID;
}

bool
	CFleetList::is_id_available(int aID)
{
	if (!length()) return true;

	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)get(i);
		if (Fleet->get_id() == aID) return false;
	}

	return true;
}

int
	CFleetList::get_total_ship_number()
{
	int
		Total = 0;

	for (int i=0 ; i<length() ; i++)
	{
		CFleet
			*Fleet = (CFleet *)get(i);
		Total += Fleet->get_current_ship();
	}

	return Total;
}

int
	CFleetList::fleet_number_by_status(int aStatus)
{
	int
		Number = 0;

	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)get(i);

		if (Fleet->get_status() != aStatus) continue;
		Number++;
	}

	return Number;
}

CFleet *
CFleetList::get_fleet_station_on_planet(int aPlanetID)
{
	for( int i = 0; i < length(); i++ )
	{
		CFleet
			*Fleet = (CFleet*)get(i);

		CMission &Mission = Fleet->get_mission();

		if (Mission.get_mission() == CMission::MISSION_STATION_ON_PLANET ||
			Mission.get_mission() == CMission::MISSION_PATROL )
		{
			if (Mission.get_target() == aPlanetID)
				return Fleet;
		}
	}

	return NULL;
}

bool
	CFleetList::update_DB()
{
	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)get(i);
		if (Fleet->is_changed() == false) continue;

		Fleet->type(QUERY_UPDATE);
		STORE_CENTER->store(*Fleet);
	}

	return true;
}

bool
	CFleetList::refresh_power()
{
	int
		OwnerGameID = 0;

	mPower = 0;
	for(int i=0 ; i<length() ; i++)
	{
		CFleet*
		fleet = (CFleet*)get(i);
		if (!fleet)
			continue;
		int
			AvailablePower = MAX_POWER - mPower;
		if (fleet->get_power() > AvailablePower)
		{
			mPower = MAX_POWER;
			break;
		}
		else
		{
			mPower += fleet->get_power();
		}

		OwnerGameID = fleet->CShipDesign::get_owner();
	}

	CPlayer *
	Owner = PLAYER_TABLE->get_by_game_id(OwnerGameID);
	if (Owner == NULL)
	{
		SLOG("ERROR : Owner is NULL in CFleetList::refresh_power(), OwnerGameID = %d", OwnerGameID);
		if (length() > 0)
		{
			CFleet *Fleet = (CFleet *)get(length()-1);
			if (Fleet != NULL)
				SLOG("Last Fleets ID: %d", Fleet->get_id());
		}
	}
	else if (Owner->get_game_id() != EMPIRE_GAME_ID)
	{
		Owner->refresh_power();
	}

	return true;
}

char *
	CFleetList::all_fleet_list_html(CPlayer *aPlayer)
{
	static CString
		FleetList;
	FleetList.clear();

	if (!length())
	{
		FleetList = "<TABLE WIDTH=570 BORDER=0 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
		FleetList += "<TR>\n";
		FleetList += "<TD CLASS=tabletxt ALIGN=\"CENTER\">\n";
		FleetList += GETTEXT("You don't have any fleets.");
		FleetList += "</TD>\n";
		FleetList += "</TR>\n";
		FleetList += "</TABLE>\n";

		return (char *)FleetList;
	}

	FleetList = "<TABLE WIDTH=570 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	FleetList += "<TR BGCOLOR=#171717>\n";

	FleetList += "<TH CLASS=tabletxt WIDTH=29><FONT COLOR=666666>";
	FleetList += GETTEXT("ID");
	FleetList += "</FONT></TH>\n";

	FleetList += "<TH WIDTH=112 CLASS=tabletxt><FONT COLOR=666666>";
	FleetList += GETTEXT("Name");
	FleetList += "</FONT></TH>\n";

	FleetList += "<TH WIDTH=144 CLASS=tabletxt><FONT COLOR=666666>";
	FleetList += GETTEXT("Commander");
	FleetList += "</FONT></TH>\n";

	FleetList += "<TH WIDTH=36 CLASS=tabletxt><FONT COLOR=666666>";
	FleetList += GETTEXT("Exp.");
	FleetList += "</FONT></TH>\n";

	FleetList += "<TH WIDTH=64 CLASS=tabletxt><FONT COLOR=666666>";
	FleetList += GETTEXT("Status");
	FleetList += "</FONT></TH>\n";

	FleetList += "<TH WIDTH=128 CLASS=tabletxt><FONT COLOR=666666>";
	FleetList += GETTEXT("Ship Class");
	FleetList += "</FONT></TH>\n";

	FleetList += "<TH WIDTH=49 CLASS=tabletxt><FONT COLOR=666666>";
	FleetList += GETTEXT("Ships");
	FleetList += "</FONT></TH>\n";
	
	FleetList += "<TH WIDTH=49 CLASS=tabletxt><FONT COLOR=666666>";
	FleetList += GETTEXT("Commander Abilities");
	FleetList += "</FONT></TH>\n";

	FleetList += "</TR>\n";

	CAdmiralList *
	AdmiralList = aPlayer->get_admiral_list();

	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)get(i);

		CAdmiral *
		Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());

		FleetList += "<TR>\n";
		FleetList.format("<TD CLASS=tabletxt WIDTH=29 ALIGN=CENTER>%d</TD>\n",
			Fleet->get_id());

		FleetList.format("<TD WIDTH=112 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->get_name());
		FleetList += "<TD WIDTH=144 CLASS=tabletxt ALIGN=LEFT>";
		if( Admiral )
			FleetList.format("<A HREF=\"/archspace/fleet/fleet_commander_information.as?ADMIRAL_ID=%d\">%s</A>",
			Admiral->get_id(), Admiral->get_name());
		FleetList += "</TD>\n";

		FleetList.format("<TD WIDTH=36 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Fleet->get_exp());

		FleetList.format("<TD WIDTH=64 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->get_detailed_status_string());

		FleetList += "<TD WIDTH=128 CLASS=tabletxt ALIGN=LEFT>";
		FleetList.format("<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
			Fleet->get_design_id(), Fleet->CShipDesign::get_name());
		FleetList += "</TD>\n";

		FleetList.format("<TD WIDTH=49 CLASS=tabletxt ALIGN=CENTER>&nbsp;%d/%d (%d)&nbsp;</TD>\n",
			Fleet->get_current_ship(), Fleet->get_max_ship(), Admiral->get_fleet_commanding());
			
		FleetList.format("<TD WIDTH=49 CLASS=tabletxt ALIGN=CENTER>&nbsp;%s/%s&nbsp;</TD>\n",
			Admiral->get_special_ability_acronym(), Admiral->get_racial_ability_acronym());

		FleetList += "</TR>\n";
	}

	FleetList += "</TABLE>\n";

	return (char *)FleetList;
}

char *
	CFleetList::all_ally_fleet_list_html()
{
	static CString
		FleetList;
	FleetList.clear();

	if (!length())
	{
		FleetList = "<TABLE WIDTH=570 BORDER=0 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
		FleetList += "<TR>\n";
		FleetList += "<TD CLASS=tabletxt ALIGN=\"CENTER\">\n";
		FleetList += GETTEXT("You don't have any fleets from allies.");
		FleetList += "</TD>\n";
		FleetList += "</TR>\n";
		FleetList += "</TABLE>\n";

		return (char *)FleetList;
	}

	FleetList = "<TABLE WIDTH=570 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	FleetList += "<TR BGCOLOR=#171717>\n";
	
	FleetList += "<FORM NAME=\"allied\" ACTION=\"fleet.as\" METHOD=POST>\n";

	FleetList += "<TH CLASS=tabletxt WIDTH=29><FONT COLOR=666666>";
	FleetList += GETTEXT("ID");
	FleetList += "</FONT></TH>\n";

	FleetList += "<TH WIDTH=64 CLASS=tabletxt><FONT COLOR=666666>";
	FleetList += GETTEXT("Owner");
	FleetList += "</FONT></TH>\n";

	FleetList += "<TH WIDTH=112 CLASS=tabletxt><FONT COLOR=666666>";
	FleetList += GETTEXT("Name");
	FleetList += "</FONT></TH>\n";

	FleetList += "<TH WIDTH=144 CLASS=tabletxt><FONT COLOR=666666>";
	FleetList += GETTEXT("Commander");
	FleetList += "</FONT></TH>\n";

	FleetList += "<TH WIDTH=36 CLASS=tabletxt><FONT COLOR=666666>";
	FleetList += GETTEXT("Exp.");
	FleetList += "</FONT></TH>\n";

	FleetList += "<TH WIDTH=128 CLASS=tabletxt><FONT COLOR=666666>";
	FleetList += GETTEXT("Ship Class");
	FleetList += "</FONT></TH>\n";

	FleetList += "<TH WIDTH=49 CLASS=tabletxt><FONT COLOR=666666>";
	FleetList += GETTEXT("Ships");
	FleetList += "</FONT></TH>\n";
	
	FleetList += "<TH WIDTH=49 CLASS=tabletxt><FONT COLOR=666666>";
	FleetList += GETTEXT("Power");
	FleetList += "</FONT></TH>\n";
	
	FleetList += "<TH WIDTH=49 CLASS=tabletxt><FONT COLOR=666666>";
	FleetList += GETTEXT("Return to Sender");
	FleetList += "</FONT></TH>\n";

	FleetList += "</TR>\n";

	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)get(i);

		CPlayer *
		Ally = PLAYER_TABLE->get_by_game_id(Fleet->get_owner());
		if (!Ally) continue;

		CAdmiralList *
		AdmiralList = Ally->get_admiral_list();
		CAdmiral *
		Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());

		FleetList += "<TR>\n";
		FleetList.format("<TD CLASS=tabletxt WIDTH=29 ALIGN=CENTER>%d</TD>\n",
			Fleet->get_id());

		FleetList.format("<TD WIDTH=64 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Ally->get_nick());

		FleetList.format("<TD WIDTH=112 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->get_name());
		FleetList.format("<TD WIDTH=144 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Admiral->get_name());

		FleetList.format("<TD WIDTH=36 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Fleet->get_exp());

		FleetList.format("<TD WIDTH=128 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->CShipDesign::get_name());

		FleetList.format("<TD WIDTH=49 CLASS=tabletxt ALIGN=CENTER>&nbsp;%d/%d (%d)&nbsp;</TD>\n",
			Fleet->get_current_ship(), Fleet->get_max_ship(), Admiral->get_fleet_commanding());
			
		FleetList.format("<TD WIDTH=49 CLASS=tabletxt ALIGN=CENTER>&nbsp;%s&nbsp;</TD>\n",
			dec2unit(Fleet->get_power()));
		
		FleetList.format("<TD WIDTH=49 CLASS=tabletxt ALIGN=CENTER><INPUT NAME=\"ALLIED\" VALUE=FLEET%dOWNER%d TYPE=IMAGE SRC=\"http://magellanwars.com/image/as_game/bu_ok.gif\" BORDER=\"0\"></TD>\n", Fleet->get_id(), Fleet->get_owner());

		FleetList += "</TR>\n";
	}

	FleetList += "</FORM>\n";

	FleetList += "</TABLE>\n";

	return (char *)FleetList;
}

char *
	CFleetList::deployment_fleet_list_html(CPlayer *aPlayer)
{
	static CString
		List;
	List.clear();

	List = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	List += "<TR BGCOLOR=#171717>\n";

	List += "<TH CLASS=tabletxt WIDTH=24><FONT COLOR=666666>";
	List += "&nbsp;";
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=59 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Capital");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=169 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Fleet Name");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=157 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ship Class");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=129 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Defensive Skill");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=24 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Bonus");
	List += "</FONT></TH>\n";

	List += "</TR>\n";

	CAdmiralList *
	AdmiralList = aPlayer->get_admiral_list();
	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)get(i);
		CMission &
		Mission = Fleet->get_mission();
		if ((Fleet->get_status() != CFleet::FLEET_STAND_BY) && (Mission.get_mission() != CMission::MISSION_RETURNING) && (Mission.get_mission() != CMission::MISSION_SORTIE)) continue;

		CAdmiral *
		Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());

		List += "<TR>\n";

		List += "<TD CLASS=tabletxt WIDTH=24 ALIGN=CENTER>";
		List.format("<INPUT TYPE=\"checkbox\" NAME=\"FLEET%d\">", i);
		List += "</TD>\n";

		List += "<TD CLASS=tabletxt WIDTH=59 ALIGN=CENTER>";
		List.format("<INPUT TYPE=\"radio\" NAME=\"CAPITAL\" VALUE=\"%d\">",
			Fleet->get_id());
		List += "</TD>\n";

		List.format("<TD WIDTH=169 CLASS=tabletxt ALIGN=CENTER>%s</TD>\n",
			Fleet->get_nick());

		List.format("<TD WIDTH=157 CLASS=tabletxt ALIGN=CENTER>%s (%d)</TD>\n",
			Fleet->CShipDesign::get_name(), Fleet->get_current_ship());

		List.format("<TD WIDTH=129 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Admiral->get_overall_defense());

		List.format("<TD WIDTH=24 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Admiral->get_armada_commanding_effect(CAdmiral::DEFENSE));

		List += "</TR>\n";
	}

	List += "</TABLE>\n";

	return (char *)List;
}

char *
	CFleetList::possessed_fleet_info_html()
{
	static CString
		FleetList;
	FleetList.clear();

	FleetList += "<TABLE WIDTH=\"550\" BORDER=\"0\" CELLSPACING=\"0\""
		" CELLPADDING=\"0\">\n";
	FleetList += "<TR>\n";
	if (!length())
	{
		FleetList += "<TD WIDTH=\"550\" ALIGN=\"CENTER\" CLASS=\"maintext\">";
		FleetList += GETTEXT("You don't have any fleets.");
		FleetList += "</TD>\n";
		FleetList += "</TR>\n";
		FleetList += "</TABLE>\n";
		return (char *)FleetList;
	}

	FleetList += "<TD WIDTH=\"268\" ALIGN=\"LEFT\">\n";
	FleetList += "<TABLE WIDTH=\"268\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\""
		" BORDERCOLOR=\"#2A2A2A\">\n";
	FleetList += "<TR BGCOLOR=\"#171717\">\n";

	FleetList += "<TH CLASS=\"tabletxt\" WIDTH=\"35\" BGCOLOR=\"#171717\">";
	FleetList.format("<FONT COLOR=\"666666\">%s</FONT>", GETTEXT("ID"));
	FleetList += "</TH>\n";

	FleetList += "<TH CLASS=\"tabletxt\" WIDTH=\"105\" BGCOLOR=\"#171717\">";
	FleetList.format("<FONT COLOR=\"666666\">%s</FONT>", GETTEXT("Name"));
	FleetList += "</TH>\n";

	FleetList += "<TH CLASS=\"tabletxt\" WIDTH=\"118\" BGCOLOR=\"#171717\">";
	FleetList.format("<FONT COLOR=\"666666\">%s</FONT>\n", GETTEXT("Ship Class"));
	FleetList += "</TH>\n";

	FleetList += "</TR>\n";

	for (int i=0 ; i<length() ; i += 2)
	{
		CFleet *
		Fleet = (CFleet *)get(i);

		FleetList += "<TR>\n";

		FleetList.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" WIDTH=\"35\">%d</TD>\n",
			Fleet->get_id());

		FleetList.format("<TD CLASS=\"tabletxt\" ALIGN=\"LEFT\" WIDTH=\"105\">&nbsp;%s</TD>\n",
			Fleet->get_name());

		FleetList += "<TD CLASS=\"tabletxt\" ALIGN=\"LEFT\" WIDTH=\"118\">";
		FleetList.format("<A HREF=/archspace/fleet/class_specification.as?DESIGN_ID=%d>&nbsp;%s</A>",
			Fleet->get_design_id(), Fleet->CShipDesign::get_name());
		FleetList += "</TD>\n",

		FleetList += "</TR>\n";
	}

	FleetList += "</TABLE>\n";
	FleetList += "</TD>\n";

	FleetList += "<TD ROWSPAN=\"4\">&nbsp; </TD>\n";

	FleetList += "<TD WIDTH=\"268\" ALIGN=\"RIGHT\" VALIGN=\"TOP\">\n";

	FleetList += "<TABLE WIDTH=\"268\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\""
		" BORDERCOLOR=\"#2A2A2A\">\n";
	FleetList += "<TR BGCOLOR=\"#171717\">\n";

	FleetList += "<TH CLASS=\"tabletxt\" WIDTH=\"35\" BGCOLOR=\"#171717\">";
	FleetList.format("<FONT COLOR=\"666666\">%s</FONT>", GETTEXT("ID"));
	FleetList += "</TH>\n";

	FleetList += "<TH CLASS=\"tabletxt\" WIDTH=\"105\" BGCOLOR=\"#171717\">";
	FleetList.format("<FONT COLOR=\"666666\">%s</FONT>", GETTEXT("Name"));
	FleetList += "</TH>\n";

	FleetList += "<TH CLASS=\"tabletxt\" WIDTH=\"118\" BGCOLOR=\"#171717\">";
	FleetList.format("<FONT COLOR=\"666666\">%s</FONT>\n", GETTEXT("Ship Class"));
	FleetList += "</TH>\n";

	FleetList += "</TR>\n";

	for (int i=1 ; i<length() ; i += 2)
	{
		CFleet *
		Fleet = (CFleet *)get(i);

		FleetList += "<TR>\n";

		FleetList.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" WIDTH=\"35\">%d</TD>\n",
			Fleet->get_id());

		FleetList.format("<TD CLASS=\"tabletxt\" ALIGN=\"LEFT\" WIDTH=\"105\">&nbsp;%s</TD>\n",
			Fleet->get_name());

		FleetList += "<TD CLASS=\"tabletxt\" ALIGN=\"LEFT\" WIDTH=\"118\">";
		FleetList.format("<A HREF=/archspace/fleet/class_specification.as?DESIGN_ID=%d>&nbsp;%s</A>",
			Fleet->get_design_id(), Fleet->CShipDesign::get_name());
		FleetList += "</TD>\n",

		FleetList += "</TR>\n";
	}

	FleetList += "</TABLE>\n";
	FleetList += "</TD>\n";
	FleetList += "</TR>\n";
	FleetList += "</TABLE>\n";

	return (char *)FleetList;
}

char *
	CFleetList::recall_fleet_list_html(CPlayer *aPlayer)
{
	static CString
		List;
	List.clear();

	if (length() == 0)
	{
		List = GETTEXT("You don't have any fleets.");
		return (char *)List;
	}

	List = "<TABLE WIDTH=580 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n"
	;
	List += "<TR BGCOLOR=#171717>\n";

	List += "<TH CLASS=tabletxt WIDTH=35><FONT COLOR=666666>";
	List += GETTEXT("ID");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=25 CLASS=tabletxt><FONT COLOR=666666>";
	List += "<INPUT TYPE=checkbox onClick=allCheck()>";
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=104 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Name");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=142 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Commander");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=53 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Exp.");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=88 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Where");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=117 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Design");
	List += "</FONT></TH>\n";
	
	List += "<TH WIDTH=117 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ship");
	List += "</FONT></TH>\n";
	
	List += "<TH WIDTH=49 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Commander Abilities");
	List += "</FONT></TH>\n";

	List += "</TR>\n";

	CAdmiralList *
	AdmiralList = aPlayer->get_admiral_list();

	for(int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)get(i);

		CMission &
		Mission = Fleet->get_mission();
		if (Mission.get_mission() != CMission::MISSION_STATION_ON_PLANET &&
			Mission.get_mission() != CMission::MISSION_DISPATCH_TO_ALLY &&
			Mission.get_mission() != CMission::MISSION_EXPEDITION &&
			Mission.get_mission() != CMission::MISSION_TRAIN) continue;

		CAdmiral *
		Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());

		List += "<TR>\n";

		List.format("<TD CLASS=tabletxt WIDTH=35 ALIGN=CENTER>%d</TD>\n",
			Fleet->get_id());

		List += "<TD WIDTH=25 CLASS=tabletxt ALIGN=CENTER>\n";
		List.format("<INPUT TYPE=checkbox NAME=FLEET%d>\n",
			i);
		List += "</TD>\n";

		List.format("<TD WIDTH=104 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->get_name());

		List += "<TD WIDTH=142 CLASS=tabletxt ALIGN=LEFT>";
		if( Admiral )
			List.format("<A HREF=\"/archspace/fleet/fleet_commander_information.as?ADMIRAL_ID=%d\">%s</A>",
			Admiral->get_id(), Admiral->get_name());
		List += "</TD>\n";

		List.format("<TD WIDTH=53 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Fleet->get_exp());

		List.format("<TD WIDTH=88 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->get_detailed_status_string());
		List += "<TD WIDTH=117 CLASS=tabletxt ALIGN=LEFT>";

		List.format("<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
			Fleet->get_design_id(), Fleet->CShipDesign::get_name());
		//List.format("&nbsp(%d)", Fleet->get_current_ship());
		List += "</TD>\n";
		
		List.format("<TD WIDTH=49 CLASS=tabletxt ALIGN=CENTER>&nbsp;%d/%d (%d)&nbsp;</TD>\n",
			Fleet->get_current_ship(), Fleet->get_max_ship(), Admiral->get_fleet_commanding());
		
		List.format("<TD WIDTH=49 CLASS=tabletxt ALIGN=CENTER>&nbsp;%s/%s&nbsp;</TD>\n",
			Admiral->get_special_ability_acronym(), Admiral->get_racial_ability_acronym());

		List += "</TR>\n";
	}

	List += "</TABLE>\n";

	return (char *)List;
}

/*
char *
	CFleetList::reassignment_fleet_list_html()
{
	static CString
		Info;
	Info.clear();

	if (!length())
	{
		Info = GETTEXT("You have no fleets created currently.");
		return (char *)Info;
	}

	Info += "\t<div id=\"ReassignmentFleetList\"><!-- Reassignment fleet list info !--></div>\n";
	return (char *)Info;
}   */

char *
	CFleetList::reassignment_fleet_list_html(CPlayer *aPlayer)
{
	static CString
		List;
	List.clear();

	if (!length())
	{
		List = GETTEXT("You don't have any fleets.");
		return (char *)List;
	}

	List = "<TABLE WIDTH=570 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	List += "<TR BGCOLOR=#171717>\n";
	List += "<TH CLASS=tabletxt WIDTH=29><FONT COLOR=666666>";
	List += GETTEXT("ID");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=29 CLASS=tabletxt><FONT COLOR=666666>";
	List += "<INPUT TYPE=checkbox onClick=allCheck()>";
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=112 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Name");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=144 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Commander");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=36 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Exp.");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=64 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Status");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=99 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ship Class");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=49 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ships");
	List += "</FONT></TH>\n";
	
	List += "<TH WIDTH=49 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Commander Abilities");
	List += "</FONT></TH>\n";

	List += "</TR>\n";

	CAdmiralList *
	AdmiralList = aPlayer->get_admiral_list();
	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet* )get(i);
		if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

		CAdmiral *
		Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());

		List += "<TR> \n";
		List.format("<TD CLASS=tabletxt WIDTH=29 ALIGN=CENTER>%d</TD>\n",
			Fleet->get_id());

		List += "<TD WIDTH=29 CLASS=tabletxt ALIGN=CENTER>\n";
		List.format("<INPUT TYPE=CHECKBOX NAME=FLEET%d>\n", i);
		List += "</TD>\n";

		List.format("<TD WIDTH=112 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->get_name());

		List += "<TD WIDTH=144 CLASS=tabletxt ALIGN=LEFT>";
		List.format("<A HREF=\"/archspace/fleet/fleet_commander_information.as?ADMIRAL_ID=%d\">%s</A>",
			Admiral->get_id(), Admiral->get_name());
		List += "</TD>\n";

		List.format("<TD WIDTH=36 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Fleet->get_exp());

		List.format("<TD WIDTH=64 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->get_detailed_status_string());

		List += "<TD WIDTH=99 CLASS=tabletxt ALIGN=LEFT>";
		List.format("<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
			Fleet->get_design_id(), Fleet->CShipDesign::get_name());
		List += "</TD>\n";

		List.format("<TD WIDTH=49 CLASS=tabletxt ALIGN=CENTER>&nbsp;%d/%d (%d)&nbsp;</TD>\n",
			Fleet->get_current_ship(), Fleet->get_max_ship(), Admiral->get_fleet_commanding());
			
		List.format("<TD WIDTH=49 CLASS=tabletxt ALIGN=CENTER>&nbsp;%s/%s&nbsp;</TD>\n",
			Admiral->get_special_ability_acronym(), Admiral->get_racial_ability_acronym());

		List += "</TR>\n";
	}

	List += "</TABLE>\n";

	return (char *)List;
}

char *
	CFleetList::reassignment_fleet_list_javascript(CPlayer *aPlayer)
{
	static CString
		Info;
	Info.clear();

	if (!length())
	{
		Info += "/***** NO POOLED ADMIRALS *****/";
		return (char *)Info;
	}

	Info += "function reassignmentFleetSwapAndSort(column)\n{\n";
	Info += "\tvar Action = \n\t{\n\t\tNONE : -1,\n\t\tASCENDING : 0,\n\t\tDESCENDING : 1\n\t}\n";
	Info += "\tswitch (ReassignmentFleetTable.getSortingOrder(column))\n\t{\n";
	Info += "\t\tcase Action.NONE:\n\t\tReassignmentFleetTable.addPriorityRank(column, Action.ASCENDING);\n\t\tif (ReassignmentFleetTable.getPriorityRank(column) > 4) ReassignmentFleetTable.setColumnData(column, ColumnData.HEADER_HTML, ReassignmentFleetTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, ReassignmentFleetTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-11.gif\\\"></TH>\");\n\t\telse ReassignmentFleetTable.setColumnData(column, ColumnData.HEADER_HTML, ReassignmentFleetTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, ReassignmentFleetTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(ReassignmentFleetTable.getPriorityRank(column)) + 1) + \".gif\\\"></TH>\");\t\tbreak;\n";
	Info += "\t\tcase Action.ASCENDING:\n\t\tReassignmentFleetTable.setSortingOrder(column, Action.DESCENDING);\n\t\tif (ReassignmentFleetTable.getPriorityRank(column) > 4) ReassignmentFleetTable.setColumnData(column, ColumnData.HEADER_HTML, ReassignmentFleetTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, ReassignmentFleetTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-12.gif\\\"></TH>\");\n\t\telse ReassignmentFleetTable.setColumnData(column, ColumnData.HEADER_HTML, ReassignmentFleetTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, ReassignmentFleetTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(ReassignmentFleetTable.getPriorityRank(column)) + 6) + \".gif\\\"></TH>\");\t\tbreak;\n";
	Info += "\t\tcase Action.DESCENDING:\n\t\tReassignmentFleetTable.removePriorityRank(column);\n\t\tReassignmentFleetTable.setColumnData(column, ColumnData.HEADER_HTML, ReassignmentFleetTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, ReassignmentFleetTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"</TH>\");\n";
	Info += "\t\tfor (var col = 0; col < ReassignmentFleetTable.getTotalColumns(); col++)\n\t\t{\n\t\t\tif (ReassignmentFleetTable.getSortingOrder(col) == Action.ASCENDING)\n\t\t\t{\n\t\t\t\tif (ReassignmentFleetTable.getPriorityRank(col) > 4) ReassignmentFleetTable.setColumnData(col, ColumnData.HEADER_HTML, ReassignmentFleetTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, ReassignmentFleetTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-11.gif\\\"></TH>\");\n\t\t\t\telse ReassignmentFleetTable.setColumnData(col, ColumnData.HEADER_HTML, ReassignmentFleetTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, ReassignmentFleetTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(ReassignmentFleetTable.getPriorityRank(col)) + 1) + \".gif\\\"></TH>\");\n\t\t\t}\n";
	Info += "\t\t\telse if (ReassignmentFleetTable.getSortingOrder(col) == Action.DESCENDING)\n\t\t\t{\n\t\t\t\tif (ReassignmentFleetTable.getPriorityRank(col) > 4) ReassignmentFleetTable.setColumnData(col, ColumnData.HEADER_HTML, ReassignmentFleetTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, ReassignmentFleetTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-11.gif\\\"></TH>\");\n\t\t\t\telse ReassignmentFleetTable.setColumnData(col, ColumnData.HEADER_HTML, ReassignmentFleetTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, ReassignmentFleetTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(ReassignmentFleetTable.getPriorityRank(col)) + 1) + \".gif\\\"></TH>\");\n\t\t\t}\n\t\t}\n\t\tbreak;\n";
	Info += "\t}\n";
	Info += "\tReassignmentFleetTable.sort();\n";
	Info += "\tdocument.getElementById(\"ReassignmentFleetList\").innerHTML = ReassignmentFleetTable.getTableHTML();\n";
	Info += "}\n\n";
	Info += "ReassignmentFleetTable = new Table();\n";
	Info += "ReassignmentFleetTable.setTableAttributes(\"WIDTH=\\\"550\\\" BORDER=\\\"1\\\" CELLSPACING=\\\"0\\\" CELLPADDING=\\\"0\\\" BORDERCOLOR=\\\"#2A2A2A\\\"\");\n";
	Info += "ReassignmentFleetTable.setHeaderRowAttributes(\"STYLE=\\\"vertical-align: bottom;\\\" BGCOLOR=\\\"#171717\\\"\");\n";
	Info += "ReassignmentFleetTable.setSortDelimiter(\"#SORT#\");\n";
	Info += "ReassignmentFleetTable.addColumn(0,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"45\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:reassignmentFleetSwapAndSort(0)\\\">ID</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"45\\\" ALIGN=\\\"CENTER\\\">\", \"</TD>\");\n";
	Info += "ReassignmentFleetTable.addColumn(1,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"45\\\"><INPUT TYPE=\\\"checkbox\\\" onClick=\\\"javascript:allCheck();\\\"/></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"45\\\" ALIGN=\\\"CENTER\\\"><INPUT TYPE=\\\"checkbox\\\" NAME=\\\"FLEET_INDEX\", \"\\\"></TD>\");\n";
	Info += "ReassignmentFleetTable.addColumn(1,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"144\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:reassignmentFleetSwapAndSort(2)\\\">Name</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"144\\\" ALIGN=\\\"LEFT\\\">\", \"</TD>\");\n";
	Info += "ReassignmentFleetTable.addColumn(1,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"122\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:reassignmentFleetSwapAndSort(3)\\\">Commander</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"122\\\" ALIGN=\\\"LEFT\\\"><A HREF=\\\"/archspace/fleet/fleet_commander_information.as?ADMIRAL_ID=\", \"</A></TD>\");\n";
	Info += "ReassignmentFleetTable.addColumn(0,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"36\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:reassignmentFleetSwapAndSort(4)\\\">Exp.</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"36\\\" ALIGN=\\\"CENTER\\\">\", \"</TD>\");\n";
	//Info += "ReassignmentFleetTable.addColumn(1,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"64\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:reassignmentFleetSwapAndSort(5)\\\">Status</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"64\\\" ALIGN=\\\"LEFT\\\">\", \"</TD>\");\n";
	Info += "ReassignmentFleetTable.addColumn(1,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"99\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:reassignmentFleetSwapAndSort(5)\\\">Design</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"99\\\" ALIGN=\\\"LEFT\\\"><A HREF=\\\"/archspace/fleet/class_specification.as?DESIGN_ID=\", \"</A></TD>\");\n";
	Info += "ReassignmentFleetTable.addColumn(0,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"49\\\">Ships</TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"49\\\" ALIGN=\\\"CENTER\\\">\", \"</TD>\");\n";
	Info += "ReassignmentFleetTable.addColumn(1,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"122\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:reassignmentFleetSwapAndSort(7)\\\">Common<BR>Ability</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"122\\\" ALIGN=\\\"LEFT\\\">\", \"</TD>\");\n";
	Info += "ReassignmentFleetTable.addColumn(1,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"122\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:reassignmentFleetSwapAndSort(8)\\\">Racial<BR>Ability</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"122\\\" ALIGN=\\\"LEFT\\\">\", \"</TD>\");\n";

	Info += "ReassignmentFleetTable.addRows(\"|\", new Array(";

	CAdmiralList *
	AdmiralList = aPlayer->get_admiral_list();

	int sillycounter = 0;

	for (int i = 0 ; i < length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)get(i);
		if (Fleet->get_status() == CFleet::FLEET_STAND_BY)
		{
			sillycounter++;
			CAdmiral *
			Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());
			if (sillycounter > 1) Info += ",\n\t";
			Info.format("\"%d|%d|%s|%d\\\">#SORT#%s#SORT#|%d|%d\\\">#SORT#%s#SORT#|%d/%d/%d|%s|%s|%d\"",
			Fleet->get_id(), i, Fleet->get_name(), Admiral->get_id(), Admiral->get_name(), Fleet->get_exp(),
			Fleet->get_design_id(), Fleet->CShipDesign::get_name(),
			Fleet->get_current_ship(), Fleet->get_max_ship(), Admiral->get_fleet_commanding(),
			Admiral->get_special_ability_name(), Admiral->get_racial_ability_name(), i);
			if (i + 1 >= length()) Info += ")";
		}
		else
		{
			if (i + 1 >= length()) Info += ")";
			continue;
		}
	}
	Info += ");\n";

	return (char *)Info;
}

char *
	CFleetList::expedition_fleet_list_html(CPlayer *aPlayer)
{
	static CString
		List;
	List.clear();

	if (!length())
	{
		List = GETTEXT("You don't have any fleets.");
		return (char *)List;
	}

	List = "<TABLE WIDTH=570 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	List += "<TR BGCOLOR=#171717>\n";
	List += "<TH CLASS=tabletxt WIDTH=29><FONT COLOR=666666>";
	List += GETTEXT("ID");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=29 CLASS=tabletxt><FONT COLOR=666666>&nbsp;</FONT>";
	List += "&nbsp;";
	List += "</TH>\n";

	List += "<TH WIDTH=112 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Name");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=144 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Commander");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=36 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Exp.");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=64 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Status");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=99 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ship Class");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=49 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ships");
	List += "</FONT></TH>\n";

	List += "</TR>\n";

	CAdmiralList *
	AdmiralList = aPlayer->get_admiral_list();
	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)get(i);
		if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

		CAdmiral *
		Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());

		List += "<TR> \n";
		List.format("<TD CLASS=tabletxt WIDTH=29 ALIGN=CENTER>%d</TD>\n",
			Fleet->get_id());

		List += "<TD WIDTH=29 CLASS=tabletxt ALIGN=CENTER>\n";
		List.format("<INPUT TYPE=RADIO NAME=FLEET_ID VALUE=%d>\n", Fleet->get_id());
		List += "</TD>\n";

		List.format("<TD WIDTH=112 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->get_name());

		List += "<TD WIDTH=144 CLASS=tabletxt ALIGN=LEFT>";
		List.format("<A HREF=\"/archspace/fleet/fleet_commander_information.as?ADMIRAL_ID=%d\">%s</A>",
			Admiral->get_id(), Admiral->get_name());
		List += "</TD>\n";

		List.format("<TD WIDTH=36 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Fleet->get_exp());

		List.format("<TD WIDTH=64 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->get_detailed_status_string());

		List += "<TD WIDTH=99 CLASS=tabletxt ALIGN=LEFT>";
		List.format("<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
			Fleet->get_design_id(), Fleet->CShipDesign::get_name());
		List += "</TD>\n";

		List.format("<TD WIDTH=49 CLASS=tabletxt ALIGN=CENTER>&nbsp;%d/%d (%d)&nbsp;</TD>\n",
			Fleet->get_current_ship(), Fleet->get_max_ship(), Admiral->get_fleet_commanding());

		List += "</TR>\n";
	}

	List += "</TABLE>\n";

	return (char *)List;
}

char *
	CFleetList::mission_fleet_list_html(CPlayer *aPlayer)
{
	static CString
		List;
	List.clear();

	if (!length())
	{
		List = GETTEXT("You don't have any fleets.");
		return (char *)List;
	}

	List = "<TABLE WIDTH=570 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	List += "<TR BGCOLOR=#171717>\n";
	List += "<TH CLASS=tabletxt WIDTH=29><FONT COLOR=666666>";
	List += GETTEXT("ID");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=29 CLASS=tabletxt><FONT COLOR=666666>";
	List += "<INPUT TYPE=checkbox onClick=allCheck()>";
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=112 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Name");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=144 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Commander");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=36 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Exp.");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=64 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Status");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=99 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ship Class");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=49 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ships");
	List += "</FONT></TH>\n";
	
	List += "<TH WIDTH=49 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Commander Abilities");
	List += "</FONT></TH>\n";

	List += "</TR>\n";

	CAdmiralList *
	AdmiralList = aPlayer->get_admiral_list();
	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet* )get(i);
		if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

		CAdmiral *
		Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());

		List += "<TR> \n";
		List.format("<TD CLASS=tabletxt WIDTH=29 ALIGN=CENTER>%d</TD>\n",
			Fleet->get_id());

		List += "<TD WIDTH=29 CLASS=tabletxt ALIGN=CENTER>\n";
		List.format("<INPUT TYPE=CHECKBOX NAME=FLEET%d>\n", i);
		List += "</TD>\n";

		List.format("<TD WIDTH=112 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->get_name());

		List += "<TD WIDTH=144 CLASS=tabletxt ALIGN=LEFT>";
		List.format("<A HREF=\"/archspace/fleet/fleet_commander_information.as?ADMIRAL_ID=%d\">%s</A>",
			Admiral->get_id(), Admiral->get_name());
		List += "</TD>\n";

		List.format("<TD WIDTH=36 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Fleet->get_exp());

		List.format("<TD WIDTH=64 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->get_detailed_status_string());

		List += "<TD WIDTH=99 CLASS=tabletxt ALIGN=LEFT>";
		List.format("<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
			Fleet->get_design_id(), Fleet->CShipDesign::get_name());
		List += "</TD>\n";

		List.format("<TD WIDTH=49 CLASS=tabletxt ALIGN=CENTER>&nbsp;%d/%d (%d)&nbsp;</TD>\n",
			Fleet->get_current_ship(), Fleet->get_max_ship(), Admiral->get_fleet_commanding());
			
		List.format("<TD WIDTH=49 CLASS=tabletxt ALIGN=CENTER>&nbsp;%s/%s&nbsp;</TD>\n",
			Admiral->get_special_ability_acronym(), Admiral->get_racial_ability_acronym());

		List += "</TR>\n";
	}

	List += "</TABLE>\n";

	return (char *)List;
}

char *
	CFleetList::disband_fleet_list_html(CPlayer *aPlayer)
{
	static CString
		List;
	List.clear();

	if (!length())
	{
		List = GETTEXT("You don't have any fleets.");
		return (char *)List;
	}

	List = "<TABLE WIDTH=570 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	List += "<TR BGCOLOR=#171717>\n";
	List += "<TH CLASS=tabletxt WIDTH=29><FONT COLOR=666666>";
	List += GETTEXT("ID");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=29 CLASS=tabletxt><FONT COLOR=666666>";
	List += "<INPUT TYPE=checkbox onClick=allCheck()>";
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=112 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Name");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=144 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Commander");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=36 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Exp.");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=64 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Status");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=99 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ship Class");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=49 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ships");
	List += "</FONT></TH>\n";

	List += "</TR>\n";

	CAdmiralList *
	AdmiralList = aPlayer->get_admiral_list();
	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet* )get(i);
		if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

		CAdmiral *
		Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());
		if( Admiral == NULL ) continue;

		List += "<TR> \n";
		List.format("<TD CLASS=tabletxt WIDTH=29 ALIGN=CENTER>%d</TD>\n",
			Fleet->get_id());

		List += "<TD WIDTH=29 CLASS=tabletxt ALIGN=CENTER>\n";
		List.format("<INPUT TYPE=CHECKBOX NAME=FLEET%d>\n", i);
		List += "</TD>\n";

		List.format("<TD WIDTH=112 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->get_name());

		List += "<TD WIDTH=144 CLASS=tabletxt ALIGN=LEFT>";
		List.format("<A HREF=\"/archspace/fleet/fleet_commander_information.as?ADMIRAL_ID=%d\">%s</A>",
			Admiral->get_id(), Admiral->get_name());
		List += "</TD>\n";

		List.format("<TD WIDTH=36 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Fleet->get_exp());

		List.format("<TD WIDTH=64 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->get_detailed_status_string());

		List += "<TD WIDTH=99 CLASS=tabletxt ALIGN=LEFT>";
		List.format("<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
			Fleet->get_design_id(), Fleet->CShipDesign::get_name());
		List += "</TD>\n";

		List.format("<TD WIDTH=49 CLASS=tabletxt ALIGN=CENTER>&nbsp;%d/%d (%d)&nbsp;</TD>\n",
			Fleet->get_current_ship(), Fleet->get_max_ship(), Admiral->get_fleet_commanding());

		List += "</TR>\n";
	}

	List += "</TABLE>\n";

	return (char *)List;
}

char *
	CFleetList::disband_confirm_fleet_list_html(CPlayer *aPlayer, CCommandSet *aFleetSet)
{
	static CString
		List;
	List.clear();

	if (!length())
	{
		List = GETTEXT("You don't have any fleets.");
		return (char *)List;
	}

	List = "<TABLE WIDTH=570 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	List += "<TR BGCOLOR=#171717>\n";

	List += "<TH CLASS=tabletxt WIDTH=29><FONT COLOR=666666>";
	List += GETTEXT("ID");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=112 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Name");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=153 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Commander");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=36 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Exp.");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=74 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Status");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=109 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ship Class");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=49 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ships");
	List += "</FONT></TH>\n";

	List += "</TR>\n";

	CAdmiralList *
	AdmiralList = aPlayer->get_admiral_list();
	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)get(i);
		if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;
		if (!aFleetSet->has(i)) continue;

		CAdmiral *
		Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());
		if( Admiral == NULL ) continue;

		List += "<TR>\n";
		List.format("<TD CLASS=tabletxt WIDTH=29 ALIGN=CENTER>%d</TD>\n",
			Fleet->get_id());

		List.format("<TD WIDTH=112 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->get_name());

		List += "<TD WIDTH=153 CLASS=tabletxt ALIGN=LEFT>";
		List.format("<A HREF=\"/archspace/fleet/fleet_commander_information.as?ADMIRAL_ID=%d\">%s</A>",
			Admiral->get_id(), Admiral->get_name());
		List += "</TD>\n";

		List.format("<TD WIDTH=36 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Fleet->get_exp());

		List.format("<TD WIDTH=74 CLASS=tabletxt ALIGN=LEFT>%s</TD>\n",
			Fleet->get_detailed_status_string());

		List += "<TD WIDTH=109 CLASS=tabletxt ALIGN=LEFT>";
		List.format("<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
			Fleet->get_design_id(), Fleet->CShipDesign::get_name());
		List += "</TD>\n";

		List.format("<TD WIDTH=49 CLASS=tabletxt ALIGN=CENTER>&nbsp;%d/%d (%d)&nbsp;</TD>\n",
			Fleet->get_current_ship(), Fleet->get_max_ship(), Admiral->get_fleet_commanding());

		List += "</TR>\n";
	}

	List += "</TABLE>\n";

	return (char *)List;
}

char *
	CFleetList::siege_planet_fleet_list_html(CPlayer *aPlayer)
{
	static CString
		List;
	List.clear();

	List = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	List += "<TR BGCOLOR=#171717>\n";

	List += "<TH CLASS=tabletxt WIDTH=24><FONT COLOR=666666>";
	List += "&nbsp;";
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=59 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Capital");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=169 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Fleet Name");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=157 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ship Class");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=129 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Offensive Skill");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=24 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Bonus");
	List += "</FONT></TH>\n";

	List += "</TR>\n";

	CAdmiralList *
	AdmiralList = aPlayer->get_admiral_list();
	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)get(i);
		if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

		CAdmiral *
		Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());

		List += "<TR>\n";

		List += "<TD CLASS=tabletxt WIDTH=24 ALIGN=CENTER>";
		List.format("<INPUT TYPE=\"checkbox\" NAME=\"FLEET%d\" VALUE=\"ON\">"
			"<INPUT TYPE=HIDDEN NAME=\"FLEET%d_ID\" VALUE=\"%d\">",
			i, i, Fleet->get_id());
		List += "</TD>\n";

		List += "<TD CLASS=tabletxt WIDTH=59 ALIGN=CENTER>";
		List.format("<INPUT TYPE=\"radio\" NAME=\"CAPITAL\" VALUE=\"%d\">",
			Fleet->get_id());
		List += "</TD>\n";

		List.format("<TD WIDTH=169 CLASS=tabletxt ALIGN=CENTER>%s</TD>\n",
			Fleet->get_nick());

		List.format("<TD WIDTH=157 CLASS=tabletxt ALIGN=CENTER>%s (%d)</TD>\n",
			Fleet->CShipDesign::get_name(), Fleet->get_current_ship());

		List.format("<TD WIDTH=129 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Admiral->get_overall_attack());

		List.format("<TD WIDTH=24 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Admiral->get_armada_commanding_effect(CAdmiral::OFFENSE));

		List += "</TR>\n";
	}

	List += "</TABLE>\n";

	return (char *)List;
}

char *
	CFleetList::blockade_fleet_list_html(CPlayer *aPlayer)
{
	static CString
		List;
	List.clear();

	List = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	List += "<TR BGCOLOR=#171717>\n";

	List += "<TH CLASS=tabletxt WIDTH=24><FONT COLOR=666666>";
	List += "&nbsp;";
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=59 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Capital");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=169 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Fleet Name");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=157 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ship Class");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=129 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Offensive Skill");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=24 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Bonus");
	List += "</FONT></TH>\n";

	List += "</TR>\n";

	CAdmiralList *
	AdmiralList = aPlayer->get_admiral_list();
	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)get(i);
		if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

		CAdmiral *
		Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());

		List += "<TR>\n";

		List += "<TD CLASS=tabletxt WIDTH=24 ALIGN=CENTER>";
		List.format("<INPUT TYPE=\"checkbox\" NAME=\"FLEET%d\" VALUE=\"ON\">"
			"<INPUT TYPE=HIDDEN NAME=\"FLEET%d_ID\" VALUE=\"%d\">",
			i, i, Fleet->get_id());
		List += "</TD>\n";

		List += "<TD CLASS=tabletxt WIDTH=59 ALIGN=CENTER>";
		List.format("<INPUT TYPE=\"radio\" NAME=\"CAPITAL\" VALUE=\"%d\">",
			Fleet->get_id());
		List += "</TD>\n";

		List.format("<TD WIDTH=169 CLASS=tabletxt ALIGN=CENTER>%s</TD>\n",
			Fleet->get_nick());

		List.format("<TD WIDTH=157 CLASS=tabletxt ALIGN=CENTER>%s (%d)</TD>\n",
			Fleet->CShipDesign::get_name(), Fleet->get_current_ship());

		List.format("<TD WIDTH=129 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Admiral->get_overall_attack());

		List.format("<TD WIDTH=24 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Admiral->get_armada_commanding_effect(CAdmiral::OFFENSE));

		List += "</TR>\n";
	}

	List += "</TABLE>\n";

	return (char *)List;
}

char *
	CFleetList::raid_fleet_list_html(CPlayer *aPlayer)
{
	static CString
		List;
	List.clear();

	List = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	List += "<TR BGCOLOR=#171717>\n";

	List += "<TH CLASS=tabletxt WIDTH=40><FONT COLOR=666666>";
	List += "&nbsp;";
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=176 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Fleet Name");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=187 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ship Class");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=137 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Offensive Ability");
	List += "</FONT></TH>\n";

	List += "</TR>\n";

	CAdmiralList *
	AdmiralList = aPlayer->get_admiral_list();
	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)get(i);
		if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

		CAdmiral *
		Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());

		List += "<TR>\n";

		List += "<TD CLASS=tabletxt WIDTH=59 ALIGN=CENTER>";
		List.format("<INPUT TYPE=\"radio\" NAME=\"RAID_FLEET_ID\" VALUE=\"%d\">",
			Fleet->get_id());
		List += "</TD>\n";

		List.format("<TD WIDTH=169 CLASS=tabletxt ALIGN=CENTER>%s</TD>\n",
			Fleet->get_nick());

		List.format("<TD WIDTH=157 CLASS=tabletxt ALIGN=CENTER>%s (%d)</TD>\n",
			Fleet->CShipDesign::get_name(), Fleet->get_current_ship());

		List.format("<TD WIDTH=129 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Admiral->get_overall_attack());

		List += "</TR>\n";
	}

	List += "</TABLE>\n";

	return (char *)List;
}

char *
	CFleetList::privateer_fleet_list_html(CPlayer *aPlayer)
{
	static CString
		List;
	List.clear();

	List = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	List += "<TR BGCOLOR=#171717>\n";

	List += "<TH CLASS=tabletxt WIDTH=40><FONT COLOR=666666>";
	List += "&nbsp;";
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=176 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Fleet Name");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=187 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Ship Class");
	List += "</FONT></TH>\n";

	List += "<TH WIDTH=137 CLASS=tabletxt><FONT COLOR=666666>";
	List += GETTEXT("Offensive Ability");
	List += "</FONT></TH>\n";

	List += "</TR>\n";

	CAdmiralList *
	AdmiralList = aPlayer->get_admiral_list();
	for (int i=0 ; i<length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)get(i);
		if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

		CAdmiral *
		Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());

		List += "<TR>\n";

		List += "<TD CLASS=tabletxt WIDTH=59 ALIGN=CENTER>";
		List.format("<INPUT TYPE=\"radio\" NAME=\"PRIVATEER_FLEET_ID\" VALUE=\"%d\">",
			Fleet->get_id());
		List += "</TD>\n";

		List.format("<TD WIDTH=169 CLASS=tabletxt ALIGN=CENTER>%s</TD>\n",
			Fleet->get_nick());

		List.format("<TD WIDTH=157 CLASS=tabletxt ALIGN=CENTER>%s (%d)</TD>\n",
			Fleet->CShipDesign::get_name(), Fleet->get_current_ship());

		List.format("<TD WIDTH=129 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
			Admiral->get_overall_attack());

		List += "</TR>\n";
	}

	List += "</TABLE>\n";

	return (char *)List;
}

void
	CFleetList::set_volatile()
{
	mIsVolatile = true;
}

void
	CFleetList::set_not_volatile()
{
	mIsVolatile = false;
}

bool
	CFleetList::create_as_empire_fleet_group_static(CAdmiralList *aAdmiralList, int aFleetGroupType, int aPositionArg)
{
	int
		FleetGroupRating,
		TechLevel,
		AdmiralFleetCommandingBonus = 0;

	if (aFleetGroupType == CEmpire::EMPIRE_FLEET_GROUP_TYPE_MAGISTRATE)
	{
		FleetGroupRating = 200;
		TechLevel = 5;
	}
	else if (aFleetGroupType == CEmpire::EMPIRE_FLEET_GROUP_TYPE_FORTRESS && aPositionArg == 1)
	{
		FleetGroupRating = 250;
		TechLevel = 5;
		AdmiralFleetCommandingBonus = CEmpire::mFortress1AdmiralFleetCommandingBonus;
	}
	else if (aFleetGroupType == CEmpire::EMPIRE_FLEET_GROUP_TYPE_FORTRESS && aPositionArg == 2)
	{
		FleetGroupRating = 300;
		TechLevel = 5;
		AdmiralFleetCommandingBonus = CEmpire::mFortress2AdmiralFleetCommandingBonus;
	}
	else if (aFleetGroupType == CEmpire::EMPIRE_FLEET_GROUP_TYPE_FORTRESS && aPositionArg == 3)
	{
		FleetGroupRating = 350;
		TechLevel = 5;
		AdmiralFleetCommandingBonus = CEmpire::mFortress3AdmiralFleetCommandingBonus;
	}
	else if (aFleetGroupType == CEmpire::EMPIRE_FLEET_GROUP_TYPE_FORTRESS && aPositionArg == 4)
	{
		FleetGroupRating = 400;
		TechLevel = 5;
		AdmiralFleetCommandingBonus = CEmpire::mFortress4AdmiralFleetCommandingBonus;
	}
	else if (aFleetGroupType == CEmpire::EMPIRE_FLEET_GROUP_TYPE_EMPIRE_CAPITAL_PLANET)
	{
		FleetGroupRating = 500;
		TechLevel = 6;
		AdmiralFleetCommandingBonus = CEmpire::mCapitalPlanetAdmiralFleetCommandingBonus;
	}
	else
	{
		SLOG("ERROR : Wrong aFleetGroupType in create_as_empire_fleet_group_static(), aFleetGroupType = %d", aFleetGroupType);
		return false;
	}

	int
		FleetGroupSize = FleetGroupRating/10 + number(FleetGroupRating/10 + 4);
	if (FleetGroupSize < 1) FleetGroupSize = 1;
	if (FleetGroupSize > 20) FleetGroupSize = 20;

	int
		AvailableRace[5] =
	{
		CRace::RACE_HUMAN,
		CRace::RACE_TECANOID,
		CRace::RACE_BUCKANEER,
		CRace::RACE_BOSALIAN,
		CRace::RACE_TARGOID
	};

	int
		CapitalFleetRating = 400 + FleetGroupRating*3;
	int
		CapitalAdmiralLevel = CapitalFleetRating/150 + number(CapitalFleetRating/150 + 2);
	int
		CapitalAdmiralRace = AvailableRace[(number(5) - 1)];
	int
		CapitalFleetTechLevel = TechLevel+1;
	if (CapitalFleetTechLevel > 6) CapitalFleetTechLevel = 6;

	CAdmiral *
	CapitalAdmiral = new CAdmiral(CapitalAdmiralLevel, 0, AdmiralFleetCommandingBonus, CapitalAdmiralRace);
	aAdmiralList->add_admiral(CapitalAdmiral);

	CFleet *
	CapitalFleet = new CFleet();
	CapitalFleet->create_as_empire_fleet(CapitalFleetRating, CapitalFleetTechLevel, 100, CapitalAdmiral, 0);
	add_fleet(CapitalFleet);

	for (int i=2 ; i<=FleetGroupSize ; i++)
	{
		int
			FleetRating;
		if (FleetGroupRating > 200)
		{
			FleetRating = (int)(CapitalFleetRating - ((double)i/20)*((double)(250-200)/200)*CapitalFleetRating);
		}
		else
		{
			FleetRating = (int)(CapitalFleetRating - ((double)i/20)*((double)(250-FleetGroupRating)/200)*CapitalFleetRating);
		}

		int
			AdmiralLevel = FleetRating/150 + number(FleetRating/150 + 2);
		int
			AdmiralRace = AvailableRace[(number(5) - 1)];

		CAdmiral *
		Admiral = new CAdmiral(AdmiralLevel, 0, AdmiralFleetCommandingBonus, AdmiralRace);
		aAdmiralList->add_admiral(Admiral);

		CFleet *
		Fleet = new CFleet();
		Fleet->create_as_empire_fleet(FleetRating, TechLevel, 100, Admiral, i-1);

		add_fleet(Fleet);
	}

	mIsVolatile = false;
	return true;
}

bool
	CFleetList::create_as_empire_fleet_group_volatile(CAdmiralList *aAdmiralList, int aFleetGroupType, int aPositionArg, int aFleetSize)
{
	int
		FleetGroupRating,
		TechLevel;

	if (aFleetGroupType == CEmpire::EMPIRE_FLEET_GROUP_TYPE_MAGISTRATE)
	{
		CMagistrateList *
		MagistrateList = EMPIRE->get_magistrate_list();
		CMagistrate *
		Magistrate = MagistrateList->get_by_cluster_id(aPositionArg);
		int
			CapturedPlanet = Magistrate->get_number_of_lost_planets();

        /*
         * TODO: Check if number of lost planets is modified when magi gains planets from retals
         * should it be? Etc. Still will work, just may not have the desired fleet level (too strong).
         */
		int PercentLost = (int)((CapturedPlanet/(CEmpire::mInitialNumberOfMagistratePlanet * 1.0))*100);

		if (PercentLost < 10)
		{
			TechLevel = 1;
			FleetGroupRating = 150;
		}
		else if (PercentLost < 25)
		{
			FleetGroupRating = 200;
			TechLevel = 2;
		}
		else if (PercentLost < 50)
		{
			TechLevel = 3;
			FleetGroupRating = 250;
		}
		else if (PercentLost < 75)
		{
			TechLevel = 3;
			FleetGroupRating = 350;
		}
		else if (PercentLost < 90)
		{
			TechLevel = 4;
			FleetGroupRating = 275;
		}
		else
		{
			FleetGroupRating = 450;
			TechLevel = 4;
		}

		if (PercentLost > 100 || PercentLost < 0)
		{
			SLOG("ERROR : Wrong CapturedPlanet in CFleetList::create_as_empire_fleet_group_volatile(), CapturedPlanet = %d, Magistrate->get_cluster_id() = %d", CapturedPlanet, Magistrate->get_cluster_id());
		}
	}
	else if (aFleetGroupType == CEmpire::EMPIRE_FLEET_GROUP_TYPE_MAGISTRATE_COUNTERATTACK)
	{
		CMagistrateList *
		MagistrateList = EMPIRE->get_magistrate_list();
		CMagistrate *
		Magistrate = MagistrateList->get_by_cluster_id(aPositionArg);
		int
			CapturedPlanet = Magistrate->get_number_of_lost_planets();

		int PercentLost = (int)((CapturedPlanet/(CEmpire::mInitialNumberOfMagistratePlanet * 1.0))*100);

		if (PercentLost < 10)
		{
			TechLevel = 1;
			FleetGroupRating = 150;
		}
		else if (PercentLost < 25)
		{
			FleetGroupRating = 200;
			TechLevel = 2;
		}
		else if (PercentLost < 50)
		{
			TechLevel = 3;
			FleetGroupRating = 200;
		}
		else if (PercentLost < 75)
		{
			TechLevel = 3;
			FleetGroupRating = 250;
		}
		else if (PercentLost < 90)
		{
			TechLevel = 4;
			FleetGroupRating = 275;
		}
		else
		{
			FleetGroupRating = 300;
			TechLevel = 4;
		}

		if (PercentLost > 100 || PercentLost < 0)
		{
			SLOG("ERROR : Wrong CapturedPlanet in CFleetList::create_as_empire_fleet_group_volatile() Counterattack, CapturedPlanet = %d, Magistrate->get_cluster_id() = %d", CapturedPlanet, Magistrate->get_cluster_id());
		}
	}
	else if (aFleetGroupType == CEmpire::EMPIRE_FLEET_GROUP_TYPE_EMPIRE_PLANET)
	{
		int
			CapturedPlanet = EMPIRE->get_number_of_lost_empire_planets();

		int PercentLost = (int)((CapturedPlanet/(CEmpire::mInitialNumberOfEmpirePlanet * 1.0))*100);

		if (PercentLost < 10)
		{
			TechLevel = 3;
			FleetGroupRating = 200;
		}
		else if (PercentLost < 25)
		{
			FleetGroupRating = 300;
			TechLevel = 3;
		}
		else if (PercentLost < 50)
		{
			TechLevel = 3;
			FleetGroupRating = 400;
		}
		else if (PercentLost < 75)
		{
			TechLevel = 4;
			FleetGroupRating = 300;
		}
		else if (PercentLost < 90)
		{
			TechLevel = 5;
			FleetGroupRating = 400;
		}
		else
		{
			FleetGroupRating = 500;
			TechLevel = 5;
		}

		if (PercentLost > 100 || PercentLost < 0)
		{
			SLOG("ERROR : Wrong CapturedPlanet in CFleetList::create_as_empire_fleet_group_volatile(), CapturedPlanet = %d, aFleetGroupType = %d", CapturedPlanet, aFleetGroupType);
		}
	}
	else if (aFleetGroupType == CEmpire::EMPIRE_FLEET_GROUP_TYPE_EMPIRE_PLANET_COUNTERATTACK)
	{
		FleetGroupRating = 150 + dice(2, 50);
		TechLevel = 5;
	}
	else
	{
		SLOG("ERROR : Wrong aFleetGroupType in create_as_empire_fleet_group_volatile(), aFleetGroupType = %d", aFleetGroupType);
		return false;
	}

	int
		FleetGroupSize = FleetGroupRating/10 + number(FleetGroupRating/10 + 4);
	if (FleetGroupSize < 1) FleetGroupSize = 1;
	if (FleetGroupSize > 20) FleetGroupSize = 20;

	int
		AvailableRace[5] =
	{
		CRace::RACE_HUMAN,
		CRace::RACE_TECANOID,
		CRace::RACE_BUCKANEER,
		CRace::RACE_BOSALIAN,
		CRace::RACE_TARGOID
	};

	int
		CapitalFleetRating = 400 + FleetGroupRating*3;
	int
		CapitalAdmiralLevel = CapitalFleetRating/150 + number(CapitalFleetRating/150 + 2);
	int
		CapitalAdmiralRace = AvailableRace[(number(5) - 1)];
	int
		CapitalFleetTechLevel = TechLevel+1;
	if (CapitalFleetTechLevel > 6) CapitalFleetTechLevel = 6;

	CAdmiral *
	CapitalAdmiral = new CAdmiral(CapitalAdmiralLevel, 0, 0, CapitalAdmiralRace);
	aAdmiralList->add_admiral(CapitalAdmiral);

	CFleet *
	CapitalFleet = new CFleet();
	CapitalFleet->create_as_empire_fleet(CapitalFleetRating, CapitalFleetTechLevel, aFleetSize, CapitalAdmiral, 0);
	add_fleet(CapitalFleet);

	for (int i=2 ; i<=FleetGroupSize ; i++)
	{
		int
			FleetRating;

		if (FleetGroupRating > 200)
		{
			FleetRating = (int)(CapitalFleetRating - ((double)i/20)*((double)(250-200)/200)*CapitalFleetRating);
		}
		else
		{
			FleetRating = (int)(CapitalFleetRating - ((double)i/20)*((double)(250-FleetGroupRating)/200)*CapitalFleetRating);
		}

		int
			AdmiralLevel = FleetRating/150 + number(FleetRating/150 + 2);
		int
			AdmiralRace = AvailableRace[(number(5) - 1)];

		CAdmiral *
		Admiral = new CAdmiral(AdmiralLevel, 0, 0, AdmiralRace);
		aAdmiralList->add_admiral(Admiral);

		CFleet *
		Fleet = new CFleet();
		Fleet->create_as_empire_fleet(FleetRating, TechLevel, aFleetSize, Admiral, i-1);

		add_fleet(Fleet);
	}

	mIsVolatile = true;
	return true;

}

CAllyFleetList::CAllyFleetList()
{
}

CAllyFleetList::~CAllyFleetList()
{
	remove_all();
}

bool
	CAllyFleetList::free_item(TSomething aItem)
{
	CFleet *
	Fleet = (CFleet *)aItem;

	if (!Fleet) return false;

	return true;
}

int
	CAllyFleetList::compare(TSomething aItem1, TSomething aItem2) const
{
	CFleet
		*Fleet1 = (CFleet *)aItem1,
	*Fleet2 = (CFleet *)aItem2;

	if (strcmp(Fleet1->get_key(), Fleet2->get_key()) > 0) return 1;
	if (strcmp(Fleet1->get_key(), Fleet2->get_key()) < 0) return -1;
	return 0;
}

int
	CAllyFleetList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CFleet *
	Fleet = (CFleet *)aItem;

	if (strcmp(Fleet->get_key(), (char *)aKey) > 0) return 1;
	if (strcmp(Fleet->get_key(), (char *)aKey) < 0) return -1;
	return 0;
}

bool
	CAllyFleetList::add_fleet(CFleet *aFleet)
{
	if (!aFleet) return false;

	if (find_sorted_key( (TConstSomething)aFleet->get_key() ) >= 0) return false;
	insert_sorted(aFleet);

	refresh_power();

	return true;
}

bool
	CAllyFleetList::remove_fleet(CPlayer *aOwner, int aFleetID)
{
	int
		Index = -1;
	CFleet *
	Fleet;

	for (int i=0 ; i<length() ; i++)
	{
		Fleet = (CFleet *)get(i);

		if (Fleet->get_owner() == aOwner->get_game_id() &&
			Fleet->get_id() == aFleetID)
		{
			Index = i;
			break;
		}
	}

	if (Index == -1)
	{
		SLOG("Could not found remove fleet %d", aFleetID);
		return false;
	}

	refresh_power();

	return remove_without_free(Index);
}

/* end of file fleet.cc */
