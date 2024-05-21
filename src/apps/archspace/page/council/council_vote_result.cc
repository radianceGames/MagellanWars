#include <libintl.h>
#include "../../pages.h"

bool
CPageCouncilVoteResult::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	CCouncil *
		Council = aPlayer->get_council();

	QUERY("VOTE", VoteString);
	if (VoteString)
	{
		int Vote = as_atoi(VoteString);

		CPlayer *
			Support = Council->get_member_by_game_id(Vote);
		int OldVote = aPlayer->get_council_vote();
		if (Support)
		{
			// council vote change panalty
			if (OldVote)
			{
				if ((OldVote != aPlayer->get_game_id()) && (OldVote != Vote))
				{
					//aPlayer->change_honor(-1);
				}
			}
			aPlayer->set_council_vote(Vote);
		}
		else
		{
			// council vote change panalty
			if (OldVote && (OldVote != aPlayer->get_game_id()))
			{
				//aPlayer->change_honor(-1);
			}
			aPlayer->set_council_vote(NONE);
		}

		if (Support)
		{
			ITEM("RESULT_MESSAGE",
					(char *)format(GETTEXT("You are supporting %1$s now."),
									Support->get_nick()));
		}
		else
		{
			ITEM("RESULT_MESSAGE",
					GETTEXT("You stop supporting any council member now."));
		}
	}
	else
	{
		CPlayer *
			Support = Council->get_member_by_game_id(aPlayer->get_council_vote());

		if (Support)
		{
			ITEM("RESULT_MESSAGE",
					(char *)format(GETTEXT("You stop supporting %1$s now."),
									Support->get_nick()));
		}
		else
		{
			ITEM("RESULT_MESSAGE",
					GETTEXT("You stop supporting any council member now."));
		}
		aPlayer->set_council_vote(NONE);
	}

//	system_log("end page handler %s", get_name());

	return output("council/council_vote_result.html");
}
