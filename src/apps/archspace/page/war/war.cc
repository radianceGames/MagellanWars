#include <libintl.h>
#include "../../pages.h"

bool
CPageWar::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	ITEM("STRING_WAR_STATUS", GETTEXT("War Status"));
	ITEM("WAR_STATUS_INFORMATION", aPlayer->war_status_information_html());

	ITEM("STRING_COUNCIL_WAR_STATUS", GETTEXT("Council War Status"));

	CCouncil *
		Council = aPlayer->get_council();
	ITEM("COUNCIL_WAR_STATUS_INFORMATION",
			Council->council_war_status_information_html());

#define MAX_BATTLE_RECORD	10

	ITEM("STRING_BATTLE_REPORT", GETTEXT("Battle Report"));
	int
		Dummy;
	ITEM("BATTLE_RECORD_LIST",
			aPlayer->battle_record_list_html(false, -1, MAX_BATTLE_RECORD, &Dummy));
#undef MAX_BATTLE_RECORD

	ITEM("SIEGE_PLANET_EXPLAIN",
			GETTEXT("Attack the enemy to seize a planet."));

	ITEM("BLOCKADE_EXPLAIN",
			GETTEXT("Block the production of an enemy planet by arms."));

	ITEM("RAID_EXPLAIN",
			GETTEXT("Bomb an enemy planet."));

	ITEM("PRIVATEER_EXPLAIN",
			GETTEXT("Loot the trading ships of the enemy."));

	ITEM("DEFENSE_PLAN_EXPLAIN",
			GETTEXT("Make your own defense plans."));

	ITEM("BATTLE_REPORT_EXPLAIN",
			GETTEXT("You can see the reports about the previous battles."));

//	system_log("end page handler %s", get_name());

	return output("war/war.html");
}

