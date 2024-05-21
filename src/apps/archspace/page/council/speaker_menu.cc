#include <libintl.h>
#include "../../pages.h"
#include "../../game.h"

void
CPageSpeakerMenu::message_page(const char* aMessage)
{
	ITEM("ERROR_MESSAGE", aMessage);
	output("council/speaker_menu_error.html");
}

bool
CPageSpeakerMenu::are_you_speaker(CPlayer* aPlayer)
{
	if (aPlayer->get_council()->get_speaker_id() != aPlayer->get_game_id() && aPlayer->get_council()->get_secondary_speaker_id() != aPlayer->get_game_id())
	{
		CPageSpeakerMenu::message_page(
					GETTEXT("You are not the speaker or sub speaker of your council. "
		                "You cannot use the council speaker menu."));
		return false;
	}
	return true;
}
bool
CPageSpeakerMenu::are_you_lead_speaker(CPlayer* aPlayer)
{
	if (aPlayer->get_council()->get_speaker_id() != aPlayer->get_game_id())
	{
		CPageSpeakerMenu::message_page(
					GETTEXT("You are not the speaker of your council. "
		                "You cannot use the council speaker menu."));
		return false;
	}
	return true;
}

bool
CPageSpeakerMenu::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	ITEM("STRING_COUNCIL_NAME", GETTEXT("Council Name"));
	ITEM("STRING_COUNCIL_SLOGAN", GETTEXT("Council Slogan"));
	ITEM("STRING_COUNCIL_PP_IN_COUNCIL_SAFE", GETTEXT("Council PP in Council Safe"));

	CCouncil *Council = aPlayer->get_council();
	ITEM("COUNCIL_NICK", Council->get_nick());
	ITEM("COUNCIL_SLOGAN", Council->get_slogan());
	ITEM("COUNCIL_PP", dec2unit(Council->get_production()));

/*
	ITEM("TEAM_SPIRIT_MENU", GETTEXT("Team Spirit"));
	ITEM("TEAM_SPIRIT_COST",
			format(GETTEXT("(%1$s PP required)"), dec2unit(4023)));
*/

	ITEM("DISTRIBUTE_MENU", GETTEXT("Distribute"));

	ITEM("PROJECT_MENU", GETTEXT("Project"));

	switch ( Council->can_buy_any_project() )
	{
		case -1 :
			ITEM("SELECT_PROJECT", GETTEXT("There are no more projects you can buy."));
			ITEM("PROJECT_SUBMIT", " ");
			break;
		case 0 :
			ITEM("SELECT_PROJECT", GETTEXT("You don't have the prerequisites for more projects yet."));
			ITEM("PROJECT_SUBMIT", " ");
			break;
		case 1 :
			ITEM("SELECT_PROJECT", Council->select_project_html());
			ITEM("PROJECT_SUBMIT",
					(char *)format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_ok.gif\" WIDTH=\"31\" HEIGHT=\"11\" BORDER=\"0\">",
									(char *)CGame::mImageServerURL));
			break;
		default :
			ITEM("SELECT_PROJECT", Council->select_project_html());
			ITEM("PROJECT_SUBMIT",
					(char *)format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_ok.gif\" WIDTH=\"31\" HEIGHT=\"11\" BORDER=\"0\">",
									(char *)CGame::mImageServerURL));
	}

	ITEM("FINANCIAL_AID_MENU",
			(char *)format(GETTEXT("Financial Aid (Max : %1$s)"),
					dec2unit(Council->get_production())));
	ITEM("SELECT_MEMBER", Council->select_member_html());

	ITEM("CHANGE_COUNCIL_NAME_MENU", GETTEXT("Change Council Name"));
	ITEM("COUNCIL_NAME", Council->get_name());
	ITEM("CHANGE_COUNCIL_SLOGAN_MENU", GETTEXT("Change Council Slogan"));
	ITEM("COUNCIL_SLOGAN", Council->get_slogan());

	ITEM("STRING_PLAYER_ADMISSION", GETTEXT("Player Admission"));
	ITEM("STRING_COUNCIL_DIPLOMACY", GETTEXT("Council Diplomacy"));

	ITEM("EXPLAIN_PLAYER_ADMISSION",
			GETTEXT("You can manage your new council members."));
	ITEM("EXPLAIN_COUNCIL_DIPLOMACY",
			GETTEXT("You can establish diplomatic relations with other councils."));

//	system_log("end page handler %s", get_name());

	return output("council/speaker_menu.html");
}
