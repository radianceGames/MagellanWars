#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

CPlayerTable::CPlayerTable(): CList(100, 100)
{
	
}

CPlayerTable::~CPlayerTable()
{
	remove_all();
}

bool
CPlayerTable::free_item(TSomething aItem)
{
	CPlayer
		*Player = (CPlayer*)aItem;

	if (Player) delete Player;

	return true;
}

int
CPlayerTable::add(CPlayer* aPlayer)
{
	if (!aPlayer) 
		return -1;

	operation[](aPlayer->get_id()) = aPlayer;	
	
	return aPlayer->get_id();
}
