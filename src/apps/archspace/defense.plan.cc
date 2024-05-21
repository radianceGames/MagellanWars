#include "common.h"
#include "util.h"
#include "defense.plan.h"
#include <cstdlib>
#include "define.h"
#include <libintl.h>
#include "archspace.h"
#include "game.h"
#include "race.h"

TZone gDefenseFleetZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CDefenseFleet),
	0,
	0,
	NULL,
	"Zone CDefenseFleet"
};

TZone gDefensePlanZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CDefensePlan),
	0,
	0,
	NULL,
	"Zone CDefensePlan"
};

CDefenseFleet::CDefenseFleet( MYSQL_ROW aRow )
{
	mOwner = atoi( aRow[FIELD_OWNER] );
	mPlanID = atoi( aRow[FIELD_PLAN_ID] );
	mFleetID = atoi( aRow[FIELD_FLEET_ID] );
	mCommand = atoi( aRow[FIELD_FLEET_COMMAND] );
	mX = atoi( aRow[FIELD_FLEET_X] );
	mY = atoi( aRow[FIELD_FLEET_Y] );
}

char *
CDefenseFleet::get_command_string_upper_case(int aCommand)
{
	if (aCommand < COMMAND_NORMAL || aCommand >= COMMAND_MAX) return NULL;

	switch (aCommand)
	{
		case COMMAND_NORMAL :
			return "NORMAL";
		case COMMAND_FORMATION :
			return "FORMATION";
		case COMMAND_PENETRATE :
			return "PENETRATE";
		case COMMAND_FLANK :
			return "FLANK";
		case COMMAND_RESERVE :
			return "RESERVE";
		case COMMAND_FREE :
			return "FREE";
		case COMMAND_ASSAULT :
			return "ASSAULT";
		case COMMAND_STAND_GROUND :
			return "STAND_GROUND";
		default :
			return NULL;
	}
}

char *
CDefenseFleet::get_command_string_normal(int aCommand)
{
	if (aCommand < COMMAND_NORMAL || aCommand >= COMMAND_MAX) return NULL;

	switch (aCommand)
	{
		case COMMAND_NORMAL :
			return GETTEXT("Normal");
		case COMMAND_FORMATION :
			return GETTEXT("Formation");
		case COMMAND_PENETRATE :
			return GETTEXT("Penetrate");
		case COMMAND_FLANK :
			return GETTEXT("Flank");
		case COMMAND_RESERVE :
			return GETTEXT("Reserve");
		case COMMAND_FREE :
			return GETTEXT("Free");
		case COMMAND_ASSAULT :
			return GETTEXT("Assault");
		case COMMAND_STAND_GROUND :
			return GETTEXT("Stand Ground");
		default :
			return NULL;
	}
}

CString &
CDefenseFleet::query()
{
	static CString
		Query;
	Query.clear();

	switch(type())
	{
		case QUERY_INSERT:
			Query.format( "INSERT INTO defense_fleet ( owner, plan_id, fleet_id, command, x, y ) VALUES ( %d, %d, %d, %d, %d, %d )", mOwner, mPlanID, mFleetID, mCommand, mX, mY );
			break;
		case QUERY_UPDATE:
			Query.format("UPDATE defense_fleet SET x = %d, y = %d", mX, mY);

#define STORE(x, y, z) \
			if (mStoreFlag.has(x)) \
				Query.format(y, z)

			STORE(STORE_OWNER, ", owner = %d", mOwner);
			STORE(STORE_PLAN_ID, ", plan_id = %d", mPlanID);
			STORE(STORE_FLEET_ID, ", fleet_id = %d", mFleetID);
			STORE(STORE_COMMAND, ", command = %d", mCommand);

			break;

		case QUERY_DELETE:
			Query.format( "DELETE FROM defense_fleet WHERE owner = %d AND plan_id = %d AND fleet_id = %d", mOwner, mPlanID, mFleetID );
			break;
	}

	mStoreFlag.clear();

	return Query;
}

CStoreCenter&
operator<<(CStoreCenter& aStoreCenter, CDefenseFleet& aFleet)
{
	aStoreCenter.store( aFleet );
	return aStoreCenter;
}
const char*
CDefenseFleet::get_option_string()
{
	CString
		option = "<OPTION VALUE=0";
	if (mCommand == COMMAND_NORMAL)
		option += " SELECTED ";
	option += ">Normal</OPTION> <OPTION VALUE=1";
	if (mCommand == COMMAND_FORMATION)
		option += " SELECTED ";
	option += ">Formation</OPTION> <OPTION VALUE=2";
	if (mCommand == COMMAND_PENETRATE)
		option += " SELECTED ";
	option += ">Penetrate</OPTION> <OPTION VALUE=3";
	if (mCommand == COMMAND_FLANK)
		option += " SELECTED ";
	option += ">Flank</OPTION> <OPTION VALUE=4";
	if (mCommand == COMMAND_RESERVE)
		option += " SELECTED ";
	option += ">Reserve</OPTION> <OPTION VALUE=5";
	if (mCommand == COMMAND_FREE)
		option += " SELECTED ";
	option += ">Free</OPTION> <OPTION VALUE=6";
	if (mCommand == COMMAND_ASSAULT)
		option += " SELECTED ";
	option += ">Assault</OPTION>";

	return (char*)option;
}

bool
CDefenseFleet::convert_coordinates()
{
	mX = 5000 - (mX - 5000);
	mY = 5000 - (mY - 5000);

	mStoreFlag += STORE_X;
	mStoreFlag += STORE_Y;

	return true;
}

int
CDefenseFleet::get_fleet_command_from_string(char *aFleetCommandString)
{
	if (aFleetCommandString == NULL) return -1;

	if (!strcmp(aFleetCommandString, "NORMAL"))
	{
		return COMMAND_NORMAL;
	}
	else if (!strcmp(aFleetCommandString, "FORMATION"))
	{
		return COMMAND_FORMATION;
	}
	else if (!strcmp(aFleetCommandString, "PENETRATE"))
	{
		return COMMAND_PENETRATE;
	}
	else if (!strcmp(aFleetCommandString, "FLANK"))
	{
		return COMMAND_FLANK;
	}
	else if (!strcmp(aFleetCommandString, "RESERVE"))
	{
		return COMMAND_RESERVE;
	}
	else if (!strcmp(aFleetCommandString, "FREE"))
	{
		return COMMAND_FREE;
	}
	else if (!strcmp(aFleetCommandString, "ASSAULT"))
	{
		return COMMAND_ASSAULT;
	}
	else if (!strcmp(aFleetCommandString, "STAND_GROUND"))
	{
		return COMMAND_STAND_GROUND;
	}
	else
	{
		return -1;
	}
}

bool
CDefenseFleetList::free_item(TSomething aItem)
{
	CDefenseFleet *
		DefenseFleet = (CDefenseFleet *)aItem;
	if (!DefenseFleet) return false;

	delete DefenseFleet;
	return true;
}

int
CDefenseFleetList::compare(TSomething aItem1, TSomething aItem2) const
{
	CDefenseFleet *
		Fleet1 = (CDefenseFleet *)aItem1;
	CDefenseFleet *
		Fleet2 = (CDefenseFleet *)aItem2;

	if (Fleet1->get_fleet_id() > Fleet2->get_fleet_id()) return 1;
	if (Fleet1->get_fleet_id() < Fleet2->get_fleet_id()) return -1;
	return 0;
}

int
CDefenseFleetList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CDefenseFleet *
		Fleet = (CDefenseFleet *)aItem;

	if (Fleet->get_fleet_id() > (int)aKey) return 1;
	if (Fleet->get_fleet_id() < (int)aKey) return -1;
	return 0;
}

bool
CDefenseFleetList::remove_defense_fleet(int aFleetID)
{
	int
		Index = find_sorted_key((void *)aFleetID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

bool
CDefenseFleetList::remove_without_free_defense_fleet(int aFleetID)
{
	int
		Index = find_sorted_key((void *)aFleetID);
	if (Index < 0) return false;

	return CSortedList::remove_without_free(Index);
}

int
CDefenseFleetList::add_defense_fleet(CDefenseFleet *aFleet)
{
	if (!aFleet) return -1;

	if (find_sorted_key( (TConstSomething)aFleet->get_fleet_id() ) >= 0)
		return -1;
	insert_sorted(aFleet);
	return aFleet->get_fleet_id();
}

CDefenseFleet *
CDefenseFleetList::get_by_id(int aFleetID)
{
	if ( !length() ) return NULL;
	int
		Index = find_sorted_key( (void *)aFleetID );
	if (Index < 0) return NULL;
	return (CDefenseFleet *)get(Index);
}

CDefensePlan::CDefensePlan()
{
	mOwner = -1;
	mID = -1;
	mType = GENERIC_PLAN;
	mName = "";
	mCapital = 0;
	mEnemy = 0;
	mMin = 50;
	mMax = 200;
	mAttackType = 0;
}

CDefensePlan::CDefensePlan( MYSQL_ROW aRow)
{
	mOwner = atoi( aRow[FIELD_OWNER] );
	mID = atoi( aRow[FIELD_ID] );
	mType = atoi( aRow[FIELD_TYPE] );
	mName = aRow[FIELD_NAME];
	mCapital = atoi( aRow[FIELD_CAPITAL] );
	mEnemy = atoi( aRow[FIELD_ENEMY] );
	mMin = atoi( aRow[FIELD_MIN] );
	mMax = atoi( aRow[FIELD_MAX] );
	mAttackType = as_atoi(aRow[FIELD_ATTACK_TYPE]);
}

bool
CDefensePlan::set_fleet( int index, CDefenseFleet *aFleet )
{
	if( index < 0 || 20 < index )
		return false;
	mFleetList.insert(index, aFleet);
	return true;
}

bool
CDefensePlan::is_plan_for( int aEnemy )
{
	if ( (mEnemy & aEnemy) == 0 ) {
		return false;
	} else {
		return true;
	}
}

bool
CDefensePlan::add_defense_fleet(CDefenseFleet *aFleet)
{
	if (mFleetList.length() > 20) return false;

	if (mFleetList.add_defense_fleet(aFleet) < 1) return false;
	return true;
}

bool
CDefensePlan::remove_defense_fleet(int aIndex)
{
	return mFleetList.CSortedList::remove(aIndex);
}

int
CDefensePlan::get_fleets_number()
{
	return mFleetList.length();
}

CString &
CDefensePlan::query()
{
	static CString
		Query;
	Query.clear();

	switch( type() ) {
		case QUERY_INSERT:
			if (add_slashes((char *)mName).length() == 0)
			{
				Query.format( "INSERT INTO plan ( owner, id, type, name, capital, enemy, min, max, attack_type ) VALUES ( %d, %d, %d, '', %d, %d, %d, %d, %d )",
								mOwner, mID, mType, mCapital, get_enemy(), mMin, mMax, mAttackType );
			}
			else
			{
				Query.format( "INSERT INTO plan ( owner, id, type, name, capital, enemy, min, max, attack_type ) VALUES ( %d, %d, %d, '%s', %d, %d, %d, %d, %d )",
								mOwner, mID, mType, (char *)add_slashes((char *)mName), mCapital, get_enemy(), mMin, mMax, mAttackType );
			}

			break;
		case QUERY_UPDATE:
			Query.format("UPDATE plan SET enemy = %d", get_enemy());

#define STORE(x, y, z) \
			if (mStoreFlag.has(x)) \
				Query.format(y, z)

			STORE(STORE_OWNER, ", owner = %d", mOwner);
			STORE(STORE_ID, ", id = %d", mID);
			STORE(STORE_TYPE, ", type = %d", mType);
			STORE(STORE_MIN, ", min = %d", mMin);
			STORE(STORE_MAX, ", max = %d", mMax);
			STORE(STORE_CAPITAL, ", capital = %d", mCapital);
			STORE(STORE_NAME, ", name = '%s'", (char *)add_slashes((char *)mName));
			STORE(STORE_ATTACK_TYPE, ", attack_type = %d", mAttackType);
			Query.format(" WHERE owner = %d AND id = %d", mOwner, mID);

			break;

		case QUERY_DELETE:
			Query.format( "DELETE FROM plan WHERE owner = %d AND id = %d", mOwner, mID );
			SLOG((char*)Query);
			break;
	}

	mStoreFlag.clear();

	return Query;
}

char *
CDefensePlan::get_for_which_race()
{
	static CString
		WhichRace;
	WhichRace.clear();

	bool
		AnyRace = false;

	if (mEnemy & RACE_HUMAN)
	{
		if (AnyRace) WhichRace += ", ";
		WhichRace += RACE_TABLE->get_name_by_id(CRace::RACE_HUMAN);
		AnyRace = true;
	}

	if (mEnemy & RACE_TARGOID)
	{
		if (AnyRace) WhichRace += ", ";
		WhichRace += RACE_TABLE->get_name_by_id(CRace::RACE_TARGOID);
		AnyRace = true;
	}

	if (mEnemy & RACE_BUCKANEER)
	{
		if (AnyRace) WhichRace += ", ";
		WhichRace += RACE_TABLE->get_name_by_id(CRace::RACE_BUCKANEER);
		AnyRace = true;
	}

	if (mEnemy & RACE_TECANOID)
	{
		if (AnyRace) WhichRace += ", ";
		WhichRace += RACE_TABLE->get_name_by_id(CRace::RACE_TECANOID);
		AnyRace = true;
	}

	if (mEnemy & RACE_EVINTOS)
	{
		if (AnyRace) WhichRace += ", ";
		WhichRace += RACE_TABLE->get_name_by_id(CRace::RACE_EVINTOS);
		AnyRace = true;
	}

	if (mEnemy & RACE_AGERUS)
	{
		if (AnyRace) WhichRace += ", ";
		WhichRace += RACE_TABLE->get_name_by_id(CRace::RACE_AGERUS);
		AnyRace = true;
	}

	if (mEnemy & RACE_BOSALIAN)
	{
		if (AnyRace) WhichRace += ", ";
		WhichRace += RACE_TABLE->get_name_by_id(CRace::RACE_BOSALIAN);
		AnyRace = true;
	}

	if (mEnemy & RACE_XELOSS)
	{
		if (AnyRace) WhichRace += ", ";
		WhichRace += RACE_TABLE->get_name_by_id(CRace::RACE_XELOSS);
		AnyRace = true;
	}

	if (mEnemy & RACE_XERUSIAN)
	{
		if (AnyRace) WhichRace += ", ";
		WhichRace += RACE_TABLE->get_name_by_id(CRace::RACE_XERUSIAN);
		AnyRace = true;
	}

	if (mEnemy & RACE_XESPERADOS)
	{
		if (AnyRace) WhichRace += ", ";
		WhichRace += RACE_TABLE->get_name_by_id(CRace::RACE_XESPERADOS);
		AnyRace = true;
	}

	if (!AnyRace) return GETTEXT("None");
	else
	{
		return (char *)WhichRace;
	}
}

char *
CDefensePlan::get_for_which_attack()
{
	static CString
		WhichAttack;
	WhichAttack.clear();

	bool
		AnyAttackType = false;
	if (mAttackType & TYPE_SIEGE)
	{
		if (AnyAttackType) WhichAttack += ", ";
		WhichAttack += GETTEXT("Siege");
		AnyAttackType = true;
	}

	if (mAttackType & TYPE_BLOCKADE)
	{
		if (AnyAttackType) WhichAttack += ", ";
		WhichAttack += GETTEXT("Blockade");
		AnyAttackType = true;
	}

	if (!AnyAttackType) return GETTEXT("None");
	else 
	{
		return (char *)WhichAttack;
	}
}

bool
CDefensePlan::convert_coordinates()
{
	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CDefenseFleet *
			DefenseFleet = (CDefenseFleet *)mFleetList.get(i);
		DefenseFleet->convert_coordinates();
	}

	return true;
}

bool
CDefensePlan::is_there_stacked_fleet()
{
	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CDefenseFleet *
			Fleet = (CDefenseFleet *)mFleetList.get(i);

		for (int j=i+1 ; j<mFleetList.length() ; j++)
		{
			CDefenseFleet *
				TempFleet = (CDefenseFleet *)mFleetList.get(j);
			if (Fleet->get_x() == TempFleet->get_x() &&
				Fleet->get_y() == TempFleet->get_y()) return true;
		}
	}

	return false;
}

CStoreCenter&
operator<<(CStoreCenter& aStoreCenter, CDefensePlan& aPlan)
{
	aStoreCenter.store( aPlan );
	return aStoreCenter;
}

bool
CDefensePlan::init_for_empire(CFleetList *aFleetList, bool aOffenseSide)
{
	mOwner = 0;
	(void)mID;
	(void)mType;
	(void)mEnemy;
	(void)mMin;
	(void)mMax;
	(void)mAttackType;

	CFleetList
		AvailableFleetList;

	for (int i=0 ; i<aFleetList->length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)aFleetList->get(i);
		if (Fleet->get_current_ship() > 0) AvailableFleetList.add_fleet(Fleet);
	}
	if (AvailableFleetList.length() == 0) return false;

	int
		FleetGroupSize = AvailableFleetList.length(),
		LineOfFlank = number(6) - 1,
		LineOfFront = number(LineOfFlank + 1),
		LineOfReserve = number(LineOfFront) - 1,
		GapOfFront = number(3),
		GapOfFlank = number(3),
		GapOfReserve = number(3),
		GapBetweenFrontFlank = number(4);
	int
		TempFleetGroupSize = FleetGroupSize;
	int
		NumberOfCapitalFleet = 0,
		NumberOfFlankFleet = 0,
		NumberOfFrontFleet = 0,
		NumberOfReserveFleet = 0;

	while(1)
	{
		while (1)
		{
			NumberOfCapitalFleet = 1;
			TempFleetGroupSize -= NumberOfCapitalFleet;
			if (TempFleetGroupSize <= 0) break;

			NumberOfFlankFleet = (int)(FleetGroupSize / 5) * 2;
			TempFleetGroupSize -= NumberOfFlankFleet;
			if (TempFleetGroupSize <= 0) break;

			if (TempFleetGroupSize < 6)
			{
				NumberOfFrontFleet = TempFleetGroupSize;
				NumberOfReserveFleet = 0;
			}
			else
			{
				NumberOfFrontFleet = (int)(TempFleetGroupSize * 7 / 18);
				NumberOfReserveFleet = TempFleetGroupSize - NumberOfFrontFleet;
			}

			break;
		}

		int
			RangeOfFrontFleet = 0,
			RangeBetweenFrontFlank = 0,
			RangeOfFlankFleet = 0;
		if (NumberOfFrontFleet > 0)
		{
			RangeOfFrontFleet = (NumberOfFrontFleet - 1) * GapOfFront;
		}
		if (NumberOfFlankFleet > 0)
		{
			RangeBetweenFrontFlank = GapBetweenFrontFlank * 2;
			RangeOfFlankFleet = ((NumberOfFlankFleet/2) - 1) * GapOfFlank +
								((NumberOfFlankFleet/2) - 1) * GapOfFlank;
		}

		int
			TotalRange = RangeOfFrontFleet + RangeBetweenFrontFlank + RangeOfFlankFleet;
		if (TotalRange > 30)
		{
			while (1)
			{
				int
					Random = number(3);
				if (Random == 1)
				{
					if (GapOfFront == 0) continue;
					GapOfFront--;
				}
				else if (Random == 2)
				{
					if (GapOfFlank == 0) continue;
					GapOfFlank--;
				}
				else if (Random == 3)
				{
					if (GapBetweenFrontFlank == 0) continue;
					GapBetweenFrontFlank--;
				}

				break;
			}
		}

		break;
	}

	int
		FrontLeftEnd,
		FrontRightEnd;

	/* Deployment for capital fleet */
	CFleet *
		Fleet = (CFleet *)AvailableFleetList.get(0);
	mCapital = Fleet->get_id();

	CDefenseFleet *
		DefenseFleet = new CDefenseFleet();
	DefenseFleet->set_owner(Fleet->get_owner());
	DefenseFleet->set_command(CDefenseFleet::COMMAND_RESERVE);
	DefenseFleet->set_x(2000);
	DefenseFleet->set_y(5000);
	DefenseFleet->set_fleet_id(Fleet->get_id());

	mFleetList.add_defense_fleet(DefenseFleet);
	AvailableFleetList.remove_without_free_fleet(Fleet->get_id());

	/* Deployment for front fleets */
	for (int i=0 ; ; i += 2)
	{
		if ((i+1) > NumberOfFrontFleet) break;

		CFleet *
			Fleet = (CFleet *)AvailableFleetList.get(i);

		CDefenseFleet *
			DefenseFleet = new CDefenseFleet();
		DefenseFleet->set_owner(Fleet->get_owner());
		DefenseFleet->set_command(CDefenseFleet::COMMAND_FORMATION);
		DefenseFleet->set_x(2000 + LineOfFront*200);
		DefenseFleet->set_y(5000 + (GapOfFront*200) * (i/2));
		DefenseFleet->set_fleet_id(Fleet->get_id());

		mFleetList.add_defense_fleet(DefenseFleet);

		FrontLeftEnd = 5000 + (GapOfFront*200) * (i/2);
	}
	for (int i=1 ; ; i += 2)
	{
		if ((i+1) > NumberOfFrontFleet) break;

		CFleet *
			Fleet = (CFleet *)AvailableFleetList.get(i);

		CDefenseFleet *
			DefenseFleet = new CDefenseFleet();
		DefenseFleet->set_owner(Fleet->get_owner());
		DefenseFleet->set_command(CDefenseFleet::COMMAND_FORMATION);
		DefenseFleet->set_x(2000 + LineOfFront*200);
		DefenseFleet->set_y(5000 - (GapOfFront*200) * ((i+1)/2));
		DefenseFleet->set_fleet_id(Fleet->get_id());

		mFleetList.add_defense_fleet(DefenseFleet);

		FrontRightEnd = 5000 - (GapOfFront*200) * ((i+1)/2);
	}

	for (int i=NumberOfFrontFleet-1 ; i>=0 ; i--)
	{
		AvailableFleetList.CList::remove_without_free(i);
	}

	/* Deployment for flank fleets */
	int
		Random = number(2);

	for (int i=0 ; i < (NumberOfFlankFleet/2) ; i++)
	{
		CFleet *
			Fleet = (CFleet *)AvailableFleetList.get(i);

		CDefenseFleet *
			DefenseFleet = new CDefenseFleet();
		DefenseFleet->set_owner(Fleet->get_owner());
		DefenseFleet->set_command(CDefenseFleet::COMMAND_FLANK);
		DefenseFleet->set_x(2000 + LineOfFlank*200);
		if (Random == 1)
		{
			DefenseFleet->set_y(FrontLeftEnd + (GapBetweenFrontFlank*200) + (GapOfFlank*i)*200);
		}
		else
		{
			DefenseFleet->set_y(FrontRightEnd - (GapBetweenFrontFlank*200) - (GapOfFlank*i)*200);
		}
		DefenseFleet->set_fleet_id(Fleet->get_id());

		mFleetList.add_defense_fleet(DefenseFleet);
	}

	for (int i=(NumberOfFlankFleet/2)-1 ; i>=0 ; i--)
	{
		AvailableFleetList.CList::remove_without_free(i);
	}

	for (int i=0 ; i < (NumberOfFlankFleet/2) ; i++)
	{
		CFleet *
			Fleet = (CFleet *)AvailableFleetList.get(i);

		CDefenseFleet *
			DefenseFleet = new CDefenseFleet();
		DefenseFleet->set_owner(Fleet->get_owner());
		DefenseFleet->set_command(CDefenseFleet::COMMAND_FLANK);
		DefenseFleet->set_x(2000 + LineOfFlank*200);
		if (Random == 1)
		{
			DefenseFleet->set_y(FrontRightEnd - (GapBetweenFrontFlank*200) - (GapOfFlank*i)*200);
		}
		else
		{
			DefenseFleet->set_y(FrontLeftEnd + (GapBetweenFrontFlank*200) + (GapOfFlank*i)*200);
		}
		DefenseFleet->set_fleet_id(Fleet->get_id());

		mFleetList.add_defense_fleet(DefenseFleet);
	}

	for (int i=(NumberOfFlankFleet/2)-1 ; i>=0 ; i--)
	{
		AvailableFleetList.CList::remove_without_free(i);
	}

	/* Deployment for reserve fleets */
	for (int i=0 ; ; i += 2)
	{
		if ((i+1) > NumberOfReserveFleet) break;

		CFleet *
			Fleet = (CFleet *)AvailableFleetList.get(i);

		CDefenseFleet *
			DefenseFleet = new CDefenseFleet();
		DefenseFleet->set_owner(Fleet->get_owner());
		DefenseFleet->set_command(CDefenseFleet::COMMAND_RESERVE);
		DefenseFleet->set_x(2000 + LineOfReserve*200);
		DefenseFleet->set_y(5000 - (GapOfReserve*200) * ((i/2) + 1));
		DefenseFleet->set_fleet_id(Fleet->get_id());

		mFleetList.add_defense_fleet(DefenseFleet);
	}
	for (int i=1 ; ; i += 2)
	{
		if ((i+1) > NumberOfReserveFleet) break;

		CFleet *
			Fleet = (CFleet *)AvailableFleetList.get(i);

		CDefenseFleet *
			DefenseFleet = new CDefenseFleet();
		DefenseFleet->set_owner(Fleet->get_owner());
		DefenseFleet->set_command(CDefenseFleet::COMMAND_RESERVE);
		DefenseFleet->set_x(2000 + LineOfReserve*200);
		DefenseFleet->set_y(5000 + (GapOfReserve*200) * ((i + 1)/2));
		DefenseFleet->set_fleet_id(Fleet->get_id());

		mFleetList.add_defense_fleet(DefenseFleet);
	}

	for (int i=NumberOfReserveFleet-1 ; i>=0 ; i--)
	{
		AvailableFleetList.CList::remove_without_free(i);
	}

	(void)mName;

	if (aOffenseSide == false) convert_coordinates();

	return true;
}

bool
CDefensePlanList::free_item(TSomething aItem)
{
	CDefensePlan *
		DefensePlan = (CDefensePlan *)aItem;
	if (!DefensePlan) return false;

	delete DefensePlan;
	return true;
}

int
CDefensePlanList::compare( TSomething aItem1, TSomething aItem2 ) const
{
	CDefensePlan
		*plan1 = (CDefensePlan*)aItem1,
		*plan2 = (CDefensePlan*)aItem2;

	if ( plan1->get_id() > plan2->get_id() ) return 1;
	if ( plan1->get_id() < plan2->get_id() ) return -1;
	return 0;
}

int
CDefensePlanList::compare_key( TSomething aItem, TConstSomething aKey ) const
{
	CDefensePlan
		*plan = (CDefensePlan*)aItem;

	if ( plan->get_id() > (int)aKey ) return 1;
	if ( plan->get_id() < (int)aKey ) return -1;
	return 0;
}

bool
CDefensePlanList::remove_defense_plan(int aPlanID)
{
	int
		index;
	index = find_sorted_key( (void*)aPlanID );
	if ( index < 0 ) return false;

	return CSortedList::remove(index);
}

int
CDefensePlanList::add_defense_plan(CDefensePlan *aPlan)
{
	if (!aPlan) return -1;

	if (find_sorted_key( (TConstSomething)aPlan->get_id() ) >= 0)
		return -1;
	insert_sorted( aPlan );
	return aPlan->get_id();
}

CDefensePlan *
CDefensePlanList::get_by_id(int aPlanID)
{
	if ( !length() ) return NULL;
	int
		index;
	index = find_sorted_key( (void*)aPlanID );
	if ( index<0 ) return NULL;
	return (CDefensePlan*)get(index);
}

CDefensePlan *
CDefensePlanList::get_generic_plan()
{
	for (int i=0 ; i<length() ; i++)
	{
		CDefensePlan *
			DefensePlan = (CDefensePlan *)get(i);

		if (DefensePlan->get_type() == CDefensePlan::GENERIC_PLAN) return DefensePlan;
	}

	return NULL;
}

int
CDefensePlanList::get_new_id()
{
	int
		ID = 1;

	while(find_sorted_key((void *)ID) >= 0) ID++;

	return ID;	
}

CDefensePlan *
CDefensePlanList::get_optimal_plan(int aRace, int aEnemyPower, int aMyPower,
									CDefensePlan::EAttackType aAttackType)
{
	if (length() == 0) return NULL;
	if (aRace < CRace::RACE_BEGIN || aRace >= CRace::RACE_MAX) return NULL;
	if (aAttackType < CDefensePlan::TYPE_SIEGE ||
		aAttackType > CDefensePlan::TYPE_ALL) return NULL;

	int
		Race;
	if (aRace == CRace::RACE_HUMAN) Race = CDefensePlan::RACE_HUMAN;
	if (aRace == CRace::RACE_TARGOID) Race = CDefensePlan::RACE_TARGOID;
	if (aRace == CRace::RACE_BUCKANEER) Race = CDefensePlan::RACE_BUCKANEER;
	if (aRace == CRace::RACE_TECANOID) Race = CDefensePlan::RACE_TECANOID;
	if (aRace == CRace::RACE_EVINTOS) Race = CDefensePlan::RACE_EVINTOS;
	if (aRace == CRace::RACE_AGERUS) Race = CDefensePlan::RACE_AGERUS;
	if (aRace == CRace::RACE_BOSALIAN) Race = CDefensePlan::RACE_BOSALIAN;
	if (aRace == CRace::RACE_XELOSS) Race = CDefensePlan::RACE_XELOSS;
	if (aRace == CRace::RACE_XERUSIAN) Race = CDefensePlan::RACE_XERUSIAN;
	if (aRace == CRace::RACE_XESPERADOS) Race = CDefensePlan::RACE_XESPERADOS;

	CDefensePlan *
		BestDefensePlan = get_generic_plan();
	int
		MinPowerRange = 999;

	double
		PowerRatio = (aEnemyPower / aMyPower)*100;

	for (int i=0 ; i<length() ; i++)
	{
		CDefensePlan *
			DefensePlan = (CDefensePlan *)get(i);
		int
			Min = DefensePlan->get_min(),
			Max = DefensePlan->get_max();

		if ((DefensePlan->get_enemy() & Race) &&
			(DefensePlan->get_attack_type() & aAttackType) &&
			(Min <= PowerRatio && Max >= PowerRatio))
		{
			if (MinPowerRange > Max-Min)
			{
				MinPowerRange = Max-Min;
				BestDefensePlan = DefensePlan;
			}
		}
	}

	return BestDefensePlan;
}

char *
CDefensePlanList::special_defense_plan_info_html()
{
	static CString
		PlanInfo;
	PlanInfo.clear();

	bool
		AnySpecialPlan = false;
	for (int i=0 ; i<length() ; i++)
	{
		CDefensePlan *
			DefensePlan = (CDefensePlan *)get(i);

		if (DefensePlan->get_type() == CDefensePlan::SPECIAL_PLAN)
		{
			AnySpecialPlan = true;
			break;
		}
	}

	if (!AnySpecialPlan)
	{
		PlanInfo = "<TR>\n";
		PlanInfo.format("<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD\n",
						GETTEXT("You don't have any special defense plans."));
		PlanInfo += "</TR>\n";

		return (char *)PlanInfo;
	}

	PlanInfo = "<TR>\n";
	PlanInfo += "<TD ALIGN=\"center\">\n";
	PlanInfo += "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\""
				" BORDERCOLOR=\"#2A2A2A\">\n";
	PlanInfo += "<TR BGCOLOR=\"#171717\">\n";

	PlanInfo += "<TH CLASS=\"tabletxt\"><FONT COLOR=\"666666\">";
	PlanInfo += "&nbsp;";
	PlanInfo += "</FONT></TH>\n";

	PlanInfo += "<TH CLASS=\"tabletxt\"><FONT COLOR=\"666666\">";
	PlanInfo += GETTEXT("ID");
	PlanInfo += "</FONT></TH>\n";

	PlanInfo += "<TH CLASS=\"tabletxt\"><FONT COLOR=\"666666\">";
	PlanInfo += GETTEXT("Name");
	PlanInfo += "</FONT></TH>\n";

	PlanInfo += "<TH CLASS=\"tabletxt\"><FONT COLOR=\"666666\">";
	PlanInfo += GETTEXT("Race");
	PlanInfo += "</FONT></TH>\n";

	PlanInfo += "<TH CLASS=\"tabletxt\"><FONT COLOR=\"666666\">";
	PlanInfo += GETTEXT("Power Ratio");
	PlanInfo += "</FONT></TH>\n";

	PlanInfo += "<TH CLASS=\"tabletxt\"><FONT COLOR=\"666666\">";
	PlanInfo += GETTEXT("Attack Type");
	PlanInfo += "</FONT></TH>\n";

	PlanInfo += "</TR>\n";

	for (int i=0 ; i<length() ; i++)
	{
		CDefensePlan *
			Plan = (CDefensePlan *)get(i);
		if (Plan->get_type() == CDefensePlan::GENERIC_PLAN) continue;

		PlanInfo += "<TR>\n";

		PlanInfo += "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">\n";
		PlanInfo.format("<INPUT TYPE=\"radio\" NAME=\"PLAN_ID\" VALUE=\"%d\">",
						Plan->get_id());
		PlanInfo += "</TD>\n";
		PlanInfo.format("<TD CLASS=\"tabletxt\" ALIGN=\"RIGHT\">&nbsp;%s</TD>\n",
						dec2unit(Plan->get_id()));
		PlanInfo.format("<TD CLASS=\"tabletxt\" ALIGN=\"LEFT\">&nbsp;%s</TD>\n",
						Plan->get_name());
		PlanInfo.format("<TD CLASS=\"tabletxt\" ALIGN=\"LEFT\">&nbsp;%s</TD>\n",
						Plan->get_for_which_race());
		PlanInfo.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%d%% - %d%%</TD>\n",
						Plan->get_min(), Plan->get_max());
		PlanInfo.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
						Plan->get_for_which_attack());

		PlanInfo += "</TR>\n";
	}

	PlanInfo += "</TABLE>\n";
	PlanInfo += "</TD>\n";
	PlanInfo += "</TR>\n";

	PlanInfo += "<TR>\n";

	PlanInfo += "<TD ALIGN=\"center\">\n";

	PlanInfo += "<TABLE WIDTH=\"330\" BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n";
	PlanInfo += "<TR>\n";

	PlanInfo += "<TD WIDTH=\"171\" CLASS=\"maintext\">";
	PlanInfo += GETTEXT("Old Special Defense Plan");
	PlanInfo += "</TD>\n";

	PlanInfo += "<TD WIDTH=\"159\" VALIGN=\"BOTTOM\">";
	PlanInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_change.gif\" WIDTH=\"67\" HEIGHT=\"11\" BORDER=\"0\" onClick=\"document.SPECIAL.PLAN_BUTTON.value=\'CHANGE\'\">",
					(char *)CGame::mImageServerURL);
	PlanInfo += "&nbsp;&nbsp;";
	PlanInfo.format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_delete.gif\" WIDTH=\"57\" HEIGHT=\"11\" BORDER=\"0\" onClick=\"document.SPECIAL.PLAN_BUTTON.value=\'DELETE\'\">",
					(char *)CGame::mImageServerURL);
	PlanInfo += "</TD>\n";

	PlanInfo += "</TR>\n";
	PlanInfo += "</TABLE>\n";

	PlanInfo += "</TD>\n";

	PlanInfo += "</TR>\n";

	return (char *)PlanInfo;
}

/* end of file defense.plan.cc */
