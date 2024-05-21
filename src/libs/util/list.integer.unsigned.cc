#include "util.h"

bool 
CUnsignedIntegerList::free_item(TSomething aItem)
{
	return true;
}

CUnsignedIntegerList::~CUnsignedIntegerList()
{
	remove_all();
}

int 
CUnsignedIntegerList::compare(TSomething aItem1, TSomething aItem2) const
{
	return (unsigned int)aItem1 - (unsigned int)aItem2;
}

int 
CUnsignedIntegerList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	return (unsigned int)aItem - (unsigned int)aKey;
}

CUnsignedIntegerList&
CUnsignedIntegerList::operator=(CUnsignedIntegerList& aList)
{
	remove_all();

	for(int i=0; i<aList.length(); i++)
		insert_sorted(aList.get(i));

	return *this;
}

CUnsignedIntegerList&
CUnsignedIntegerList::operator+=(CUnsignedIntegerList& aList)
{
	for(int i=0; i<aList.length(); i++)
		insert_sorted(aList.get(i));

	return *this;
}


