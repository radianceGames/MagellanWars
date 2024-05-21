#include "util.h"

bool 
CStringList::free_item(TSomething aItem)
{
	delete [] (char*)aItem;
	return true;
}

CStringList::~CStringList()
{
	remove_all();
}

int 
CStringList::compare(TSomething aItem1, TSomething aItem2) const
{
	return strcmp((char*)aItem1, (char*)aItem2);
}

int 
CStringList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	return strcmp((char *)aItem, (char *)aKey);
}

