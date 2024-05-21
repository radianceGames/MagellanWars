#include <libintl.h>
#include "../../pages.h"
#include "../../game.h"
#include "../../archspace.h"
bool
CPageEmpireRequestBoonResult::handler(CPlayer *aPlayer)
{
	CString	Message;
	CEmpireAction *Action;

	if(!GAME->mUpdateTurn) 
	{
		Action = new CEmpireAction();

		Action->set_id( aPlayer->get_empire_action_list()->get_new_id() );
		Action->set_owner( aPlayer->get_game_id() );
		Action->set_action( CEmpireAction::EA_ACTION_REQUEST_BOON );
		Action->set_target( CEmpireAction::EA_REWARD_DENIED );
		Action->set_answer( CEmpireAction::EA_ANSWER_COMPLETE );
		Action->set_time();
		Action->save_new_action();
		aPlayer->get_empire_action_list()->add_empire_action(Action);

		Message = GETTEXT("You requested the Empire to reward your loyalty. They responded that they did not think you worthy of their blessing.");

		ITEM("RESULT_MESSAGE", (char*)Message);

		return output("empire/request_boon_result.html");
	}
	else
	{
		if (!CGame::mUpdateTurn)
		{
		ITEM("RESULT_MESSAGE", GETTEXT("The empire will not grant boons until time has started."));
		return output("empire/request_boon_result.html");
		}    

		if (aPlayer->test_degree_of_amity())
		{
			Action = aPlayer->grant_boon(CEmpireAction::EA_ACTION_REQUEST_BOON);

			aPlayer->change_empire_relation(-10);
			aPlayer->change_honor_with_news(-4);

			Message.format(GETTEXT("You requested the Empire to reward your loyalty. They accepted and sent you %1$s."),
							Action->get_target_amount_str(aPlayer));
		} else {
			aPlayer->change_honor_with_news(-2);

			Action = new CEmpireAction();

			Action->set_id( aPlayer->get_empire_action_list()->get_new_id() );
			Action->set_owner( aPlayer->get_game_id() );
			Action->set_action( CEmpireAction::EA_ACTION_REQUEST_BOON );
			Action->set_target( CEmpireAction::EA_REWARD_DENIED );
			Action->set_answer( CEmpireAction::EA_ANSWER_COMPLETE );
			Action->set_time();
			Action->save_new_action();
			aPlayer->get_empire_action_list()->add_empire_action(Action);

			Message = GETTEXT("You requested the Empire to reward your loyalty. They responded that they did not think you worthy of their blessing.");
		}

		ITEM("RESULT_MESSAGE", (char*)Message);

		return output("empire/request_boon_result.html");
	}
}
