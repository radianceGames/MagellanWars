#include "util.h"

bool 
CIntegerList::free_item(TSomething aItem)
{
	return true;
}

CIntegerList::~CIntegerList()
{
	remove_all();
}

int 
CIntegerList::compare(TSomething aItem1, TSomething aItem2) const
{
	return (int)aItem1 - (int)aItem2;
}

int 
CIntegerList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	return (int)aItem - (int)aKey;
}

CIntegerList&
CIntegerList::operator=(CIntegerList& aList)
{
	remove_all();

	for(int i=0; i<aList.length(); i++)
		insert_sorted(aList.get(i));

	return *this;
}

CIntegerList&
CIntegerList::operator+=(CIntegerList& aList)
{
	for(int i=0; i<aList.length(); i++)
		insert_sorted(aList.get(i));

	return *this;
}


