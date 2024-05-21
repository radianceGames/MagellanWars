#include <libintl.h>
#include "../../pages.h"
#include "../../game.h"
#include <cmath>

bool
CPageEmpireBribeResult::handler(CPlayer *aPlayer)
{
	QUERY("BRIBE_PP", BribePPStr);

	if (!CGame::mUpdateTurn)
	{
		ITEM("RESULT_MESSAGE", GETTEXT("The empire will not accept bribes until time has started."));
		return output("empire/bribe_result.html");
	}

	if (!BribePPStr)
	{
		ITEM("RESULT_MESSAGE", GETTEXT("Enter amount of PP for bribe."));
		return output("empire/bribe_result.html");
	}

	int
		BribePP = as_atoi(BribePPStr);
	if (BribePP < 1)
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("Enter proper amount of PP for bribe."));
		return output("empire/bribe_result.html");
	}
	if (aPlayer->get_production() < BribePP)
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("Enter proper amount of PP for bribe."));
		return output("empire/bribe_result.html");
	}

	time_t
		now = time(0);
	const int
		one_day = 24*60*60;

	for (int i=0 ; i<aPlayer->get_empire_action_list()->length(); i++)
	{
		CEmpireAction *
			Action = (CEmpireAction *)(aPlayer->get_empire_action_list()->get(i));
		if (Action->get_action() == CEmpireAction::EA_ACTION_BRIBE &&
			Action->get_time() > now-one_day)
		{
			ITEM("RESULT_MESSAGE",
				(char*)format(GETTEXT("You may not bribe for another %1$s due to the limit of once per day."), timetostring(Action->get_time() - time(0) +(one_day))));
			return output("empire/bribe_result.html");
		}
	}

	int
		ActualBribePP;
	if( aPlayer->has_ability( ABILITY_DIPLOMAT ) )
		ActualBribePP = BribePP/10*(13+aPlayer->get_control_model()->get_diplomacy());
	else
		ActualBribePP = BribePP/10*(10+aPlayer->get_control_model()->get_diplomacy());

	int
		RelationChange = (int)sqrt( (double)(ActualBribePP/10000) );
	if( RelationChange > 10 ) RelationChange = 10;

	aPlayer->change_reserved_production(-BribePP);
	aPlayer->change_empire_relation( RelationChange );

	CEmpireAction
		*Action = new CEmpireAction();
	Action->set_id( aPlayer->get_empire_action_list()->get_new_id() );
	Action->set_owner( aPlayer->get_game_id() );
	Action->set_action( CEmpireAction::EA_ACTION_BRIBE );
	Action->set_amount( BribePP );
	Action->set_answer( CEmpireAction::EA_ANSWER_COMPLETE );
	Action->set_time();
	if( RelationChange > 0 ) Action->set_target(1);

	Action->save_new_action();

	aPlayer->get_empire_action_list()->add_empire_action(Action);

	CString
		Message;

	if( RelationChange == 0 )
		Message = GETTEXT("The Magistrate seems almost insulted by the amount you have offered. He returns your next couple of messages unopened. Seems your bribe was not well received.");
	else
		Message = GETTEXT("The Magistrate thanks you whole heartedly and begins to tell the Emperor good things about you. The Empire seems to be happier with you.");

	ITEM("RESULT_MESSAGE", (char*)Message);

	return output("empire/bribe_result.html");
}
