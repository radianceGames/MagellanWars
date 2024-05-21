#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../define.h"

bool
CPagePrivateerResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	QUERY("TARGET_PLAYER_ID", TargetPlayerIDString);
	CHECK(TargetPlayerIDString == NULL,
			GETTEXT("You didn't enter a target player's ID."));

	int
		TargetPlayerID = as_atoi(TargetPlayerIDString);
	CPlayer *
		TargetPlayer = PLAYER_TABLE->get_by_game_id(TargetPlayerID);
	CHECK(!TargetPlayer,
			GETTEXT("That player doesn't exist.<BR>Please try again."));

	CHECK(TargetPlayer->get_game_id() == EMPIRE_GAME_ID,
			GETTEXT("You can't privateer in the Empire's domain."));

	CHECK(TargetPlayer->get_game_id() == aPlayer->get_game_id(),
			GETTEXT("You can't privateer in your own domain."));

	CHECK(TargetPlayer->is_dead(),
			(char *)format(GETTEXT("%1$s is dead."),
							TargetPlayer->get_nick()));

    CString
		Attack;
    Attack = aPlayer->check_attackableHi(TargetPlayer);
    CHECK(Attack.length(), Attack);

	QUERY("PRIVATEER_FLEET_ID", PrivateerFleetIDString);
	int
		PrivateerFleetID = as_atoi(PrivateerFleetIDString);
	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	CFleet *
		PrivateerFleet = (CFleet *)FleetList->get_by_id(PrivateerFleetID);
	CHECK(!PrivateerFleet,
			GETTEXT("You did not select any fleet.<BR>"
					"Please try again."));
	//SLOG("PrivateerFleet status %s", PrivateerFleet->get_status_string());

	CHECK(PrivateerFleet->get_status() != CFleet::FLEET_STAND_BY,
			GETTEXT("The selected fleet is not on stand-by."));

	PrivateerFleet->init_mission(CMission::MISSION_PRIVATEER, TargetPlayerID);
	PrivateerFleet->set_status(CFleet::FLEET_PRIVATEER);

	CString
		Result;
	Result.clear();

	Result = "<CENTER>\n";
	Result += GETTEXT("PRIVATEER ORDER");
	Result += "<BR><BR><BR>\n";
	Result.format(GETTEXT("%1$s : %2$s's planet(s)"),
					GETTEXT("TARGET"),
					TargetPlayer->get_nick());
	Result += "<BR>\n";
	Result.format("%s : %s",
					GETTEXT("FLEET"),
					PrivateerFleet->get_nick());
	Result += "<BR><BR>\n";
	Result += "</CENTER>\n";
	Result.format(GETTEXT("%1$s started to %2$s's planet for privateer."),
					PrivateerFleet->get_nick(),
					TargetPlayer->get_nick());

#if defined(PROTECTION_BEGINNER) || defined(PROTECTION_24HOURS)
	if(aPlayer->get_protected_mode() != CPlayer::PROTECTED_NONE)
	{
		aPlayer->set_protected_mode(CPlayer::PROTECTED_NONE);
	}
#endif

	ITEM("RESULT_MESSAGE", (char *)Result);

//	system_log( "end page handler %s", get_name() );
	if (aPlayer->is_protected())
	{
	   aPlayer->clear_lost_planet();
	}

	return output("war/privateer_result.html");
}

