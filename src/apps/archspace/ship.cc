#include "common.h"
#include "util.h"
#include "ship.h"
#include <libintl.h>
#include "component.h"
#include "player.h"
#include "archspace.h"
#include "game.h"
#include "race.h"
#include <cstdlib>

TZone gShipSizeZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CShipSize),
	0,
	0,
	NULL,
	"Zone CShipSize"
};

TZone gShipDesignZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CShipDesign),
	0,
	0,
	NULL,
	"Zone CShipDesign"
};

TZone gShipToBuildZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CShipToBuild),
	0,
	0,
	NULL,
	"Zone CShipToBuild"
};

TZone gShipBuildQZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CShipBuildQ),
	0,
	0,
	NULL,
	"Zone CShipBuildQ"
};

TZone gDockedShipZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CDockedShip),
	0,
	0,
	NULL,
	"Zone CDockedShip"
};

TZone gDamagedShipZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CDamagedShip),
	0,
	0,
	NULL,
	"Zone CDamagedShip"
};

const char *
CShipSize::html_print_row()
{
	static CString
		buf;
	buf.clear();

	buf.format( "Class : %d<BR>\n", get_class() );
	buf.format( "Name : %s(%d)<BR>\n", get_name(), get_id() );
	buf.format( "Space : %d<BR>\n", get_space() );
	buf.format( "Weapon Slot : %d<BR>\n", get_weapon() );
	buf.format( "Space/Slot : %d<BR>\n", get_slot() );
	buf.format( "Device Slot : %d<BR>\n", get_device() );
	buf.format( "Cost : %d<BR>\n", get_cost() );
	buf.format( "MP Upkeep : %d<BR>\n", get_upkeep() );

	return buf;
}

char *
CShipSize::input_weapon_list_html(CPlayer *aPlayer)
{
	static CString
		WeaponList;
	WeaponList.clear();

	for (int i=0 ; i<get_weapon() ; i++)
	{
		WeaponList += "<TR>\n";
		WeaponList.format("<TD CLASS=\"tabletxt\" WIDTH=\"75\">%s %d</TD>\n",
							GETTEXT("Weapon"), i+1);
		WeaponList += "<TD COLSPAN=\"3\" CLASS=\"tabletxt\" WIDTH=\"262\">\n";
		CComponentList *
			ComponentList = (CComponentList *)(aPlayer->get_component_list());
		WeaponList += ComponentList->weapon_list_html(i+1, get_slot());
		WeaponList += "</TD>\n";
		WeaponList += "</TR>\n";
	}

	return (char *)WeaponList;
}

char *
CShipSize::input_device_list_html(CPlayer *aPlayer)
{
	static CString
		DeviceList;
	DeviceList.clear();

	CComponentList *
		ComponentList = (CComponentList *)(aPlayer->get_component_list());

	if (!get_device())
	{
		DeviceList = "<TR>\n";
		DeviceList.format("<TD CLASS=\"tabletxt\" WIDTH=\"75\">%s</TD>\n",
							GETTEXT("Device"));
		DeviceList += "<TD COLSPAN=\"3\" CLASS=\"tabletxt\" WIDTH=\"262\">\n";
		DeviceList.format("%s\n", GETTEXT("No devices researched"));
		DeviceList += "</TD>\n";
		DeviceList += "</TR>\n";

		return (char *)DeviceList;
	}

	for (int i=0 ; i<get_device() ; i++)
	{
		DeviceList += "<TR>\n";
		DeviceList.format("<TD CLASS=\"tabletxt\" WIDTH=\"75\">%s %d</TD>\n",
							GETTEXT("Device"), i+1);
		DeviceList += "<TD COLSPAN=\"3\" CLASS=\"tabletxt\" WIDTH=\"262\">\n";
		DeviceList += ComponentList->device_list_html(i+1);
		DeviceList += "</TD>\n";
		DeviceList += "</TR>\n";
	}

	return (char *)DeviceList;
}

CShipSizeList::CShipSizeList()
{
}

CShipSizeList::~CShipSizeList()
{
	remove_all();
}

bool
CShipSizeList::free_item( TSomething aItem )
{
	(void)aItem;
	return true;
}

int
CShipSizeList::compare( TSomething aItem1, TSomething aItem2) const
{
	CShipSize
		*ship1 = (CShipSize*)aItem1,
		*ship2 = (CShipSize*)aItem2;

	if ( ship1->get_id() > ship2->get_id() ) return 1;
	if ( ship1->get_id() < ship2->get_id() ) return -1;
	return 0;
}

int
CShipSizeList::compare_key( TSomething aItem, TConstSomething aKey ) const
{
	CShipSize
		*ship = (CShipSize*)aItem;

	if ( ship->get_id() > (int)aKey ) return 1;
	if ( ship->get_id() < (int)aKey ) return -1;
	return 0;
}

bool
CShipSizeList::remove_ship_size(int aShipID)
{
	int
		index;
	index = find_sorted_key( (void*)aShipID );
	if (index < 0) return false;

	return CSortedList::remove(index);
}

int
CShipSizeList::add_ship_size(CShipSize *aShip)
{
	if (!aShip) return -1;

	if (find_sorted_key( (TConstSomething)aShip->get_id() ) >= 0)
		return -1;
	insert_sorted( aShip );

	return aShip->get_id();
}

CShipSize *
CShipSizeList::get_by_id(int aShipID)
{
	if ( !length() ) return NULL;

	int
		index;

	index = find_sorted_key( (void*)aShipID );

	if (index < 0) return NULL;

	return (CShipSize*)get(index);
}

CShipSizeTable::CShipSizeTable()
{
}

CShipSizeTable::~CShipSizeTable()
{
	remove_all();
}

bool
CShipSizeTable::free_item( TSomething aItem )
{
	CShipSize
		*ship = (CShipSize*)aItem;

	if (!ship) return false;

	delete ship;

	return true;
}

int
CShipSizeTable::compare( TSomething aItem1, TSomething aItem2 ) const
{
	CShipSize
		*ship1 = (CShipSize*)aItem1,
		*ship2 = (CShipSize*)aItem2;

	if ( ship1->get_id() > ship2->get_id() ) return 1;
	if ( ship1->get_id() < ship2->get_id() ) return -1;
	return 0;
}

int
CShipSizeTable::compare_key( TSomething aItem, TConstSomething aKey ) const
{
	CShipSize
		*ship = (CShipSize*)aItem;

	if ( ship->get_id() > (int)aKey ) return 1;
	if ( ship->get_id() < (int)aKey ) return -1;
	return 0;
}

bool
CShipSizeTable::remove_ship_size(int aShipID)
{
	int
		index;
	index = find_sorted_key( (void*)aShipID );
	if (index < 0) return false;

	return CSortedList::remove(index);
}

int
CShipSizeTable::add_ship_size(CShipSize *aShip)
{
	if (!aShip) return -1;

	if (find_sorted_key( (TConstSomething)aShip->get_id() ) >= 0)
		return -1;
	insert_sorted( aShip );

	return aShip->get_id();
}

CShipSize*
CShipSizeTable::get_by_id( int aShipID )
{
	if ( !length() ) return NULL;

	int
		index;

	index = find_sorted_key( (void*)aShipID );

	if (index < 0) return NULL;

	return (CShipSize*)get(index);
}

char *
CShipSizeTable::available_size_list_html(CPlayer *aPlayer)
{
	static CString
		SizeList;
	SizeList.clear();



	int
		Tech = 0;

        Tech = (aPlayer->count_tech_by_category( CTech::TYPE_MATTER_ENERGY )) / 3;


        if(Tech < 4)
        {
                Tech = 4;
        }
        Tech -= 2;

        SizeList = "<SELECT NAME=\"SHIP_SIZE\">\n";

        for (int i=1 ; i < (Tech + DEFAULT_SHIP_SIZE_NUMBER + 1) && i < 11 ; i++)
        {

                CShipSize *
                        Ship = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(4000 + i);

                if(!Ship)
                        SLOG("Ship %d null", 4000+i);

                SizeList.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
                                                Ship->get_class(), Ship->get_name());
                if(i >= 7)
                {
                        Tech = (aPlayer->count_tech_by_category( CTech::TYPE_MATTER_ENERGY ) - 5) / 3;
                        Tech -= 2;
                }

        }

/*(	Tech = (aPlayer->count_tech_by_category( CTech::TYPE_MATTER_ENERGY ));

	SizeList = "<SELECT NAME=\"SHIP_SIZE\">\n";

	for (int i=1 ; i < (Tech + 1) && i <= MAX_SHIP_CLASS ; i++)
	{

		CShipSize *
			Ship = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(4000 + i);

		if(!Ship)
			SLOG("Ship %d null", 4000+i);

		SizeList.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
						Ship->get_class(), Ship->get_name());
	}*/

	SizeList += "</SELECT>\n";

	return (char *)SizeList;
}

char *
CShipSizeTable::size_information_html(CPlayer *aPlayer)
{
	static CString
		SizeInfo;
	SizeInfo.clear();

//	int
//		Tech = (aPlayer->count_tech_by_category( CTech::TYPE_SCHEMATICS ));

	SizeInfo = "<TABLE WIDTH=\"550\" BORDER=\"1\""
				" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">";
	SizeInfo += "<TR BGCOLOR=\"#171717\">\n";

	SizeInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"35\" ALIGN=\"CENTER\">";
	SizeInfo.format("<FONT COLOR=\"#666666\">%s</FONT>", GETTEXT("Size"));
	SizeInfo += "</TH>\n";

	SizeInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"112\" ALIGN=\"CENTER\">";
	SizeInfo.format("<FONT COLOR=\"#666666\">%s</FONT>", GETTEXT("Name"));
	SizeInfo += "</TH>\n";

	SizeInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"52\" ALIGN=\"CENTER\">";
	SizeInfo.format("<FONT COLOR=\"#666666\">%s</FONT>", GETTEXT("Space"));
	SizeInfo += "</TH>\n";

	SizeInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"70\" ALIGN=\"CENTER\">";
	SizeInfo.format("<FONT COLOR=\"#666666\">%s</FONT>", GETTEXT("Weapon"));
	SizeInfo += "</TH>\n";

	SizeInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"85\" ALIGN=\"CENTER\">";
	SizeInfo.format("<FONT COLOR=\"#666666\">%s</FONT>", GETTEXT("Space/Slot"));
	SizeInfo += "</TH>\n";

	SizeInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"80\" ALIGN=\"CENTER\">";
	SizeInfo.format("<FONT COLOR=\"#666666\">%s</FONT>", GETTEXT("Device Slot"));
	SizeInfo += "</TH>\n";

	SizeInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"100\" ALIGN=\"CENTER\">";
	SizeInfo.format("<FONT COLOR=\"#666666\">%s</FONT>", GETTEXT("Building Cost"));
	SizeInfo += "</TH>\n";

/*	SizeInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"100\" ALIGN=\"CENTER\">";
	SizeInfo.format("<FONT COLOR=\"#666666\">%s</FONT>", GETTEXT("MatterEnergy Techs Required"));
	SizeInfo += "</TH>\n";    */

	SizeInfo += "</TR>\n";
//	int MEreq = 0;
	for (int i=1 ; i <= MAX_SHIP_CLASS ; i++)
	{
		CShipSize *
			Ship = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(4000 + i);

		if(!Ship)
			SLOG("Ship %d null", 4000+i);

//		MEreq = i;
		SizeInfo.format("<TR ALIGN=\"CENTER\">"
						"<TD CLASS=\"tabletxt\" WIDTH=\"35\">"
						"<FONT COLOR=\"#666666\"></FONT>%d</TD>"
						"<TD WIDTH=\"112\" CLASS=\"tabletxt\">%s</TD>"
						"<TD WIDTH=\"52\" CLASS=\"tabletxt\">%d</TD>"
						"<TD WIDTH=\"70\" CLASS=\"tabletxt\">%d</TD>"
						"<TD CLASS=\"tabletxt\" WIDTH=\"85\">%d</TD>"
						"<TD CLASS=\"tabletxt\" WIDTH=\"80\">%d</TD>"
						"<TD CLASS=\"tabletxt\" WIDTH=\"100\">%s</TD>"
						"</TR>\n",
						i, Ship->get_name(), Ship->get_space(), Ship->get_weapon(),
						Ship->get_slot(),
						Ship->get_device(), dec2unit(Ship->get_cost()));


	}

	SizeInfo += "</TABLE>\n";

	return (char *)SizeInfo;
}

bool
CShipScript::get(CShipSizeTable *aShipTable)
{
	system_log("start script no:%d", mRoot.length());

	int i=0;

	TSomething
		ship;

	while( (ship = get_array(i++)) )
	{
		//system_log( "Ship Name:%s", get_name(ship) );

		TSomething
			something;

		CShipSize
			*tmpShip;

		tmpShip = new CShipSize;

		tmpShip->set_name( get_name(ship) );

		something = get_section( "Number", ship );
		if (something) {
			tmpShip->set_id( atoi( get_data(something) ) );
		} else {
			system_log( "WRONG SHIP FILE at %s", get_name(ship) );
			return false;
		}

		something = get_section("Description", ship);
		if (something)
		{
			tmpShip->set_description(get_data(something));
		} else
		{
			system_log( "WRONG SHIP FILE at %s", get_name(ship) );
			return false;
		}

		something = get_section( "Class", ship );
		if (something) {
			tmpShip->set_class( atoi(get_data(something)) );
		} else {
			system_log( "WRONG SHIP FILE at %s", get_name(ship) );
			return false;
		}

		something = get_section( "Space", ship );
		if (something) {
			tmpShip->set_space( atoi(get_data(something)) );
		} else {
			system_log( "WRONG SHIP FILE at %s", get_name(ship) );
			return false;
		}

		something = get_section( "Weapon", ship );
		if (something) {
			tmpShip->set_weapon( atoi(get_data(something)) );
		} else {
			system_log( "WRONG SHIP FILE at %s", get_name(ship) );
			return false;
		}

		something = get_section( "Slot", ship );
		if (something) {
			tmpShip->set_slot( atoi(get_data(something)) );
		} else {
			system_log( "WRONG SHIP FILE at %s", get_name(ship) );
			return false;
		}

		something = get_section( "Device", ship );
		if (something) {
			tmpShip->set_device( atoi(get_data(something)) );
		} else {
			system_log( "WRONG SHIP FILE at %s", get_name(ship) );
			return false;
		}

		something = get_section( "Cost", ship );
		if (something) {
			tmpShip->set_cost( atoi(get_data(something)) );
		} else {
			system_log( "WRONG SHIP FILE at %s", get_name(ship) );
			return false;
		}

		something = get_section( "Upkeep", ship );
		if (something) {
			tmpShip->set_upkeep( atoi(get_data(something)) );
		} else {
			system_log( "WRONG SHIP FILE at %s", get_name(ship) );
			return false;
		}

		aShipTable->add_ship_size(tmpShip);
	}

	system_log("end script");
	return true;
};

CShipDesign::CShipDesign()
{
	for ( int i=0 ; i<WEAPON_MAX_NUMBER ; i++)
	{
		mWeaponList[i] = 0;
		mWeaponNumberList[i] = 0;
	}
	for ( int i=0 ; i<DEVICE_MAX_NUMBER ; i++)
	{
		mDeviceList[i] = 0;
	}
	mBlackMarketDesign = false;
	mEmpireDesign = false;
}

CShipDesign::CShipDesign( MYSQL_ROW aRow )
{
	enum
	{
		FIELD_OWNER = 0,
		FIELD_DESIGN_ID,
		FIELD_NAME,
		FIELD_BODY,
		FIELD_ARMOR,
		FIELD_ENGINE,
		FIELD_COMPUTER,
		FIELD_SHIELD,
		FIELD_WEAPON_0,
		FIELD_WEAPON_1,
		FIELD_WEAPON_2,
		FIELD_WEAPON_3,
		FIELD_WEAPON_4,
		FIELD_WEAPON_5,
		FIELD_WEAPON_6,
		FIELD_WEAPON_7,
		FIELD_WEAPON_8,
		FIELD_WEAPON_9,
		FIELD_WEAPON_NUMBER_0,
		FIELD_WEAPON_NUMBER_1,
		FIELD_WEAPON_NUMBER_2,
		FIELD_WEAPON_NUMBER_3,
		FIELD_WEAPON_NUMBER_4,
		FIELD_WEAPON_NUMBER_5,
		FIELD_WEAPON_NUMBER_6,
		FIELD_WEAPON_NUMBER_7,
		FIELD_WEAPON_NUMBER_8,
		FIELD_WEAPON_NUMBER_9,
		FIELD_DEVICE_0,
		FIELD_DEVICE_1,
		FIELD_DEVICE_2,
		FIELD_DEVICE_3,
		FIELD_DEVICE_4,
		FIELD_DEVICE_5,
		FIELD_DEVICE_6,
		FIELD_DEVICE_7,
		FIELD_BUILD_TIME,
		FIELD_BUILD_COST,
		FIELD_BLACK_MARKET_DESIGN,
		FIELD_EMPIRE_DESIGN
	};

	mDesignID = atoi( aRow[FIELD_DESIGN_ID] );
	mName = (char *)aRow[FIELD_NAME];
	mOwner = atoi( aRow[FIELD_OWNER] );
	mBody = atoi( aRow[FIELD_BODY] );
	mArmor = atoi( aRow[FIELD_ARMOR] );
	mEngine = atoi( aRow[FIELD_ENGINE] );
	mComputer = atoi( aRow[FIELD_COMPUTER] );
	mShield = atoi( aRow[FIELD_SHIELD] );
	mWeaponList[0] = atoi( aRow[FIELD_WEAPON_0] );
	mWeaponList[1] = atoi( aRow[FIELD_WEAPON_1] );
	mWeaponList[2] = atoi( aRow[FIELD_WEAPON_2] );
	mWeaponList[3] = atoi( aRow[FIELD_WEAPON_3] );
	mWeaponList[4] = atoi( aRow[FIELD_WEAPON_4] );
	mWeaponList[5] = atoi( aRow[FIELD_WEAPON_5] );
	mWeaponList[6] = atoi( aRow[FIELD_WEAPON_6] );
	mWeaponList[7] = atoi( aRow[FIELD_WEAPON_7] );
	mWeaponList[8] = atoi( aRow[FIELD_WEAPON_8] );
	mWeaponList[9] = atoi( aRow[FIELD_WEAPON_9] );
	mWeaponNumberList[0] = atoi( aRow[FIELD_WEAPON_NUMBER_0] );
	mWeaponNumberList[1] = atoi( aRow[FIELD_WEAPON_NUMBER_1] );
	mWeaponNumberList[2] = atoi( aRow[FIELD_WEAPON_NUMBER_2] );
	mWeaponNumberList[3] = atoi( aRow[FIELD_WEAPON_NUMBER_3] );
	mWeaponNumberList[4] = atoi( aRow[FIELD_WEAPON_NUMBER_4] );
	mWeaponNumberList[5] = atoi( aRow[FIELD_WEAPON_NUMBER_5] );
	mWeaponNumberList[6] = atoi( aRow[FIELD_WEAPON_NUMBER_6] );
	mWeaponNumberList[7] = atoi( aRow[FIELD_WEAPON_NUMBER_7] );
	mWeaponNumberList[8] = atoi( aRow[FIELD_WEAPON_NUMBER_8] );
	mWeaponNumberList[9] = atoi( aRow[FIELD_WEAPON_NUMBER_9] );
	mDeviceList[0] = atoi( aRow[FIELD_DEVICE_0] );
	mDeviceList[1] = atoi( aRow[FIELD_DEVICE_1] );
	mDeviceList[2] = atoi( aRow[FIELD_DEVICE_2] );
	mDeviceList[3] = atoi( aRow[FIELD_DEVICE_3] );
	mDeviceList[4] = atoi( aRow[FIELD_DEVICE_4] );
	mDeviceList[5] = atoi( aRow[FIELD_DEVICE_5] );
	mDeviceList[6] = atoi( aRow[FIELD_DEVICE_6] );
	mDeviceList[7] = atoi( aRow[FIELD_DEVICE_7] );
	mBuildTime = atoi( aRow[FIELD_BUILD_TIME] );
	mBuildCost = atoi( aRow[FIELD_BUILD_COST] );
	mBlackMarketDesign = (bool)atoi( aRow[FIELD_BLACK_MARKET_DESIGN]);
	mEmpireDesign = (bool)atoi( aRow[FIELD_EMPIRE_DESIGN]);
}

CShipDesign::~CShipDesign()
{
}

const char *
CShipDesign::get_name()
{
	static CString
		Name;
	Name.clear();

	Name = mName;
	Name.htmlspecialchars();
	Name.nl2br();
	return (char *)Name;
}

const char *
CShipDesign::get_nick()
{
	static CString
		Nick;
	Nick.clear();

	Nick.format("%s (#%d)", CShipDesign::get_name(), get_design_id());
	return (char *)Nick;
}

int
CShipDesign::get_max_hp_without_effect()
{
	CShipSize *
		ShipSize = SHIP_SIZE_TABLE->get_by_id(mBody);
	if (ShipSize == NULL)
	{
		SLOG("ERROR : ShipSize is NULL in CShipDesign::get_max_hp_without_effect(), mOwner = %d, mDesignID = %d", mOwner, mDesignID);
		return -1;
	}

	CArmor *
		Armor = (CArmor *)COMPONENT_TABLE->get_by_id(mArmor);
	if (Armor == NULL)
	{
		SLOG("ERROR : Armor is NULL in CShipDesign::get_max_hp_without_effect(), mOwner = %d, mDesignID = %d", mOwner, mDesignID);
		return -1;
	}

	int
		MaxHP = (int)((double)(ShipSize->get_space()) * Armor->get_hp_multiplier());
	return MaxHP;
}

CString &
CShipDesign::query()
{
	static CString
		query;
	query.clear();

	switch( type() ) {
		case QUERY_INSERT:
		    query.format("INSERT INTO class set owner=%d, design_id=%d, body=%d, name='%s', armor=%d, engine=%d, computer=%d, shield=%d, weapon1=%d, weapon2=%d, weapon3=%d, weapon4=%d, weapon5=%d, weapon6=%d, weapon7=%d, weapon8=%d, weapon9=%d, weapon10=%d, weapon_number1=%d, weapon_number2=%d, weapon_number3=%d, weapon_number4=%d, weapon_number5=%d, weapon_number6=%d, weapon_number7=%d, weapon_number8=%d, weapon_number9=%d, weapon_number10=%d, device1=%d, device2=%d, device3=%d, device4=%d, device5=%d, device6=%d, device7=%d, device8=%d, time=%d, cost=%d, black_market_design=%d, empire_design=%d",
                          mOwner, mDesignID, mBody, (char*)add_slashes((char*)mName), mArmor, mEngine, mComputer, mShield, get_weapon(0), get_weapon(1), get_weapon(2), get_weapon(3), get_weapon(4), get_weapon(5), get_weapon(6), get_weapon(7), get_weapon(8), get_weapon(9), get_weapon_number(0), get_weapon_number(1), get_weapon_number(2), get_weapon_number(3), get_weapon_number(4), get_weapon_number(5), get_weapon_number(6), get_weapon_number(7), get_weapon_number(8), get_weapon_number(9), get_device(0), get_device(1), get_device(2), get_device(3), get_device(4), get_device(5), get_device(6), get_device(7), mBuildTime, mBuildCost, (int)mBlackMarketDesign, (int)mEmpireDesign);

			//query.format( "INSERT INTO class ( owner, design_id, body, name, armor, engine, computer, shield, weapon1, weapon2, weapon3, weapon4, weapon5, weapon6, weapon7, weapon8, weapon9, weapon10, weapon_number1, weapon_number2, weapon_number3, weapon_number4, weapon_number5, weapon_number6, weapon_number7, weapon_number8, weapon_number9, weapon_number10, device1, device2, device3, device4, device5, device6, device7, device8, time, cost, black_market_design, empire_design ) VALUES ( %d, %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
			//mOwner, mDesignID, mBody, (char*)add_slashes((char*)mName), mArmor, mEngine, mComputer, mShield, get_weapon(0), get_weapon(1), get_weapon(2), get_weapon(3), get_weapon(4), get_weapon(5), get_weapon(6), get_weapon(7), get_weapon(8), get_weapon(9), get_weapon_number(0), get_weapon_number(1), get_weapon_number(2), get_weapon_number(3), get_weapon_number(4), get_weapon_number(5), get_weapon_number(6), get_weapon_number(7), get_weapon_number(8), get_weapon_number(9), get_device(0), get_device(1), get_device(2), get_device(3), get_device(4), get_device(5), get_device(6), get_device(7), mBuildTime, mBuildCost, (int)mBlackMarketDesign, (int)mEmpireDesign );
			break;
		case QUERY_UPDATE:
			query.format( "UPDATE class SET time = %d, black_market_design =%d, empire_design =%d WHERE owner = %d AND design_id = %d", mBuildTime, (int)mBlackMarketDesign, (int)mEmpireDesign, mOwner, mDesignID );
			break;
		case QUERY_DELETE:
			query.format( "DELETE FROM class WHERE owner = %d AND design_id = %d", mOwner, mDesignID );
			break;
	}

	mStoreFlag.clear();

	return query;
}

char *
CShipDesign::print_html(CPlayer *aPlayer, int aPP )
{
	static CString
		Class;

	Class.clear();

	Class += "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\""
					" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	Class += "<TR>\n";
	Class += "<TH CLASS=\"tabletxt\" WIDTH=\"99\""
					" BGCOLOR=\"#171717\" ALIGN=\"LEFT\">&nbsp;\n";

	Class.format("<FONT COLOR=\"#666666\">%s </FONT></TH>\n",
						GETTEXT("Name"));

        Class.format("<TD COLSPAN=\"3\" CLASS=\"tabletxt\">&nbsp;%s </TD>\n",
						get_name());

	Class += "<TD CLASS=\"tabletxt\" WIDTH=\"106\" ALIGN=\"CENTER\">";

	if (can_delete(aPlayer) == DELETE_OK)
	{
		Class.format("<A HREF=\"ship_design_delete_confirm.as?DESIGN_ID=%d\">",
						mDesignID);
		Class.format("<IMG SRC=\"%s/image/as_game/bu_delete.gif\" BORDER=\"0\">",
						(char *)CGame::mImageServerURL);
		Class += "</A>";
	} else
	{
		Class += "&nbsp;";
	}
	Class += "</TD>\n";
	Class += "</TR>\n";

	Class += "<TR>\n";
	Class += "<TH BGCOLOR=\"#171717\" CLASS=\"tabletxt\" WIDTH=\"109\" ALIGN=\"LEFT\">&nbsp;\n";

	Class.format("<FONT COLOR=\"#666666\">%s </FONT></TH>\n",
						GETTEXT("Type"));

	if (mBlackMarketDesign)
		Class.format("<TD COLSPAN=\"4\" CLASS=\"tabletxt\">&nbsp;%s </TD>\n", GETTEXT("Black Market [restricted]"));
	else if (mEmpireDesign)
		Class.format("<TD COLSPAN=\"4\" CLASS=\"tabletxt\">&nbsp;%s </TD>\n", GETTEXT("Empire [restricted]"));
	else
		Class.format("<TD COLSPAN=\"4\" CLASS=\"tabletxt\">&nbsp;%s </TD>\n", GETTEXT("Custom"));

	Class += "</TR>\n";

	Class += "<TR BGCOLOR=\"#171717\">\n";
	Class.format("<TH CLASS=\"tabletxt\" WIDTH=\"99\" ALIGN=\"LEFT\">"
					"<FONT COLOR=\"#666666\">&nbsp;%s\n", GETTEXT("Armor"));
	Class += "</FONT></TH>\n";
	Class.format("<TH CLASS=\"tabletxt\" WIDTH=\"109\" ALIGN=\"LEFT\">"
						"<FONT COLOR=\"#666666\">&nbsp;%s\n", GETTEXT("Class"));
	Class += "</FONT></TH>\n";
	Class.format("<TH CLASS=\"tabletxt\" WIDTH=\"115\" ALIGN=\"LEFT\">"
						"<FONT COLOR=\"#666666\">&nbsp;%s\n", GETTEXT("Computer"));
	Class += "</FONT></TH>\n";
	Class.format("<TH CLASS=\"tabletxt\" WIDTH=\"109\" ALIGN=\"LEFT\">"
					"<FONT COLOR=\"#666666\">&nbsp;%s\n", GETTEXT("Engine"));
	Class += "</FONT></TH>\n";
	Class.format("<TH COLSPAN=\"2\" CLASS=\"tabletxt\" WIDTH=\"106\" ALIGN=\"LEFT\">"
					"<FONT COLOR=\"#666666\">&nbsp;%s\n", GETTEXT("Shield"));
	Class += "</FONT></TH>\n";
	Class += "</TR>\n";
	Class += "<TR>\n";

	CArmor *
		Armor = (CArmor *)COMPONENT_TABLE->get_by_id( get_armor() );
	Class.format("<TD CLASS=\"tabletxt\" WIDTH=\"99\" ALIGN=\"LEFT\">&nbsp;%s</TD>\n",
						Armor->get_name() );

	CShipSize *
		ShipSize = (CShipSize *)SHIP_SIZE_TABLE->get_by_id( get_body() );
	Class.format("<TD WIDTH=\"109\" CLASS=\"tabletxt\">&nbsp;%s</TD>\n",
						ShipSize->get_name() );

	CComputer *
		Computer = (CComputer *)COMPONENT_TABLE->get_by_id( get_computer() );
	Class.format("<TD WIDTH=\"115\" CLASS=\"tabletxt\">&nbsp;%s</TD>\n",
						Computer->get_name() );

	CEngine *
		Engine = (CEngine *)COMPONENT_TABLE->get_by_id( get_engine() );
	Class.format("<TD WIDTH=\"109\" CLASS=\"tabletxt\">&nbsp;%s</TD>\n",
						Engine->get_name() );

	CShield *
		Shield = (CShield *)COMPONENT_TABLE->get_by_id( get_shield() );
	Class.format("<TD COLSPAN=\"2\" CLASS=\"tabletxt\" WIDTH=\"106\">&nbsp;%s</TD>\n",
						Shield->get_name() );

	Class += "</TR>\n";
	Class += "<TR>\n";
	Class += "<TH CLASS=\"tabletxt\" WIDTH=\"99\" BGCOLOR=\"#171717\" ALIGN=\"LEFT\">";
	Class.format("<FONT COLOR=\"#666666\">&nbsp;%s</FONT></TH>\n",
					GETTEXT("Weapon"));

	bool
		AnyWeapon = false;

	Class += "<TD COLSPAN=\"5\" CLASS=\"tabletxt\">\n";
	for (int i=0 ; i<WEAPON_MAX_NUMBER ; i++)
	{
		CWeapon
			*Weapon = (CWeapon *)COMPONENT_TABLE->get_by_id( get_weapon(i) );
		if (!Weapon) continue;
		Class.format("&nbsp;%s * %d<BR>\n",
						Weapon->get_name(), get_weapon_number(i));
		AnyWeapon = true;
	}
	Class += "</TD>\n";

	if (!AnyWeapon)
		Class.format("<TD COLSPAN=\"5\" CLASS=\"tabletxt\">&nbsp;%s</TD>\n",
						GETTEXT("None"));

	Class += "</TR>\n";
	Class += "<TR>\n";
	Class += "<TH CLASS=\"tabletxt\" WIDTH=\"99\" BGCOLOR=\"#171717\" ALIGN=\"LEFT\">";
	Class.format("<FONT COLOR=\"#666666\">&nbsp;%s</FONT></TH>\n",
					GETTEXT("Device"));

	bool
		AnyDevice = false;

	Class += "<TD COLSPAN=\"5\" CLASS=\"tabletxt\">\n";
	for (int i=0 ; i<DEVICE_MAX_NUMBER ; i++)
	{
		CDevice
			*Device = (CDevice *)COMPONENT_TABLE->get_by_id( get_device(i) );
		if (!Device) continue;
		Class.format("&nbsp;%s<BR>\n",
						Device->get_name());
		AnyDevice = true;
	}
	if (!AnyDevice)
		Class.format("&nbsp;%s<BR>\n",
						GETTEXT("None"));

	Class += "</TD>\n";
	Class += "</TR>\n";
	Class += "</TABLE>\n";
	Class += "<BR>\n";

	return (char *)Class;
}

char *
CShipDesign::class_info_for_building_html(CPlayer *aPlayer)
{
	static CString
		ClassInfo;
	ClassInfo.clear();

	ClassInfo = "<TR>\n";
	ClassInfo += "<TD WIDTH=\"24\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">\n";
	ClassInfo.format("<INPUT TYPE=\"radio\" NAME=\"CLASS\" VALUE=\"%d\">\n",
						get_design_id());
	ClassInfo += "</TD>\n";
	ClassInfo.format("<TD CLASS=\"tabletxt\" ALIGN=\"LEFT\" WIDTH=\"130\">%s</TD>\n",
						get_name());
	CShipSize *
		Body = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(get_body());
	ClassInfo.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" WIDTH=\"83\">%s</TD>\n",
						Body->get_name());

	CArmor *
		Armor = (CArmor *)COMPONENT_TABLE->get_by_id(get_armor());
	if (Armor->get_armor_type() == AT_BIO &&
		aPlayer->has_ability(ABILITY_GREAT_SPAWNING_POOL))
	{
		ClassInfo += "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" WIDTH=\"105\">";
		if (aPlayer->has_ability(ABILITY_HIVE_SHIP_YARD))
		{
			ClassInfo.format("%s PP", dec2unit(Body->get_cost() * 70 / 100));
		}
		else
		{
			ClassInfo.format("%s PP", dec2unit(Body->get_cost() * 80 / 100));
		}
		ClassInfo += "</TD>\n";
	}
	else
	{
		ClassInfo += "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" WIDTH=\"105\">";
		if (aPlayer->has_ability(ABILITY_HIVE_SHIP_YARD))
		{
			ClassInfo.format("%s PP", dec2unit(Body->get_cost() * 90 / 100));
		}
		else
		{
			ClassInfo.format("%s PP", dec2unit(Body->get_cost()));
		}
		ClassInfo += "</TD>\n";
	}

	int
		RealShipProduction = aPlayer->calc_real_ship_production();
	if (!RealShipProduction)
	{
		CString
			TurnString;
		TurnString.clear();
		TurnString.format(GETTEXT("%1$s turns"), "?");

		ClassInfo.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" WIDTH=\"104\">%s</TD>\n",
							(char *)TurnString);
	} else
	{
		int
			TimeCost;

		if (Body->get_cost() % RealShipProduction)
		{
			TimeCost = Body->get_cost() / RealShipProduction + 1;
		} else TimeCost = Body->get_cost() / RealShipProduction;

		CString
			TurnString;
		TurnString.clear();
		TurnString.format(GETTEXT("%1$s turn%2$s"), dec2unit(TimeCost), plural(TimeCost));

		ClassInfo.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" WIDTH=\"104\">%s</TD>\n",
							(char *)TurnString);
	}

	int
		SameClass;
	CDockedShip *
		DockedShip = aPlayer->get_dock()->get_by_id(get_design_id());
	if (!DockedShip)
	{
		SameClass = 0;
	} else SameClass = DockedShip->get_number();

	ClassInfo.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\" WIDTH=\"90\">%d</TD>\n",
						SameClass);
	ClassInfo += "</TR>\n";

	return (char *)ClassInfo;
}

int
CShipDesign::can_delete(CPlayer *aPlayer)
{
	// start telecard 2001/03/05
	if( aPlayer->get_ship_design_list()->get_by_id( mDesignID ) == NULL )
	{
		return DELETE_PROHIBITED;
	}
	// end telecard

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	for (int i=0 ; i<FleetList->length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)FleetList->get(i);
		if (Fleet->get_design_id() == mDesignID) return USED_IN_FLEET;
	}

	CDock *
		Dock = aPlayer->get_dock();
	for (int i=0 ; i<Dock->length() ; i++)
	{
		CDockedShip *
			DockedShip = (CDockedShip *)Dock->get(i);
		if (DockedShip->get_design_id() == mDesignID) return USED_IN_DOCK;
	}

	CRepairBay *
		RepairBay = aPlayer->get_repair_bay();
	for (int i=0 ; i<RepairBay->length() ; i++)
	{
		CDamagedShip *
			DamagedShip = (CDamagedShip *)RepairBay->get(i);
		if (DamagedShip->get_design_id() == mDesignID) return USED_IN_REPAIR_BAY;
	}

	CShipBuildQ *
		ShipBuildQ = aPlayer->get_build_q();
	for (int i=0 ; i<ShipBuildQ->length() ; i++)
	{
		CShipToBuild *
			ShipToBuild = (CShipToBuild *)ShipBuildQ->get(i);
		if (ShipToBuild->get_design_id() == mDesignID) return USED_IN_BUILD_Q;
	}

	return DELETE_OK;
}

// Cycles through all the prerequisites and
// determines if a ship design is legal
bool CShipDesign::check_legality() {
    bool isLegal = true;

    CShipSize *Body = SHIP_SIZE_TABLE->get_by_id(get_body());
    CComponent *Armor = (CComponent *)COMPONENT_TABLE->get_by_id(get_armor());
    CComponent *Shield = (CComponent *)COMPONENT_TABLE->get_by_id(get_shield());
    CComponent *Engine = (CComponent *)COMPONENT_TABLE->get_by_id(get_engine());
    CComponent *Computer = (CComponent *)COMPONENT_TABLE->get_by_id(get_computer());
    CComponent *Weapon;
    CComponent *Device;

    //SLOG("Start checking!");
    // Check each component for attribute requirements
    if (isLegal) {
        isLegal = check_component_legality(Armor);
    }
    //SLOG("Armor checked!");

    if (isLegal) {
        isLegal = check_component_legality(Computer);
    }
    //SLOG("Comp checked!");
    
    if (isLegal) {
        isLegal = check_component_legality(Shield);
    }
    //SLOG("Shield checked!");
    
    if (isLegal) {
        isLegal = check_component_legality(Engine);
    }
    //SLOG("Engine checked!");
    
    int i;
    for (i = 0; i < Body->get_weapon() && isLegal && i < WEAPON_MAX_NUMBER; i++) {
        Weapon = (CComponent *)COMPONENT_TABLE->get_by_id(get_weapon(i));
        if (isLegal) {
            isLegal = check_component_legality(Weapon);
        }
    }
    //SLOG("Weapons checked!");

    for (i = 0; i < Body->get_device() && isLegal && i < DEVICE_MAX_NUMBER; i++) {
        if (get_device(i) != 0) {
            Device = (CComponent *)COMPONENT_TABLE->get_by_id(get_device(i));
            if (isLegal) {
                isLegal = check_component_legality(Device);
            }
        }
    }
    //SLOG("Devices checked!");



    return isLegal;
}

// Returns true if the specified component is legal on the
// current ship design
bool CShipDesign::check_component_legality(CComponent *aComponent) {
    bool isLegal = true;

    CArmor *Armor = (CArmor *)COMPONENT_TABLE->get_by_id(get_armor());
    /*
    if (isLegal && aComponent->has_attribute(CComponent::CA_PSI_RACE_ONLY)) {
        if (PLAYER_TABLE->get_by_game_id(get_owner())->get_race() != CRace::RACE_XELOSS &&
            PLAYER_TABLE->get_by_game_id(get_owner())->get_race() != CRace::RACE_BOSALIAN) {
            isLegal = false;
        }
    }
    */
    if (isLegal && aComponent->has_attribute(CComponent::CA_BIO_ARMOR_ONLY)) {
        if (Armor->get_armor_type() != AT_BIO) {
            isLegal = false;
        }
    }
    
    if (isLegal && aComponent->has_attribute(CComponent::CA_NON_BIO_ARMOR_ONLY)) {
        if (Armor->get_armor_type() == AT_BIO) {
            isLegal = false;
        }
    }

    if (isLegal && aComponent->has_attribute(CComponent::CA_REACTIVE_ARMOR_ONLY)) {
        if (Armor->get_armor_type() != AT_REACTIVE) {
            isLegal = false;
        }
    }
    
    if (isLegal && aComponent->has_attribute(CComponent::CA_NORMAL_ARMOR_ONLY)) {
        if (Armor->get_armor_type() != AT_NORM) {
            isLegal = false;
        }
    }
    
    if (isLegal && aComponent->has_attribute(CComponent::CA_HYBRID_ARMOR_ONLY)) {
        if (Armor->get_armor_type() != AT_HYBRID) {
            isLegal = false;
        }
    }

    if (isLegal && aComponent->has_attribute(CComponent::CA_CUSTOMIZABLE_ARMOR_ONLY)) {
        if (Armor->get_armor_type() != AT_CUSTOMIZABLE) {
            isLegal = false;
        }
    }

    return isLegal;
}

CStoreCenter&
operator<<(CStoreCenter& aStoreCenter, CShipDesign &aDesign)
{
	aStoreCenter.store( aDesign );
	return aStoreCenter;
}


bool
CShipDesignList::free_item( TSomething aItem )
{
	CShipDesign
		*design = (CShipDesign*)aItem;
	if(design == NULL)
		return false;
	delete design;
	return true;
}

int
CShipDesignList::compare(TSomething aItem1, TSomething aItem2 ) const
{
	CShipDesign
		*design1 = (CShipDesign*)aItem1,
		*design2 = (CShipDesign*)aItem2;

	if ( design1->get_design_id() > design2->get_design_id() ) return 1;
	if ( design1->get_design_id() < design2->get_design_id() ) return -1;
	return 0;
}

int
CShipDesignList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CShipDesign
		*design = (CShipDesign*)aItem;

	if ( design->get_design_id() > (int)aKey ) return 1;
	if ( design->get_design_id() < (int)aKey ) return -1;
	return 0;
}

bool
CShipDesignList::remove_ship_design(int aDesignID)
{
	int
		index;
	index = find_sorted_key( (void*)aDesignID );
	if ( index < 0 ) return false;

	return CSortedList::remove(index);
}

bool
CShipDesignList::remove_without_free_ship_design(int aDesignID)
{
	int
		index;
	index = find_sorted_key( (void*)aDesignID );
	if ( index < 0 ) return false;

	return remove_without_free( index );
}

int
CShipDesignList::add_ship_design(CShipDesign *aDesign)
{
	if ( !aDesign ) return -1;

	if ( find_sorted_key( (TConstSomething)aDesign->get_design_id() ) >= 0 )
		return -1;
	insert_sorted( aDesign );

	return aDesign->get_design_id();
}

CShipDesign*
CShipDesignList::get_by_id( int aDesignID )
{
	if ( !length() ) return NULL;

	int
		index;
	index = find_sorted_key( (void*)aDesignID );

	if ( index < 0 ) return NULL;

	return (CShipDesign*)get(index);
}

int
CShipDesignList::max_design_id()
{
	int
		design_id = 0;

	CShipDesign
		*design;
	for( int i=0 ; i<length() ; i++)
	{
		design = (CShipDesign*)get(i);
		if( design->get_design_id() > design_id ) {
			design_id = design->get_design_id();
		}
	}

	return design_id;
}

const char*
CShipDesignList::print_html(CPlayer *aPlayer, int aPP)
{
	static CString
		Buf;

	Buf.clear();

	if (!length())
	{
		Buf = "<TABLE WIDTH=\"550\" BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\""
				" BORDERCOLOR=\"#2A2A2A\">\n";
		Buf += "<TR ALIGN=\"CENTER\" VALIGN=\"TOP\">\n";
		Buf.format("<TD CLASS=\"tabletxt\" COLSPAN=\"4\" HEIGHT=\"25\">%s</TD>\n",
					GETTEXT("There are no classes you designed."));
		Buf += "</TR>\n";
		Buf += "</TABLE>\n";
		return (char *)Buf;
	}

	for( int i=0 ; i<length() ; i++)
	{
		CShipDesign
			*Design = (CShipDesign *)get(i);
		Buf += Design->print_html(aPlayer, aPP);
	}

	return (char *)Buf;
}

char *
CShipDesignList::building_class_select_html(CPlayer *aPlayer)
{
	static CString
		ClassSelect;
	ClassSelect.clear();

	if (!length())
	{
		ClassSelect = "<TABLE WIDTH=\"550\" BORDER=\"0\" CELLSPACING=\"0\""
						" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
		ClassSelect += "<TR ALIGN=\"CENTER\" VALIGN=\"TOP\">\n";
		ClassSelect += "<TH CLASS=\"maintext\" COLSPAN=\"4\" HEIGHT=\"25\">";
		ClassSelect += GETTEXT("There are no classes you designed.");
		ClassSelect += "</TH>\n";
		ClassSelect += "</TR>\n";
		ClassSelect += "</TABLE>\n";

		return (char *)ClassSelect;
	}

	ClassSelect = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\""
					" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	ClassSelect += "<TR BGCOLOR=\"#171717\">\n";
	ClassSelect += "<TH WIDTH=\"24\" CLASS=\"tabletxt\"><FONT COLOR=\"666666\">"
					"</FONT></TH>\n";
	ClassSelect += "<TH WIDTH=\"130\" CLASS=\"tabletxt\"><FONT COLOR=\"666666\">";
	ClassSelect += GETTEXT("Name");
	ClassSelect += "</FONT></TH>\n";
	ClassSelect += "<TH WIDTH=\"83\" CLASS=\"tabletxt\"><FONT COLOR=\"666666\">";
	ClassSelect += GETTEXT("Size");
	ClassSelect += "</FONT></TH>\n";
	ClassSelect += "<TH WIDTH=\"105\" CLASS=\"tabletxt\"><FONT COLOR=\"666666\">";
	ClassSelect += GETTEXT("Cost<BR>per Ship");
	ClassSelect += "</FONT> </TH>\n";
	ClassSelect += "<TH WIDTH=\"104\" CLASS=\"tabletxt\"><FONT COLOR=\"666666\">";
	ClassSelect += GETTEXT("Building Time<BR>Per Ship");
	ClassSelect += "</FONT></TH>\n";
	ClassSelect += "<TH WIDTH=\"90\" CLASS=\"tabletxt\"><FONT COLOR=\"666666\">";
	ClassSelect += GETTEXT("Ships in<BR>the Pool");
	ClassSelect += "</FONT></TH>\n";
	ClassSelect += "</TR>\n";

	for (int i=0 ; i<length() ; i++)
	{
		CShipDesign *
			ShipDesign = (CShipDesign *)get(i);
		if ( (!ShipDesign->is_black_market_design()) && (!ShipDesign->is_empire_design()) )
		    ClassSelect += ShipDesign->class_info_for_building_html(aPlayer);
	}

	ClassSelect += "</TABLE>\n";

	return (char *)ClassSelect;
}

CShipToBuild::CShipToBuild( int aOwner, CShipDesign *aDesign, int aNumber, int aOrder )
{
	set_owner( aOwner );
	*(CShipDesign*)this = *aDesign;
	mNumber = aNumber;
	if( aOrder <= 0 ) mOrder = time(0);
	else mOrder = aOrder;
}

CShipToBuild::~CShipToBuild()
{
}

CString&
CShipToBuild::query()
{
	static CString
		Query;

	Query.clear();

	switch( type() ){
		case QUERY_INSERT :
			Query.format( "INSERT INTO ship_building_q ( owner, design_id, number, time_order ) VALUES ( %d, %d, %d, %d )",
				get_owner(), get_design_id(), get_number(), get_order() );
			break;
		case QUERY_DELETE :
			Query.format( "DELETE FROM ship_building_q WHERE owner = %d AND time_order = %d", get_owner(), get_order() );
			break;
		case QUERY_UPDATE :
			Query.format( "UPDATE ship_building_q SET number = %d WHERE owner = %d AND time_order = %d", get_number(), get_owner(), get_order() );
			break;
	}

	mStoreFlag.clear();

	return Query;
}

CStoreCenter&
operator<<(CStoreCenter& aStoreCenter, CShipToBuild& aShip)
{
	aStoreCenter.store( aShip );
	return aStoreCenter;
}

int
CShipToBuild::change_number( int aNumber )
{
	mNumber += aNumber;
	if( mNumber < 0 ) mNumber = 0;

	mStoreFlag += STORE_NUMBER;

	return mNumber;
}

const char *
CShipToBuild::print_html(CPlayer *aPlayer, int aPP)
{
	static CString
		Buf;

	Buf = CShipDesign::print_html(aPlayer, aPP);
	Buf.format( "<TR><TD>Number to Build</TD><TD>%d</TD><TD>Delete</TD><TD><INPUT TYPE=\"checkbox\" NAME = \"O%d\" VALUE=\"on\"></TD></TR>\n", mNumber, mOrder );

	return (char*)Buf;
}

char *
CShipToBuild::ship_to_build_info_html(int aIndex)
{
	static CString
		Info;
	Info.clear();

	Info = "<TR>\n";
	Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"57\" ALIGN=\"CENTER\">%d</TD>",
					aIndex+1);
	Info.format("<TD WIDTH=\"126\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>",
					get_name());
	Info.format("<TD WIDTH=\"102\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%d</TD>",
					get_number());

	CShipSize *
		ShipSize = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(get_body());
	Info.format("<TD WIDTH=\"86\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>",
					ShipSize->get_name());

	CArmor *
		Armor = (CArmor *)COMPONENT_TABLE->get_by_id(get_armor());
	CPlayer *
		Owner = PLAYER_TABLE->get_by_game_id(get_owner());

	if (!Owner)
	{
		SLOG("ERROR : There is no owner of this docked ship(Owner : %d)", get_owner());
		return NULL;
	}

	if (Armor->get_armor_type() == AT_BIO &&
		Owner->has_ability(ABILITY_GREAT_SPAWNING_POOL))
	{
		Info += "<TD CLASS=\"tabletxt\" WIDTH=\"95\" ALIGN=\"CENTER\">";
		if (Owner->has_ability(ABILITY_HIVE_SHIP_YARD))
		{
			Info.format("%s PP", dec2unit(ShipSize->get_cost() * 70 / 100));
		}
		else
		{
			Info.format("%s PP", dec2unit(ShipSize->get_cost() * 80 / 100));
		}
		Info += "</TD>\n";
	}
	else
	{
		Info += "<TD CLASS=\"tabletxt\" WIDTH=\"95\" ALIGN=\"CENTER\">";
		if (Owner->has_ability(ABILITY_HIVE_SHIP_YARD))
		{
			Info.format("%s PP", dec2unit(ShipSize->get_cost() * 90 / 100));
		}
		else
		{
			Info.format("%s PP", dec2unit(ShipSize->get_cost()));
		}
		Info += "</TD>\n";
	}

	Info += "<TD CLASS=\"tabletxt\" WIDTH=\"70\" ALIGN=\"CENTER\">";
	Info.format("<A HREF=ship_building_delete.as?INDEX=%d>",
					aIndex);
	Info.format("<IMG SRC=\"%s/image/as_game/bu_delete.gif\" WIDTH=\"57\" HEIGHT=\"11\" BORDER=\"0\"></TD>\n",
				(char *)CGame::mImageServerURL);
	Info += "</A>\n";
	Info += "</TR>\n";

	return (char *)Info;
}

bool
CShipBuildQ::free_item( TSomething aItem )
{
	CShipToBuild
		*Ship = (CShipToBuild*)aItem;
	if(Ship == NULL)
		return false;
	delete Ship;
	return true;
}

int
CShipBuildQ::compare(TSomething aItem1, TSomething aItem2) const
{
	CShipToBuild
		*Ship1 = (CShipToBuild*)aItem1,
		*Ship2 = (CShipToBuild*)aItem2;

	if ( Ship1->get_order() > Ship2->get_order() ) return 1;
	if ( Ship1->get_order() < Ship2->get_order() ) return -1;
	return 0;
}

int
CShipBuildQ::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CShipToBuild
		*Ship = (CShipToBuild*)aItem;

	if ( Ship->get_order() > (int)aKey ) return 1;
	if ( Ship->get_order() < (int)aKey ) return -1;
	return 0;
}

bool
CShipBuildQ::remove_ship_to_build(CShipToBuild *aShip)
{
	int
		index;
	index = find_sorted_key( (TConstSomething)aShip->get_order() );
	if ( index < 0 ) return false;

	return CSortedList::remove(index);
}

int
CShipBuildQ::add_ship_to_build(CShipToBuild *aShip)
{
	if ( !aShip ) return -1;

	if ( find_sorted_key( (TConstSomething)(aShip->get_order()) ) >= 0 )
		return -1;
	insert_sorted( aShip );

	return aShip->get_order();
}

const char*
CShipBuildQ::print_html(CPlayer *aPlayer, int aPP)
{
	static CString
		Buf;

	Buf = "<TABLE>\n";

	for( int i = 0; i < length(); i++ ){
		CShipToBuild
			*Ship = (CShipToBuild*)get(i);
		Buf += Ship->print_html(aPlayer, aPP);
	}

	Buf += "</TABLE>\n";

	return (char*)Buf;
}

char *
CShipBuildQ::building_queue_info_html()
{
	static CString
		QueueInfo;
	QueueInfo.clear();

	if (!length())
	{
		QueueInfo = "<TABLE WIDTH=\"550\" BORDER=\"0\" CELLSPACING=\"0\""
					" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
		QueueInfo += "<TD CLASS=\"maintext\" ALIGN=center>\n";
		QueueInfo += GETTEXT("You have no ships to build.");
		QueueInfo += "</TD>\n";
		QueueInfo += "</TABLE>";

		return (char *)QueueInfo;
	}

	QueueInfo = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\""
				" BORDERCOLOR=\"#2A2A2A\">\n";
	QueueInfo += "<TR BGCOLOR=\"#171717\">\n";
	QueueInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"57\"><FONT COLOR=\"#666666\">";
	QueueInfo += GETTEXT("Priority");
	QueueInfo += "</FONT></TH>\n";
	QueueInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"126\"><FONT COLOR=\"666666\">";
	QueueInfo += GETTEXT("Class Name");
	QueueInfo += "</FONT>\n";
	QueueInfo += "</TH>\n";
	QueueInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"102\"><FONT COLOR=\"#666666\">";
	QueueInfo += GETTEXT("Number of<BR>Ships to Build");
	QueueInfo += "</FONT></TH>\n";
	QueueInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"86\"><FONT COLOR=\"#666666\">";
	QueueInfo += GETTEXT("Ship Size");
	QueueInfo += "</FONT></TH>\n";
	QueueInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"95\"><FONT COLOR=\"666666\">";
	QueueInfo += GETTEXT("Building Cost<BR>Per Ship");
	QueueInfo += "</FONT></TH>\n";
	QueueInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"70\">&nbsp;</TH>\n";
	QueueInfo += "</TR>\n";

	for(int i=0 ; i<length() ; i++)
	{
		CShipToBuild *
			Ship = (CShipToBuild *)get(i);
		QueueInfo += Ship->ship_to_build_info_html(i);
	}
	QueueInfo += "</TABLE>\n";

	return (char *)QueueInfo;
}

CDockedShip::CDockedShip(CShipDesign *aClass, int aNumber):
	CShipDesign()
{
	mNumber = aNumber;
	*(CShipDesign*)this = *aClass;
}

CDockedShip::~CDockedShip()
{
}

int
CDockedShip::change_number(int aNumber)
{
	mNumber += aNumber;
	if (mNumber < 0) mNumber = 0;

	mStoreFlag += STORE_NUMBER;

	CPlayer *
		Owner = PLAYER_TABLE->get_by_game_id(CShipDesign::get_owner());
	if (Owner != NULL)
	{
		if (Owner->get_game_id() != EMPIRE_GAME_ID)
		{
			Owner->get_dock()->refresh_power();
		}
	}

	return mNumber;
}

int
CDockedShip::calc_upkeep()
{
	double
		Upkeep;

#define SPACE_MINING_MODULE		5525

	for (int i=0 ; i<DEVICE_MAX_NUMBER ; i++)
	{
		if (get_device(i) == SPACE_MINING_MODULE) return 0;
	}

#undef SPACE_MINING_MODULE

	CShipSize *
		Body = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(get_body());

	Upkeep = get_number() * Body->get_upkeep();

	return (int)Upkeep;
}

int
CDockedShip::get_power()
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

		for (int i=0 ; i< WEAPON_MAX_NUMBER ; i++)
		{
			Component = COMPONENT_TABLE->get_by_id( get_weapon(i) );
			if( Component == NULL ) continue;

			TotalLevel += Component->get_level();
			TotalComponents++;
		}
		for (int i=0 ; i< DEVICE_MAX_NUMBER ; i++)
		{
			Component = COMPONENT_TABLE->get_by_id( get_device(i) );
			if( Component == NULL ) continue;

			TotalLevel += 5;//+= Component->get_level();
			TotalComponents++;
		}
		CShipSize *
				Ship = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(get_body());

		TotalPower = (float)((float)Ship->get_space() / 100.0f) * (2.5f + (((float)((float)TotalLevel / (float)TotalComponents)) / 2.0f));
		//SLOG("POWER LOGING: Total Power %d, Ship Space %d, Total Level %d, TotalComponent %d, Number Of Ships %d  ",(int)TotalPower, Ship->get_space(),TotalLevel,TotalComponents,get_number());
	return (int)TotalPower * get_number();

}

CString&
CDockedShip::query()
{
	static CString
		Query;

	Query.clear();

	switch( type() ){
		case QUERY_INSERT :
			Query.format( "INSERT INTO docked_ship ( owner, design_id, number ) VALUES ( %d, %d, %d )",
				get_owner(), get_design_id(), get_number() );
			break;
		case QUERY_DELETE :
			Query.format( "DELETE FROM docked_ship WHERE owner = %d AND design_id = %d", get_owner(), get_design_id() );
			break;
		case QUERY_UPDATE :
			Query.format( "UPDATE docked_ship SET number = %d WHERE owner = %d AND design_id = %d", get_number(), get_owner(), get_design_id() );
			break;
	}

	mStoreFlag.clear();

	return Query;
}

const char *
CDockedShip::print_html()
{
	static CString
		Buf;

	Buf.clear();

	Buf.format( "<TR><TD><A HREF=#%d>%s</A></TD>\n", get_design_id(), get_name() );
	Buf.format( "<TD>%d</TD>\n", get_number() );
	Buf.format( "<TD>%s</TD>\n", SHIP_SIZE_TABLE->get_by_id(get_body())->get_name() );
	Buf.format( "<TD>%d</TD>\n", get_build_cost()/10 );
	Buf.format( "<TD><FORM METHOD=POST ACTION=\"/archspace/scrap.as\">\n<INPUT TYPE=\"text\" NAME = \"ScrapNumber\">\n<INPUT TYPE=\"hidden\" NAME=\"DesignID\" VALUE=\"%d\">\n<INPUT TYPE=\"submit\">\n</FORM></TD></TR>\n", get_design_id() );

	return (char*)Buf;
}

char *
CDockedShip::docked_ship_info_html(int aIndex)
{
	static CString
		ShipInfo;
	ShipInfo.clear();

	ShipInfo = "<TR>\n";
	ShipInfo += "<FORM METHOD=post ACTION=ship_pool_scrap_confirm.as";
	if (aIndex == 0)
	{
		ShipInfo += " NAME=\"FIRST\">\n";
	} else
	{
		ShipInfo += ">\n";
	}
	ShipInfo.format("<TD WIDTH=\"126\" CLASS=\"tabletxt\" ALIGN=\"LEFT\">%s</TD>\n",
						get_name());
	ShipInfo.format("<TD CLASS=\"tabletxt\" WIDTH=\"75\" ALIGN=\"CENTER\">%d</TD>\n",
						get_number());

	CShipSize *
		Body = SHIP_SIZE_TABLE->get_by_id(get_body());
	ShipInfo.format("<TD WIDTH=\"98\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
						Body->get_name());

	CArmor *
		Armor = (CArmor *)COMPONENT_TABLE->get_by_id(get_armor());
	CPlayer *
		Owner = PLAYER_TABLE->get_by_game_id(get_owner());

	if (!Owner)
	{
		SLOG("ERROR : There is no owner of this docked ship(Owner : %d)", get_owner());
		return NULL;
	}

	if (Armor->get_armor_type() == AT_BIO &&
		Owner->has_ability(ABILITY_GREAT_SPAWNING_POOL))
	{
		if (Owner->has_ability(ABILITY_HIVE_SHIP_YARD))
		{
			ShipInfo.format("<TD CLASS=\"tabletxt\" WIDTH=\"109\" ALIGN=\"CENTER\">%s</TD>\n",
							dec2unit((Body->get_cost() * 70 / 100) / 10));
		}
		else
		{
			ShipInfo.format("<TD CLASS=\"tabletxt\" WIDTH=\"109\" ALIGN=\"CENTER\">%s</TD>\n",
							dec2unit((Body->get_cost() * 80 / 100) / 10));
		}
	}
	else
	{
		if (Owner->has_ability(ABILITY_HIVE_SHIP_YARD))
		{
			ShipInfo.format("<TD CLASS=\"tabletxt\" WIDTH=\"109\" ALIGN=\"CENTER\">%s</TD>\n",
							dec2unit((Body->get_cost() * 90 / 100) / 10));
		}
		else
		{
			ShipInfo.format("<TD CLASS=\"tabletxt\" WIDTH=\"109\" ALIGN=\"CENTER\">%s</TD>\n",
										dec2unit(Body->get_cost()/10));
		}
	}

	ShipInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"130\" ALIGN=\"CENTER\">\n";
	ShipInfo += "<INPUT TYPE=\"text\" SIZE=\"10\" MAXLENGTH=\"15\" NAME=\"SCRAP_NUMBER\" CLASS=\"newInput\">\n";
	ShipInfo.format("<INPUT TYPE=\"hidden\" NAME=\"DESIGN_ID\" VALUE=\"%d\">\n",
					get_design_id());
	ShipInfo.format("<INPUT TYPE=\"image\" SRC=\"%s/image/as_game/bu_ok.gif\" WIDTH=\"31\" HEIGHT=\"11\"></TD>\n",
					(char *)CGame::mImageServerURL);
	ShipInfo += "</FORM>\n";
	ShipInfo += "</TR>\n";

	return (char *)ShipInfo;
}

CStoreCenter&
operator<<(CStoreCenter& aStoreCenter, CDockedShip& aShip)
{
	aStoreCenter.store( aShip );
	return aStoreCenter;
}

CDock::CDock()
{
	mPower = 0;
}

CDock::~CDock()
{
	remove_all();
}

bool
CDock::free_item( TSomething aItem )
{
	CDockedShip
		*Ship = (CDockedShip*)aItem;
	if(Ship == NULL)
		return false;
	delete Ship;
	return true;
}

int
CDock::compare(TSomething aItem1, TSomething aItem2 ) const
{
	CDockedShip
		*Ship1 = (CDockedShip*)aItem1,
		*Ship2 = (CDockedShip*)aItem2;

	if ( Ship1->get_design_id() > Ship2->get_design_id() ) return 1;
	if ( Ship1->get_design_id() < Ship2->get_design_id() ) return -1;
	return 0;
}

int
CDock::compare_key(TSomething aItem, TConstSomething aKey ) const
{
	CDockedShip
		*Ship = (CDockedShip*)aItem;

	if ( Ship->get_design_id() > (int)aKey ) return 1;
	if ( Ship->get_design_id() < (int)aKey ) return -1;
	return 0;
}

bool
CDock::remove_docked_ship(CDockedShip *aShip)
{
	int
		index;
	index = find_sorted_key( (TConstSomething)aShip->get_design_id() );
	if ( index < 0 ) return false;

	if (CSortedList::remove(index) == true)
	{
		refresh_power();
		return true;
	}

	return false;
}

int
CDock::add_docked_ship(CDockedShip *aShip)
{
	if ( !aShip ) return -1;

	if ( find_sorted_key( (TConstSomething)(aShip->get_design_id()) ) >= 0 )
		return -1;
	insert_sorted( aShip );

	refresh_power();

	return aShip->get_design_id();
}

CDockedShip*
CDock::get_by_id( int aDesignID )
{
	if ( !length() ) return NULL;

	int
		index;
	index = find_sorted_key( (void*)aDesignID );

	if ( index < 0 ) return NULL;

	return (CDockedShip*)get(index);
}

int
CDock::count_ship( int aDesignID )
{
	CDockedShip
		*Ship = get_by_id( aDesignID );

	if( Ship == NULL )
		return 0;
	else
		return Ship->get_number();
}

bool
CDock::change_ship( CShipDesign *aClass, int aNumber )
{
	if (!aClass || aNumber == 0) return false;

	CDockedShip *
		Ship = get_by_id( aClass->get_design_id() );

	if (!Ship)
	{
		if (aNumber < 0) return false;

		Ship = new CDockedShip(aClass, aNumber);

		add_docked_ship(Ship);

		Ship->type(QUERY_INSERT);
		STORE_CENTER->store(*Ship);
	}
	else
	{
		int
			NewShipNumber = Ship->get_number() + aNumber;

		if (NewShipNumber <= 0)
		{
			Ship->type(QUERY_DELETE);
			STORE_CENTER->store(*Ship);
			remove_docked_ship(Ship);
		}
		else
		{
			Ship->change_number(aNumber);

			Ship->type(QUERY_UPDATE);
			*STORE_CENTER << *Ship;
		}
	}

	CPlayer *
		Owner = PLAYER_TABLE->get_by_game_id(Ship->get_owner());
	if (Owner) Owner->refresh_power();

	return true;
}

bool
CDock::refresh_power()
{
	int
		OwnerGameID = 0;

	mPower = 0;
	for(int i=0 ; i<length() ; i++)
	{
		CDockedShip *
			DockedShip = (CDockedShip *)get(i);

		int
			AvailablePower = MAX_POWER - mPower;
		if (DockedShip->get_power() > AvailablePower)
		{
			mPower = MAX_POWER;
			break;
		}
		else
		{
			mPower += DockedShip->get_power();
		}

		OwnerGameID = DockedShip->CShipDesign::get_owner();
	}

	CPlayer *
		Owner = PLAYER_TABLE->get_by_game_id(OwnerGameID);
	if (Owner == NULL)
	{
		SLOG("ERROR : Owner is NULL in CDock::refresh_power(), OwnerGameID = %d", OwnerGameID);
	}
	else if (Owner->get_game_id() != EMPIRE_GAME_ID)
	{
		Owner->refresh_power();
	}

	return true;
}

const char *
CDock::print_html()
{
	static CString
		Buf;

	Buf = "<TABLE>\n";

	Buf += "<TR>\n";
	Buf.format("<TD>%s</TD>\n", GETTEXT("Ship Class"));
	Buf.format("<TD>%s</TD>\n", GETTEXT("Available Ships"));
	Buf.format("<TD>%s</TD>\n", GETTEXT("Size"));
	Buf.format("<TD>%s</TD>\n", GETTEXT("Scrap Income per Ship"));
	Buf.format("<TD>%s</TD>\n", GETTEXT("Ships to Scrap"));
	Buf += "</TR>\n";

	for( int i = 0; i < length(); i++ ){
		CDockedShip
			*Ship = (CDockedShip*)get(i);

		Buf += Ship->print_html();
	}

	Buf += "</TABLE>\n";

	return (char *)Buf;
}

int
CDock::get_ship_number()
{
	int
		Total = 0;

	for (int i=0 ; i<length() ; i++)
	{
		Total += ((CDockedShip *)get(i))->get_number();
	}

	return Total;
}

char *
CDock::dock_info_html()
{
	static CString
		DockInfo;
	DockInfo.clear();

	if (!length())
	{
		DockInfo = "<TABLE BORDER=\"0\">";
		DockInfo += "<TR>\n";
		DockInfo.format("<TD CLASS=\"maintext\" ALIGN=\"center\">%s</TD>\n",
						GETTEXT("You have no ships in the pool."));
		DockInfo += "</TR>\n";
		DockInfo += "</TABLE>\n";

		return (char *)DockInfo;
	}

	DockInfo = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\""
				" BORDERCOLOR=\"#2A2A2A\">\n";

	DockInfo += "<TR BGCOLOR=\"#171717\">\n";

	DockInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"126\" BGCOLOR=\"#171717\">";
	DockInfo.format("<FONT COLOR=\"666666\">%s</FONT></TH>\n",
						GETTEXT("Ship Class"));

	DockInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"75\" BGCOLOR=\"#171717\">";
	DockInfo.format("<FONT COLOR=\"666666\">%s</FONT></TH>\n",
						GETTEXT("Ships<BR>Available"));

	DockInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"98\" BGCOLOR=\"#171717\">";
	DockInfo.format("<FONT COLOR=\"666666\">%s</FONT></TH>\n",
						GETTEXT("Ship Size"));

	DockInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"109\" BGCOLOR=\"#171717\">";
	DockInfo.format("<FONT COLOR=\"666666\">%s</FONT></TH>\n",
						GETTEXT("Scrap Income<BR>per Ship"));

	DockInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"130\" BGCOLOR=\"#171717\">";
	DockInfo.format("<FONT COLOR=\"666666\">%s</FONT></TH>\n",
						GETTEXT("Ships<BR>to Scrap"));

	DockInfo += "</TR>\n";

	for (int i=0 ; i<length() ; i++)
	{
		CDockedShip *
			Ship = (CDockedShip *)get(i);

		DockInfo += Ship->docked_ship_info_html(i);
	}

	DockInfo += "</TABLE>\n";

	return (char *)DockInfo;
}

char *
CDock::radio_ship_reassignment_html(int designID)
{
	static CString
		DockInfo;
	DockInfo.clear();

	if (!length())
	{
		DockInfo = "<TR ALIGN=\"CENTER\" VALIGN=\"TOP\">\n";
		DockInfo += "<TD CLASS=\"maintext\">";
		DockInfo += GETTEXT("You don't have any ships in the pool.");
		DockInfo += "</TD>\n";
		DockInfo += "</TR>\n";

		return (char *)DockInfo;
	}
	else if(length() == 1)
	{
		CDockedShip *
			ship = (CDockedShip *)get(0);
		if( ship->get_design_id() == designID )
		{
			DockInfo = "<TR ALIGN=\"CENTER\" VALIGN=\"TOP\">\n";
			DockInfo += "<TD CLASS=\"maintext\">";
			DockInfo += GETTEXT("You don't have any ships in the pool for reassignment.");
			DockInfo += "</TD>\n";
			DockInfo += "</TR>\n";
			DockInfo += "<TR ALIGN=\"CENTER\" VALIGN=\"TOP\">\n";
			DockInfo += "<TD CLASS=\"maintext\">";
			DockInfo += GETTEXT("Use Remove/Add Ship menu instead of Ship Reassignment menu if you just want to change number of ships.");
			DockInfo += "</TD>\n";
			DockInfo += "</TR>\n";
			return (char *)DockInfo;
		}
	}

	DockInfo = "<TABLE WIDTH=\"550\" BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n";
	DockInfo += "<TR>\n";
	DockInfo += "<TD WIDTH=\"260\" ALIGN=\"LEFT\" VALIGN=\"TOP\">\n";
	DockInfo += "<TABLE WIDTH=\"260\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	DockInfo += "<TR BGCOLOR=\"#171717\">\n";

	DockInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"120\" BGCOLOR=\"#171717\"><FONT COLOR=\"666666\">";
	DockInfo += GETTEXT("Ship Class");
	DockInfo += "</FONT></TH>\n";

	DockInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"80\" BGCOLOR=\"#171717\"><FONT COLOR=\"666666\">";
	DockInfo += GETTEXT("Available Ships");
	DockInfo += "</FONT></TH>\n";

	DockInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"60\" BGCOLOR=\"#171717\"><FONT COLOR=\"666666\">";
	DockInfo += "</FONT></TH>\n";
	DockInfo += "</TR>\n";

	for (int i=0 ; i<length() ; i += 2)
	{
		CDockedShip *
			Ship = (CDockedShip *)get(i);

		DockInfo += "<TR>\n";

		DockInfo += "<TD WIDTH=\"120\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">\n";
		DockInfo.format("<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
						Ship->get_design_id(), Ship->get_name());
		DockInfo += "</TD>\n";

		DockInfo.format("<TD WIDTH=\"80\" CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
						Ship->get_number());

		DockInfo += "<TD WIDTH=\"60\" CLASS=tabletxt ALIGN=CENTER>\n";
		if(Ship->get_design_id() != designID)
		{
		DockInfo.format("<INPUT TYPE=RADIO NAME=SHIP VALUE=%d>\n",
						Ship->get_design_id());
		}
		else
		{
			DockInfo += "&nbsp;";
		}
		DockInfo += "</TD>\n";

		DockInfo += "</TR>\n";
	}

	DockInfo += "</TABLE>\n";
	DockInfo += "</TD>\n";
	DockInfo += "<TD WIDTH=\"30\">&nbsp;</TD>\n";
	DockInfo += "<TD WIDTH=\"260\" ALIGN=\"RIGHT\" VALIGN=\"TOP\">\n";
	DockInfo += "<TABLE WIDTH=\"260\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	DockInfo += "<TR BGCOLOR=\"#171717\">\n";
	DockInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"120\" BGCOLOR=\"#171717\"><FONT COLOR=\"666666\">";
	DockInfo += GETTEXT("Ship Class");
	DockInfo += "</FONT></TH>\n";
	DockInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"80\" BGCOLOR=\"#171717\"><FONT COLOR=\"666666\">";
	DockInfo += GETTEXT("Available Ships");
	DockInfo += "</FONT></TH>\n";
	DockInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"60\" BGCOLOR=\"#171717\"><FONT COLOR=\"666666\">";
	DockInfo += "</FONT></TH>\n";
	DockInfo += "</TR>\n";

	for (int i=1 ; i<length() ; i += 2)
	{
		CDockedShip *
			Ship = (CDockedShip *)get(i);

		DockInfo += "<TR>\n";

		DockInfo += "<TD WIDTH=\"120\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">\n";
		DockInfo.format("<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
						Ship->get_design_id(), Ship->get_name());
		DockInfo += "</TD>\n";

		DockInfo.format("<TD WIDTH=\"80\" CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
						Ship->get_number());

		DockInfo += "<TD WIDTH=\"60\" CLASS=tabletxt ALIGN=CENTER>\n";
		if( Ship->get_design_id() != designID )
		{
			DockInfo.format("<INPUT TYPE=RADIO NAME=SHIP VALUE=%d>\n",
						Ship->get_design_id());
		}
		else
		{
			DockInfo += "&nbsp;";
		}
		DockInfo += "</TD>\n";

		DockInfo += "</TR>\n";
	}

	DockInfo += "</TABLE>\n";
	DockInfo += "</TD>\n";
	DockInfo += "</TR>\n";
	DockInfo += "</TABLE>\n";

	return (char *)DockInfo;
}

char*
CDock::print_html_select()
{
	static CString
		Buf;

	Buf = "<SELECT NAME=\"SHIP_CLASS_ID\" onChange=\"shipSel()\">\n";

	for(int i=0 ; i<length(); i++)
	{
		CDockedShip *
			DockedShip = (CDockedShip *)get(i);
		CShipDesign *
			Class = (CShipDesign *)DockedShip;

		Buf.format("<OPTION VALUE = %d>%s</OPTION>\n",
					Class->CShipDesign::get_design_id(), Class->get_name());
	}

	Buf += "</SELECT><BR>\n";

	return (char *)Buf;
}

char*
CDock::print_javascript_select()
{
	static CString
		Buf;
	Buf.clear();

	for(int i=0 ; i<length(); i++)
	{
		CDockedShip *
			DockedShip = (CDockedShip *)get(i);
		CShipDesign *
			Class = (CShipDesign *)DockedShip;

		Buf.format("<OPTION VALUE=\\\"%d\\\">%s</OPTION>",
					Class->CShipDesign::get_design_id(), Class->get_name());
	}
	Buf += "</SELECT>";
	return (char *)Buf;
}

CDamagedShip::CDamagedShip(CShipDesign *aClass, int aHP, int aID)
{
	*(CShipDesign*)this = *aClass;
	mID = aID;
	mHP = aHP;
}

CDamagedShip::~CDamagedShip()
{
}

CString&
CDamagedShip::query()
{
	static CString
		Query;

	Query.clear();

	switch (mQueryType)
	{
		case QUERY_INSERT :
			Query.format("INSERT INTO damaged_ship ( owner, id, design_id, hp ) VALUES ( %d, %d, %d, %d)",
				get_owner(), get_id(), get_design_id(), get_hp() );
			break;
		case QUERY_DELETE :
			Query.format("DELETE FROM damaged_ship WHERE owner = %d AND id = %d", get_owner(), get_id());
			break;
		case QUERY_UPDATE :
			Query.format("UPDATE damaged_ship SET hp = %d WHERE owner = %d AND id = %d", get_hp(), get_owner(), get_id());
			break;
	}

	mStoreFlag.clear();

	return Query;
}

int
CDamagedShip::change_hp(int aHP)
{
	mHP += aHP;
	if (mHP > get_max_hp()) mHP = get_max_hp();
	if (mHP < 1) mHP = 1;

	mStoreFlag += STORE_HP;

	return mHP;
}

int
CDamagedShip::repair(int aPercent)
{
	int
		HP = get_max_hp() * aPercent / 100;

	return change_hp(HP);
}

bool
CDamagedShip::is_fully_repaired()
{
	if (mHP >= get_max_hp()) return true;

	return false;
}

int
CDamagedShip::get_repair_ratio()
{
	return 100*mHP/get_max_hp();
}

const char *
CDamagedShip::print_html()
{
	static CString
		Buf;

	Buf.clear();

	Buf.format("<TR><TD><A HREF=#%d>%s</A></TD>\n", get_design_id(), get_name());
	Buf.format("<TD>%s</TD>\n", SHIP_SIZE_TABLE->get_by_id(get_body())->get_name());
	Buf.format("<TD>%d/%d</TD></TR>\n", mHP, get_max_hp());

	return (char*)Buf;
}

CStoreCenter&
operator<<(CStoreCenter& aStoreCenter, CDamagedShip& aShip)
{
	aStoreCenter.store( aShip );
	return aStoreCenter;
}

bool
CRepairBay::free_item( TSomething aItem )
{
	CDamagedShip
		*Ship = (CDamagedShip*)aItem;
	if(Ship == NULL)
		return false;
	delete Ship;
	return true;
}

int
CRepairBay::compare(TSomething aItem1, TSomething aItem2) const
{
	CDamagedShip
		*Ship1 = (CDamagedShip*)aItem1,
		*Ship2 = (CDamagedShip*)aItem2;

	if ( Ship1->get_id() > Ship2->get_id() ) return 1;
	if ( Ship1->get_id() < Ship2->get_id() ) return -1;
	return 0;
}

int
CRepairBay::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CDamagedShip
		*Ship = (CDamagedShip*)aItem;

	if ( Ship->get_id() > (int)aKey ) return 1;
	if ( Ship->get_id() < (int)aKey ) return -1;
	return 0;
}

bool
CRepairBay::remove_damaged_ship(CDamagedShip *aShip)
{
	int
		index;
	index = find_sorted_key( (TConstSomething)aShip->get_id() );
	if ( index < 0 ) return false;

	return CSortedList::remove(index);
}

int
CRepairBay::add_damaged_ship(CDamagedShip *aShip)
{
	if ( !aShip ) return -1;

	if( aShip->get_id() < 0 ){
		if( length() == 0 ){
			aShip->set_id(1);
		} else {
			CDamagedShip
				*LastShip = (CDamagedShip*)get(length()-1);
			aShip->set_id(LastShip->get_id()+1);
		}
	}

	insert_sorted( aShip );

	return aShip->get_id();
}

CDamagedShip*
CRepairBay::get_by_id( int aID )
{
	if ( !length() ) return NULL;

	int
		index;
	index = find_sorted_key( (void*)aID );

	if ( index < 0 ) return NULL;

	return (CDamagedShip*)get(index);
}

const char *
CRepairBay::print_html()
{
	static CString
		Buf;

	Buf = "<TABLE><TR><TD>Ship Class</TD>\n<TD>Size</TD>\n<TD> HP / Max HP </TD></TR>\n";

	for( int i = 0; i < length(); i++ ){
		CDamagedShip
			*Ship = (CDamagedShip*)get(i);

		Buf += Ship->print_html();
	}

	Buf += "</TABLE>\n";

	return (char*)Buf;

}

char *
CRepairBay::repairing_bay_info_html()
{
	static CString
		BayInfo;
	BayInfo.clear();

	if (!length())
	{
		BayInfo = GETTEXT("You don't have any damaged ships.");

		return (char *)BayInfo;
	}

	BayInfo = "<TABLE WIDTH=\"550\" BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n";
	BayInfo += "<TR>\n";

	BayInfo += "<TD ROWSPAN=\"4\" WIDTH=\"262\" ALIGN=\"LEFT\" VALIGN=\"TOP\">\n";
	BayInfo += "<TABLE WIDTH=\"260\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";

	BayInfo += "<TR BGCOLOR=\"#171717\">\n";

	BayInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"109\" BGCOLOR=\"#171717\"><FONT COLOR=\"666666\">";
	BayInfo += GETTEXT("Ship Class");
	BayInfo += "</FONT></TH>\n";

	BayInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"73\" BGCOLOR=\"#171717\"><FONT COLOR=\"666666\">";
	BayInfo += GETTEXT("Ship Size");
	BayInfo += "</FONT></TH>\n";

	BayInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"70\" BGCOLOR=\"#171717\"><FONT COLOR=\"666666\">";
	BayInfo += GETTEXT("Current HP<BR>/MAX HP");
	BayInfo += "</FONT></TH>\n";

	BayInfo += "</TR>\n";

	for (int i=0 ; i<length() ; i += 2)
	{
		CDamagedShip *
			Ship = (CDamagedShip *)get(i);

		BayInfo += "<TR>\n";
		BayInfo.format("<TD WIDTH=\"109\" CLASS=\"tabletxt\" ALIGN=\"LEFT\">&nbsp;%s</TD>\n",
						Ship->get_name());
		CShipSize *
			Body = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(Ship->get_body());
		BayInfo.format("<TD WIDTH=\"73\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
						Body->get_name());
		BayInfo.format("<TD WIDTH=\"70\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%d / %d</TD>\n",
						Ship->get_hp(), Ship->get_max_hp());
		BayInfo += "</TR>\n";
	}

	BayInfo += "</TABLE>\n";
	BayInfo += "</TD>\n";

	BayInfo += "<TD ROWSPAN=\"4\">&nbsp;</TD>\n";

	BayInfo += "<TD ROWSPAN=\"4\" WIDTH=\"262\" ALIGN=\"RIGHT\" VALIGN=\"TOP\">\n";
	BayInfo += "<TABLE WIDTH=\"260\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	BayInfo += "<TR BGCOLOR=\"#171717\">\n";

	BayInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"109\" BGCOLOR=\"#171717\"><FONT COLOR=\"666666\">";
	BayInfo += GETTEXT("Ship Class");
	BayInfo += "</FONT></TH>\n";

	BayInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"73\" BGCOLOR=\"#171717\"><FONT COLOR=\"666666\">";
	BayInfo += GETTEXT("Ship Size");
	BayInfo += "</FONT></TH>\n";

	BayInfo += "<TH CLASS=\"tabletxt\" WIDTH=\"70\" BGCOLOR=\"#171717\"><FONT COLOR=\"666666\">";
	BayInfo += GETTEXT("Current HP<BR>/MAX HP");
	BayInfo += "</FONT></TH>\n";

	BayInfo += "</TR>\n";

	for (int i=1 ; i<length() ; i += 2)
	{
		CDamagedShip *
			Ship = (CDamagedShip *)get(i);

		BayInfo += "<TR>\n";
		BayInfo.format("<TD WIDTH=\"109\" CLASS=\"tabletxt\" ALIGN=\"LEFT\">&nbsp;%s</TD>\n",
						Ship->get_name());
		CShipSize *
			Body = (CShipSize *)SHIP_SIZE_TABLE->get_by_id(Ship->get_body());
		BayInfo.format("<TD WIDTH=\"73\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
						Body->get_name());
		BayInfo.format("<TD WIDTH=\"70\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%d / %d</TD>\n",
						Ship->get_hp(), Ship->get_max_hp());
		BayInfo += "</TR>\n";
	}

	BayInfo += "</TABLE>\n";
	BayInfo += "</TD>\n";

	BayInfo += "</TR>\n";
	BayInfo += "</TABLE>\n";

	return (char *)BayInfo;
}

/* end of file ship.cc */

