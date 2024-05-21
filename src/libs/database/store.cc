#include "store.h"

CStore::CStore()
{
	mQueryType = QUERY_NONE;
}

CStore::~CStore()
{
}

int
CStore::type(int aType)
{
	if (aType < 0) 
	{
		return mQueryType;
	}
	return (mQueryType = aType);
}

bool
CStore::is_changed()
{
	if (mStoreFlag.is_empty() == true)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool
CStore::set_all_changed()
{
	for (int i=0 ; i<128*8 ; i++)
	{
		mStoreFlag += i;
	}

	return true;
}

