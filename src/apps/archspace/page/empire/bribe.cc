#include <libintl.h>
#include "../../pages.h"
#include "../../game.h"

bool
CPageEmpireBribe::handler(CPlayer *aPlayer)
{
	ITEM( "DESCRIPTION", (char*)(*mBribeDescription) );
	ITEM( "EMPIRE_RELATION", aPlayer->get_degree_name_of_empire_relation() );
	ITEM( "COURT_RANK", aPlayer->get_court_rank_name() );

	time_t
		now = time(0);
	const int
		one_day = 24*60*60;

	for (int i=0 ; i<aPlayer->get_empire_action_list()->length(); i++)
	{
		CEmpireAction *
			Action = (CEmpireAction *)(aPlayer->get_empire_action_list()->get(i));
		if (Action->get_action() == CEmpireAction::EA_ACTION_BRIBE &&
			Action->get_time() > now - one_day)
		{
			ITEM("BRIBE_AMOUNT",
				(char*)format(GETTEXT("You may not bribe for another %1$s due to the limit of once per day."),
				timetostring(Action->get_time() - time(0) + one_day)));
			return output("empire/bribe.html");
		}
	}

	ITEM( "BRIBE_AMOUNT", "<INPUT TYPE=\"text\" SIZE=\"30\" MAXLENGTH=\"80\" NAME=\"BRIBE_PP\" CLASS=\"newInput\">");

	return output("empire/bribe.html");
}
