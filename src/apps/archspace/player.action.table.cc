#include "action.h"
#include <cstdlib>
#include "define.h"
#include "archspace.h"
#include "player.h"

CPlayerActionTable::CPlayerActionTable()
{
}

CPlayerActionTable::~CPlayerActionTable()
{
	remove_all();
}

CPlayerAction *
CPlayerActionTable::get_by_owner_type(CPlayer *aPlayer, int aType)
{
	if (aPlayer == NULL) return NULL;

	for (int i=0 ; i<length() ; i++)
	{
		CPlayerAction *
			PlayerAction = (CPlayerAction *)get(i);
		CPlayer *
			Owner = PlayerAction->get_owner();
		if (Owner->get_game_id() == aPlayer->get_game_id() &&
			PlayerAction->get_type() == aType)
		{
			return PlayerAction;
		}
	}

	return NULL;
}

void
CPlayerActionTable::load(CMySQL &aMySQL)
{
	#define FIELD_ACTION 2

	//aMySQL.query( "LOCK TABLE player_action READ" );
	aMySQL.query( "SELECT "
						"id, "
						"start_time, "
						"action, "
						"owner, "
						"argument "
					"FROM player_action" );

	aMySQL.use_result();

	while(aMySQL.fetch_row())
	{
		CPlayerAction *Action;

		switch((CAction::EActionType)atoi(aMySQL.row(FIELD_ACTION)))
		{
			case CAction::ACTION_PLAYER_COUNCIL_DONATION:
					Action = new CPlayerActionCouncilDonation();
					break;
			case CAction::ACTION_PLAYER_BREAK_PACT:
					Action = new CPlayerActionBreakPact();
					break;
			case CAction::ACTION_PLAYER_BREAK_ALLY:
					Action = new CPlayerActionBreakAlly();
					break;
			case CAction::ACTION_PLAYER_SIEGE_BLOCKADE_RESTRICTION:
					Action = new CPlayerActionSiegeBlockadeRestriction();
					break;
			case CAction::ACTION_PLAYER_SIEGE_BLOCKADE_PROTECTION:
					Action = new CPlayerActionSiegeBlockadeProtection();
					break;
			case CAction::ACTION_PLAYER_EMPIRE_INVASION_HISTORY:
					Action = new CPlayerActionEmpireInvasionHistory();
					break;
            case CAction::ACTION_PLAYER_DECLARE_HOSTILE:
                    Action = new CPlayerActionDeclareHostile();
                    break;
			case CAction::ACTION_COUNCIL_DECLARE_TOTAL_WAR:
			case CAction::ACTION_COUNCIL_DECLARE_WAR:
			case CAction::ACTION_COUNCIL_BREAK_ALLY:
			case CAction::ACTION_COUNCIL_BREAK_PACT:
			case CAction::ACTION_COUNCIL_IMPROVE_RELATION:
			case CAction::ACTION_EMPIRE_BRIBE:
			case CAction::ACTION_COUNCIL_MERGING_OFFER:
			case CAction::ACTION_NONE:
					SLOG("error in load player action, Action = %d", atoi(aMySQL.row(FIELD_ACTION)));
					continue;
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
CPlayerActionTable::process_expire(CAction* aAction)
{
	CPlayerAction *Action = (CPlayerAction*)aAction;
	Action->feedback();
	Action->get_owner()->remove_action(Action->get_id());
	Action->type(QUERY_DELETE);
	STORE_CENTER->store(*Action);
	remove_action(Action->get_id());
	return true;
}
