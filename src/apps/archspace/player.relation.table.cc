#include "relation.h"
#include "archspace.h"
#include "player.h"
#include "game.h"

CPlayerRelationTable::~CPlayerRelationTable()
{
	remove_all();
}

bool
CPlayerRelationTable::load(CMySQL &aMySQL)
{
	//aMySQL.query("LOCK TABLE player_relation READ");
	aMySQL.query("SELECT "
				"id, "
				"player1, "
				"player2, "
				"relation, "
				"time "
			"FROM player_relation");

	aMySQL.use_result();

	while(aMySQL.fetch_row())
	{
		CPlayerRelation* Relation = new CPlayerRelation(aMySQL.row());

		if (!Relation->get_player1())
		{
			SLOG("WARNING : WRONG RELATION PLAYER1");
			Relation->type(QUERY_DELETE);
			STORE_CENTER->store(*Relation);
			delete Relation;
			continue;
		}

		if (!Relation->get_player2())
		{
			SLOG("WARNING : WRONG RELATION PLAYER2");
			Relation->type(QUERY_DELETE);
			STORE_CENTER->store(*Relation);
			delete Relation;
			continue;
		}

		Relation->get_player1()->add_relation(Relation);
		Relation->get_player2()->add_relation(Relation);

		add_relation(Relation);
	}

	return true;
}

bool
CPlayerRelationTable::free_item(TSomething aItem)
{
	CPlayerRelation 
		*PlayerRelation = (CPlayerRelation*)aItem;

	assert(PlayerRelation);

	delete PlayerRelation;

	return true;
}

void 
CPlayerRelationTable::timeout()
{
	int Count = 0;
	int Index = 0;

	while(Index < length())
	{
		CPlayerRelation *Relation = (CPlayerRelation*)get(Index);
		assert(Relation);
		if (Relation->get_relation() == CRelation::RELATION_TRUCE)
		{
			int DiffTime = CGame::get_game_time() - Relation->get_time();
			if (DiffTime >= ONE_WEEK) 
			{
				Relation->CStore::type(QUERY_DELETE);
				STORE_CENTER->store(*Relation);
				Relation->get_player1()->remove_relation(
						Relation->get_id());
				Relation->get_player2()->remove_relation(
						Relation->get_id());
				remove_relation(Relation->get_id());
				Count++;
			} else Index++;
		} else Index++;
	}
//	SLOG("remove relationship by timeout %d/%d", Count, length());
}
