#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>
#include "../../archspace.h"
#include "../../game.h"

bool
CPageFinancialAid::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	ITEM("MENU_TITLE", GETTEXT("Financial Aid"));

	CCouncil *Council = aPlayer->get_council();

	QUERY("MEMBER", MemberString);
	if (!MemberString)
	{
		ITEM("RESULT_MESSAGE", GETTEXT("Select Member"));
		return output("council/speaker_menu_financial_aid_result.html");
	}

	int Member = as_atoi(MemberString);
	if (!Member)
	{
		ITEM("RESULT_MESSAGE", GETTEXT("Select Member"));
		return output("council/speaker_menu_financial_aid_result.html");
	}

	QUERY("AID_PP", PPString);
	if (!PPString)
	{
		ITEM("RESULT_MESSAGE", GETTEXT("Enter amount of financial aid."));
		return output("council/speaker_menu_financial_aid_result.html");
	}

	int PP = as_atoi(PPString);
	if (PP<1)
	{
		ITEM("RESULT_MESSAGE", GETTEXT("Enter amount of financial aid."));
		return output("council/speaker_menu_financial_aid_result.html");
	}

	if (PP > Council->get_production())
	{
		ITEM("RESULT_MESSAGE",
				(char *)format(GETTEXT("Your council has less PP than %1$s PP."),
						dec2unit(PP)));
		return output("council/speaker_menu_financial_aid_result.html");
	}

	CPlayer *
		AidTarget = Council->get_member_by_game_id(Member);	
	if (AidTarget == NULL)
	{
		ITEM("RESULT_MESSAGE", GETTEXT("Select Member"));
		return output("council/speaker_menu_financial_aid_result.html");
	}

	bool OverflowCheck = false;

	if (AidTarget->get_production() >= MAX_PLAYER_PP)
	{
		ITEM("RESULT_MESSAGE",
			GETTEXT("No financial aid can be given to the target player because he has the maximum allowable PP."));

		return output("council/speaker_menu_financial_aid_result.html");		
	}

	if (PP > MAX_PLAYER_PP - AidTarget->get_production())
	{
		PP = MAX_PLAYER_PP - AidTarget->get_production();
		OverflowCheck = true;
	}

	AidTarget->change_reserved_production(PP);
	Council->change_production(-PP);

	CString
		MaxPlayerPPString,
		NewPPString;
	MaxPlayerPPString = dec2unit(MAX_PLAYER_PP);
	NewPPString = dec2unit(PP);

	if(OverflowCheck == true)
	{
		ITEM("RESULT_MESSAGE", (char *)format(GETTEXT("The max PP the player can have is %1$s PP. %2$s PP has been given to the player while the rest has been returned to the council safe."), 
						(char *)MaxPlayerPPString, (char *)NewPPString));
	}
	else
	{
		ITEM("RESULT_MESSAGE",
				(char *)format(GETTEXT("%1$s PP has been transferred to %2$s successfully."),
						dec2unit(PP), AidTarget->get_nick()));
	}

	ADMIN_TOOL->add_financial_aid_log(
			(char *)format("The speaker of the council %s has given %d PP to the player %s.",
							Council->get_nick(), PP, AidTarget->get_nick()));

//	system_log("end page handler %s", get_name());

	return output("council/speaker_menu_financial_aid_result.html");
}
