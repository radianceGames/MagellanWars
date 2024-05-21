#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageSpyConfirm::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	QUERY("FROM_WHAT", FromWhatString);
	if (!FromWhatString)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You didn't choose any method so that you could select a target."));

		return output("diplomacy/spy_error.html");
	}

	CPlayer *
		TargetPlayer;

	if (!strcmp(FromWhatString, "FROM_SELECT"))
	{
		QUERY("PLAYER_ID", PlayerIDString);
		int
			PlayerID = as_atoi(PlayerIDString);
		TargetPlayer = PLAYER_TABLE->get_by_game_id(PlayerID);

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
	}
	else if (!strcmp(FromWhatString, "FROM_INPUT"))
	{
		QUERY("TARGET_ID", TargetIDString);
		int
			TargetID = as_atoi(TargetIDString);
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
	}
	else
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You chose wrong method so that you could select a target."));

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

	#define METEOR_STRIKE			8012
	#define STELLAR_BOMBARDMENT		8014

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

	static CString
		Info;
	Info.clear();

	CRelation *
		Relation = aPlayer->get_relation(TargetPlayer);

	if (!Relation)
	{
		Info.format(GETTEXT("The Target : %1$s"),
					TargetPlayer->get_nick(),
					CRelation::get_relation_description(CRelation::RELATION_NONE));
	}
	else
	{
		Info.format(GETTEXT("The Target : %1$s in %2$s Relation"),
					TargetPlayer->get_nick(),
					CRelation::get_relation_description(Relation->get_relation()));
	}

	Info += "<BR>\n";

	Info.format(GETTEXT("Special Operation : %1$s (%2$s)"),
				Spy->get_name(), Spy->get_type_name());

	int
		HonorLoss = CSpy::get_honor_loss(aPlayer, TargetPlayer, Spy->get_type());
	if (HonorLoss > 0)
	{
		Info += "<BR><BR>\n";
		Info.format(GETTEXT("You may lose some honor by this operation"
							" if you are detected.(Honor -%1$d)"),
					HonorLoss);
	}

	ITEM("INFORMATION_MESSAGE", (char *)Info);

	ITEM("CONFIRM_MESSAGE", GETTEXT("Are you sure to execute this operation?"));

	ITEM("TARGET_ID", TargetPlayer->get_game_id());
	ITEM("SPY_ID", Spy->get_id());

//	system_log("end page handler %s", get_name());

	return output( "diplomacy/spy_confirm.html" );
}
