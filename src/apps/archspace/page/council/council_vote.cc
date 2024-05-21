#include <libintl.h>
#include "../../define.h"
#include "../../archspace.h"
#include "../../pages.h"

bool
CPageCouncilVote::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	CCouncil *Council = aPlayer->get_council();
	Council->process_vote();

	CPlayer *
		Speaker = Council->get_speaker();
	if (Speaker != NULL)
	{
		ITEM("CURRENT_SPEAKER",
				(char *)format(GETTEXT("Current council speaker: %1$s"),
						Speaker->get_nick()));
	}
	else
	{
		ITEM("CURRENT_SPEAKER",
				(char *)format(GETTEXT("Current council speaker: %1$s"),
						GETTEXT("None")));
	}

#define	EIGHT_HOURS 28800

	if (CGame::mUpdateTurn)
	{
		ITEM("NEXT_ELECTION",
				(char *)format(GETTEXT("Next council speaker election: %1$s"), timetostring(GAME->mLastCouncilElection - time(0) + EIGHT_HOURS)));
	}
	else
	{
		ITEM("NEXT_ELECTION", GETTEXT("Elections will not happen until after time starts."));
	}
#undef EIGHT_HOURS

	ITEM("SELECT_PLAYER_MESSAGE",
			GETTEXT("Select the name of the player for whom you want to vote."));

	ITEM("STRING_NAME", GETTEXT("Name"));
	ITEM("STRING_RACE", GETTEXT("Race"));
	ITEM("STRING_SUPPORT", GETTEXT("Supporting"));
	ITEM("STRING_GAIN", GETTEXT("Gain"));
	ITEM("STRING_VOTE", GETTEXT("Vote"));

	ITEM("VOTE_MEMBER_LIST",
			Council->vote_member_list_html(aPlayer));

//	system_log("end page handler %s", get_name());

	return output("council/council_vote.html");
}
