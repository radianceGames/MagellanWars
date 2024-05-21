#include "player.h"

CPlayerList::CPlayerList()
{
}

CPlayerList::~CPlayerList()
{
	remove_all();
}

bool
CPlayerList::free_item(TSomething aItem)
{
	(void)aItem;
	return true;
}

int
CPlayerList::compare(TSomething aItem1, TSomething aItem2) const
{
	CPlayer
		*Player1 = (CPlayer*)aItem1,
		*Player2 = (CPlayer*)aItem2;

	if (Player1->get_game_id() > Player2->get_game_id()) return 1;
	if (Player1->get_game_id() < Player2->get_game_id()) return -1;
	return 0;
}

int
CPlayerList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CPlayer
		*Player = (CPlayer*)aItem;

	if (Player->get_game_id() > (int)aKey) return 1;
	if (Player->get_game_id() < (int)aKey) return -1;
	return 0;
}

bool
CPlayerList::remove_player(int aID)
{
	int 
		Index;

	Index = find_sorted_key((void *)aID);	
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

bool
CPlayerList::add_player(CPlayer* aPlayer)
{
	if (!aPlayer) return false;

	if (find_sorted_key((TConstSomething)aPlayer->get_game_id()) >= 0) 
		return false;
	insert_sorted(aPlayer);

	return true;
}

CPlayer*
CPlayerList::get_by_game_id(int aGameID)
{
	if (!length()) return NULL;

	int 
		Index = find_sorted_key((void*)aGameID);

	if (Index < 0) return NULL;

	return (CPlayer *)get(Index);
}
