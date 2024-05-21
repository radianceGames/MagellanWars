#include "relation.h"
#include "archspace.h"
#include "council.h"
#include "game.h"

CCouncilRelationTable::~CCouncilRelationTable()
{
	remove_all();
}

bool
CCouncilRelationTable::load(CMySQL &aMySQL)
{
	//aMySQL.query("LOCK TABLE council_relation READ");
	aMySQL.query("SELECT "
				"id, "
				"council1, "
				"council2, "
				"relation, "
				"time "
			"FROM council_relation");

	aMySQL.use_result();

	while(aMySQL.fetch_row())
	{
		CCouncilRelation* Relation = new CCouncilRelation(aMySQL.row());

		if (!Relation->get_council1())
		{
			SLOG("WARNING : WRONG RELATION PLAYER1");
			Relation->type(QUERY_DELETE);
			STORE_CENTER->store(*Relation);
			delete Relation;
			continue;
		}

		if (!Relation->get_council2())
		{
			SLOG("WARNING : WRONG RELATION PLAYER2");
			Relation->type(QUERY_DELETE);
			STORE_CENTER->store(*Relation);
			delete Relation;
			continue;
		}

		Relation->get_council1()->add_relation(Relation);
		Relation->get_council2()->add_relation(Relation);

		if (Relation->get_relation() == CRelation::RELATION_SUBORDINARY)
			Relation->get_council2()->
					set_supremacy_council(Relation->get_council1());

		add_relation(Relation);
	}

	return true;
}

bool
CCouncilRelationTable::free_item(TSomething aItem)
{
	CCouncilRelation 
		*CouncilRelation = (CCouncilRelation*)aItem;

	assert(CouncilRelation);

	delete CouncilRelation;

	return true;
}

void 
CCouncilRelationTable::timeout()
{
	int Count = 0;
	int Index = 0;

	while(Index < length())
	{
		CCouncilRelation *Relation = (CCouncilRelation*)get(Index);
		assert(Relation);
		if (Relation->get_relation() == CRelation::RELATION_TRUCE)
		{
			int DiffTime = CGame::get_game_time() - Relation->get_time();
			if (DiffTime >= ONE_WEEK) 
			{
				Relation->CStore::type(QUERY_DELETE);
				STORE_CENTER->store(*Relation);
				Relation->get_council1()->remove_relation(
						Relation->get_id());
				Relation->get_council2()->remove_relation(
						Relation->get_id());
				remove_relation(Relation->get_id());
				Count++;
			} else Index++;
		} else Index++;
	}
//	SLOG("remove council relationship by timeout %d/%d", Count, length());
}
