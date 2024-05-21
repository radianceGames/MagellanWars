#include "universe.h"
#include <cstdlib>

CClusterNameList::CClusterNameList():CSortedList(510, 50)
{
}

CClusterNameList::~CClusterNameList()
{
	remove_all();
}

bool
CClusterNameList::free_item(TSomething aItem)
{
	assert(aItem);

	char *String = (char*)aItem;

	::free(String);

	return true;
}

int
CClusterNameList::compare(TSomething aItem1, TSomething aItem2) const
{
	assert(aItem1);
	assert(aItem2);

	char
		*Name1 = (char*)aItem1,
		*Name2 = (char*)aItem2;

	return strcmp(Name1, Name2);
}

int
CClusterNameList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	assert(aItem);
	assert(aKey);

	char
		*Name = (char*)aItem;

	return strcmp(Name, (char*)aKey);
}

void 
CClusterNameList::add_name(const char* aName)
{
	assert(aName);

	int 
		Index = find_sorted_key((TConstSomething)aName);

	if (Index >= 0) return;

	char *Buffer = (char*)::malloc(strlen(aName)+1);
	strcpy(Buffer, aName);

	insert_sorted((TSomething)Buffer);
}

const char*
CClusterNameList::get_random_name()
{
	if (!length()) return NULL;

	int 
		Index = number(length())-1;	

	return (char*)get(Index);
}
