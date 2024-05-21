#include "action.h"
#include "game.h"

CActionTable::CRecentlyIndex::CRecentlyIndex()
{
}

CActionTable::CRecentlyIndex::~CRecentlyIndex()
{
	remove_all();
}

bool
CActionTable::CRecentlyIndex::free_item(TSomething aItem)
{
	(void)aItem;

	return true;
}

int
CActionTable::CRecentlyIndex::compare(
		TSomething aItem1, TSomething aItem2) const
{
	CAction *Action1 = (CAction*)aItem1;
	CAction *Action2 = (CAction*)aItem2;

	if ((Action1->get_start_time() + Action1->get_period())
			< (Action2->get_start_time() + Action2->get_period()))
		return -1;
	if ((Action1->get_start_time() + Action1->get_period())
			> (Action2->get_start_time() + Action2->get_period()))
		return 1;

	return 0;
}

int 
CActionTable::CRecentlyIndex::compare_key( 
		TSomething aItem, TConstSomething aKey) const
{
	(void)aItem;
	(void)aKey;

	assert(0);

	return 0;
}

CActionTable::CActionTable()
{
}

CActionTable::~CActionTable()
{
	remove_all();
}

bool
CActionTable::free_item(TSomething aItem)
{
	CAction
		*Action = (CAction *)aItem;

	assert(Action);
	
	delete Action;

	return true;
}

void 
CActionTable::expire()
{
	if (!length()) return;

//	SLOG("ActionTable check expire count:%d, time:%d", length(), CGame::get_game_time());

	int Count = 0;

	while(true)
	{
		CAction* Action = (CAction*)mRecentlyIndex.get(0);
		if (!Action) break;
		
        if (Action->get_period() != -1 && (Action->get_start_time() + 
					Action->get_period())
								<= CGame::get_game_time())
		{
			process_expire(Action);
			Count++;
		} else break;
	}

//	SLOG("ActionTable expired Action %d", Count);
}

bool
CActionTable::add_action(CAction *aAction)
{
	if (CActionList::add_action(aAction))
		mRecentlyIndex.insert_sorted(aAction);

	return true;
}

bool
CActionTable::remove_action(int aActionID)
{
	int
		Index = find_sorted_key((TSomething)aActionID);
	if (Index < 0) return false;

	CAction *
		Action = (CAction *)get(Index);
	if (Action)
	{
		int
			Index2 = mRecentlyIndex.find(Action);
		if (Index >= 0)
			mRecentlyIndex.CSortedList::remove(Index2);
	} 

	return CSortedList::remove(Index);
}
