#include "relation.h"
#include "player.h"

CPlayerRelationList::CPlayerRelationList()
{
	mOwner = NULL;
}

CPlayerRelationList::~CPlayerRelationList()
{
	remove_all();
}

void 
CPlayerRelationList::set_owner(CPlayer *aOwner)
{
	mOwner = aOwner;
}

CPlayerRelation*
CPlayerRelationList::find_by_target_id(int aID)
{
	for(int i=0; i<length(); i++)
	{
		CPlayerRelation* Relation = (CPlayerRelation*)get(i);
		if (Relation->get_player1() == mOwner)
		{
			if (Relation->get_player2()->get_game_id() == aID)
				return Relation;
		} else {
			if (Relation->get_player1()->get_game_id() == aID)
				return Relation;
		}
	}

	return NULL;
}

bool 
CPlayerRelationList::remove_player_relation_by_target_id(int aTargetID)
{
	CPlayerRelation* PRelation = NULL;

	for(int i=0; i<length(); i++)
	{
		CPlayerRelation* Relation = (CPlayerRelation*)get(i);
		if (Relation->get_player1() == mOwner)
		{
			if (Relation->get_player2()->get_game_id() == aTargetID)
			{
				PRelation = Relation;
				break;
			}
		} else {
			if (Relation->get_player1()->get_game_id() == aTargetID)
			{
				PRelation = Relation;
				break;
			}
		}
	}

	if (!PRelation) return false;

	return remove_relation(PRelation->get_id());
}

char *
CPlayerRelationList::select_player_by_relation_html(CPlayer *aPlayer, int aRelation)
{
	static CString
		Select;
	Select.clear();

	Select = "<SELECT NAME=\"PLAYER_ID\">\n";

	for (int i=0 ; i<length() ; i++)
	{
		CPlayerRelation *
			Relation = (CPlayerRelation *)get(i);

		if (Relation->get_relation() != aRelation) continue;

		CPlayer *
			Player = NULL;
		CPlayer *
			Player1 = Relation->get_player1();
		CPlayer *
			Player2 = Relation->get_player2();

		if (Player1->get_game_id() == aPlayer->get_game_id())
		{
			Player = Player2;
		} else if (Player2->get_game_id() == aPlayer->get_game_id())
		{
			Player = Player1;
		}

		Select.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
					Player->get_game_id(), Player->get_nick());
	}

	Select += "</SELECT>\n";

	return (char *)Select;
}

