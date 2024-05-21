#include "player.h"
#include <cstdlib>
#include "archspace.h"
#include "game.h"
#include "preference.h"

CPlayerTable::CPortalIDHashList::CPortalIDHashList():
	CSortedList(PLAYER_TABLE_HASH_LIMIT, PLAYER_TABLE_HASH_BLOCK)
{
}

CPlayerTable::CPortalIDHashList::~CPortalIDHashList()
{
	remove_all();
}

bool
CPlayerTable::CPortalIDHashList::free_item(TSomething aItem)
{
	(void)aItem;

	return true;
}

int
CPlayerTable::CPortalIDHashList::compare(TSomething aItem1, TSomething aItem2) const
{
	CPlayer
		*Player1 = (CPlayer*)aItem1,
		*Player2 = (CPlayer*)aItem2;

	if (Player1->get_portal_id() > Player2->get_portal_id()) return 1;
	if (Player1->get_portal_id() < Player2->get_portal_id()) return -1;
	return 0;
}

int
CPlayerTable::CPortalIDHashList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CPlayer
		*Player = (CPlayer*)aItem;

	if (Player->get_portal_id() > (int)aKey) return 1;
	if (Player->get_portal_id() < (int)aKey) return -1;
	return 0;
}

CPlayerTable::CNameHashList::CNameHashList():
	CSortedList(PLAYER_TABLE_HASH_LIMIT, PLAYER_TABLE_HASH_BLOCK)
{
}

CPlayerTable::CNameHashList::~CNameHashList()
{
	remove_all();
}

bool
CPlayerTable::CNameHashList::free_item(TSomething aItem)
{
	(void)aItem;

	return true;
}

int
CPlayerTable::CNameHashList::compare(TSomething aItem1, TSomething aItem2) const
{
	CPlayer
		*Player1 = (CPlayer*)aItem1,
		*Player2 = (CPlayer*)aItem2;

	return strcmp(Player1->get_real_name(), Player2->get_real_name());
}

int
CPlayerTable::CNameHashList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CPlayer
		*Player = (CPlayer*)aItem;

	return strcmp(Player->get_real_name(), (char*)aKey);
}


CPlayerTable::CGameIDList::CGameIDList():
	CList(PLAYER_TABLE_LIMIT, PLAYER_TABLE_BLOCK)
{
}

CPlayerTable::CGameIDList::~CGameIDList()
{
	remove_all();
}

bool
CPlayerTable::CGameIDList::free_item(TSomething aItem)
{
	(void)aItem;

	return true;
}


CPlayer* CPlayerTable::mUpdatePlayer = NULL;


CPlayerTable::CPlayerTable():
	CSortedList(PLAYER_TABLE_LIMIT, PLAYER_TABLE_BLOCK)
{
	mMaxID = 0;
	mUpdatePlayer = NULL;

	mOverallRankTable.set_rank_type(CRank::PLAYER_OVERALL);
	mFleetRankTable.set_rank_type(CRank::PLAYER_FLEET);
	mPlanetRankTable.set_rank_type(CRank::PLAYER_PLANET);
	mTechRankTable.set_rank_type(CRank::PLAYER_TECH);
}

CPlayerTable::~CPlayerTable()
{
	remove_all();
}

bool
CPlayerTable::free_item(TSomething aItem)
{
	CPlayer
		*Player = (CPlayer*)aItem;

	if (!Player) return false;

	delete Player;

	return true;
}

int
CPlayerTable::compare(TSomething aItem1, TSomething aItem2) const
{
	CPlayer
		*Player1 = (CPlayer*)aItem1,
		*Player2 = (CPlayer*)aItem2;

	if (Player1->get_game_id() < Player2->get_game_id()) return -1;
	if (Player1->get_game_id() > Player2->get_game_id()) return 1;
	return 0;
}

int
CPlayerTable::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CPlayer
		*Player = (CPlayer*)aItem;

//	SLOG("compare key player %s, %d", Player->get_nick(), (int)aKey);

	if (Player->get_game_id() < (int)aKey) return -1;
	if (Player->get_game_id() > (int)aKey) return 1;
	return 0;
}

bool
CPlayerTable::load(CMySQL &aMySQL)
{
	SLOG( "Player loading" );

	//aMySQL.query( "LOCK TABLE player READ" );

	CString
		Query;

	Query.format( "SELECT "
			"game_id, "
			"portal_id, "
			"name, "
			"home_cluster_id, "
			"last_login, "
			"last_login_ip, "

			"mode, "
			"race, "
			"honor, "

			"research_invest, "
			"tick, "
			"turn, "
			"production, "
			"ship_production, "
			"invested_ship_production, "
			"research, "
			"ability, "

			"research_tech, "

			"admiral_timer, "
			"honor_timer, "

			"last_turn_production, "
			"last_turn_research, "
			"last_turn_military, "

			"council_id, "
			"council_vote, "
			"council_production, "
			"council_donation, "

			"security_level, "
			"alertness, "
			"empire_relation, "

			"protected_mode, "
			"protected_terminate_time, "

			"news_turn, "
			"news_production, "
			"news_research, "
			"news_population, "
			"planet_invest_pool, "
			"admission_time_limit, "
			"rating "
			"FROM player ORDER BY tick");

	aMySQL.query( (char*)Query );
	aMySQL.use_result();

	while(aMySQL.fetch_row())
	{
		add_player(new CPlayer(aMySQL.row()));
	}

	aMySQL.free_result();

	//aMySQL.query( "UNLOCK TABLES" );

	SLOG("%d Players are loaded", length());

	load_tech(aMySQL);
	load_admiral(aMySQL);
	load_ship_design(aMySQL);
	load_ship_build_q(aMySQL);
	load_dock(aMySQL);
	load_repair_bay(aMySQL);
	load_fleet(aMySQL);	/* telecard 2000/10/19 */
	load_plan(aMySQL);	/* telecard 2000/10/27 */
	load_empire_action(aMySQL);
	load_event(aMySQL);
	load_effect(aMySQL);
	load_preferences(aMySQL);
	refresh_rank_table();

	return true;
}

bool
CPlayerTable::load_preferences(CMySQL &aMySQL)
{
	if( length() < 1 )
		return true;

	SLOG( "preferences loading" );
	int TCount = 0;

	//aMySQL.query( "LOCK TABLE player_pref READ" );

	for( int i = 0; i < length(); i ++ )
	{
	CPlayer *Target = (CPlayer*)get(i);
	if (Target == NULL)
	{
		SLOG("CPlayer Target is NULL in CPlayerTable::load_preferences");
		continue;
	}
	else if (Target->is_dead())
	{
		SLOG("CPlayer Target is dead in CPlayerTable::load_preferences - do we care?");
		continue;
	}
	CString
			Query;

	Query.format( "SELECT player_id, java_choice, accept_truce, accept_pact, accept_ally, commander_view FROM player_pref WHERE player_id=%d", Target->get_game_id());
	aMySQL.query( (char*)Query );

	aMySQL.use_result();

	// load the players preferences
		if (aMySQL.fetch_row())
		{
			CPreference *Preference = new CPreference(aMySQL.row());
			Target->set_preference(Preference);
			TCount++;
		}
		else // create a new preference set and store it
		{
			CPreference *Preference = new CPreference(Target->get_game_id());
			Target->set_preference(Preference);
			TCount++;
		}
		aMySQL.free_result();
	}

	//aMySQL.query("UNLOCK TABLES");

	SLOG("%d preference records are loaded", TCount);

	return true;
}


bool
CPlayerTable::load_effect(CMySQL &aMySQL)
{
	int
		MaxID;

	if( length() > 0 )
		MaxID = ((CPlayer*)get(length()-1))->get_game_id();
	else
		return true;

	SLOG( "effect loading" );
	int TCount = 0;

	//aMySQL.query( "LOCK TABLE player_effect READ" );

	for( int i = 0; i < MaxID+1000; i += 1000 ){
		CString
			Query;

		Query.format( "SELECT id, owner, life, type, target, apply, arg1, arg2, source_type, source FROM player_effect WHERE owner > %d AND owner <= %d", i, i+1000 );
		aMySQL.query( (char*)Query );

		aMySQL.use_result();
		while(aMySQL.fetch_row()) {
			CPlayer *
				Owner = get_by_game_id(as_atoi(aMySQL.row(CPlayerEffect::FIELD_OWNER)));
			if (Owner==NULL)
			{
				SLOG( "WRONG PLAYER player effect %s LOADING : NO OWNER ID (%s)", aMySQL.row(CPlayerEffect::FIELD_ID), aMySQL.row(CPlayerEffect::FIELD_OWNER) );
				CString
					Query;

				Query.format( "DELETE FROM player_effect WHERE owner = %s AND id = %s", aMySQL.row(CPlayerEffect::FIELD_OWNER), aMySQL.row(CPlayerEffect::FIELD_ID) );
				STORE_CENTER->query("player_effect", (char*)Query);

				continue;
			}
			CPlayerEffect
				*Effect = new CPlayerEffect(aMySQL.row());

			Owner->get_effect_list()->add_player_effect(Effect);

			TCount++;
		}

		aMySQL.free_result();
	}

	//aMySQL.query("UNLOCK TABLES");

	SLOG("%d effect records are loaded", TCount);

	return true;
}

bool
CPlayerTable::load_event(CMySQL &aMySQL)
{
	int
		MaxID;

	if( length() > 0 )
		MaxID = ((CPlayer*)get(length()-1))->get_game_id();
	else
		return true;

	SLOG( "event loading" );
	int TCount = 0;

	//aMySQL.query( "LOCK TABLE player_event READ" );

	for( int i = 0; i < MaxID+1000; i += 1000 ){
		CString
			Query;

		Query.format( "SELECT id, owner, event, life, time, answered FROM player_event WHERE owner > %d AND owner <= %d", i, i+1000 );
		aMySQL.query( (char*)Query );

		aMySQL.use_result();
		while(aMySQL.fetch_row()) {
			CPlayer *
				Owner = get_by_game_id(as_atoi(aMySQL.row(CEventInstance::STORE_OWNER)));
			if (Owner==NULL)
			{
				SLOG( "WRONG PLAYER player event %s LOADING : NO OWNER ID (%s)", aMySQL.row(CEventInstance::STORE_ID), aMySQL.row(CEventInstance::STORE_OWNER) );
				CString
					Query;

				Query.format( "DELETE FROM player_event WHERE owner = %s AND id = %s", aMySQL.row(CEventInstance::STORE_OWNER), aMySQL.row(CEventInstance::STORE_ID) );
				STORE_CENTER->query("player_event", (char*)Query);

				continue;
			}
			CEvent
				*Event = EVENT_TABLE->get_by_id(as_atoi(aMySQL.row(CEventInstance::STORE_EVENT)));
			if(Event==NULL){
				SLOG( "WRONG EVENT player event %s LOADING : NO EVENT (%s)", aMySQL.row(CEventInstance::STORE_ID), aMySQL.row(CEventInstance::STORE_EVENT) );
				CString
					Query;

				Query.format( "DELETE FROM player_event WHERE owner = %s AND id = %s", aMySQL.row(CEventInstance::STORE_OWNER), aMySQL.row(CEventInstance::STORE_ID) );
				STORE_CENTER->query("player_event", (char*)Query);
				continue;
			}
			CEventInstance
				*Instance = new CEventInstance(Event, aMySQL.row());

			Owner->get_event_list()->add_event_instance(Instance);

			TCount++;
		}

		aMySQL.free_result();
	}

	//aMySQL.query("UNLOCK TABLES");

	SLOG("%d event records are loaded", TCount);

	return true;
}

bool
CPlayerTable::load_empire_action(CMySQL &aMySQL)
{
	int
		MaxID;

	if( length() > 0 )
		MaxID = ((CPlayer*)get(length()-1))->get_game_id();
	else
		return true;

	SLOG( "empire action loading" );
	int TCount = 0;

	//aMySQL.query( "LOCK TABLE empire_action READ" );

	for( int i = 0; i < MaxID+1000; i += 1000 ){
		CString
			Query;

		Query.format( "SELECT id, owner, action, target, amount, answer, time FROM empire_action WHERE owner > %d AND owner <= %d", i, i+1000 );
		aMySQL.query( (char*)Query );

		aMySQL.use_result();
		while(aMySQL.fetch_row()) {
			CPlayer *
				Owner = get_by_game_id(as_atoi(aMySQL.row(CEmpireAction::STORE_OWNER)));
			if (!Owner)
			{
				SLOG( "WRONG PLAYER empire action %s LOADING : NO OWNER ID (%s)", aMySQL.row(CEmpireAction::STORE_ID), aMySQL.row(CEmpireAction::STORE_OWNER) );
				CString
					Query;

				Query.format( "DELETE FROM empire_action WHERE owner = %s AND id = %s", aMySQL.row(CEmpireAction::STORE_OWNER), aMySQL.row(CEmpireAction::STORE_ID) );
				STORE_CENTER->query("empire_action", (char*)Query);

				continue;
			}
			CEmpireAction
				*Action = new CEmpireAction(aMySQL.row());

			Owner->get_empire_action_list()->add_empire_action(Action);

			TCount++;
		}

		aMySQL.free_result();
	}

	//aMySQL.query("UNLOCK TABLES");

	SLOG("%d empire action records are loaded", TCount);

	return true;
}

bool
CPlayerTable::load_tech(CMySQL &aMySQL)
{
	int
		MaxID;

	if( length() > 0 )
		MaxID = ((CPlayer*)get(length()-1))->get_game_id();
	else
		return true;

	SLOG( "Tech loading" );
	int TCount = 0;

	//aMySQL.query( "LOCK TABLE tech READ" );

	for( int i = 0; i < MaxID+1000; i += 1000 ){
		CString
			Query;

		Query.format( "SELECT owner, info, life, matter, social, upgrade, schematics, amatter FROM tech WHERE owner > %d AND owner <= %d", i, i+1000 );
		aMySQL.query( (char*)Query );

		aMySQL.use_result();
		while(aMySQL.fetch_row()) {
			CPlayer *
				Owner = get_by_game_id(as_atoi(aMySQL.row(0)));
			if (!Owner)
			{
				SLOG( "WRONG PLAYER TECH LOADING : NO OWNER ID (%d)", as_atoi(aMySQL.row(0)));
				CString
					Query;

				Query.format( "DELETE FROM tech WHERE owner = %s", aMySQL.row(0) );
				STORE_CENTER->query("tech", (char*)Query);

				continue;
			}

			CKnownTechList *
				KnownTechList = Owner->get_tech_list();
			KnownTechList->initialize(aMySQL.row());

			Owner->initialize_tech_control_model();

			TCount++;
		}

		aMySQL.free_result();
	}

	//aMySQL.query("UNLOCK TABLES");

	SLOG("%d Tech records are loaded", TCount);

	return true;
}

bool
CPlayerTable::load_admiral(CMySQL &aMySQL)
{
	int
		MaxID;

	if( length() > 0 )
		MaxID = ((CPlayer*)get(length()-1))->get_game_id();
	else
		return true;

	SLOG( "Admiral loading" );
	int ACount = 0;

	enum
	{
		FIELD_ADMIRAL_ID = 0,
		FIELD_ADMIRAL_OWNER
	};

	//aMySQL.query("LOCK TABLE admiral READ" );

	for (int i=0 ; i<MaxID+100 ; i += 100)
	{
		CString
			Query;

		Query.format("SELECT * FROM admiral WHERE owner >= %d AND owner < %d", i, i+100 );
		aMySQL.query((char *)Query);

		aMySQL.use_result();
		while (aMySQL.fetch_row() != NULL)
		{
			CAdmiral *
				Admiral = new CAdmiral(aMySQL.row());

			if (Admiral->get_owner() == EMPIRE_GAME_ID)
			{
				delete Admiral;
				continue;
			}

			CPlayer *
				Owner = get_by_game_id(Admiral->get_owner());
			if (Owner == NULL)
			{
				SLOG("ERROR : No Owner in CPlayerTable::load_admiral(), Admiral ID = %s, Owner = %s", aMySQL.row(FIELD_ADMIRAL_ID), aMySQL.row(FIELD_ADMIRAL_OWNER));

				Admiral->type(QUERY_DELETE);
				STORE_CENTER->store(*Admiral);

				delete Admiral;
				continue;
			}

			if (Admiral->get_fleet_number() > 0)
			{
				Owner->add_admiral_list(Admiral);
			}
			else
			{
				Owner->add_admiral_pool(Admiral);
			}

			ACount++;
		}

		aMySQL.free_result();
	}

	//aMySQL.query( "UNLOCK TABLES" );

	SLOG( "%d Admirals are loaded", ACount );

	return true;
}

bool
CPlayerTable::load_ship_design(CMySQL &aMySQL)
{
	int
		MaxID;

	if( length() > 0 )
		MaxID = ((CPlayer*)get(length()-1))->get_game_id();
	else
		return true;

	system_log( "start ship design loading" );
	int
		RCount = 0,
		CCount = 0;

	//aMySQL.query( "LOCK TABLE class READ" );

	for (int i=1 ; i<MaxID+100 ; i += 100)
	{
		CString
			Query;

		Query.format( "SELECT * FROM class WHERE owner >= %d AND owner < %d", i, i+100 );
		aMySQL.query((char *)Query);
		aMySQL.use_result();

		while (aMySQL.fetch_row())
		{
			CShipDesign *
				Design = new CShipDesign(aMySQL.row());
			if (Design == NULL)
			{
				SLOG("ERROR : Not enough memory for CShipDesign in CPlayerTable::load_ship_design()");
				continue;
			}
			if (Design->get_owner() == 0)
			{
				delete Design;
				continue;
			}

			if (!Design->get_design_id())
			{
				system_log( "player %d not exist for ship design %s(%d)", Design->get_owner(), Design->get_name(), Design->get_design_id() );

				Design->type(QUERY_DELETE);
				STORE_CENTER->store(*Design);

				delete Design;
				continue;
			}

			CPlayer
				*Player = get_by_game_id( Design->get_owner() );

			RCount++;
			if( Player == NULL ){
				system_log( "player %d not exist for ship design %s(%d)", Design->get_owner(), Design->get_name(), Design->get_design_id() );

				Design->type(QUERY_DELETE);
				STORE_CENTER->store(*Design);

				delete Design;
				continue;
			}
			Player->get_ship_design_list()->add_ship_design(Design);

			CCount++;
		}

		aMySQL.free_result();
	}

	//aMySQL.query( "UNLOCK TABLES" );

	system_log( "%d/%d ship designs are loaded", CCount, RCount );

	return true;
}

bool
CPlayerTable::load_ship_build_q(CMySQL &aMySQL)
{
	int
		MaxID;

	if( length() > 0 )
		MaxID = ((CPlayer*)get(length()-1))->get_game_id();
	else
		return true;

	enum {
		FIELD_OWNER,
		FIELD_DESIGN_ID,
		FIELD_NUMBER,
		FIELD_ORDER
	};

	system_log( "start ship_build_q loading" );
	int
		RCount = 0,
		CCount = 0;

	//aMySQL.query( "LOCK TABLE ship_building_q READ" );

	for( int i = 0; i < MaxID+1000; i += 1000 ){
		CString
			Query;

		Query.format( "SELECT owner, design_id, number, time_order FROM ship_building_q WHERE owner > %d AND owner <= %d", i, i+1000 );
		aMySQL.query( (char*)Query );
		aMySQL.use_result();

		while( aMySQL.fetch_row() ){
			RCount++;

			int
				OwnerID = atoi( aMySQL.row()[FIELD_OWNER] ),
				DesignID = atoi( aMySQL.row()[FIELD_DESIGN_ID] ),
				Number = atoi( aMySQL.row()[FIELD_NUMBER] ),
				Order = atoi( aMySQL.row()[FIELD_ORDER] );

			CPlayer
				*Owner = get_by_game_id( OwnerID );
			if( Owner == NULL ){
				CString
					Query;
				Query.format( "DELETE FROM ship_building_q WHERE owner = %d", OwnerID );
				STORE_CENTER->query("ship_building_q", (char*)Query);
				continue;
			}

			CShipDesign
				*Design = Owner->get_ship_design_list()->get_by_id( DesignID );
			if( Design == NULL ){
				CString
					Query;
				Query.format( "DELETE FROM ship_building_q WHERE owner = %d AND design_id = %d", OwnerID, DesignID );

				STORE_CENTER->query("ship_building_q", (char*)Query);
				continue;
			}

			CShipToBuild
				*Ship = new CShipToBuild( OwnerID, Design, Number, Order );
			Owner->get_build_q()->add_ship_to_build(Ship);

			CCount++;
		}

		aMySQL.free_result();
	}
	//aMySQL.query( "UNLOCK TABLES" );

	system_log( "%d/%d ship build q are loaded", CCount, RCount );

	return true;
}

bool
CPlayerTable::load_dock(CMySQL &aMySQL)
{
	int
		MaxID;

	if( length() > 0 )
		MaxID = ((CPlayer*)get(length()-1))->get_game_id();
	else
		return true;

	enum {
		FIELD_OWNER,
		FIELD_DESIGN_ID,
		FIELD_NUMBER
	};

	system_log( "start dock loading" );
	int
		RCount = 0,
		CCount = 0;

	//aMySQL.query( "LOCK TABLE docked_ship READ" );

	for( int i = 0; i < MaxID+100; i += 100 ){
		CString
			Query;
		Query.format( "SELECT owner, design_id, number FROM docked_ship WHERE owner > %d AND owner <= %d", i, i+100 );

		aMySQL.query( (char*)Query );
		aMySQL.use_result();

		while( aMySQL.fetch_row() ){
			RCount++;

			int
				OwnerID = atoi( aMySQL.row()[FIELD_OWNER] ),
				DesignID = atoi( aMySQL.row()[FIELD_DESIGN_ID] ),
				Number = atoi( aMySQL.row()[FIELD_NUMBER] );

			CPlayer
				*Owner = get_by_game_id( OwnerID );
			if (!Owner)
			{
				CString
					Query;
				Query.format( "DELETE FROM docked_ship WHERE owner = %d", OwnerID );
				STORE_CENTER->query("docked_ship", (char *)Query);
				continue;
			}

			CShipDesign
				*Design = Owner->get_ship_design_list()->get_by_id( DesignID );
			if (!Design)
			{
				CString
					Query;
				Query.format( "DELETE FROM docked_ship WHERE owner = %d AND design_id = %d", OwnerID, DesignID );
				STORE_CENTER->query("docked_ship", (char *)Query);
				continue;
			}

			CDockedShip
				*Ship = new CDockedShip( Design, Number );
			Owner->get_dock()->add_docked_ship(Ship);

			CCount++;
		}

		aMySQL.free_result();
	}

	//aMySQL.query( "UNLOCK TABLES" );

	system_log( "%d/%d docked ships are loaded", CCount, RCount );

	return true;
}

bool
CPlayerTable::load_repair_bay(CMySQL &aMySQL)
{
	if (length() == 0) return true;

	int
		MaxID = ((CPlayer *)get(length()-1))->get_game_id();

	system_log("start ship design loading");

	int
		RCount = 0,
		CCount = 0;

	//aMySQL.query("LOCK TABLE damaged_ship READ");

	for (int i=0 ; i<MaxID+100 ; i += 100)
	{
		CString
			Query;
		Query.format("SELECT owner, id, design_id, hp FROM damaged_ship WHERE owner >= %d AND owner < %d", i, i+100);
		aMySQL.query((char *)Query);

		aMySQL.use_result();

		while (aMySQL.fetch_row() != NULL)
		{
			RCount++;

			int
				OwnerID = atoi(aMySQL.row()[CDamagedShip::STORE_OWNER]),
				ID = atoi(aMySQL.row()[CDamagedShip::STORE_ID]),
				DesignID = atoi(aMySQL.row()[CDamagedShip::STORE_DESIGN_ID]),
				HP = atoi(aMySQL.row()[CDamagedShip::STORE_HP]);

			CPlayer *
				Owner = get_by_game_id(OwnerID);
			if (Owner == NULL) continue;

			CShipDesign *
				Design = Owner->get_ship_design_list()->get_by_id(DesignID);
			if (Design == NULL) continue;

			CDamagedShip *
				Ship = new CDamagedShip(Design, HP, ID);

			Owner->get_repair_bay()->add_damaged_ship(Ship);

			CCount++;
		}

		aMySQL.free_result();
	}

	//aMySQL.query("UNLOCK TABLES");

	system_log("%d/%d repair_bay ships are loaded", CCount, RCount);

	return true;
}

/* start telecard 2000/10/19 */

bool
CPlayerTable::load_fleet(CMySQL &aMySQL)
{

	if (length() == 0) return true;

	int
		MaxID = ((CPlayer*)get(length()-1))->get_game_id();

	system_log( "start fleet loading : %d", MaxID );

	int
		RCount = 0,
		CCount = 0;

	//aMySQL.query( "LOCK TABLE fleet READ" );

	aMySQL.query( "SELECT count(*) FROM fleet" );
	aMySQL.use_result();
	aMySQL.fetch_row();
	SLOG( "total fleet # = %s", aMySQL.row(0) );
	aMySQL.free_result();

	for (int i=0 ; i<MaxID+100 ; i += 100)
	{
		CString
			Query;
		Query.format("SELECT * FROM fleet WHERE owner >= %d AND owner < %d", i, i+100);
		aMySQL.query((char *)Query );

		aMySQL.use_result();

		while (aMySQL.fetch_row())
		{
			if (as_atoi(aMySQL.row(CFleet::STORE_OWNER)) == EMPIRE_GAME_ID) continue;

			RCount++;

			CPlayer *
				player = get_by_game_id(as_atoi(aMySQL.row(CFleet::STORE_OWNER)) );

			if (player == NULL)
			{
				system_log("player %s not exist for fleet %s(%s)", aMySQL.row(CFleet::STORE_OWNER), aMySQL.row(CFleet::STORE_NAME), aMySQL.row(CFleet::STORE_ID) );
				CString
					Query;
				Query.format( "DELETE FROM fleet WHERE owner = %s AND id = %s", aMySQL.row(CFleet::STORE_OWNER), aMySQL.row(CFleet::STORE_ID) );
				STORE_CENTER->query("fleet", (char *)Query);

				continue;
			}

			int AdmiralID = atoi(aMySQL.row(CFleet::STORE_ADMIRAL));

			CAdmiralList *
				AdmiralList = player->get_admiral_list();

			if (AdmiralList->get_by_id(AdmiralID) == NULL)
			{
				SLOG("Fleet unassigned Admiral %s, Player = %s, Fleet ID = %s", aMySQL.row(CFleet::STORE_ADMIRAL), player->get_nick(), aMySQL.row(CFleet::STORE_ID) );
				CAdmiral *
					Admiral = (CAdmiral*)player->get_admiral_pool()->get_by_id(AdmiralID);

				if (Admiral == NULL)
				{
					SLOG("ERROR : No admiral found for a fleet!, AdmiralID = %d, aMySQL.row(CFleet::STORE_ID) = %s", AdmiralID, aMySQL.row(CFleet::STORE_ID));
					CString
						Query;
					Query.format( "DELETE FROM fleet WHERE owner = %s AND id = %s", aMySQL.row(CFleet::STORE_OWNER), aMySQL.row(CFleet::STORE_ID) );
					STORE_CENTER->query("fleet", (char *)Query);

					continue;
				}

				player->get_admiral_pool()->remove_without_free_admiral(Admiral->get_id());
				player->get_admiral_list()->add_admiral(Admiral);
				Admiral->set_fleet_number(atoi(aMySQL.row(CFleet::STORE_ID)));
				Admiral->type(QUERY_UPDATE);
			}

			if (!player->get_ship_design_list()->get_by_id(atoi(aMySQL.row(CFleet::STORE_SHIP_CLASS))))
			{
				SLOG("ERROR : Wrong design ID %s in CFleet %s(Owner ID : %d)",
						aMySQL.row(CFleet::STORE_SHIP_CLASS), aMySQL.row(CFleet::STORE_ID), player->get_game_id());
				CString
					Query;
				Query.format( "DELETE FROM fleet WHERE owner = %s AND id = %s", aMySQL.row(CFleet::STORE_OWNER), aMySQL.row(CFleet::STORE_ID) );
				STORE_CENTER->query("fleet", (char *)Query);

				continue;
			}

			CFleet*
				fleet = new CFleet(player->get_ship_design_list(), aMySQL.row());
			player->get_fleet_list()->add_fleet(fleet);

			CMission &
				Mission = fleet->get_mission();
			if (Mission.get_mission() == CMission::MISSION_DISPATCH_TO_ALLY)
			{
				CPlayer *
					Ally = PLAYER_TABLE->get_by_game_id(Mission.get_target());
				if (Ally == NULL)
				{
					SLOG("ERROR : There is no player with ID %d.(in fleet %s dispatching)",
							Mission.get_target(), fleet->get_nick());

					fleet->end_mission();

					fleet->type(QUERY_UPDATE);
					STORE_CENTER->store(*fleet);
				}
				else
				{
					CAllyFleetList *
						DispatchedFleetList = Ally->get_ally_fleet_list();
					DispatchedFleetList->add_fleet(fleet);
				}
			}
			CCount++;
		}
		aMySQL.free_result();
	}

	//aMySQL.query( "UNLOCK TABLES" );

	SLOG( "fleet loading %d/%d", CCount, RCount );

	return true;
}

/* end telecard 2000/10/19 */

/* start telecard 2000/10/25 */
bool
CPlayerTable::load_plan(CMySQL &aMySQL)
{
	int
		MaxID;

	if( length() > 0 )
		MaxID = ((CPlayer*)get(length()-1))->get_game_id();
	else
		return true;

	system_log( "start load plan" );

	//aMySQL.query( "LOCK TABLE plan READ" );

	for( int i = 0; i < MaxID+1000; i += 1000 ){
		CString
			Query;
		Query.format( "SELECT owner, id, type, name, capital, enemy, min, max, attack_type FROM plan WHERE owner > %d AND owner <= %d", i, i+1000 );
		aMySQL.query( (char*)Query );
		aMySQL.use_result();
		while( aMySQL.fetch_row() ) {
			CPlayer
				*player = get_by_game_id( atoi(aMySQL.row(0)) );
			if( player == NULL ){
				system_log( "player %s not exist for plan %s(%s)", aMySQL.row(0), aMySQL.row(3), aMySQL.row(1) );
				CString
					Query;
				Query.format( "DELETE FROM plan WHERE owner = %s AND id = %s", aMySQL.row(0), aMySQL.row(1) );
				STORE_CENTER->query("plan", (char*)Query);
				continue;
			}
			CDefensePlan *
				plan = new CDefensePlan(aMySQL.row());
			player->get_defense_plan_list()->add_defense_plan(plan);
		}
		aMySQL.free_result();
	}
	//aMySQL.query( "UNLOCK TABLES" );

	//aMySQL.query( "LOCK TABLE defense_fleet READ" );
	for( int i = 0; i < MaxID+100; i += 100 ){
		CString
			Query;
		Query.format( "SELECT owner, plan_id, fleet_id, command, x, y FROM defense_fleet WHERE owner > %d AND owner <= %d", i, i+100 );
		aMySQL.query( (char*)Query );
		aMySQL.use_result();
		while( aMySQL.fetch_row() ) {
			CPlayer
				*player = get_by_game_id( atoi(aMySQL.row(0)) );
			if( player == NULL ){
				system_log( "player %s not exist for fleet %s", aMySQL.row(0), aMySQL.row(2) );
				CString
					Query;
				Query.format( "DELETE FROM defense_fleet WHERE owner = %s", aMySQL.row(0) );
				STORE_CENTER->query("defense_fleet", (char*)Query);

				continue;
			}

			CDefensePlan
				*plan = player->get_defense_plan_list()->get_by_id( atoi(aMySQL.row(1)) );
			if( plan == NULL) {
				system_log( "plan %s of player %s for fleet %s not exists",
					aMySQL.row(1), aMySQL.row(0), aMySQL.row(2) );
				CString
					Query;
				Query.format( "DELETE FROM defense_fleet WHERE owner = %s AND plan_id = %s", aMySQL.row(0), aMySQL.row(1) );
				STORE_CENTER->query("defense_fleet", (char*)Query);

				continue;
			}

			CDefenseFleet *
				DefenseFleet = new CDefenseFleet(aMySQL.row());
			plan->add_defense_fleet(DefenseFleet);
		}
		aMySQL.free_result();
	}

	//aMySQL.query( "UNLOCK TABLES" );

	return true;
}
/* end telecard 2000/10/25 */

int
CPlayerTable::add_player(CPlayer* aPlayer)
{
	if (!aPlayer) return -1;
	if (aPlayer->get_game_id() < 0) return -1;

	int
		Index = find_sorted_key(
					(TConstSomething)aPlayer->get_game_id());
	if (Index >= 0) return -1;

	insert_sorted(aPlayer);

	int
		Unique;

	Index = aPlayer->get_portal_id() % MAX_HASHTABLE;
	mPortalIDIndex[Index].insert_sorted(aPlayer, &Unique);
	if (Unique > 0)
		SLOG("There is already exist same portal id(%d)",
									aPlayer->get_portal_id());

	unsigned int
		Sum = 0;
	char* playername = (char*)aPlayer->get_real_name();
	for(int i=0; playername[i]; i++)
	{
		Sum += (unsigned int)(playername[i]);
		if ((((unsigned int)playername[i]) > 96)&&(((unsigned int)playername[i]) < 123))
		{
			Sum -= 32;
		}
	}
	Index = MAX_HASHTABLE - (Sum % MAX_HASHTABLE) - 1;
//	SLOG("Name Hash Index %d, Sum:%d", Index, Sum);
	mNameIndex[Index].insert_sorted(aPlayer, &Unique);

	mGameIDIndex.at(aPlayer->get_game_id(), aPlayer);

	if (mMaxID < aPlayer->get_game_id())
		mMaxID = aPlayer->get_game_id();

	insert_update_list(aPlayer);

	return aPlayer->get_game_id();
}

void
CPlayerTable::insert_update_list(CPlayer *aPlayer)
{
	if (mUpdatePlayer)
	{
		CPlayer *Player = mUpdatePlayer->prev();
		Player->next(aPlayer);
		aPlayer->prev(Player);
		aPlayer->next(mUpdatePlayer);
		mUpdatePlayer->prev(aPlayer);
	} else {
		aPlayer->prev(aPlayer);
		aPlayer->next(aPlayer);
		mUpdatePlayer = aPlayer;
	}
}

void
CPlayerTable::delete_update_list(CPlayer *aPlayer)
{
	if (mUpdatePlayer == aPlayer)
	{
		if (aPlayer->next() == aPlayer)
		{
			mUpdatePlayer = NULL;
		} else {
			aPlayer->prev()->next(aPlayer->next());
			aPlayer->next()->prev(aPlayer->prev());
			mUpdatePlayer = aPlayer->next();
		}
		return;
	}
	aPlayer->prev()->next(aPlayer->next());
	aPlayer->next()->prev(aPlayer->prev());
}



CPlayer*
CPlayerTable::get_by_game_id(int aGameID)
{
	if (aGameID >= mGameIDIndex.length()) return NULL;

	CPlayer*
		Player = (CPlayer*)mGameIDIndex.get(aGameID);
	if (!Player) return NULL;
	return Player;
}

CPlayer*
CPlayerTable::get_by_portal_id(int aPortalID)
{
	int
		Index = aPortalID % MAX_HASHTABLE;
	int
		Pos = mPortalIDIndex[Index].find_sorted_key(
					(TConstSomething)aPortalID);
	if (Pos < 0) return NULL;
	CPlayer
		*Player = (CPlayer*)mPortalIDIndex[Index].get(Pos);
	if (!Player) return NULL;

	return Player;
}

CPlayer*
CPlayerTable::get_by_name(const char* aName)
{
	unsigned int
		Sum = 0;

	if (!aName) return NULL;
	if (!strlen(aName)) return NULL;

	char *
		Name = (char *)aName;

	for (int i=0 ; Name[i] ; i++) {
		Sum += (unsigned int)(Name[i]);
		if ((((unsigned int)Name[i]) > 96)&&(((unsigned int)Name[i]) < 123))
		{
			Sum -= 32;
		}
	}

	int
		Index = MAX_HASHTABLE - (Sum % MAX_HASHTABLE) - 1;

	int
		Pos = mNameIndex[Index].find_sorted_key(aName);
	if (Pos < 0) return NULL;
	CPlayer
		*Player = (CPlayer*)mNameIndex[Index].get(Pos);
	if (!Player) return NULL;

	return Player;
}

bool
CPlayerTable::remove_player(int aGameID)
{
	CPlayer *
		Player = (CPlayer*)mGameIDIndex.get(aGameID);

	if (Player == NULL) return false;

	delete_update_list(Player);

	mGameIDIndex.replace(aGameID, NULL);

	int
		Index = Player->get_portal_id() % MAX_HASHTABLE;
	int
		Pos = mPortalIDIndex[Index].find_sorted_key((void *)Player->get_portal_id());
	if (Pos >= 0)
	{
		mPortalIDIndex[Index].CSortedList::remove(Pos);
	}
	else
	{
		SLOG( "is dead : PortalIDIndex remove fail : %s PortalID=%d Index=%d Pos=%d", Player->get_nick(), Player->get_portal_id(), Index, Pos );
	}

	unsigned int
		Sum = 0;
	char* playername = (char*)Player->get_real_name();
	for(int i=0; playername[i]; i++)
	{
		Sum += (unsigned int)(playername[i]);
		if ((((unsigned int)playername[i]) > 96)&&(((unsigned int)playername[i]) < 123))
		{
			Sum -= 32;
		}
	}
	Index = MAX_HASHTABLE - (Sum % MAX_HASHTABLE) - 1;
	Pos = mNameIndex[Index].find_sorted_key(playername);
	if (Pos >= 0)
	{
		mNameIndex[Index].CSortedList::remove(Pos);
	}
	else
	{
		SLOG( "is dead : NameIndex remove fail : %s Sum=%d Index=%d Pos=%d", Player->get_nick(), Sum, Index, Pos );
	}

	Pos = find_sorted_key((void *)aGameID);
	CSortedList::remove(Pos);

	mOverallRankTable.remove_rank(aGameID);
	mFleetRankTable.remove_rank(aGameID);
	mPlanetRankTable.remove_rank(aGameID);
	mTechRankTable.remove_rank(aGameID);
	//mBountyList.remove_bounty(aGameID);

	return true;
}

void
CPlayerTable::give_planet_to_owner( CPlanet *aPlanet )
{
	CPlayer
		*Owner = aPlanet->get_owner();

	if(!Owner)
	{
		SLOG("wrong planet owner planet[%d] - %d",
				Owner->get_game_id(), aPlanet->get_owner());
		return;
	}

	Owner->add_planet( aPlanet );
}

void*
CPlayerTable::update(void *aArg)
{
	SLOG("Update Thread");
	if(!GAME->mUpdateTurn) GAME->mServerStartTime = time(0);
	static int UpdatePlayerPerMinute = 0;
	static time_t UpdateTimer = CGame::get_game_time();

	// optimizing update
	const int
		LoopPerEachUpdate = 5;
	int
		NumberofPlayer = 0;
	int
		DelayedPlayer = 0,
		DelayedSecond = 0;
	int
		MaxUpdatePerMinuteConstant = CGame::mSecondPerTurn/120;

	if( MaxUpdatePerMinuteConstant < 1 )
		MaxUpdatePerMinuteConstant = 1;

	while(CArchspace::get_server_loop())
	{
		CGame::lock();

		if (EMPIRE->is_dead() == true)
		{
			CGlobalEnding *
				GlobalEndingData = GAME->get_global_ending_data();
			if (GlobalEndingData->is_final_score() == false)
			{
				GAME->set_global_ending_data();
			}

			CGame::unlock();
			pth_nap((pth_time_t){1, 0});

			continue;
		}

		if (mUpdatePlayer == NULL)
		{
			CGame::unlock();
			pth_nap((pth_time_t){1, 0});

			continue;
		}
		NumberofPlayer = PLAYER_TABLE->length();

		for (int i=0 ; i<LoopPerEachUpdate ; i++)
		{
			if (((CGame::get_game_time() - UpdateTimer) >= 60))
			{
				if( DelayedPlayer == 0 )
					SLOG("%d Update per %d seconds, %.2f per second",
						UpdatePlayerPerMinute,
						CGame::get_game_time()-UpdateTimer,
						((double)UpdatePlayerPerMinute/
								(CGame::get_game_time()-UpdateTimer)));
				else
					SLOG("%d Update per %d seconds, %.2f per second, %d delayed player, %d avr. delay",
						UpdatePlayerPerMinute,
						CGame::get_game_time()-UpdateTimer,
						((double)UpdatePlayerPerMinute/
								(CGame::get_game_time()-UpdateTimer)),
						DelayedPlayer,
						DelayedSecond/DelayedPlayer);
				UpdateTimer = CGame::get_game_time();
				DelayedPlayer = DelayedSecond = UpdatePlayerPerMinute = 0;
			}

			CPlayer *Update = mUpdatePlayer;
			if (Update->is_dead() || Update->get_game_id() == EMPIRE_GAME_ID)
			{
				mUpdatePlayer = mUpdatePlayer->next();
				continue;
			}
//		SLOG("%s Tick %d -- %d", Update->get_nick(), Update->get_tick(), CGame::get_game_time());
			if (Update->get_tick() > CGame::get_game_time())
			{
				pth_nap((pth_time_t){0, 500});
				break;
			}

			// ignore delay time 3 seconds
			if (Update->get_tick()+3 < CGame::get_game_time()){
				DelayedSecond += CGame::get_game_time()-Update->get_tick();
				DelayedPlayer++;
//				SLOG("%s has a update time delay %d second", Update->get_nick(), CGame::get_game_time()-Update->get_tick());
			}

			if(GAME->mUpdateTurn)
			{
				Update->set_tick(CGame::get_game_time()+CGame::mSecondPerTurn);
			}
			mUpdatePlayer = mUpdatePlayer->next();

			if (CGame::mUpdateTurn)
			   Update->update_turn();

			Update->type(QUERY_UPDATE);
			*STORE_CENTER << *Update;
			Update->save_planet();
			UpdatePlayerPerMinute++;
		}

		CGame::unlock();

		// prevent too much query generation in short time by Nara 2001. 6. 25.
		if( UpdatePlayerPerMinute > NumberofPlayer/MaxUpdatePerMinuteConstant ) pth_nap((pth_time_t){10, 0});

//		SLOG("--------------- %s updated -------------", Update->get_nick());
	}
SLOG("Update Thread Done.");
	return NULL;
}

void
CPlayerTable::refresh_rank_table()
{
	mOverallRankTable.remove_all();
	mFleetRankTable.remove_all();
	mPlanetRankTable.remove_all();
	mTechRankTable.remove_all();

	CRank *
		Rank = NULL;
	for (int i=0 ; i<length() ; i++)
	{
		CPlayer *
			Player = (CPlayer *)get(i);
		if (Player->get_game_id() == EMPIRE_GAME_ID) continue;
		if (Player->is_dead()) continue;

		Player->refresh_power();

		Rank = new CRank();
		Rank->set_id(Player->get_game_id());
		Rank->set_power(Player->get_power());
		mOverallRankTable.add_rank(Rank);

		CFleetList *
			FleetList = Player->get_fleet_list();
		CDock *
			Dock = Player->get_dock();

		Rank = new CRank();
		Rank->set_id(Player->get_game_id());
		Rank->set_power(FleetList->get_power() + Dock->get_power());
		Rank->set_number(FleetList->length());
		mFleetRankTable.add_rank(Rank);

		CPlanetList *
			PlanetList = Player->get_planet_list();

		Rank = new CRank();
		Rank->set_id(Player->get_game_id());
		Rank->set_power(PlanetList->get_power());
		Rank->set_number(PlanetList->length());
		mPlanetRankTable.add_rank(Rank);

		CKnownTechList *
			KnownTechList = Player->get_tech_list();

		Rank = new CRank();
		Rank->set_id(Player->get_game_id());
		Rank->set_power(KnownTechList->get_power());
		Rank->set_number(KnownTechList->length());
		mTechRankTable.add_rank(Rank);
	}

	mOverallRankTable.set_time(time(0));
	mFleetRankTable.set_time(time(0));
	mPlanetRankTable.set_time(time(0));
	mTechRankTable.set_time(time(0));
}

void
CPlayerTable::add_player_rank(CPlayer *aPlayer)
{
	if (!aPlayer) return;

	aPlayer->refresh_power();

	CRank
		*OverallRank = new CRank(),
		*FleetRank = new CRank(),
		*PlanetRank = new CRank(),
		*TechRank = new CRank();
	OverallRank->initialize_from_player(aPlayer, CRank::PLAYER_OVERALL);
	FleetRank->initialize_from_player(aPlayer, CRank::PLAYER_FLEET);
	PlanetRank->initialize_from_player(aPlayer, CRank::PLAYER_PLANET);
	TechRank->initialize_from_player(aPlayer, CRank::PLAYER_TECH);

	mOverallRankTable.add_rank(OverallRank);
	mFleetRankTable.add_rank(FleetRank);
	mPlanetRankTable.add_rank(PlanetRank);
	mTechRankTable.add_rank(TechRank);
}

void
CPlayerTable::remove_player_rank(int aPlayerID)
{
	mOverallRankTable.remove_rank(aPlayerID);
	mFleetRankTable.remove_rank(aPlayerID);
	mPlanetRankTable.remove_rank(aPlayerID);
	mTechRankTable.remove_rank(aPlayerID);
}

char *
CPlayerTable::select_player_except_one(CPlayer *aPlayer)
{
	static CString
		Select;
	Select.clear();

	if ( !length() ) return NULL;

	Select = "<SELECT NAME=\"PLAYER\">\n";
	for ( int i=0; i<PLAYER_TABLE->length() ; i++ )
	{
		CPlayer *
			Player = (CPlayer *)PLAYER_TABLE->get(i);
		if( Player->is_dead() ) continue;
		if( Player == aPlayer ) continue;
		Select.format( "<OPTION VALUE=%d>%s</OPTION>\n",
						Player->get_game_id(), Player->get_nick() );
	}
	Select += "</SELECT>\n";

	return (char *)Select;
}

void
CPlayerTable::verify_player()
{
	CCommandSet
		BasicTechInfo,
		BasicTechLife,
		BasicTechMatter,
		BasicTechSocial;

	for(int i=0; i<TECH_TABLE->length(); i++)
	{
		CTech *
			Tech = (CTech *)TECH_TABLE->get(i);

		if (Tech->is_attribute(CTech::ATTR_BASIC) == false) continue;

		switch (Tech->get_type())
		{
			case CTech::TYPE_INFORMATION:
				BasicTechInfo += Tech->get_id() % 100;
				break;
			case CTech::TYPE_LIFE:
				BasicTechLife += Tech->get_id() % 100;
				break;
			case CTech::TYPE_MATTER_ENERGY:
				BasicTechMatter += Tech->get_id() % 100;
				break;
			case CTech::TYPE_SOCIAL:
				BasicTechSocial += Tech->get_id() % 100;
				break;
			default:
				break;
		}
	}

	for (int i=length()-1 ; i>=0 ; i--)
	{
		CPlayer *
			Player = (CPlayer *)get(i);
		if (Player->get_game_id() == EMPIRE_GAME_ID) continue;

		CKnownTechList *
			KnownTechList = Player->get_tech_list();
		CPlanetList *
			PlanetList = Player->get_planet_list();
		CFleetList *
			FleetList = Player->get_fleet_list();
		CAdmiralList
			*AdmiralList = Player->get_admiral_list(),
			*AdmiralPool = Player->get_admiral_pool();

		// verify tech
		bool
			Update = false;
		if ((KnownTechList->get_information() & BasicTechInfo) != BasicTechInfo)
		{
			Update = true;
		}
		if ((KnownTechList->get_life() & BasicTechLife) != BasicTechLife)
		{
			Update = true;
		}
		if ((KnownTechList->get_matter_energy() & BasicTechMatter) != BasicTechMatter)
		{
			Update = true;
		}
		if ((KnownTechList->get_social() & BasicTechSocial) != BasicTechSocial)
		{
			Update = true;
		}
		if (Update)
		{
			SLOG("ERROR : %s doesn't have all basic techs", Player->get_nick());

			Player->discover_basic_techs();
		}

		if (PlanetList->length() == 0 && Player->is_dead() == false)
		{
			Player->set_dead(
					(char *)format("Player %1$s has been set dead by CPlayerTable::verify_player().", Player->get_nick()));
		}

		if (Player->is_dead() == true) continue;

		// verify admiral

		// check if every fleet has admiral and admiral is attatched to the fleet

		// verify wrongly attatched admiral
		for (int j=FleetList->length()-1 ; j>=0 ; j--)
		{
			CFleet *
				Fleet = (CFleet *)FleetList->get(j);

			for (int k=AdmiralList->length()-1 ; k>=0 ; k--)
			{
				CAdmiral *
					Admiral = (CAdmiral *)AdmiralList->get(k);

				if (Admiral->get_fleet_number() != Fleet->get_id()) continue;
				if (Admiral->get_id() != Fleet->get_admiral_id())
				{
					SLOG( "wrong attatched admiral : %s %s-%d-%d, %s-%d-%d", Player->get_nick(), Fleet->get_name(), Fleet->get_id(), Fleet->get_admiral_id(), Admiral->get_name(), Admiral->get_id(), Admiral->get_fleet_number() );
					Admiral->set_fleet_number(0);
					Player->get_admiral_list()->remove_without_free_admiral( Admiral->get_id() );
					Player->get_admiral_pool()->add_admiral( Admiral );

					Admiral->type(QUERY_UPDATE);
					STORE_CENTER->store(*Admiral);
				}
			}

		// if the admiral is not attatched,
		//	if the admiral is free, attatch it.
			CAdmiral *
				Admiral = AdmiralPool->get_by_id(Fleet->get_admiral_id());
			if (Admiral != NULL)
			{
				SLOG( "unattatched admiral : %s %s-%d-%d %s", Player->get_nick(), Fleet->get_name(), Fleet->get_id(), Fleet->get_admiral_id(), Admiral->get_name() );
				Admiral->set_fleet_number( Fleet->get_id() );
				Player->get_admiral_pool()->remove_without_free_admiral( Admiral->get_id() );
				Player->get_admiral_list()->add_admiral( Admiral );

				Admiral->type(QUERY_UPDATE);
				STORE_CENTER->store(*Admiral);
			}

		//	else disband the unattatched fleet
			Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());
			if (Admiral == NULL)
			{
				SLOG( "unexisting admiral : %s %s-%d-%d", Player->get_nick(), Fleet->get_name(), Fleet->get_id(), Fleet->get_admiral_id() );

				Player->get_dock()->change_ship((CShipDesign*)Fleet, Fleet->get_current_ship());
				Fleet->type(QUERY_DELETE);
				STORE_CENTER->store(*Fleet);

				Player->get_fleet_list()->remove_fleet(Fleet->get_id());
			}
		}

		// check if every admiral has proper fleet value.
		// if not, put the admiral into pull

		for (int j=AdmiralList->length()-1 ; j>=0 ; j--)
		{
			CAdmiral *
				Admiral = (CAdmiral *)AdmiralList->get(j);

			CFleet *
				Fleet = (CFleet *)FleetList->get_by_id(Admiral->get_fleet_number());
			if (Fleet == NULL)
			{
				SLOG( "unattatched admiral : %s %s-%d-%d", Player->get_nick(), Admiral->get_name(), Admiral->get_id(), Admiral->get_fleet_number() );
				Admiral->set_fleet_number( 0 );
				Player->get_admiral_list()->remove_without_free_admiral( Admiral->get_id() );
				Player->get_admiral_pool()->add_admiral( Admiral );

				Admiral->type(QUERY_UPDATE);
				STORE_CENTER->store(*Admiral);
			}
		}


		// verify planet
		bool
			wrong_planet = false;
		for (int j=0 ; j<PlanetList->length() ; j++)
		{
			CPlanet *
				Planet = (CPlanet *)PlanetList->get(j);

			if (Planet == NULL)
			{
				SLOG( "%s has wrong planet order at %d/%d", Player->get_nick(), j, Player->get_planet_list()->length() );
				Player->get_planet_list()->remove_without_free(j);
				j--;
				wrong_planet = true;
			}
		}
		if (wrong_planet == true)
		{
			for (int j=0 ; j<PlanetList->length() ; j++)
			{
				CPlanet *
					Planet = (CPlanet *)PlanetList->get(j);

				Planet->set_order(j);

				Planet->type(QUERY_UPDATE);
				STORE_CENTER->store(*Planet);
			}
		}
	}
}
