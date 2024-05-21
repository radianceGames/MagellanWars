#include <libintl.h>
#include "../../pages.h"

bool
CPageFleetCommander::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

 	// if player doesn't have a preference object yet, make it
   	if (aPlayer->get_preference() == NULL)
       	aPlayer->set_preference(new CPreference(aPlayer->get_game_id()));
	ITEM( "RESULT_MESSAGE", " ");

	ITEM("JS_FLEET_COMMANDER_NUMBER",
			aPlayer->get_admiral_pool()->length());

	ITEM("ATTACHED_FLEET_COMMANDER_JAVASCRIPT",
			aPlayer->get_admiral_list()->attached_fleet_commander_info_javascript(aPlayer));

	ITEM("POOLED_FLEET_COMMANDER_JAVASCRIPT",
			aPlayer->get_admiral_pool()->pool_fleet_commander_info_javascript(aPlayer));

	ITEM("STRING_ATTACHED_FLEET_COMMANDER_S_",
			GETTEXT("Attached Fleet Commander(s)"));

	ITEM("ATTACHED_FLEET_COMMANDER_INFO",
			aPlayer->get_admiral_list()->attached_fleet_commander_info_html());

	ITEM("STRING_FLEET_COMMANDER_S__IN_THE_POOL",
			GETTEXT("Fleet Commander(s) in the Pool"));

	ITEM("POOL_FLEET_COMMANDER_INFO",
			aPlayer->get_admiral_pool()->pool_fleet_commander_info_html());

 	ITEM("PLAYER_COMMANDER_VIEW_OPTIONS",
			aPlayer->get_preference()->commander_view_html());
//	system_log("end page handler %s", get_name());

	return output( "fleet/fleet_commander.html" );
}

