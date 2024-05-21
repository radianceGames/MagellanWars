#include "runtime.h"

CTrigger::CTrigger()
{
	mLastAction = time(0);
}

CTrigger::~CTrigger()
{
}

bool
CTrigger::shoot()
{
	time_t 
		Now = time(0);
	time_t
		Gap = Now - mLastAction;

	if (Gap >= interval())
	{
		if (!handler()) return false;
		mLastAction = Now;
	}
	return true;
}
