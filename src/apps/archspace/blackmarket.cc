#include <libintl.h>
#include "blackmarket.h"
#include <cstdlib>
#include "ship.h"
#include "fleet.h"
#include "admiral.h"
#include "planet.h"
#include "tech.h"
#include "archspace.h"
#include "project.h"
#include "game.h"
#include "race.h"
#include "admin.h"
#include "prerequisite.h"
#include "util.h"
#include "define.h"

int CBlackMarket::mBlackMarketItemRegen = 3600;
int CBlackMarket::mBidExpireTime = 15;

CBid::CBid()
{
}

CBid::CBid(MYSQL_ROW aRow)
{
	enum
	{
		FIELD_ID = 0,
		FIELD_TYPE,
		FIELD_ITEM,
		FIELD_WINNER,
		FIELD_PRICE,
		FIELD_OPEND,
		FIELD_EXPIRE,
		FIELD_CLOSED,
		FIELD_NUMBER_OF_PLANET
	};

	mID = atoi( aRow[FIELD_ID] );
	mType = atoi( aRow[FIELD_TYPE] );
	mItem = atoi( aRow[FIELD_ITEM] );
	mWinner = atoi( aRow[FIELD_WINNER] );
	mPrice = atoi( aRow[FIELD_PRICE] );
	mOpend = atoi( aRow[FIELD_OPEND] );
	mExpire = atoi( aRow[FIELD_EXPIRE] );
	mClosed = atoi( aRow[FIELD_CLOSED] );
	mNumberOfPlanet = atoi( aRow[FIELD_NUMBER_OF_PLANET] );
}

void
CBid::set_expire_time( int aTime )
{
	if( aTime < 0 )
	{
		aTime = 0;
	}
	mExpire = aTime;
}

bool
CBid::set_price(int aPrice)
{
	if (aPrice > MAX_BID_PRICE)
	{
		mPrice = MAX_BID_PRICE;
	}
	else if (aPrice < 0)
	{
		mPrice = 0;
	}
	else
	{
		mPrice = aPrice;
	}

	return true;
}

CString&
CBid::query()
{
	static CString
		query;
	query.clear();
	switch( type() )
	{
		case QUERY_INSERT:
			query.format("INSERT INTO blackmarket (id, type, item, winner, price, opend, expire, closed, number_of_planet ) VALUES ( %d, %d, %d, %d, %d, %d, %d, %d, %d )",
				mID,
				mType,
				mItem,
				mWinner,
				mPrice,
				mOpend,
				mExpire,
				mNumberOfPlanet,
				mClosed );
			break;
		case QUERY_UPDATE:
			query.format( "UPDATE blackmarket SET winner = %d, price = %d, expire = %d, closed = %d, number_of_planet = %d WHERE id = %d",
				mWinner,
				mPrice,
				mExpire,
				mClosed,
				mNumberOfPlanet,
				mID );
			break;
		case QUERY_DELETE:
			query.format( "DELETE FROM blackmarket WHERE id = %d", mID );
			break;
	}

	mStoreFlag.clear();

	return query;
}

CStoreCenter&
operator<<(CStoreCenter& aStoreCenter, CBid &aBid)
{
	aStoreCenter.store( aBid );
	return aStoreCenter;
}

CBidList::CBidList()
{
}

CBidList::~CBidList()
{
	remove_all();
}

bool
CBidList::free_item( TSomething aItem )
{
	(void)aItem;
	return true;
}

int
CBidList::compare( TSomething aItem1, TSomething aItem2) const
{
	CBid
		*bid1 = (CBid*)aItem1,
		*bid2 = (CBid*)aItem2;

	if ( bid1->get_id() > bid2->get_id() ) return 1;
	if ( bid1->get_id() < bid2->get_id() ) return -1;
	return 0;
}

int
CBidList::compare_key( TSomething aItem, TConstSomething aKey ) const
{
	CBid
		*bid = (CBid*)aItem;
	if ( bid->get_id() > (int)aKey ) return 1;
	if ( bid->get_id() < (int)aKey ) return -1;
	return 0;
}

bool
CBidList::remove_bid(int aBidID)
{
	int
		index;
	index = find_sorted_key( (void*)aBidID );
	if ( index < 0 ) return false;
	return CSortedList::remove(index);
}

int
CBidList::add_bid(CBid *aBid)
{
	if ( !aBid )
	{
		return -1;
	}
	if ( find_sorted_key( (TConstSomething)aBid->get_id() ) >= 0 )
	{
		return -1;
	}
	insert_sorted( aBid );
	return aBid->get_id();
}

CBid*
CBidList::get_by_id(int aBidID)
{
	if( !length() )
	{
		return NULL;
	}

	int
		index;
	index = find_sorted_key( (void*)aBidID );
	if( index < 0 )
	{
		return NULL;
	}
	return (CBid*)get(index);
}

int
CBidList::count_by_type(int aType)
{
	int
		Count = 0;

	for (int i=0 ; i<length() ; i++)
	{
		CBid *
			Bid = (CBid *)get(i);
		if (Bid->get_type() == aType) Count++;
	}

	return Count;
}

CBlackMarket::CBlackMarket()
{
	mBidList = new CBidList;

	mShipDesignList = new CShipDesignList;
	mFleetList = new CFleetList;
	mAdmiralList = new CAdmiralList;
	mPlanetList = new CSortedPlanetList;
}

CBlackMarket::~CBlackMarket()
{
	if (mBidList) delete mBidList;

	if (mShipDesignList) delete mShipDesignList;
	if (mFleetList) delete mFleetList;
	if (mAdmiralList) delete mAdmiralList;
	if (mPlanetList) delete mPlanetList;
}

void
CBlackMarket::add_bid(CBid *aBid)
{
	mBidList->add_bid(aBid);
}

bool
CBlackMarket::remove_bid(CBid *aBid)
{
	if (aBid == NULL) return false;

	return mBidList->remove_bid(aBid->get_id());
}

void
CBlackMarket::load(CMySQL *aMySQL)
{
	//aMySQL->query( "LOCK TABLE blackmarket READ" );
	aMySQL->query( "SELECT * FROM blackmarket" );
	aMySQL->use_result();
	while(aMySQL->fetch_row())
	{
		add_bid(new CBid(aMySQL->row()));
	}
	aMySQL->free_result();
	//aMySQL->query( "UNLOCK TABLES" );
}

int
CBlackMarket::get_new_bid_id()
{
	int index = 0;
	for(int i=0 ; i<mBidList->length() ; i++)
	{
		CBid*
			bid = (CBid*)mBidList->get(i);
		if( index <= bid->get_id() )
		{
			index = bid->get_id();
		}
	}
	return ++index;
}

bool
CBlackMarket::add_new_item( CTech* aTech )
{
	if( aTech == NULL )
	{
		return false;
	}

	CBid*
		newBid = new CBid();
	newBid->set_id( get_new_bid_id() );
	newBid->set_type( CBid::ITEM_TECH );
	newBid->set_item( aTech->get_id() );
	newBid->set_winner_id( 0 );	// no winner yet.
	newBid->set_price( 100 * aTech->get_level() );
	time_t currentTime = time( 0 );
	newBid->set_open_time( currentTime );
	//newBid->set_expire_time( currentTime/60 + TECH_EXPIRE_TIME ); //minute
	newBid->set_expire_time( TECH_EXPIRE_TIME );
	newBid->set_closed_time( 0 );	// not closed

	newBid->type(QUERY_INSERT);
	*STORE_CENTER << *newBid;

	add_bid(newBid);
	return true;
}

bool
CBlackMarket::add_new_item( CFleet* aFleet, CShipDesign* aDesign )
{
	if( aFleet==NULL || aDesign==NULL)
	{
		return false;
	}

	if( mFleetList->get_by_id( aFleet->get_id() ) != NULL
		|| mShipDesignList->get_by_id( aDesign->get_design_id() ) != NULL )
	{
		return false;
	}

	CBid*
		newBid = new CBid();
	newBid->set_id( get_new_bid_id() );
	newBid->set_type( CBid::ITEM_FLEET );
	newBid->set_item( aFleet->get_id() );
	newBid->set_winner_id( 0 );	// no winner yet.
	newBid->set_price( aFleet->get_build_cost() * aFleet->get_current_ship() / 2);
	time_t currentTime = time( 0 );
	newBid->set_open_time( currentTime );
	//newBid->set_expire_time( currentTime/60 + FLEET_EXPIRE_TIME );	//minute
	newBid->set_expire_time( FLEET_EXPIRE_TIME );	//minute
	newBid->set_closed_time( 0 );	// not closed.

	newBid->type(QUERY_INSERT);
	*STORE_CENTER << *newBid;

	mFleetList->add_fleet(aFleet);
	mShipDesignList->add_ship_design(aDesign);
	add_bid(newBid);
	CEmpire::update_max_empire_fleet_id(aFleet);

	return true;
}

bool
CBlackMarket::add_new_item( CAdmiral* aAdmiral)
{
	if( aAdmiral == NULL )
	{
		return false;
	}

	if( mAdmiralList->get_by_id( aAdmiral->get_id() ) != NULL )
	{
		return false;
	}

	CBid*
		newBid = new CBid();
	newBid->set_id( get_new_bid_id() );
	newBid->set_type( CBid::ITEM_ADMIRAL );
	newBid->set_item( aAdmiral->get_id() );
	newBid->set_winner_id( 0 );	// no winner yet.
	newBid->set_price( aAdmiral->get_level() * (aAdmiral->get_fleet_commanding()-5) * (aAdmiral->get_real_efficiency()-24) * 100 );
	time_t currentTime = time( 0 );
	newBid->set_open_time( currentTime );
	//newBid->set_expire_time( currentTime/60 + ADMIRAL_EXPIRE_TIME ); //minute
	newBid->set_expire_time( ADMIRAL_EXPIRE_TIME ); //minute
	newBid->set_closed_time( 0 );	// not closed.

	newBid->type(QUERY_INSERT);
	*STORE_CENTER << *newBid;

	mAdmiralList->add_admiral( aAdmiral );
	add_bid( newBid );
	return true;
}

bool
CBlackMarket::add_new_item( CProject* aProject )
{
	if (aProject == NULL) return false;

	CBid *
		newBid = new CBid();
	newBid->set_id( get_new_bid_id() );
	newBid->set_type( CBid::ITEM_PROJECT );
	newBid->set_item( aProject->get_id() );
	newBid->set_winner_id( 0 );	// no winner yet.
	newBid->set_price( aProject->get_cost() );
	time_t currentTime = time( 0 );
	newBid->set_open_time( currentTime );
	//newBid->set_expire_time( currentTime/60 + PROJECT_EXPIRE_TIME );	//minute
	newBid->set_expire_time( PROJECT_EXPIRE_TIME );	//minute
	newBid->set_closed_time( 0 );	// not closed.

	newBid->type(QUERY_INSERT);
	*STORE_CENTER << *newBid;

	add_bid( newBid );
	return true;
}

bool
CBlackMarket::add_new_item( CPlanet* aPlanet)
{
	if( aPlanet == NULL )
	{
		return false;
	}
	if( mPlanetList->get_by_id( aPlanet->get_id() ) != NULL )
	{
		return false;
	}

	CBid*
		newBid = new CBid();
	newBid->set_id( get_new_bid_id() );
	newBid->set_type( CBid::ITEM_PLANET );
	newBid->set_item( aPlanet->get_id() );
	newBid->set_winner_id( 0 );	// no winner yet.
	newBid->set_price( (aPlanet->get_size()+1)*(aPlanet->get_resource()+1)*1000 );
	time_t currentTime = time( 0 );
	newBid->set_open_time( currentTime );
	//newBid->set_expire_time( currentTime/60 + PLANET_EXPIRE_TIME );	//minute
	newBid->set_expire_time( PLANET_EXPIRE_TIME );	//minute
	newBid->set_closed_time( 0 );	// not closed.
	newBid->set_number_of_planet(0);

	newBid->type(QUERY_INSERT);
	*STORE_CENTER << *newBid;

	aPlanet->set_order( mPlanetList->length() );
	mPlanetList->add_planet( aPlanet );
	add_bid( newBid );
	return true;
}

void
CBlackMarket::create_new_tech()
{
	int
		level = number(100);
	int levelCap[] = { 50, 20, 10, 9, 5, 2, 2, 1, 1 };
	for(int i=0 ; i<9 ; i++)
	{
		level -= levelCap[i];
		if( level <= 0 )
		{
			level = i+2;
			break;
		}
	}
	CTechList*
		techList = TECH_TABLE->get_by_level( level );
	int
		techIndex = number( techList->length() )-1;
	CTech*
		tech = (CTech*)techList->get(techIndex);
	add_new_item(tech);

	ADMIN_TOOL->add_new_bid_log(
			(char *)format("New tech. %s has been created.", tech->get_name_with_level()));
}

void
CBlackMarket::create_new_fleet()
{
	int classDayTable[16][MAX_SHIP_CLASS] = {
			{ 43, 32, 15, 10,  0,  0,  0,  0, 0, 0},
			{ 42, 31, 16, 11,  0,  0,  0,  0, 0, 0},
			{ 36, 25, 18, 12,  9,  0,  0,  0, 0, 0},
			{ 33, 23, 19, 14, 11,  0,  0,  0, 0, 0},
			{ 31, 20, 18, 14, 11,  6,  0,  0, 0, 0},
			{ 30, 19, 18, 15, 11,  7,  0,  0, 0, 0},
			{ 28, 18, 16, 13, 11,  8,  6,  0, 0, 0},
			{ 28, 18, 15, 12, 11,  9,  7,  0, 0, 0},
			{ 25, 15, 14, 11, 11, 10,  7,  7, 0, 0},
			{ 25, 15, 14, 11, 11, 10,  7,  7, 0, 0},
			{ 23, 12, 13, 10, 11, 10,  8,  7, 6, 0},
			{ 22, 11, 12, 10, 12, 10,  8,  7, 7, 1},
			{ 20, 10, 12, 10, 12, 10,  8,  7, 7, 3},
			{ 18, 10, 12, 10, 12, 10,  9,  8, 7, 3},
			{ 13, 10, 11, 10, 13, 10,  9,  9, 8, 4},
			{  0,  0,  0,  0,  0, 10, 15, 20, 15, 10}
		};
	int classIndex = number( 100 );
	//int week = GAME->get_game_time() / (60*60*24*7);
	int day = GAME->get_game_time();
	day = day/60;
	day = day/60;
	day = day/24;
	if (day > 15)
		day = 15;

	/*
    if (SPECIAL_EVENT_ENABLED) {
        day = 15;
    }
    */
	for(int i=0 ; i < MAX_SHIP_CLASS ; i++)
	{
		classIndex -= classDayTable[day][i];
		if( classIndex <= 0 )
		{
			classIndex = i+1;
			break;
		}
	}

	int classLevelTable[MAX_SHIP_CLASS/2][MAX_TECH_LEVEL] = {
			{ 50, 30, 20,  0,  0,  0 },
			{ 30, 30, 30, 10,  0,  0 },
			{ 15, 35, 25, 15, 10,  0 },
			{ 10, 30, 25, 25, 10,  0 },
			{  0, 25, 40, 25, 10,  0 }
		};
	int levelIndex = number(100);
	for(int i=0 ; i < MAX_TECH_LEVEL; i++)
	{
		levelIndex -= classLevelTable[classIndex/2][i];
		if( levelIndex <= 0 )
		{
			levelIndex = i+1;
			break;
		}
	}

	if ((classIndex > 8) && (levelIndex < 2)) levelIndex++;

	// random ship design generate
	CShipSize *
		shipSize = SHIP_SIZE_TABLE->get_by_id( 4000 + classIndex );
	if (shipSize == NULL)
	{
		SLOG("ERROR : shipSize is NULL in CBlackMarket::create_new_fleet(), classIndex = %d", classIndex);
		return;
	}

	CShipDesign *
		shipDesign = new CShipDesign;
	shipDesign->set_design_id( mShipDesignList->max_design_id()+1 );
	CString
		name = shipSize->get_name();
		name.format( " Mk. %d", levelIndex );
	shipDesign->set_name( name );
	shipDesign->set_owner( 0 );	//owner is black market
	shipDesign->set_body( shipSize->get_id() );
	CComponentList
		*weaponList = new CComponentList,
		*deviceList = new CComponentList;
	for(int i=0 ; i<COMPONENT_TABLE->length() ; i++)
	{
		CComponent*
			component = (CComponent*)COMPONENT_TABLE->get(i);
		if( component->get_level() == levelIndex )
		{
			switch( component->get_category() )
			{
				case CComponent::CC_ARMOR:
				{
					if ( component->get_id() != 5118 ) { // Disable Amorphic I from spawning
                        shipDesign->set_armor( component->get_id() );
                    }
					break;
				}
				case CComponent::CC_ENGINE:
				{
					shipDesign->set_engine( component->get_id() );
					break;
				}
				case CComponent::CC_COMPUTER:
				{
					shipDesign->set_computer( component->get_id() );
					break;
				}
				case CComponent::CC_SHIELD:
				{
					shipDesign->set_shield( component->get_id() );
					break;
				}
			} //switch
		} //if

		if ((component->get_level() <= levelIndex ) && (component->get_category()==CComponent::CC_DEVICE))
			deviceList->add_component( component );
		if ((component->get_level() == levelIndex ) && (component->get_category()==CComponent::CC_WEAPON))
			weaponList->add_component( component );
	} //for

#define PSI_BLASTER			6105
#define PSIONIC_PULSE_SHOCKER		6208
#define PSI_STORM_LAUNCHER		6110
#define NAHODOOM_RAY_LAUNCHER		6308
#define CLOAKING_FIELD_GENERATOR	5512
#define AUTO_REPAIR_DEVICE		5514
#define INSANITY_FIELD_GENERATOR	5507
#define ANTI_PSI_DEVICE			5526
#define PSI_DRIVE			5503
#define PSI_CONTROL_SYSTEM		5506
#define MIND_TRACKER			5519

	for (int i=0 ; i<shipSize->get_weapon() ; i++)
	{
		CWeapon *
			weapon = (CWeapon *)weaponList->get(number(weaponList->length()) - 1);
		if (weapon == NULL)
		{
			SLOG("ERROR : weapon is NULL in CBlackMarket::create_new_fleet()");
			continue;
		}
		if ((weapon->get_id() != PSI_BLASTER)
			&& (weapon->get_id() != PSIONIC_PULSE_SHOCKER)
			&& (weapon->get_id() != PSI_STORM_LAUNCHER)
			&& (weapon->get_id() != NAHODOOM_RAY_LAUNCHER))
		{
			shipDesign->set_weapon(i, weapon->get_id());
			int
				WeaponNumber = shipSize->get_slot()/weapon->get_space();
			if (WeaponNumber < 1) WeaponNumber = 1;
			shipDesign->set_weapon_number(i, WeaponNumber);
		}
		else i--;
	}

	CIntegerList
		DeviceIDList;

	int MaxDevices = shipSize->get_device();
	if ((levelIndex == 1) && (MaxDevices > 3)) MaxDevices = 3;

	for (int i=0 ; i<MaxDevices ; i++)
	{
		CDevice *
			device = (CDevice *)deviceList->get(number(deviceList->length()) - 1);
		if (device == NULL)
		{
			SLOG("ERROR : device is NULL in CBlackMarket::create_new_fleet()");
			continue;
		}
		if (DeviceIDList.find_sorted_key((void *)device->get_id()) == -1) // disable duplicates
		{
			if ( ((device->get_id() == CLOAKING_FIELD_GENERATOR) && (classIndex > 5))
				|| ((device->get_id() == AUTO_REPAIR_DEVICE) && (classIndex < 6))
				|| ((device->get_id() == ANTI_PSI_DEVICE))
				|| ((device->get_id() == PSI_DRIVE))
				|| ((device->get_id() == PSI_CONTROL_SYSTEM))
				|| ((device->get_id() == MIND_TRACKER))
				|| ((device->get_id() == INSANITY_FIELD_GENERATOR))
                || ((device->get_id() >= 5537)) ) // Disable special devices from ever spawning
				i--;
			else
			{
				DeviceIDList.insert_sorted((void *)device->get_id());
				shipDesign->set_device(i, device->get_id());
			}
		}
		else i--;
	}
	shipDesign->set_build_cost(shipSize->get_cost());

#undef PSI_BLASTER
#undef PSIONIC_PULSE_SHOCKER
#undef PSI_STORM_LAUNCHER
#undef NAHODOOM_RAY_LAUNCHER
#undef CLOAKING_FIELD_GENERATOR
#undef AUTO_REPAIR_DEVICE
#undef INSANITY_FIELD_GENERATOR
#undef ANTI_PSI_DEVICE
#undef PSI_DRIVE
#undef PSI_CONTROL_SYSTEM
#undef MIND_TRACKER

	// random fleet generate
	int base[MAX_SHIP_CLASS] = { 5, 4, 3, 3, 2, 2, 1, 1, 1, 1};
	int baseDice[MAX_SHIP_CLASS] = { 4, 4, 3, 2, 3, 3, 2, 2, 0, 0};
	int dayDice[MAX_SHIP_CLASS] = { 5, 5, 4, 2, 2, 2, 3, 2, 2, 1};
	int dayRoll[MAX_SHIP_CLASS] = { 2, 2, 2, 2, 2, 2, 3, 3, 3, 4};

	int ships = base[classIndex-1]
				+ number( baseDice[classIndex-1] );
	for(int i=0 ; i<day/dayRoll[classIndex-1] ; i++)
		ships += number( dayDice[classIndex-1] );

	if(ships > 40) ships = 40;

	if(ships < 1) ships = 1;

	CFleet*
		fleet = new CFleet;
	fleet->set_ship_class( shipDesign );
	fleet->set_owner( 0 );	//owner is black market
	fleet->set_id(CEmpire::mMaxEmpireFleetID + 1);
	name.clear();
	name = shipDesign->get_name();
	name.format( " * %d ", ships );
	fleet->set_name( name );
	fleet->set_admiral( 0 );	// there is no admiral
	fleet->set_max_ship( ships );
	fleet->set_current_ship(ships );

	if (add_new_item(fleet, shipDesign) == true)
	{
		shipDesign->type(QUERY_INSERT);
		*STORE_CENTER << *shipDesign;
		fleet->type(QUERY_INSERT);
		*STORE_CENTER << *fleet;
	}
	else
	{
		SLOG("ERROR : Duplicate fleet id %d or class id %d int create_new_fleet()",
				fleet->get_id(), shipDesign->get_design_id());
	}

	ADMIN_TOOL->add_new_bid_log(
			(char *)format("New fleet %s has been created.", fleet->get_nick()));
	//SLOG( "end CBlackMarket::create_new_fleet()" );
}

void
CBlackMarket::create_new_admiral()
{
	//int week = GAME->get_game_time() / (60*60*24*7);

	int day = GAME->get_game_time();
	day = day/60;
	day = day/60;
	day = day/24;

	int genius = day - 5;
	int level = number(day);

    CAdmiral*
		admiral = new CAdmiral(level, genius, 0, -1);

	/*CAdmiral*
		admiral = new CAdmiral(level, genius, 0, -1);    */

	if (add_new_item(admiral) == true)
	{
		admiral->type(QUERY_INSERT);
		*STORE_CENTER << *admiral;
	}
	else
	{
		SLOG("ERROR : Duplicate admiral id %d in create_new_admira()", admiral->get_id());
	}

	ADMIN_TOOL->add_new_bid_log(
			(char *)format("New admiral %s has been created.", admiral->get_nick()));
}

void
CBlackMarket::create_new_project()
{
	int aNumber = 0;

	CProjectList BMProjectList;

    #define UNITY 8004
	#define INGENUITY 8005
	#define RESPONSIBILITY 8006

	for (int i= 0; i < PROJECT_TABLE->length(); i++)
	{
		CProject *aProject = (CProject*)PROJECT_TABLE->get(i);
  		if (aProject == NULL)
  		    continue;
        if (aProject->get_type() == CProject::TYPE_BM) {
            // Ensure that ultimate projects have low spawn rates
            if (aProject->get_id() == UNITY || aProject->get_id() == INGENUITY || aProject->get_id() == RESPONSIBILITY) {
                //SLOG("%d detected", aProject->get_id());
                if (number(2) == 1) {
                    //SLOG("Keeping", aProject->get_id());
                    BMProjectList.add_project(aProject);
                } else {
                    //SLOG("Excluding", aProject->get_id());
                    // Do nothing
                }
            } else {
  		        BMProjectList.add_project(aProject);
            }
        }
//  		SLOG("Project Type %d Name %s", aProject->get_type(), aProject->get_name());

	}

	#undef UNITY
	#undef INGENUITY
	#undef RESPONSIBILITY

	if (BMProjectList.length() < 1)
	    return;
//	SLOG("BMProjectList.length() -- %d", BMProjectList.length());
	aNumber = number(BMProjectList.length())-1;
//	SLOG("RANDOM NUMBER: %d", aNumber);
	CProject *project = (CProject*)BMProjectList.get(aNumber);

    if (add_new_item(project))
    {
		ADMIN_TOOL->add_new_bid_log(
			(char *)format("New project %s has been created.", project->get_name()));
	}
	else
	{
		ADMIN_TOOL->add_new_bid_log(
			(char *)format("Error adding project %s.", project->get_name()));
	}
}

void
CBlackMarket::create_new_planet()
{
	int
		clusterID = number( UNIVERSE->length() -1 );
	CCluster*
		cluster = (CCluster*)UNIVERSE->get( clusterID );
	CPlanet*
		planet = new CPlanet;
	int
		raceID = number( 10 );
	CRace
		race;
	race.set_id( raceID );
	planet->initialize( &race );
	planet->set_owner_id( 0 );
	planet->set_cluster( cluster );
	planet->set_name( cluster->get_new_planet_name() );
	planet->change_population( 50000 );
	planet->set_size(number(5)-1);
	planet->set_resource(number(3)+1);

	if( add_new_item( planet ) == true )
	{
		PLANET_TABLE->add_planet(planet);
		planet->type(QUERY_INSERT);
		STORE_CENTER->store(*planet);
	}
	else
	{
		SLOG( "duplicate planet id %d in create_new_planet", planet->get_id() );
	}

	ADMIN_TOOL->add_new_bid_log(
			(char *)format("New planet %s has been created.", planet->get_name()));
}

void
CBlackMarket::add_item( CShipDesign* aDesign )
{
	mShipDesignList->add_ship_design(aDesign);
}

void
CBlackMarket::add_item( CFleet* aFleet )
{
	mFleetList->add_fleet(aFleet);
}

void
CBlackMarket::add_item( CAdmiral* aAdmiral )
{
	mAdmiralList->add_admiral( aAdmiral );
}

void
CBlackMarket::add_item( CPlanet* aPlanet )
{
	mPlanetList->add_planet( aPlanet );
}

void
CBlackMarket::expire(CBid *aBid)
{
	if (aBid == NULL)
	{
		SLOG("ERROR : aBid is NULL in CBlackMarket::expire()");
		return;
	}

	if (aBid->get_winner_id() != 0)
	{
		CPlayer *
			winner = PLAYER_TABLE->get_by_game_id(aBid->get_winner_id());
		if (winner == NULL)
		{
			SLOG("ERROR : winner is NULL in CBlackMarket::expire(), aBid->get_id() = %d, aBid->get_winner_id() = %d", aBid->get_id(), aBid->get_winner_id());
			// TODO: relist item or handle this better
			aBid->set_winner_id(0);
			aBid->set_open_time(time(0));
			aBid->set_closed_time(0);

			switch (aBid->get_type())
			{
                case CBid::ITEM_TECH:
                {
                     //aBid->set_price();
                     aBid->set_expire_time(TECH_EXPIRE_TIME);
                     break;
                }
                case CBid::ITEM_FLEET:
                {
                     //aBid->set_price();
                     aBid->set_expire_time(FLEET_EXPIRE_TIME);
                     break;
                }
                case CBid::ITEM_ADMIRAL:
                {
                     //aBid->set_price();
                     aBid->set_expire_time(ADMIRAL_EXPIRE_TIME);
                     break;
                }
                case CBid::ITEM_PROJECT:
                {
                     //aBid->set_price();
                     aBid->set_expire_time(PROJECT_EXPIRE_TIME);
                     break;
                }
                case CBid::ITEM_PLANET:
                {
                     //aBid->set_price();
                     aBid->set_expire_time(PLANET_EXPIRE_TIME);
                     break;
                }
            }

			return;
		}

		switch (aBid->get_type())
		{
			case CBid::ITEM_TECH:
			{
				winner->discover_tech(aBid->get_item());
				CTech *
					tech = TECH_TABLE->get_by_id(aBid->get_item());
				CString
					news;
				news.format(GETTEXT("You won the bid and learned %1$s."),
							tech->get_name_with_level());
				winner->time_news(news);

				ADMIN_TOOL->add_winning_bid_log(
						(char *)format("The player %s has won the bid for the tech. %s with %d PP.",
										winner->get_nick(),
										tech->get_name_with_level(),
										aBid->get_price()));

				break;
			}
			case CBid::ITEM_FLEET:
			{
				CFleet *
					fleet = mFleetList->get_by_id(aBid->get_item());
				if (fleet == NULL)
				{
					SLOG("ERROR : fleet is NULL in CBlackMarket::expire(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					break;
				}
				CShipDesign *
					design = mShipDesignList->get_by_id(fleet->get_design_id());
				if (design == NULL)
				{
					SLOG("ERROR : design is NULL in CBlackMarket::expire(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					break;
				}
				CAdmiral *
					admiral = mAdmiralList->get_by_id(fleet->get_admiral_id());
				if (admiral == NULL)
				{
					SLOG("ERROR : admiral is NULL in CBlackMarket::expire(), aBid->get_id() = %d, aBid->get_type() = %d, fleet->get_admiral_id() = %d", aBid->get_id(), aBid->get_type(), fleet->get_admiral_id());
					break;
				}

				// Admiral Update
				mAdmiralList->remove_without_free_admiral(admiral->get_id());
				admiral->set_owner(winner->get_game_id());
				winner->get_admiral_list()->add_admiral(admiral);

				bool
					CanDelete = true;
				// Ship Design Update
				for (int i=0 ; i<mFleetList->length() ; i++)
				{
					CFleet *
						TempFleet = (CFleet *)mFleetList->get(i);
					if (TempFleet->get_id() == fleet->get_id()) continue;
					if (TempFleet->get_design_id() == design->get_design_id())
					{
						CanDelete = false;
						break;
					}
				}

				CShipDesign *
					NewShipDesign = new CShipDesign();
				*NewShipDesign = *design;

				NewShipDesign->set_owner(winner->get_game_id());
				NewShipDesign->set_design_id(winner->get_ship_design_list()->max_design_id()+1);
				NewShipDesign->make_black_market_design();
				winner->get_ship_design_list()->add_ship_design(NewShipDesign);

				NewShipDesign->type(QUERY_INSERT);
				STORE_CENTER->store(*NewShipDesign);

				if (CanDelete == true)
				{
					mShipDesignList->remove_without_free_ship_design(design->get_design_id());

					design->type(QUERY_DELETE);
					STORE_CENTER->store(*design);
				}

				// Fleet Update
				mFleetList->remove_without_free_fleet( fleet->get_id() );

				fleet->type(QUERY_DELETE);
				STORE_CENTER->store(*fleet);

				fleet->set_id( winner->get_fleet_list()->get_new_fleet_id() );
				fleet->set_owner( winner->get_game_id() );
				fleet->set_design_id( NewShipDesign->get_design_id() );
				fleet->change_exp(10);
				winner->get_fleet_list()->add_fleet(fleet);

				fleet->type(QUERY_INSERT);
				STORE_CENTER->store(*fleet);


				admiral->set_fleet_number(fleet->get_id());
				admiral->type(QUERY_UPDATE);
				*STORE_CENTER << *admiral;

				CString
					news;
				news.clear();
				news.format(GETTEXT("You won the bid and got the fleet %1$s."),
							fleet->get_name());
				winner->time_news( news );

				ADMIN_TOOL->add_winning_bid_log(
						(char *)format("The player %s has won the bid for the fleet %s with %d PP.",
										winner->get_nick(),
										fleet->get_nick(),
										aBid->get_price()));

				break;
			}
			case CBid::ITEM_ADMIRAL:
			{
				CAdmiral
					*admiral = mAdmiralList->get_by_id( aBid->get_item() );
				if( admiral == NULL )
				{
					SLOG("ERROR : admiral is NULL in CBlackMarket::expire(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					break;
				}
				mAdmiralList->remove_without_free_admiral( admiral->get_id() );
				admiral->set_owner( winner->get_game_id() );
				winner->add_admiral_pool( admiral );
				admiral->type(QUERY_UPDATE);
				*STORE_CENTER << *admiral;
				CString
					news;
				news.clear();
				news.format(GETTEXT("You won the bid and got the admiral %1$s."),
							admiral->get_name());
				winner->time_news( news );

				ADMIN_TOOL->add_winning_bid_log(
						(char *)format("The player %s has won the bid for the admiral %s with %d PP.",
										winner->get_nick(),
										admiral->get_nick(),
										aBid->get_price()));

				break;
			}
			case CBid::ITEM_PROJECT:
			{
        		if(!winner->has_project(aBid->get_item()))
                   winner->buy_bm_project( aBid->get_item() );
				CProject*
					project = PROJECT_TABLE->get_by_id( aBid->get_item() );
				CString
					news;
				news.clear();
				news.format(GETTEXT("You won the bid and achieved %1$s."),
							project->get_name());
				winner->time_news( news );
				
				#define UNITY 8004
				#define INGENUITY 8005
				#define RESPONSIBILITY 8006

				if (project->get_id() == UNITY || project->get_id() == INGENUITY || project->get_id() == RESPONSIBILITY) {
                    news.clear();
                    news.format("The player %s has won the bid for the project %s.",
                                 winner->get_nick(),
                                 project->get_name());
                    for (int i = 0; i < PLAYER_TABLE->length(); i++) {
                        if (PLAYER_TABLE->get_by_game_id(i)) {
                             PLAYER_TABLE->get_by_game_id(i)->time_news( news );
                        }
                    }
                }

                #undef UNITY
                #undef INGENUITY
                #undef RESPONSIBILITY

				ADMIN_TOOL->add_winning_bid_log(
						(char *)format("The player %s has won the bid for the project %s with %d PP.",
										winner->get_nick(),
										project->get_name(),
										aBid->get_price()));

				break;
			}
			case CBid::ITEM_PLANET:
			{
				CPlanet
					*planet = mPlanetList->get_by_id( aBid->get_item() );
				if( planet == NULL )
				{
					SLOG("ERROR : planet is NULL in CBlackMarket::expire(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					break;
				}
				mPlanetList->remove_planet( planet->get_id() );
				planet->set_order( winner->get_planet_list()->length() );
				winner->add_planet( planet );
				planet->start_terraforming();
				planet->type(QUERY_UPDATE);
				//*STORE_CENTER << *planet;
				STORE_CENTER->store(*planet);
				CString
					news;
				news.clear();
				news.format(GETTEXT("You won the bid and got the planet %1$s."),
							planet->get_name());
				winner->time_news( news );

				ADMIN_TOOL->add_winning_bid_log(
						(char *)format("The player %s has won the bid for the planet %s with %d PP.",
										winner->get_nick(),
										planet->get_name(),
										aBid->get_price()));

				break;
			}
		}
	}
	else	// nobodys bid.
	{
		switch (aBid->get_type())
		{
			case CBid::ITEM_TECH:
			{
				// no processing is required
				break;
			}
			case CBid::ITEM_FLEET:
			{
				CFleet *
					fleet = mFleetList->get_by_id(aBid->get_item());
				if (fleet == NULL)
				{
					SLOG("ERROR : fleet is NULL in CBlackMarket::expire(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					break;
				}
				CShipDesign *
					design = mShipDesignList->get_by_id(fleet->get_design_id());
				if (design == NULL)
				{
					SLOG("ERROR : design is NULL in CBlackMarket::expire(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					break;
				}
				design->type(QUERY_DELETE);
				STORE_CENTER->store(*design);
				mShipDesignList->remove_ship_design(design->get_design_id());

				fleet->type(QUERY_DELETE);
				STORE_CENTER->store(*fleet);
				mFleetList->remove_fleet(fleet->get_id());

				break;
			}
			case CBid::ITEM_ADMIRAL:
			{
				CAdmiral *
					admiral = mAdmiralList->get_by_id(aBid->get_item());
				if (admiral == NULL)
				{
					SLOG("ERROR : admiral is NULL in CBlackMarket::expire(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					break;
				}
				admiral->type(QUERY_DELETE);
				STORE_CENTER->store(*admiral);
				mAdmiralList->remove_admiral(aBid->get_item());
				break;
			}
			case CBid::ITEM_PROJECT:
			{
				// no processing is required
				break;
			}
			case CBid::ITEM_PLANET:
			{
				CPlanet *
					planet = mPlanetList->get_by_id(aBid->get_item());
				if (planet == NULL)
				{
					SLOG("ERROR : planet is NULL in CBlackMarket::expire(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					break;
				}
				planet->type(QUERY_DELETE);
				STORE_CENTER->store(*planet);

				mPlanetList->remove_planet(aBid->get_item());
				PLANET_TABLE->remove_planet(aBid->get_item());
				break;
			}
		}
	}

	aBid->type(QUERY_DELETE);
	STORE_CENTER->store(*aBid);
	mBidList->remove_bid(aBid->get_id());
}

void
CBlackMarket::expire_all()
{
 for (int i=0; i < mBidList->length(); i++)
 {
    CBid *aBid = (CBid*)mBidList->get(i);
    if (aBid == NULL)
	{
		SLOG("ERROR : aBid is NULL in CBlackMarket::expire_all()");
		return;
	}

	if (aBid->get_winner_id() != 0)
	{
		CPlayer *
			winner = PLAYER_TABLE->get_by_game_id(aBid->get_winner_id());
		if (winner == NULL)
		{
			SLOG("ERROR : winner is NULL in CBlackMarket::expire_all(), aBid->get_id() = %d, aBid->get_winner_id() = %d", aBid->get_id(), aBid->get_winner_id());
			return;
		}

		switch (aBid->get_type())
		{
			case CBid::ITEM_TECH:
			{
				winner->discover_tech(aBid->get_item());
				CTech *
					tech = TECH_TABLE->get_by_id(aBid->get_item());
				CString
					news;
				news.format(GETTEXT("You won the bid and learned %1$s."),
							tech->get_name_with_level());
				winner->time_news(news);

				ADMIN_TOOL->add_winning_bid_log(
						(char *)format("The player %s has won the bid for the tech. %s with %d PP.",
										winner->get_nick(),
										tech->get_name_with_level(),
										aBid->get_price()));

				break;
			}
			case CBid::ITEM_FLEET:
			{
				CFleet *
					fleet = mFleetList->get_by_id(aBid->get_item());
				if (fleet == NULL)
				{
					SLOG("ERROR : fleet is NULL in CBlackMarket::expire_all(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					return;
				}
				CShipDesign *
					design = mShipDesignList->get_by_id(fleet->get_design_id());
				if (design == NULL)
				{
					SLOG("ERROR : design is NULL in CBlackMarket::expire_all(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					return;
				}
				CAdmiral *
					admiral = mAdmiralList->get_by_id(fleet->get_admiral_id());
				if (admiral == NULL)
				{
					SLOG("ERROR : admiral is NULL in CBlackMarket::expire_all(), aBid->get_id() = %d, aBid->get_type() = %d, fleet->get_admiral_id() = %d", aBid->get_id(), aBid->get_type(), fleet->get_admiral_id());
					return;
				}

				// Admiral Update
				mAdmiralList->remove_without_free_admiral(admiral->get_id());
				admiral->set_owner(winner->get_game_id());
				winner->get_admiral_list()->add_admiral(admiral);

				admiral->type(QUERY_UPDATE);
				*STORE_CENTER << *admiral;

				bool
					CanDelete = true;
				// Ship Design Update
				for (int i=0 ; i<mFleetList->length() ; i++)
				{
					CFleet *
						TempFleet = (CFleet *)mFleetList->get(i);
					if (TempFleet->get_id() == fleet->get_id()) continue;
					if (TempFleet->get_design_id() == design->get_design_id())
					{
						CanDelete = false;
						break;
					}
				}

				CShipDesign *
					NewShipDesign = new CShipDesign();
				*NewShipDesign = *design;

				NewShipDesign->set_owner(winner->get_game_id());
				NewShipDesign->set_design_id(winner->get_ship_design_list()->max_design_id()+1);
				winner->get_ship_design_list()->add_ship_design(NewShipDesign);

				NewShipDesign->type(QUERY_INSERT);
				STORE_CENTER->store(*NewShipDesign);

				if (CanDelete == true)
				{
					mShipDesignList->remove_without_free_ship_design(design->get_design_id());

					design->type(QUERY_DELETE);
					STORE_CENTER->store(*design);
				}

				// Fleet Update
				mFleetList->remove_without_free_fleet( fleet->get_id() );

				fleet->type(QUERY_DELETE);
				STORE_CENTER->store(*fleet);

				fleet->set_id( winner->get_fleet_list()->get_new_fleet_id() );
				fleet->set_owner( winner->get_game_id() );
				fleet->set_design_id( NewShipDesign->get_design_id() );
				winner->get_fleet_list()->add_fleet(fleet);

				fleet->type(QUERY_INSERT);
				STORE_CENTER->store(*fleet);

				CString
					news;
				news.clear();
				news.format(GETTEXT("You won the bid and got the fleet %1$s."),
							fleet->get_name());
				winner->time_news( news );

				ADMIN_TOOL->add_winning_bid_log(
						(char *)format("The player %s has won the bid for the fleet %s with %d PP.",
										winner->get_nick(),
										fleet->get_nick(),
										aBid->get_price()));

				break;
			}
			case CBid::ITEM_ADMIRAL:
			{
				CAdmiral
					*admiral = mAdmiralList->get_by_id( aBid->get_item() );
				if( admiral == NULL )
				{
					SLOG("ERROR : admiral is NULL in CBlackMarket::expire_all(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					return;
				}
				mAdmiralList->remove_without_free_admiral( admiral->get_id() );
				admiral->set_owner( winner->get_game_id() );
				winner->add_admiral_pool( admiral );
				admiral->type(QUERY_UPDATE);
				*STORE_CENTER << *admiral;
				CString
					news;
				news.clear();
				news.format(GETTEXT("You won the bid and got the admiral %1$s."),
							admiral->get_name());
				winner->time_news( news );

				ADMIN_TOOL->add_winning_bid_log(
						(char *)format("The player %s has won the bid for the admiral %s with %d PP.",
										winner->get_nick(),
										admiral->get_nick(),
										aBid->get_price()));

				break;
			}
			case CBid::ITEM_PROJECT:
			{
				winner->buy_bm_project( aBid->get_item() );
				CProject*
					project = PROJECT_TABLE->get_by_id( aBid->get_item() );
				CString
					news;
				news.clear();
				news.format(GETTEXT("You won the bid and achieved %1$s."),
							project->get_name());
				winner->time_news( news );
				
				#define UNITY 8004
				#define INGENUITY 8005
				#define RESPONSIBILITY 8006

				if (project->get_id() == UNITY || project->get_id() == INGENUITY || project->get_id() == RESPONSIBILITY) {
                    news.clear();
                    news.format("The player %s has won the bid for the project %s.",
                                 winner->get_nick(),
                                 project->get_name());
                    for (int i = 0; i < PLAYER_TABLE->length(); i++) {
                        if (PLAYER_TABLE->get_by_game_id(i)) {
                             PLAYER_TABLE->get_by_game_id(i)->time_news( news );
                        }
                    }
                }

                #undef UNITY
                #undef INGENUITY
                #undef RESPONSIBILITY

				ADMIN_TOOL->add_winning_bid_log(
						(char *)format("The player %s has won the bid for the project %s with %d PP.",
										winner->get_nick(),
										project->get_name(),
										aBid->get_price()));

				break;
			}
			case CBid::ITEM_PLANET:
			{
				CPlanet
					*planet = mPlanetList->get_by_id( aBid->get_item() );
				if( planet == NULL )
				{
					SLOG("ERROR : planet is NULL in CBlackMarket::expire_all(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					return;
				}
				mPlanetList->remove_planet( planet->get_id() );
				planet->set_order( winner->get_planet_list()->length() );
				winner->add_planet( planet );
				planet->type(QUERY_UPDATE);
				//*STORE_CENTER << *planet;
				STORE_CENTER->store(*planet);
				CString
					news;
				news.clear();
				news.format(GETTEXT("You won the bid and got the planet %1$s."),
							planet->get_name());
				winner->time_news( news );

				ADMIN_TOOL->add_winning_bid_log(
						(char *)format("The player %s has won the bid for the planet %s with %d PP.",
										winner->get_nick(),
										planet->get_name(),
										aBid->get_price()));

				break;
			}
		}
	}
	else	// nobodys bid.
	{
		switch (aBid->get_type())
		{
			case CBid::ITEM_TECH:
			{
				// no processing is required
				break;
			}
			case CBid::ITEM_FLEET:
			{
				CFleet *
					fleet = mFleetList->get_by_id(aBid->get_item());
				if (fleet == NULL)
				{
					SLOG("ERROR : fleet is NULL in CBlackMarket::expire_all(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					return;
				}
				CShipDesign *
					design = mShipDesignList->get_by_id(fleet->get_design_id());
				if (design == NULL)
				{
					SLOG("ERROR : design is NULL in CBlackMarket::expire_all(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					return;
				}
				design->type(QUERY_DELETE);
				STORE_CENTER->store(*design);
				mShipDesignList->remove_ship_design(design->get_design_id());

				fleet->type(QUERY_DELETE);
				STORE_CENTER->store(*fleet);
				mFleetList->remove_fleet(fleet->get_id());

				break;
			}
			case CBid::ITEM_ADMIRAL:
			{
				CAdmiral *
					admiral = mAdmiralList->get_by_id(aBid->get_item());
				if (admiral == NULL)
				{
					SLOG("ERROR : admiral is NULL in CBlackMarket::expire_all(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					return;
				}
				admiral->type(QUERY_DELETE);
				STORE_CENTER->store(*admiral);
				mAdmiralList->remove_admiral(aBid->get_item());
				break;
			}
			case CBid::ITEM_PROJECT:
			{
				// no processing is required
				break;
			}
			case CBid::ITEM_PLANET:
			{
				CPlanet *
					planet = mPlanetList->get_by_id(aBid->get_item());
				if (planet == NULL)
				{
					SLOG("ERROR : planet is NULL in CBlackMarket::expire_all(), aBid->get_id() = %d, aBid->get_type() = %d, aBid->get_item() = %d", aBid->get_id(), aBid->get_type(), aBid->get_item());
					return;
				}
				planet->type(QUERY_DELETE);
				STORE_CENTER->store(*planet);

				mPlanetList->remove_planet(aBid->get_item());
				PLANET_TABLE->remove_planet(aBid->get_item());
				break;
			}
		}
	}

	aBid->type(QUERY_DELETE);
	STORE_CENTER->store(*aBid);
	mBidList->remove_bid(aBid->get_id());
 }
}

void
CBlackMarket::bid(int aBidID, int aPlayerID, int price)
{
	CBid *
		bid = mBidList->get_by_id(aBidID);
	if (bid == NULL) return;

	if (bid->get_price() * 1.05 > price) return;
	if (bid->get_winner_id() == aPlayerID) return;

	CPlayer *
		winner = PLAYER_TABLE->get_by_game_id(aPlayerID);
	if (winner == NULL) return;

	if (bid->get_winner_id() != 0)
	{
		CPlayer *
			previousWinner = PLAYER_TABLE->get_by_game_id(bid->get_winner_id());
		if (previousWinner != NULL)
		{
			if (previousWinner->is_dead() == false)
			{
				CString
					news;
				switch (bid->get_type())
				{
					case CBid::ITEM_TECH:
					{
						previousWinner->change_reserved_production( bid->get_price() );
						CTech*
							tech = TECH_TABLE->get_by_id( bid->get_item() );
						news.format(GETTEXT("Other player outbidded for the tech %1$s."),
									tech->get_name_with_level());
						break;
					}
					case CBid::ITEM_FLEET:
					{
						previousWinner->change_reserved_production( bid->get_price() );
						CFleet*
							fleet = mFleetList->get_by_id( bid->get_item() );
						CAdmiral*
							admiral = mAdmiralList->get_by_id( fleet->get_admiral_id() );
						mAdmiralList->remove_without_free_admiral( admiral->get_id() );
						admiral->set_owner( previousWinner->get_game_id() );
						previousWinner->get_admiral_pool()->add_admiral( admiral );
						admiral->type(QUERY_UPDATE);
						*STORE_CENTER << *admiral;
						news.format(GETTEXT("Other player outbidded for the fleet %1$s."),
									fleet->get_name());
						break;
					}
					case CBid::ITEM_ADMIRAL:
					{
						previousWinner->change_reserved_production( bid->get_price() );
						CAdmiral*
							admiral = mAdmiralList->get_by_id( bid->get_item() );

						news.format(GETTEXT("Other player outbidded for the admiral %1$s."),
									admiral->get_nick());
						break;
					}
					case CBid::ITEM_PROJECT:
					{
						previousWinner->change_reserved_production( bid->get_price() );
						CProject*
							project = PROJECT_TABLE->get_by_id( bid->get_item() );
						news.format(GETTEXT("Other player outbidded for the project %1$s."),
									project->get_name());
						break;
					}
					case CBid::ITEM_PLANET:
					{
						previousWinner->change_reserved_production(bid->get_price() * bid->get_number_of_planet());
						CPlanet
							*planet = mPlanetList->get_by_id( bid->get_item() );
						news.format(GETTEXT("Other player outbidded for the planet %1$s."),
									planet->get_name());
						break;
					}
				}//switch
				previousWinner->time_news( news );
			}
			else	// previousWinner->is_dead() == TRUE
			{
				if(bid->get_type() == CBid::ITEM_FLEET)
				{
					CFleet*
						fleet = mFleetList->get_by_id( bid->get_item() );
					CAdmiral*
						admiral = mAdmiralList->get_by_id( fleet->get_admiral_id() );
					admiral->type(QUERY_DELETE);
					*STORE_CENTER << *admiral;
					mAdmiralList->remove_admiral( admiral->get_id() );
				}
			}
		}
	}
	bid->set_winner_id( aPlayerID );
	if( bid->get_type() != CBid::ITEM_PLANET )
	{
		winner->change_reserved_production( -price );
	}
	else
	{
		winner->change_reserved_production( -price * (BLACK_MARKET_PLANET_COUNT_OFFSET + winner->get_planet_list()->length()) );
		bid->set_number_of_planet(BLACK_MARKET_PLANET_COUNT_OFFSET + winner->get_planet_list()->length());
	}
	if (bid->get_expire_time() < mBidExpireTime)
	{
		bid->set_expire_time(mBidExpireTime);
	}
	bid->set_price( price );

	bid->type(QUERY_UPDATE);
	*STORE_CENTER << *bid;

	winner->type(QUERY_UPDATE);
	STORE_CENTER->store(*winner);
}

bool
CBlackMarket::initialize(CMySQL *aMySQL)
{
	SLOG("Initializing BlackMarket");

	//aMySQL->query("LOCK TABLE class READ");
	aMySQL->query("SELECT * FROM class WHERE owner = 0");
	aMySQL->use_result();

	while (aMySQL->fetch_row())
	{
		CShipDesign *
			Design = new CShipDesign(aMySQL->row());
		add_item(Design);
	}

	aMySQL->free_result();
	//aMySQL->query("UNLOCK TABLES");

	load(aMySQL);

	for (int i=0 ; i<mBidList->length() ; i++)
	{
		CBid *
			Bid = (CBid *)mBidList->get(i);

		switch (Bid->get_type())
		{
			case CBid::ITEM_TECH :
				break;

			case CBid::ITEM_FLEET :
			{
				//aMySQL->query("LOCK TABLE fleet READ");
				aMySQL->query((char *)format("SELECT * FROM fleet WHERE owner = 0 AND id = %d", Bid->get_item()));
				aMySQL->use_result();

				if (aMySQL->fetch_row())
				{
					CFleet *
						Fleet = new CFleet(mShipDesignList, aMySQL->row());
					mFleetList->add_fleet(Fleet);

					CEmpire::update_max_empire_fleet_id(Fleet);
				}
				else
				{
					SLOG("ERROR : No Fleet(#d) found in CBlackMarket::initialize()", Bid->get_item());
					Bid->type(QUERY_DELETE);
					STORE_CENTER->store(*Bid);
					mBidList->remove_bid(Bid->get_id());
				}

				aMySQL->free_result();
				//aMySQL->query("UNLOCK TABLES");
			}
				break;

			case CBid::ITEM_ADMIRAL :
			{
				//aMySQL->query("LOCK TABLE admiral READ");
				aMySQL->query((char *)format("SELECT * FROM admiral WHERE owner = 0 AND id = %d", Bid->get_item()));
				aMySQL->use_result();

				if (aMySQL->fetch_row())
				{
					CAdmiral *
						Admiral = new CAdmiral(aMySQL->row());
					mAdmiralList->add_admiral(Admiral);
				}
				else
				{
					SLOG("ERROR : No Admiral(%d) found in CBlackMarket::initialize()", Bid->get_item());
					Bid->type(QUERY_DELETE);
					STORE_CENTER->store(*Bid);
					mBidList->remove_bid(Bid->get_id());
				}

				aMySQL->free_result();
				//aMySQL->query("UNLOCK TABLES");
			}
				break;

			case CBid::ITEM_PROJECT :
				break;

			case CBid::ITEM_PLANET :
			{
				//aMySQL->query("LOCK TABLE planet READ");
				aMySQL->query((char *)format("SELECT * FROM planet WHERE owner = 0 AND id = %d", Bid->get_item()));
				aMySQL->use_result();

				if (aMySQL->fetch_row())
				{
					CPlanet *
						Planet = new CPlanet(aMySQL->row());
					mPlanetList->add_planet(Planet);
					PLANET_TABLE->add_planet(Planet);
				}
				else
				{
					SLOG("ERROR : No Planet(%d) found in CBlackMarket::initialize()", Bid->get_item());
					Bid->type(QUERY_DELETE);
					STORE_CENTER->store(*Bid);
					mBidList->remove_bid(Bid->get_id());
				}

				aMySQL->free_result();
				//aMySQL->query("UNLOCK TABLES");
			}
				break;

			default :
			{
				SLOG("ERROR : Wrong bid type in CBlackMarket::initialize() (%d)", Bid->get_type());
				Bid->type(QUERY_DELETE);
				STORE_CENTER->store(*Bid);
				mBidList->remove_bid(Bid->get_id());
			}
				break;
		}
	}

	return true;
}

void
CBlackMarket::add_planet( CPlanet *aPlanet )
{
	mPlanetList->add_planet( aPlanet );
}

const char*
CBlackMarketHTML::get_tech_list_html(CPlayer *aPlayer)
{
	static CString
		html;
	html.clear();
	html.format( "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n" );
	html.format( "<TR BGCOLOR=\"#171717\"><TH CLASS=\"tabletxt\" COLOR=\"#666666\">&nbsp;</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">%s</TH>", GETTEXT("Technology(Level)") );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">%s</TH>", GETTEXT("Prerequisite(s)") );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">%s</TH>", GETTEXT("Time Remaining") );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">%s</TH></TR>\n", GETTEXT("Current Bid") );
	CBidList
		*bidList = BLACK_MARKET->get_bid_list();
	for( int i=0 ; i<bidList->length() ; i++ )
	{
		CBid
			*bid = (CBid*)bidList->get(i);
		if( bid->get_type() == CBid::ITEM_TECH )
		{
			int
				techID = bid->get_item();
			CTech *
				tech = TECH_TABLE->get_by_id(techID);
			if (tech == NULL)
			{
				SLOG("ERROR : tech is NULL in CBlackMarketHTML::get_tech_list_html(), bid->get_id() = %d, bid->get_item() = %d", bid->get_id(), bid->get_item());
				continue;
			}

			if(aPlayer->has_tech(techID))
			{
				html.format("<TR><TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">&nbsp</TD>");
				html.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\"><A HREF=\"../../encyclopedia/tech/%d.html\"><FONT COLOR=\"#0000FF\">%s(%d)</FONT></A></TD>", tech->get_id(), tech->get_name(), tech->get_level() );
			}
			else
			{
				if ((tech->evaluate(aPlayer))&&( bid->get_winner_id() != aPlayer->get_game_id() ))
				{
					html.format("<TR><TD CLASS=\"tabletxt\" ALIGN=\"CENTER\"><INPUT TYPE=RADIO NAME=TECH_ITEM_ID VALUE=%d></TD>", bid->get_id() );
				}
				else
				{
					html.format("<TR><TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">&nbsp</TD>");
				}
				html.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\"><A HREF=\"../../encyclopedia/tech/%d.html\">%s(%d)</A></TD>", tech->get_id(), tech->get_name(), tech->get_level() );
			}
			html.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">");
			html += tech->tech_description(aPlayer);
			html.format("</TD>");
			html.format( "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%d min</TD>", bid->get_expire_time() );
			//html.format( "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%d</TD></TR>\n", bid->get_price() );
			if( bid->get_winner_id() == aPlayer->get_game_id() )
			{
				html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#0000FF\">%s</TD>", dec2unit( bid->get_price() ) );
			}
			else if( bid->get_price()*1.05 > aPlayer->get_production() )
			{
				html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#FF0000\">%s</TD>", dec2unit( bid->get_price() ) );
			}
			else
			{
				html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>", dec2unit( bid->get_price() ) );
			}
			html.format( "</TR>\n");
		}
	}
	html.format( "</TABLE>" );
	return (char*)html;
}

const char*
CBlackMarketHTML::get_fleet_list_html( CPlayer *aPlayer )
{
	static CString
		html;
	html.clear();
	CFleetList*
		fleetList = BLACK_MARKET->get_fleet_list();
	CShipDesignList*
		shipDesignList = BLACK_MARKET->get_ship_design_list();
	CBidList*
		bidList = BLACK_MARKET->get_bid_list();
	html.format( "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n" );
	html.format( "<TR BGCOLOR=\"#171717\">" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">&nbsp</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">Fleet Name</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">Ship Size</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">Ship Number</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">Time Remaining</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">Current Bid</TH></TR>\n" );
	int
		maxCommanding = 0;
	for(int i=0 ; i<aPlayer->get_admiral_pool()->length() ; i++)
	{
		CAdmiral*
			admiral = (CAdmiral*)aPlayer->get_admiral_pool()->get(i);
		if( admiral->get_fleet_commanding() > maxCommanding )
		{
			maxCommanding = admiral->get_fleet_commanding();
		}
	}

	for(int i=0 ; i<bidList->length() ; i++)
	{
		CBid*
			bid = (CBid*)bidList->get(i);
		if( bid->get_type() != CBid::ITEM_FLEET )
		{
			continue;
		}
		CFleet *
			fleet = fleetList->get_by_id(bid->get_item());
		if (fleet == NULL)
		{
			SLOG("ERROR : fleet is NULL in CBlackMarketHTML::get_fleet_list_html(), bid->get_id() = %d, bid->get_item() = %d", bid->get_id(), bid->get_item());
			continue;
		}
		CShipDesign *
			shipDesign = shipDesignList->get_by_id(fleet->get_design_id());
		if (shipDesign == NULL)
		{
			SLOG("ERROR : shipDesign is NULL in CBlackMarketHTML::get_fleet_list_html(), bid->get_id() = %d, bid->get_item() = %d, fleet->get_design_id() = %d", bid->get_id(), bid->get_item(), fleet->get_design_id() );
			continue;
		}
		html.format( "<TR>" );
		if( bid->get_winner_id() == aPlayer->get_game_id()
			|| maxCommanding < fleet->get_max_ship()
			|| bid->get_price()*1.05 > aPlayer->get_production() )
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">&nbsp;</TD>" );
		}
		else
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\"><INPUT TYPE=RADIO NAME=FLEET_ITEM_ID VALUE=%d></TD>", bid->get_id() );
		}
		html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>", fleet->get_name() );
		html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>", fleet->get_size_name() );
		if( maxCommanding < fleet->get_max_ship() )
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#FF0000\">%d</TD>", fleet->get_current_ship() );
		}
		else
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%d</TD>", fleet->get_current_ship() );
		}
		html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%d min</TD>", bid->get_expire_time() );
		/*
		if( bid->get_winner_id() != aPlayer->get_game_id() )
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%d</TD>", bid->get_price() );
		}
		else
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#0000FF\">%d</TD>", bid->get_price() );
		}
		*/
		if( bid->get_winner_id() == aPlayer->get_game_id() )
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#0000FF\">%s</TD>", dec2unit( bid->get_price() ) );
		}
		else if( bid->get_price()*1.05 > aPlayer->get_production() )
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#FF0000\">%s</TD>", dec2unit( bid->get_price() ) );
		}
		else
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>", dec2unit( bid->get_price() ) );
		}
		html.format( "</TR>\n" );
	}//for
	html.format( "</TABLE>" );
	return (char*)html;
}

const char*
CBlackMarketHTML::get_admiral_list_html( CPlayer* aPlayer )
{
	static CString
		html;
	html.clear();
	html.format( "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n" );
	html.format( "<TR BGCOLOR=\"#171717\">" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">&nbsp;</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">Name</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">Race</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">Level</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">Exp</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">Fleet Commanding</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">Efficiency</TH>" );
	/*html.format( "<TH class=tabletxt COLOR=\"#666666\" colspan=\"2\">Abilities</TH>\n");*/
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">Time Remaining</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">Current Bid</TH>" );
	html.format( "</TR>\n" );
	CAdmiralList*
		admiralList = BLACK_MARKET->get_admiral_list();
	CBidList*
		bidList = BLACK_MARKET->get_bid_list();
	for( int i=0 ; i<bidList->length() ; i++ )
	{
		CBid *
			bid = (CBid *)bidList->get(i);
		if (bid == NULL)
		{
			SLOG("ERROR : bid is NULL in CBlackMarketHTML::get_admiral_list_html(), i = %d", i);
			continue;
		}
		if( bid->get_type() != CBid::ITEM_ADMIRAL )
		{
			continue;
		}
		CAdmiral *
			admiral = admiralList->get_by_id(bid->get_item());
		if (admiral == NULL)
		{
			SLOG("ERROR : admiral is NULL in CBlackMarketHTML::get_admiral_list_html(), bid->get_id() = %d, bid->get_item() = %d", bid->get_id(), bid->get_item());
			continue;
		}

		html.format("<TR>");
		if( bid->get_winner_id() != aPlayer->get_game_id()
			&& bid->get_price()*1.05 <= aPlayer->get_production())
		/*&&
            (aPlayer->get_admiral_list()->length() +
            aPlayer->get_admiral_pool()->length()) < CPlayer::mMaxAdmirals*/
		{
			//html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\"><INPUT TYPE=RADIO NAME=ADMIRAL_ITEM_ID VALUE=%d></TD>", admiral->get_id() );
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\"><INPUT TYPE=RADIO NAME=ADMIRAL_ITEM_ID VALUE=%d></TD>", bid->get_id() );
		}
		else
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">&nbsp;</TD>" );
		}
		html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>", admiral->get_name() );
		html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>", RACE_TABLE->get_name_by_id( admiral->get_race() ) );
		html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%d</TD>", admiral->get_level() );
		html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%d</TD>", admiral->get_exp() );
		html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%d</TD>", admiral->get_fleet_commanding() );
		html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%d</TD>", admiral->get_real_efficiency() );
		/*html.format( "<TD class=maintext ALIGN=center>%s</TD>\n", admiral->get_special_ability_name() );
		html.format( "<TD class=maintext ALIGN=center>%s</TD>\n", admiral->get_racial_ability_name() );*/
		html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%d min</TD>", bid->get_expire_time() );
		//html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%d</TD>", bid->get_price() );
		/*
		if( bid->get_winner_id() != aPlayer->get_game_id() )
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%d</TD>", bid->get_price() );
		}
		else
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#0000FF\">%d</TD>", bid->get_price() );
		}
		*/
		if( bid->get_winner_id() == aPlayer->get_game_id() )
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#0000FF\">%s</TD>", dec2unit( bid->get_price() ) );
		}
		else if( bid->get_price()*1.05 > aPlayer->get_production() )
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#FF0000\">%s</TD>", dec2unit( bid->get_price() ) );
		}
		else
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>", dec2unit( bid->get_price() ) );
		}
		html.format( "</TR>\n" );
	}
	html.format( "</TABLE>" );
	return (char*)html;
}

const char*
CBlackMarketHTML::get_project_list_html(CPlayer* aPlayer)
{
	static CString
		html;
	html.clear();
	html.format( "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n" );
	html.format( "<TR>\n" );
	html.format( "<TH CLASS=\"tabletxt\" WIDTH=\"38\" BGCOLOR=\"#171717\">&nbsp;</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" WIDTH=\"194\" BGCOLOR=\"#171717\">%s</TH>\n", GETTEXT("Project Name") );
	//html.format( "<TH WIDTH=\"255\" CLASS=\"tabletxt\" BGCOLOR=\"#171717\">%s\n</TH>", GETTEXT("Effect") );
	html.format( "<TH WIDTH=\"205\" CLASS=\"tabletxt\" BGCOLOR=\"#171717\">%s\n</TH>", GETTEXT("Prerequisite(s)") );
	html.format( "<TH CLASS=\"tabletxt\" BGCOLOR=\"#171717\">Time Remaining</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" BGCOLOR=\"#171717\">Current Bid</TH>" );
	html.format( "</TR>\n" );

	CBidList*
		bidList = BLACK_MARKET->get_bid_list();
	for (int i=0 ; i<bidList->length() ; i++)
	{
		CBid *
			bid = (CBid *)bidList->get(i);
		if (bid->get_type() != CBid::ITEM_PROJECT) continue;

		CProject *
			project = PROJECT_TABLE->get_by_id(bid->get_item());
		if (project == NULL)
		{
			SLOG("ERROR : project is NULL in CBlackMarketHTML::get_admiral_list_html(), bid->get_id() = %d, bid->get_item() = %d", bid->get_id(), bid->get_item());
			continue;
		}

		html += "<TR>\n";

		if(aPlayer->has_project(bid->get_item()))
		{
		    html.format("<TR><TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">&nbsp</TD>");
			html.format("<TD CLASS=\"maintext\" ALIGN=\"center\"><A HREF=\"../../encyclopedia/project/%d.html\"><FONT COLOR=\"#0000FF\">%s</FONT></A></TD>",
						project->get_id(),
                        project->get_name());
		}
		else
		{
			if ((project->evaluate(aPlayer)) &&
				aPlayer->get_game_id() != bid->get_winner_id() &&
				bid->get_price()*1.05 <= aPlayer->get_production())
			{
				html.format("<TD CLASS=\"maintext\" ALIGN=\"center\"><INPUT TYPE=RADIO NAME=PROJECT_ITEM_ID VALUE=%d></TD>", bid->get_id());
				html.format("<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#0000FF\"><A HREF=\"../../encyclopedia/project/%d.html\">%s</A></TD>", project->get_id(),project->get_name());
			}
			else
			{
				html += "<TD CLASS=\"maintext\" ALIGN=\"center\">&nbsp;</TD>";
				html.format("<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#0000FF\"><FONT COLOR=red><A HREF=\"../../encyclopedia/project/%d.html\">%s</A></FONT></TD>", project->get_id(),project->get_name());
			}

		}

//		CControlModel &
//			effect = (CControlModel &)project->get_effect();
//		html.format("<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>",
//					effect.get_effects_string_for_html() );

		html.format("<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>",
					project->tech_description(aPlayer));

//        	html.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">");
//              html += project->project_description();
		html.format("<TD CLASS=\"maintext\" ALIGN=\"center\">%s %s%s</TD>",
					dec2unit(bid->get_expire_time()), GETTEXT("min"), plural(bid->get_expire_time()));
		if (bid->get_winner_id() == aPlayer->get_game_id())
		{
			html.format("<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#0000FF\">%s</TD>", dec2unit(bid->get_price()));
		}
		else if(bid->get_price()*1.05 > aPlayer->get_production())
		{
			html.format("<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#FF0000\">%s</TD>", dec2unit(bid->get_price()));
		}
		else
		{
			html.format("<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>",
						dec2unit(bid->get_price()));
		}
		html += "</TR>\n";
	}
	html += "</TABLE>\n";

	return (char *)html;
}

const char*
CBlackMarketHTML::get_planet_list_html(CPlayer* aPlayer)
{
	static CString
		html;
	html.clear();
	html.format( "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n" );
	html.format( "<TR BGCOLOR=\"#171717\">" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">&nbsp</TH>" );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">%s</TH>", GETTEXT("Planet Name") );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">%s</TH>", GETTEXT("Cluster Name") );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">%s</TH>", GETTEXT("Population") );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">%s</TH>", GETTEXT("Environment") );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">%s</TH>", GETTEXT("Time Remaining") );
	html.format( "<TH CLASS=\"tabletxt\" COLOR=\"#666666\">%s</TH>", GETTEXT("Current Bid") );
	html.format( "</TR>\n" );

	CSortedPlanetList*
		planetList = BLACK_MARKET->get_planet_list();
	CBidList*
		bidList = BLACK_MARKET->get_bid_list();
	for( int i=0 ; i<bidList->length() ; i++ )
	{
		CBid *
			bid = (CBid *)bidList->get(i);
		if (bid == NULL)
		{
			SLOG( "bid %d NULL", i );
			continue;
		}
		if( bid->get_type() != CBid::ITEM_PLANET )
		{
			continue;
		}

		CPlanet*
			planet = planetList->get_by_id( bid->get_item() );
		if( planet == NULL )
		{
			SLOG( "bid %d planet %d NULL", bid->get_id(), bid->get_item() );
			continue;
		}
		html.format( "<TR>" );
		if( planet->calc_environment_control(aPlayer) > -10
			&& bid->get_winner_id() != aPlayer->get_game_id()
			&& bid->get_price()*1.05 <= aPlayer->get_production() )
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\"><INPUT TYPE=RADIO NAME=PLANET_ITEM_ID VALUE=%d></TD>", bid->get_id() );
		}
		else
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">&nbsp;</TD>" );
		}
		html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>", planet->get_name() );
		CCluster
			*cluster = planet->get_cluster();
		html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>", cluster->get_name() );
		html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%d</TD>", planet->get_population() );
		if( planet->calc_environment_control(aPlayer) > -10 )
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>", CControlModel::get_environment_description( planet->calc_environment_control(aPlayer) ) );
		}
		else
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#FF0000\">%s</TD>", CControlModel::get_environment_description( planet->calc_environment_control(aPlayer) ) );
		}
		html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%d min</TD>", bid->get_expire_time() );
		//html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%d</TD>", bid->get_price() );
		/*
		if( bid->get_winner_id() != aPlayer->get_game_id() )
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%d</TD>", bid->get_price() );
		}
		else
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#0000FF\">%d</TD>", bid->get_price() );
		}
		*/
		if( bid->get_winner_id() == aPlayer->get_game_id() )
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#0000FF\">%s</TD>", dec2unit( bid->get_price() ) );
		}
		else if( bid->get_price()*1.05 > aPlayer->get_production() )
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\" style=\"color:#FF0000\">%s</TD>", dec2unit( bid->get_price() ) );
		}
		else
		{
			html.format( "<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>", dec2unit( bid->get_price() ) );
		}
		html.format( "</TR>\n" );
	}

	html.format( "</TABLE>" );
	return (char*)html;
}

