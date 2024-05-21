#include <libintl.h>
#include "fortress.h"
#include "archspace.h"
#include "game.h"
#include "council.h"

TZone gFortressZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CFortress),
	0,
	0,
	NULL,
	"Zone CFortress"
};

CFortress::CFortress()
{
}

CFortress::~CFortress()
{
}

CString &
CFortress::query()
{
	static CString 
		Query;
	Query.clear();

	switch (mQueryType)
	{
		case QUERY_INSERT :
			break;

		case QUERY_UPDATE :
		{
			Query.format("UPDATE fortress SET layer = %d, sector = %d, fortress_order = %d",
							mLayer, mSector, mOrder);

#define STORE(x, y, z) \
			if (mStoreFlag.has(x)) \
				Query.format(y, z)

			STORE(STORE_OWNER_ID, ", owner = %d", mOwnerID);

			Query.format(" WHERE layer = %d AND sector = %d AND fortress_order = %d",
							mLayer, mSector, mOrder);

			break;
		}

		case QUERY_DELETE :
			break;
	}

	mStoreFlag.clear();

	return Query;
}

CPlayer *
CFortress::get_owner2()
{
	CPlayer *
		Owner = PLAYER_TABLE->get_by_game_id(mOwnerID);
	return Owner;
}

bool
CFortress::set_layer(int aLayer)
{
	if (aLayer < 1 || aLayer > 4) return false;

	mLayer = aLayer;
	mStoreFlag += STORE_LAYER;

	refresh_name();

	return true;
}

bool
CFortress::set_sector(int aSector)
{
	if (aSector < 1) return false;

	mSector = aSector;
	mStoreFlag += STORE_SECTOR;

	refresh_name();

	return true;
}

bool
CFortress::set_order(int aOrder)
{
	if (aOrder < 0) return false;

	mOrder = aOrder;
	mStoreFlag += STORE_ORDER;

	refresh_name();

	return true;
}

bool
CFortress::set_owner_id(int aOwnerID)
{
	CPlayer *
		Owner = PLAYER_TABLE->get_by_game_id(aOwnerID);
	if (Owner == NULL)
	{
		mOwnerID = EMPIRE_GAME_ID;
	}
	else
	{
		mOwnerID = aOwnerID;
	}
	mStoreFlag += STORE_OWNER_ID;
	return true;
}

bool
CFortress::add_admiral(CAdmiral *aAdmiral)
{
	return mAdmiralList.add_admiral(aAdmiral);
}

bool
CFortress::remove_admiral(int aAdmiralID)
{
	return mAdmiralList.remove_admiral(aAdmiralID);
}

bool
CFortress::add_fleet(CFleet *aFleet)
{
	return mFleetList.add_fleet(aFleet);
}

bool
CFortress::remove_fleet(int aFleetID)
{
	return mFleetList.remove_fleet(aFleetID);
}

bool
CFortress::is_dead()
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
CFortress::regen_empire_fleets()
{
	if (is_dead() == true) return true;

	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)mFleetList.get(i);
		int
			CurrentShip = Fleet->get_current_ship();
		if (mLayer == 1)
		{
			Fleet->set_current_ship(CurrentShip + CEmpire::mAmountOfFortress1ShipRegen);
		}
		else if (mLayer == 2)
		{
			Fleet->set_current_ship(CurrentShip + CEmpire::mAmountOfFortress2ShipRegen);
		}
		else if (mLayer == 3)
		{
			Fleet->set_current_ship(CurrentShip + CEmpire::mAmountOfFortress3ShipRegen);
		}
		else if (mLayer == 4)
		{
			Fleet->set_current_ship(CurrentShip + CEmpire::mAmountOfFortress4ShipRegen);
		}
		else
		{
			SLOG("ERROR : Wrong Fortress Layer in CFortress::regen_empire_fleets(), mLayer = %d", mLayer);
		}

		Fleet->type(QUERY_UPDATE);
		STORE_CENTER->store(*Fleet);
	}

	return true;
}

bool
CFortress::update_DB()
{
	if (is_changed() == true)
	{
		type(QUERY_UPDATE);
		STORE_CENTER->store(*this);
	}

	mAdmiralList.update_DB();
	mFleetList.update_DB();

	return true;
}

bool
CFortress::initialize(int aLayer, int aSector, int aOrder, int aOwnerID)
{
	SLOG("SYSTEM : Initialize Fortress(Layer : %d, Sector : %d, Order : %d) Fleet/Admiral", aLayer, aSector, aOrder);

	mLayer = aLayer;
	mSector = aSector;
	mOrder = aOrder;
	mOwnerID = aOwnerID;

	refresh_name();

	mFleetList.create_as_empire_fleet_group_static(&mAdmiralList, CEmpire::EMPIRE_FLEET_GROUP_TYPE_FORTRESS, mLayer);

	/* Store Fortress Admiral and Fortress Fleet */
	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)mFleetList.get(i);
		CAdmiral *
			Admiral = mAdmiralList.get_by_id(Fleet->get_admiral_id());
		if (Admiral == NULL)
		{
			SLOG("ERROR : No fortress(#%d, #%d, #%d) admiral found in CFortress::initialize(), Fleet->get_admiral_id() = %d", mLayer, mSector, mOrder, Fleet->get_admiral_id());
			continue;
		}

		Fleet->type(QUERY_INSERT);
		STORE_CENTER->store(*Fleet);

		Admiral->type(QUERY_INSERT);
		STORE_CENTER->store(*Admiral);
	}

	/* Store Fortress Admiral Info and Fortress Fleet Info */
	for (int i=0 ; i<mFleetList.length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)mFleetList.get(i);
		CAdmiral *
			Admiral = mAdmiralList.get_by_id(Fleet->get_admiral_id());

		CEmpireAdmiralInfo *
			EmpireAdmiralInfo = new CEmpireAdmiralInfo();
		EmpireAdmiralInfo->set_admiral_id(Admiral->get_id());
		EmpireAdmiralInfo->set_admiral_type(CEmpire::LAYER_FORTRESS);
		EmpireAdmiralInfo->set_position_arg1(mLayer);
		EmpireAdmiralInfo->set_position_arg2(mSector);
		EmpireAdmiralInfo->set_position_arg3(mOrder);

		EmpireAdmiralInfo->type(QUERY_INSERT);
		STORE_CENTER->store(*EmpireAdmiralInfo);

		CEmpireAdmiralInfoList *
			EmpireAdmiralInfoList = EMPIRE->get_empire_admiral_info_list();
		EmpireAdmiralInfoList->add_empire_admiral_info(EmpireAdmiralInfo);

		CEmpireFleetInfo *
			EmpireFleetInfo = new CEmpireFleetInfo();
		EmpireFleetInfo->set_fleet_id(Fleet->get_id());
		EmpireFleetInfo->set_fleet_type(CEmpire::LAYER_FORTRESS);
		EmpireFleetInfo->set_position_arg1(mLayer);
		EmpireFleetInfo->set_position_arg2(mSector);
		EmpireFleetInfo->set_position_arg3(mOrder);

		EmpireFleetInfo->type(QUERY_INSERT);
		STORE_CENTER->store(*EmpireFleetInfo);

		CEmpireFleetInfoList *
			EmpireFleetInfoList = EMPIRE->get_empire_fleet_info_list();
		EmpireFleetInfoList->add_empire_fleet_info(EmpireFleetInfo);
	}

	return true;
}

bool
CFortress::is_taken_by_council(CCouncil *aCouncil)
{
	CPlayer *
		Owner = aCouncil->get_member_by_game_id(mOwnerID);
	if (Owner != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool
CFortress::is_attackable(CPlayer *aAttacker)
{
	CFortressList *
		FortressList = EMPIRE->get_fortress_list();

	CCouncil *
		Council = aAttacker->get_council();
	int
		CouncilID = Council->get_id();

	switch (mLayer)
	{
		case 1 :
		{
			int
				NumberOfPlanet = EMPIRE_PLANET_INFO_LIST->get_number_of_planets_by_owner(aAttacker, CEmpire::LAYER_EMPIRE_PLANET);
			if (NumberOfPlanet == 0) return false;

			if (mSector != (CouncilID%16 + 1)) return false;

			if (is_taken_by_council(Council) == true) return false;

			return true;
		}
			break;

		case 2 :
		{
			bool
				AnyTakenFortress = false;

			for (int i=0 ; i<FortressList->length() ; i++)
			{
				CFortress *
					Fortress = (CFortress *)FortressList->get(i);
				if (Fortress->get_layer() != 1) continue;

				if (Fortress->is_taken_by_council(Council) == true)
				{
					AnyTakenFortress = true;
					break;
				}
			}

			if (AnyTakenFortress == false) return false;

			if (mSector != (CouncilID%8 + 1)) return false;

			if (is_taken_by_council(Council) == true) return false;

			return true;
		}
			break;

		case 3 :
		{
			bool
				AnyTakenFortress = false;

			for (int i=0 ; i<FortressList->length() ; i++)
			{
				CFortress *
					Fortress = (CFortress *)FortressList->get(i);
				if (Fortress->get_layer() != 2) continue;

				if (Fortress->is_taken_by_council(Council) == true)
				{
					AnyTakenFortress = true;
					break;
				}
			}

			if (AnyTakenFortress == false) return false;

			if (mSector != (CouncilID%4 + 1)) return false;

			if (is_taken_by_council(Council) == true) return false;

			return true;
		}
			break;

		case 4 :
		{
			bool
				AnyTakenFortress = false;

			for (int i=0 ; i<FortressList->length() ; i++)
			{
				CFortress *
					Fortress = (CFortress *)FortressList->get(i);
				if (Fortress->get_layer() != 3) continue;

				if (Fortress->is_taken_by_council(Council) == true)
				{
					AnyTakenFortress = true;
					break;
				}
			}

			if (AnyTakenFortress == false) return false;

			if (mSector != (CouncilID%2 + 1)) return false;

			if (is_taken_by_council(Council) == true) return false;

			return true;
		}
			break;

		default :
		{
			SLOG("ERROR : Wrong mLayer in CFortress::is_attackable(), mLayer = %d, mSector = %d, mOrder = %d", mLayer, mSector, mOrder);
			return false;
		}
			break;
	}
}

void
CFortress::refresh_name()
{
	mName.clear();

	CString
		LayerString,
		SectorString,
		OrderString;
	LayerString = dec2unit(mLayer);
	SectorString = dec2unit(mSector);
	OrderString = dec2unit(mOrder);

	mName.format(GETTEXT("Fortress (#%1$s/#%2$s/#%3$s)"),
					(char *)LayerString, (char *)SectorString, (char *)OrderString);
}

CFortressList::CFortressList()
{
}

CFortressList::~CFortressList()
{
}

bool
CFortressList::free_item(TSomething aItem)
{
	CFortress *
		Fortress = (CFortress *)aItem;
	if (Fortress == NULL) return false;

	delete Fortress;

	return true;
}

int
CFortressList::compare(TSomething aItem1, TSomething aItem2) const
{
	CFortress
		*Fortress1 = (CFortress *)aItem1,
		*Fortress2 = (CFortress *)aItem2;

	if (Fortress1->get_key() > Fortress2->get_key()) return 1;
	if (Fortress1->get_key() < Fortress2->get_key()) return -1;
	return 0;
}

int
CFortressList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CFortress *
		Fortress = (CFortress *)aItem;

	if (Fortress->get_key() > (int)aKey) return 1;
	if (Fortress->get_key() < (int)aKey) return -1;
	return 0;
}

bool
CFortressList::add_fortress(CFortress *aFortress)
{
	if (!aFortress) return false;

	if (find_sorted_key((TConstSomething)aFortress->get_key()) >= 0) 
		return false;
	insert_sorted(aFortress);

	return true;
}

bool
CFortressList::remove_fortress(int aLayer, int aSector, int aOrder)
{
	int 
		Index = find_sorted_key((void *)CFortress::get_key(aLayer, aSector, aOrder));
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

CFortress *
CFortressList::get_by_layer_sector_order(int aLayer, int aSector, int aOrder)
{
	if (length() == 0) return NULL;

	int 
		Index = find_sorted_key((void *)CFortress::get_key(aLayer, aSector, aOrder));

	if (Index < 0) return NULL;

	return (CFortress *)get(Index);
}

CFortress *
CFortressList::get_by_key(int aKey)
{
	if (length() == 0) return NULL;

	int 
		Index = find_sorted_key((void *)aKey);

	if (Index < 0) return NULL;

	return (CFortress *)get(Index);
}

bool
CFortressList::update_DB()
{
	for (int i=0 ; i<length() ; i++)
	{
		CFortress *
			Fortress = (CFortress *)get(i);
		Fortress->update_DB();
	}

	return true;
}

int
CFortressList::get_number_of_fortresses_taken_by_council(CCouncil *aCouncil, int aLayer)
{
	int
		Number = 0;

	for (int i=0 ; i<length() ; i++)
	{
		CFortress *
			Fortress = (CFortress *)get(i);
		if (Fortress->get_layer() == aLayer || aLayer == 0)
		{
			if (aCouncil->get_member_by_game_id(Fortress->get_owner_id()) != NULL)
			{
				Number++;
			}
		}
	}

	return Number;
}

char *
CFortressList::get_fortress_status_html(CPlayer *aPlayer)
{
	CCouncil *
		Council = aPlayer->get_council();
	int
		CouncilID = Council->get_id();

	CString
		BlankLine;
	BlankLine = "<TR>\n";
	BlankLine += "<TD>&nbsp;</TD>\n";
	BlankLine += "</TR>\n";

	CString
		FirstRow;
	FirstRow = "<TR BGCOLOR=#171717>\n";
	FirstRow += "<TH ALIGN=\"CENTER\" WIDTH=30>&nbsp</TH>\n";
	FirstRow.format("<TH ALIGN=\"CENTER\"><FONT COLOR=#666666>%s</FONT></TH>\n",
					GETTEXT("Order"));
	FirstRow.format("<TH ALIGN=\"CENTER\"><FONT COLOR=#666666>%s</FONT></TH>\n",
					GETTEXT("Owner"));
	FirstRow.format("<TH ALIGN=\"CENTER\"><FONT COLOR=#666666>%s</FONT></TH>\n",
					GETTEXT("Home Cluster"));
	FirstRow.format("<TH ALIGN=\"CENTER\"><FONT COLOR=#666666>%s</FONT></TH>\n",
					GETTEXT("# of Member(s)"));
	FirstRow += "</TR>\n";

	CString
		Layer1,
		Layer2,
		Layer3,
		Layer4;

	Layer1 = "<TR>\n";
	Layer1.format("<TH ALIGN=\"CENTER\" CLASS=\"maintext\">%s</TH>\n", GETTEXT("Layer 1"));
	Layer1 += "</TR>\n";

	Layer1 += (char *)BlankLine;

	Layer1 += "<TR>\n";
	Layer1 += "<TD ALIGN=\"CENTER\">\n";
	Layer1 += "<TABLE WIDTH=570 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A CLASS=\"tabletxt\">\n";

	Layer1 += (char *)FirstRow;

	Layer2 = "<TR>\n";
	Layer2.format("<TH ALIGN=\"CENTER\" CLASS=\"maintext\">%s</TH>\n", GETTEXT("Layer 2"));
	Layer2 += "</TR>\n";

	Layer2 += (char *)BlankLine;

	Layer2 += "<TR>\n";
	Layer2 += "<TD ALIGN=\"CENTER\">\n";
	Layer2 += "<TABLE WIDTH=570 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A CLASS=\"tabletxt\">\n";

	Layer2 += (char *)FirstRow;

	Layer3 = "<TR>\n";
	Layer3.format("<TH ALIGN=\"CENTER\" CLASS=\"maintext\">%s</TH>\n", GETTEXT("Layer 3"));
	Layer3 += "</TR>\n";

	Layer3 += (char *)BlankLine;

	Layer3 += "<TR>\n";
	Layer3 += "<TD ALIGN=\"CENTER\">\n";
	Layer3 += "<TABLE WIDTH=570 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A CLASS=\"tabletxt\">\n";

	Layer3 += (char *)FirstRow;

	Layer4 = "<TR>\n";
	Layer4.format("<TH ALIGN=\"CENTER\" CLASS=\"maintext\">%s</TH>\n", GETTEXT("Layer 4"));
	Layer4 += "</TR>\n";

	Layer4 += (char *)BlankLine;

	Layer4 += "<TR>\n";
	Layer4 += "<TD ALIGN=\"CENTER\">\n";
	Layer4 += "<TABLE WIDTH=570 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A CLASS=\"tabletxt\">\n";

	Layer4 += (char *)FirstRow;

	static CString
		Result;
	Result.clear();

	for (int i=0 ; i<length() ; i++)
	{
		CFortress *
			Fortress = (CFortress *)get(i);

		if (Fortress->get_layer() == 1 &&
			Fortress->get_sector() != CouncilID%16 + 1) continue;

		if (Fortress->get_layer() == 2 &&
			Fortress->get_sector() != CouncilID%8 + 1) continue;

		if (Fortress->get_layer() == 3 &&
			Fortress->get_sector() != CouncilID%4 + 1) continue;

		if (Fortress->get_layer() == 4 &&
			Fortress->get_sector() != CouncilID%2 + 1) continue;

		switch (Fortress->get_layer())
		{
			case 1 :
			{
				Layer1 += "<TR>\n";

				Layer1 += "<TD ALIGN=\"CENTER\">";
				if (Fortress->is_attackable(aPlayer) == true)
				{
					Layer1.format("<INPUT TYPE=radio NAME=FORTRESS_KEY VALUE=%d>",
									Fortress->get_key());
				}
				else
				{
					Layer1 += "&nbsp;";
				}
				Layer1 += "</TD>\n";

				Layer1.format("<TD ALIGN=\"CENTER\">%s</TD>\n",
										dec2unit(Fortress->get_order()));
				CPlayer *
					Owner = PLAYER_TABLE->get_by_game_id(Fortress->get_owner_id());
				if (Owner == NULL)
				{
					SLOG("ERROR : Owner is NULL in CFortressList::get_fortress_status_html(), Fortress->get_owner_id() = %d", Fortress->get_owner_id());
					break;
				}
				if (Owner->get_game_id() == EMPIRE_GAME_ID)
				{
					Layer1.format("<TD ALIGN=\"CENTER\">%s</TD>\n", Owner->get_name());
					Layer1.format("<TD ALIGN=\"CENTER\">%s</TD>\n", GETTEXT("N/A"));
					Layer1.format("<TD ALIGN=\"CENTER\">%s</TD>\n", GETTEXT("N/A"));
				}
				else
				{
					CCouncil *
						Council = COUNCIL_TABLE->get_by_id(Owner->get_council_id());
					if (Council == NULL)
					{
						SLOG("ERROR : Council is NULL in CFortressList::get_fortress_status_html(), Owner->get_council_id() = %d", Owner->get_council_id());
						break;
					}

					Layer1.format("<TD ALIGN=\"CENTER\">%s</TD>\n", Council->get_nick());

					int
						HomeClusterID = Council->get_home_cluster_id();
					CCluster *
						HomeCluster = UNIVERSE->get_by_id(HomeClusterID);
					if (HomeCluster == NULL)
					{
						SLOG("ERROR : HomeCluster is NULL in CFortressList::get_fortress_status_html(), HomeClusterID = %d, ClusterID = %d", HomeClusterID, HomeCluster->get_id());
						break;
					}

					Layer1.format("<TD ALIGN=\"CENTER\">%s</TD>\n", HomeCluster->get_nick());
					Layer1.format("<TD ALIGN=\"CENTER\">%s</TD>\n", dec2unit(Council->get_number_of_members()));
				}

				Layer1 += "</TR>\n";
			}
				break;

			case 2 :
			{
				Layer2 += "<TR>\n";

				Layer2 += "<TD ALIGN=\"CENTER\">";
				if (Fortress->is_attackable(aPlayer) == true)
				{
					Layer2.format("<INPUT TYPE=radio NAME=FORTRESS_KEY VALUE=%d>",
									Fortress->get_key());
				}
				else
				{
					Layer2 += "&nbsp;";
				}
				Layer2 += "</TD>\n";

				Layer2.format("<TD ALIGN=\"CENTER\">%s</TD>\n",
										dec2unit(Fortress->get_order()));
				CPlayer *
					Owner = PLAYER_TABLE->get_by_game_id(Fortress->get_owner_id());
				if (Owner->get_game_id() == EMPIRE_GAME_ID)
				{
					Layer2.format("<TD ALIGN=\"CENTER\">%s</TD>\n", Owner->get_name());
					Layer2.format("<TD ALIGN=\"CENTER\">%s</TD>\n", GETTEXT("N/A"));
					Layer2.format("<TD ALIGN=\"CENTER\">%s</TD>\n", GETTEXT("N/A"));
				}
				else
				{
					CCouncil *
						Council = COUNCIL_TABLE->get_by_id(Owner->get_council_id());
					if (Council == NULL)
					{
						SLOG("ERROR : Council is NULL in CFortressList::get_fortress_status_html(), Owner->get_council_id() = %d", Owner->get_council_id());
						break;
					}

					Layer2.format("<TD ALIGN=\"CENTER\">%s</TD>\n", Council->get_nick());

					int
						HomeClusterID = Council->get_home_cluster_id();
					CCluster *
						HomeCluster = UNIVERSE->get_by_id(HomeClusterID);
					if (HomeCluster == NULL)
					{
						SLOG("ERROR : HomeCluster is NULL in CFortressList::get_fortress_status_html(), HomeClusterID = %d, ClusterID = %d", HomeClusterID, HomeCluster->get_id());
						break;
					}

					Layer2.format("<TD ALIGN=\"CENTER\">%s</TD>\n", HomeCluster->get_nick());
					Layer2.format("<TD ALIGN=\"CENTER\">%s</TD>\n", dec2unit(Council->get_number_of_members()));
				}

				Layer2 += "</TR>\n";
			}
				break;

			case 3 :
			{
				Layer3 += "<TR>\n";

				Layer3 += "<TD ALIGN=\"CENTER\">";
				if (Fortress->is_attackable(aPlayer) == true)
				{
					Layer3.format("<INPUT TYPE=radio NAME=FORTRESS_KEY VALUE=%d>",
									Fortress->get_key());
				}
				else
				{
					Layer3 += "&nbsp;";
				}
				Layer3 += "</TD>\n";

				Layer3.format("<TD ALIGN=\"CENTER\">%s</TD>\n",
										dec2unit(Fortress->get_order()));
				CPlayer *
					Owner = PLAYER_TABLE->get_by_game_id(Fortress->get_owner_id());
				if (Owner->get_game_id() == EMPIRE_GAME_ID)
				{
					Layer3.format("<TD ALIGN=\"CENTER\">%s</TD>\n", Owner->get_name());
					Layer3.format("<TD ALIGN=\"CENTER\">%s</TD>\n", GETTEXT("N/A"));
					Layer3.format("<TD ALIGN=\"CENTER\">%s</TD>\n", GETTEXT("N/A"));
				}
				else
				{
					CCouncil *
						Council = COUNCIL_TABLE->get_by_id(Owner->get_council_id());
					if (Council == NULL)
					{
						SLOG("ERROR : Council is NULL in CFortressList::get_fortress_status_html(), Owner->get_council_id() = %d", Owner->get_council_id());
						break;
					}

					Layer3.format("<TD ALIGN=\"CENTER\">%s</TD>\n", Council->get_nick());

					int
						HomeClusterID = Council->get_home_cluster_id();
					CCluster *
						HomeCluster = UNIVERSE->get_by_id(HomeClusterID);
					if (HomeCluster == NULL)
					{
						SLOG("ERROR : HomeCluster is NULL in CFortressList::get_fortress_status_html(), HomeClusterID = %d, ClusterID = %d", HomeClusterID, HomeCluster->get_id());
						break;
					}

					Layer3.format("<TD ALIGN=\"CENTER\">%s</TD>\n", HomeCluster->get_nick());
					Layer3.format("<TD ALIGN=\"CENTER\">%s</TD>\n", dec2unit(Council->get_number_of_members()));
				}

				Layer3 += "</TR>\n";
			}
				break;

			case 4 :
			{
				Layer4 += "<TR>\n";

				Layer4 += "<TD ALIGN=\"CENTER\">";
				if (Fortress->is_attackable(aPlayer) == true)
				{
					Layer4.format("<INPUT TYPE=radio NAME=FORTRESS_KEY VALUE=%d>",
									Fortress->get_key());
				}
				else
				{
					Layer4 += "&nbsp;";
				}
				Layer4 += "</TD>\n";

				Layer4.format("<TD ALIGN=\"CENTER\">%s</TD>\n",
										dec2unit(Fortress->get_order()));
				CPlayer *
					Owner = PLAYER_TABLE->get_by_game_id(Fortress->get_owner_id());
				if (Owner->get_game_id() == EMPIRE_GAME_ID)
				{
					Layer4.format("<TD ALIGN=\"CENTER\">%s</TD>\n", Owner->get_name());
					Layer4.format("<TD ALIGN=\"CENTER\">%s</TD>\n", GETTEXT("N/A"));
					Layer4.format("<TD ALIGN=\"CENTER\">%s</TD>\n", GETTEXT("N/A"));
				}
				else
				{
					CCouncil *
						Council = COUNCIL_TABLE->get_by_id(Owner->get_council_id());
					if (Council == NULL)
					{
						SLOG("ERROR : Council is NULL in CFortressList::get_fortress_status_html(), Owner->get_council_id() = %d", Owner->get_council_id());
						break;
					}

					Layer4.format("<TD ALIGN=\"CENTER\">%s</TD>\n", Council->get_nick());

					int
						HomeClusterID = Council->get_home_cluster_id();
					CCluster *
						HomeCluster = UNIVERSE->get_by_id(HomeClusterID);
					if (HomeCluster == NULL)
					{
						SLOG("ERROR : HomeCluster is NULL in CFortressList::get_fortress_status_html(), HomeClusterID = %d, ClusterID = %d", HomeClusterID, HomeCluster->get_id());
						break;
					}

					Layer4.format("<TD ALIGN=\"CENTER\">%s</TD>\n", HomeCluster->get_nick());
					Layer4.format("<TD ALIGN=\"CENTER\">%s</TD>\n", dec2unit(Council->get_number_of_members()));
				}

				Layer4 += "</TR>\n";
			}
				break;

			default :
			{
				SLOG("ERROR : Fortress->get_layer() is wrong in CFortressList::get_fortress_status_html(), Fortress->get_layer() = %d, Fortress->get_sector() = %d, Fortress->get_order() = %d", Fortress->get_layer(), Fortress->get_sector(), Fortress->get_order());

			}
				break;
		}
	}

	Layer1 += "</TABLE>\n";
	Layer1 += "</TD>\n";
	Layer1 += "</TR>\n";

	Layer2 += "</TABLE>\n";
	Layer2 += "</TD>\n";
	Layer2 += "</TR>\n";

	Layer3 += "</TABLE>\n";
	Layer3 += "</TD>\n";
	Layer3 += "</TR>\n";

	Layer4 += "</TABLE>\n";
	Layer4 += "</TD>\n";
	Layer4 += "</TR>\n";

	Result = (char *)Layer1;
	Result += (char *)BlankLine;
	Result += (char *)Layer2;
	Result += (char *)BlankLine;
	Result += (char *)Layer3;
	Result += (char *)BlankLine;
	Result += (char *)Layer4;

	return (char *)Result;
}

