#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageSpyResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

#define GENERAL_INFORMATION_GATHERING	8001
#define DETAILED_INFORMATION_GATHERING	8002
#define STEAL_SECRET_INFO				8003
#define COMPUTER_VIRUS_INFILTRATION		8004
#define DEVASTATING_NETWORK_WORM		8005
#define SABOTAGE						8006
#define INCITE_RIOT						8007
#define STEAL_TECHNOLOGY				8008
#define ARTIFICIAL_DISEASE				8009
#define RED_DEATH						8010
#define STRIKE_BASE						8011
#define METEOR_STRIKE					8012
#define EMP_STORM						8013
#define STELLAR_BOMBARDMENT				8014
#define ASSASSINATION					8015

	QUERY("TARGET_ID", TargetIDString);
	int
		TargetID = as_atoi(TargetIDString);
	CPlayer *
		TargetPlayer = PLAYER_TABLE->get_by_game_id(TargetID);

	if (!TargetPlayer)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You chose wrong player."));
		return output("diplomacy/spy_error.html");
	}
	if (TargetPlayer->is_dead())
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("%1$s is dead."),
								TargetPlayer->get_nick()));
		return output("diplomacy/spy_error.html");
	}
	if(TargetPlayer->get_game_id() == EMPIRE_GAME_ID)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You have chosen the empire as a special operation target."));

		return output("diplomacy/spy_error.html");
	}
	if(TargetPlayer->get_game_id() == aPlayer->get_game_id())
	{
		ITEM("ERROR_MESSAGE", GETTEXT("Spying on yourself is prohibited."));

		return output("diplomacy/spy_error.html");
	}

	QUERY("SPY_ID", SpyIDString);
	int
		SpyID = as_atoi(SpyIDString);
	CSpy *
		Spy = SPY_TABLE->get_by_id(SpyID);
	if (!Spy)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You chose wrong spy mission. Please try gain."));

		return output("diplomacy/spy_error.html");
	}

	if (aPlayer->get_production() < Spy->get_cost())
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Your PP is not enough to perform this special operation."));
		return output("diplomacy/spy_error.html");
	}

	if (!Spy->evaluate(aPlayer))
	{
			ITEM("ERROR_MESSAGE",
					GETTEXT("You don't know the prerequisite technology"
							" to perform this special operation."));
			return output("diplomacy/spy_error.html");
	}

	if (!CGame::mUpdateTurn)
	{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("You can't perform special operations until time has started.")));
			return output("diplomacy/spy_error.html");
	}

	if (aPlayer->has_ability(ABILITY_NO_SPY))
	{
		if (Spy->get_id() != METEOR_STRIKE && Spy->get_id() != STELLAR_BOMBARDMENT)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("You can't perform %1$s for some reason."),
									Spy->get_name()));
			return output("diplomacy/spy_error.html");
		}
	}

	if (TargetPlayer->has_ability(ABILITY_NO_SPY))
	{
		if (Spy->get_id() != METEOR_STRIKE && Spy->get_id() != STELLAR_BOMBARDMENT)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("You can't perform %1$s for some reason."),
									Spy->get_name()));
			return output("diplomacy/spy_error.html");
		}
	}

	if (aPlayer->has_ability(ABILITY_PACIFIST) && Spy->get_type() == CSpy::TYPE_ATROCIOUS)
	{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("You can't perform %1$s because you love peace."),
									Spy->get_name()));
			return output("diplomacy/spy_error.html");
	}

	bool
		Success;
	ITEM("RESULT_MESSAGE", aPlayer->process_spy(Spy->get_id(), TargetPlayer, &Success));

	aPlayer->change_reserved_production(-Spy->get_cost());

	if (Success)
	{
		switch (Spy->get_id())
		{
			case GENERAL_INFORMATION_GATHERING :
				ITEM("RESULT_IMAGE", "spy_general_information.gif");
				break;

			case DETAILED_INFORMATION_GATHERING :
				ITEM("RESULT_IMAGE", "spy_detailed_information.gif");
				break;

			case STEAL_SECRET_INFO :
				ITEM("RESULT_IMAGE", "spy_steal_secret.gif");
				break;

			case COMPUTER_VIRUS_INFILTRATION :
				ITEM("RESULT_IMAGE", "spy_computer_virus.gif");
				break;

			case DEVASTATING_NETWORK_WORM :
				ITEM("RESULT_IMAGE", "spy_network_worm.gif");
				break;

			case SABOTAGE :
				ITEM("RESULT_IMAGE", "spy_sabotage.gif");
				break;

			case INCITE_RIOT :
				ITEM("RESULT_IMAGE", "spy_incite_riot.gif");
				break;

			case STEAL_TECHNOLOGY :
				ITEM("RESULT_IMAGE", "spy_steal_technology.gif");
				break;

			case ARTIFICIAL_DISEASE :
				ITEM("RESULT_IMAGE", "spy_artificial_disease.gif");
				break;

			case RED_DEATH :
				ITEM("RESULT_IMAGE", "spy_red_death.gif");
				break;

			case STRIKE_BASE :
				ITEM("RESULT_IMAGE", "spy_strike_base.gif");
				break;

			case METEOR_STRIKE :
				ITEM("RESULT_IMAGE", "spy_meteor_strike.gif");
				break;

			case EMP_STORM :
				ITEM("RESULT_IMAGE", "spy_emp_storm.gif");
				break;

			case STELLAR_BOMBARDMENT :
				ITEM("RESULT_IMAGE", "spy_stellar_bombardment.gif");
				break;

			case ASSASSINATION :
				ITEM("RESULT_IMAGE", "spy_assassination.gif");
				break;

			default :
				ITEM("RESULT_IMAGE", "spy_failure.gif");
		}
	}
	else
	{
		ITEM("RESULT_IMAGE", "spy_failure.gif");
	}

//	system_log("end page handler %s", get_name());
	if (aPlayer->is_protected())
	{
	   aPlayer->clear_lost_planet();
	}

	return output( "diplomacy/spy_result.html" );
}
