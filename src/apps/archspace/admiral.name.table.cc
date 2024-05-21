#include "admiral.h"
#include "race.h"

CAdmiralNameTable::CAdmiralNameTable()
{
}

CAdmiralNameTable::~CAdmiralNameTable()
{
	remove_all();
}

bool
CAdmiralNameTable::free_item(TSomething aItem)
{
	CAdmiralNameList
		*NameList = (CAdmiralNameList *)aItem;

	if (!NameList) return false;

	delete NameList;

	return true;
}

int
CAdmiralNameTable::compare(TSomething aItem1, TSomething aItem2) const
{
	CAdmiralNameList
		*Item1 = (CAdmiralNameList*)aItem1,
		*Item2 = (CAdmiralNameList*)aItem2;

	if (Item1->get_id() > Item2->get_id()) return 1;
	if (Item1->get_id() < Item2->get_id()) return -1;
	return 0;
}

int
CAdmiralNameTable::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CAdmiralNameList
		*Item = (CAdmiralNameList *)aItem;

	if (Item->get_id() > (int)aKey) return 1;
	if (Item->get_id() < (int)aKey) return -1;
	return 0;
}

bool
CAdmiralNameTable::add_name_list(CAdmiralNameList *aAdmiralNameList)
{
	if (!aAdmiralNameList) return false;

	insert_sorted(aAdmiralNameList);

	return true;
}

const char *
CAdmiralNameTable::get_random_name(int aRace)
{
	static CString Result;

	Result.clear();

	assert(aRace >= CRace::RACE_HUMAN);
	assert(aRace <= MAX_RACE);

	if (aRace == CRace::RACE_XESPERADOS) 
		aRace = number(CRace::RACE_XESPERADOS-1);
	if (aRace == CRace::RACE_EVINTOS) 
		return get_evintos_name();

#define FIRST_NAME		0
#define LAST_NAME		1

	int Index = find_sorted_key((TSomething)(aRace*2+FIRST_NAME));
	if (Index < 0) return NULL;
	CAdmiralNameList *List = (CAdmiralNameList*)get(Index);
	int Number = number(List->length())-1;
	char *Name = (char*)List->get(Number);
	Result += Name;

	Index = find_sorted_key((TSomething)(aRace*2+LAST_NAME));
	if (Index >= 0)
	{
		List = (CAdmiralNameList*)get(Index);
		Number = number(List->length())-1;
		Name = (char*)List->get(Number);
		Result += " ";
		Result += Name;
	}

#undef FIRST_NAME
#undef LAST_NAME

	return (char*)Result;
}

const char *
CAdmiralNameTable::get_evintos_name()
{
	int
		Alphabet = number(4),
		Number = number(4);

	static char Buf[12];
	int Index = 0;

	for(int i=0; i<Alphabet; i++)
		Buf[Index++] = (char)(number(26)+'A'-1);

	Buf[Index++] = '-';

	for(int i=0; i<Number; i++)
		Buf[Index++] = (char)(number(9)+'1'-1);
	
	Buf[Index] = 0;

	return Buf;
}

