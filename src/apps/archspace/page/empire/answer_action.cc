#include <libintl.h>
#include "../../pages.h"

bool
CPageEmpireAnswerAction::handler(CPlayer *aPlayer)
{
	QUERY("ACTION_ID", ActionIDStr);

	if (!ActionIDStr)
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("Any action was not chosen."));
		return output("empire/answer_action.html");
	}

	int
		ActionID = as_atoi(ActionIDStr);
	CEmpireAction
		*Action = aPlayer->get_empire_action_list()->get_by_id(ActionID);
	if (Action == NULL)
	{
		ITEM("RESULT_MESSAGE",
			GETTEXT("You chose the action that doesn't exist."));
		return output("empire/answer_action.html");
	}
	if (Action->get_answer()!=CEmpireAction::EA_ANSWER_WAITING)
	{
		ITEM("RESULT_MESSAGE",
			GETTEXT("This action has already been answered."));
		return output("empire/answer_action.html");
	}

	QUERY("ANSWER", Answer );

	CString
		Message;

	switch( Action->get_action() ){
		case CEmpireAction::ED_DEMAND_TRIBUTE :
			if( Answer && strcmp( Answer, "yes" ) == 0 ){
				ITEM("RESULT_MESSAGE", Action->accept_demand(aPlayer));
			} else {
				ITEM("RESULT_MESSAGE", Action->reject_demand(aPlayer));
			}
			break;
		case CEmpireAction::ED_DEMAND_LEAVE_COUNCIL :
			if( Answer && strcmp( Answer, "yes" ) == 0 ){
				ITEM("RESULT_MESSAGE", Action->accept_demand(aPlayer));
			} else {
				ITEM("RESULT_MESSAGE", Action->reject_demand(aPlayer));
			}
			break;
		case CEmpireAction::ED_DEMAND_DECLARE_WAR :
			if( Answer && strcmp( Answer, "yes" ) == 0 ){
				ITEM("RESULT_MESSAGE", Action->accept_demand(aPlayer));
			} else {
				ITEM("RESULT_MESSAGE", Action->reject_demand(aPlayer));
			}
			break;
		case CEmpireAction::ED_DEMAND_MAKE_TRUCE :
			if( Answer && strcmp( Answer, "yes" ) == 0 ){
				ITEM("RESULT_MESSAGE", Action->accept_demand(aPlayer));
			} else {
				ITEM("RESULT_MESSAGE", Action->reject_demand(aPlayer));
			}
			break;
		case CEmpireAction::ED_DEMAND_FREE_SUBORDINARY_COUNCIL :
			if( Answer && strcmp( Answer, "yes" ) == 0 ){
				ITEM("RESULT_MESSAGE", Action->accept_demand(aPlayer));
			} else {
				ITEM("RESULT_MESSAGE", Action->reject_demand(aPlayer));
			}
			break;
		case CEmpireAction::ED_DEMAND_ARMAMENT_REDUCTION :
			if( Answer && strcmp( Answer, "yes" ) == 0 ){
				ITEM("RESULT_MESSAGE", Action->accept_demand(aPlayer));
			} else {
				ITEM("RESULT_MESSAGE", Action->reject_demand(aPlayer));
			}
			break;
		case CEmpireAction::ED_GRANT_RANK :
			if( Answer && strcmp( Answer, "yes" ) == 0 ){
				ITEM("RESULT_MESSAGE", Action->accept_demand(aPlayer));
			} else {
				ITEM("RESULT_MESSAGE", Action->reject_demand(aPlayer));
			}
			break;
		default :
			Message.format( "illegal action - %d", Action->get_action() );
			ITEM("RESULT_MESSAGE", (char*)Message);
			break;
	}

	return output("empire/answer_action.html");
}
