#include "action.h"
#include <cstdlib>
#include "define.h"
#include "archspace.h"
#include "council.h"

CCouncilActionTable::CCouncilActionTable()
{
}

CCouncilActionTable::~CCouncilActionTable()
{
	remove_all();
}

CCouncilAction *
CCouncilActionTable::get_by_owner_type(CCouncil *aCouncil, int aType)
{
	if (aCouncil == NULL) return NULL;

	for (int i=0 ; i<length() ; i++)
	{
		CCouncilAction *
			CouncilAction = (CCouncilAction *)get(i);
		CCouncil *
			Owner = CouncilAction->get_owner();
		if (Owner->get_id() == aCouncil->get_id() &&
			CouncilAction->get_type() == aType)
		{
			return CouncilAction;
		}
	}

	return NULL;
}

void
CCouncilActionTable::load(CMySQL &aMySQL)
{
	#define FIELD_ACTION 2

	//aMySQL.query( "LOCK TABLE council_action READ" );
	aMySQL.query( "SELECT * FROM council_action" );

	aMySQL.use_result();

	while(aMySQL.fetch_row())
	{
		CCouncilAction *Action;

		switch((CAction::EActionType)atoi(aMySQL.row(FIELD_ACTION)))
		{
			case CAction::ACTION_COUNCIL_DECLARE_TOTAL_WAR:
				Action = new CCouncilActionDeclareTotalWar();
				break;
			case CAction::ACTION_COUNCIL_DECLARE_WAR:
				Action = new CCouncilActionDeclareWar();
				break;
			case CAction::ACTION_COUNCIL_BREAK_PACT:
				Action = new CCouncilActionBreakPact();
				break;
			case CAction::ACTION_COUNCIL_BREAK_ALLY:
				Action = new CCouncilActionBreakAlly();
				break;
			case CAction::ACTION_COUNCIL_IMPROVE_RELATION:
				Action = new CCouncilActionImproveRelation();
				break;
			case CAction::ACTION_PLAYER_COUNCIL_DONATION:
			case CAction::ACTION_PLAYER_BREAK_PACT:
			case CAction::ACTION_PLAYER_BREAK_ALLY:
			case CAction::ACTION_EMPIRE_BRIBE:
				break;
			case CAction::ACTION_COUNCIL_MERGING_OFFER :
				Action = new CCouncilActionMergingOffer();
				break;

			case CAction::ACTION_PLAYER_SIEGE_BLOCKADE_RESTRICTION :
				break;

			case CAction::ACTION_PLAYER_SIEGE_BLOCKADE_PROTECTION :
				break;

			case CAction::ACTION_PLAYER_EMPIRE_INVASION_HISTORY :
				break;

			case CAction::ACTION_NONE:
				SLOG("error in load council action");
				return;
		}

		Action->load_from_database(aMySQL.row());

		if (!Action->get_owner())
		{
			SLOG("WARNING : WRONG ACTION OWNER : ID=%d ACT=%d OWNER=%d",
					Action->get_id(), 
					Action->get_type(), 
					Action->get_owner());
			Action->type(QUERY_DELETE);
			STORE_CENTER->store(*Action);
			delete Action;
			continue;
		}

		Action->get_owner()->add_action(Action);
		add_action(Action);
	}

	aMySQL.free_result();
	//aMySQL.query( "UNLOCK TABLES" );
}

bool
CCouncilActionTable::process_expire(CAction* aAction)
{
	CCouncilAction *Action = (CCouncilAction*)aAction;
	Action->feedback();
	Action->get_owner()->remove_action(Action->get_id());
	Action->type(QUERY_DELETE);
	STORE_CENTER->store(*Action);
	remove_action(Action->get_id());
	return true;
}
