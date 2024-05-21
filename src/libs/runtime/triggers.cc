#include "runtime.h"
#include "net.h"

CTriggerStation::CTriggerStation():CList(16, 16)
{
}

CTriggerStation::~CTriggerStation()
{
	system_log("free trigger station");
	remove_all();
}

bool
CTriggerStation::free_item(TSomething aItem)
{
	CTrigger 
		*Trigger = (CTrigger*)aItem;

//	system_log("free trigger %s", Trigger->name());

	delete Trigger;

	return true;
}

bool
CTriggerStation::initialize()
{
	return registration();
}

void 
CTriggerStation::fire()
{
	static int 
		Tick = 0;

	Tick++;
	if (Tick >= 100000) Tick = 0;
	
	if ((Tick % (CLoopServer::get_loop_per_second()/2))) return;

	for(int i=0; i<length(); i++)
	{
		CTrigger
			*Trigger = (CTrigger*)get(i);
		Trigger->shoot();
	}
}

void
CTriggerStation::fire(const char *aTriggerName)
{
	for(int i=0; i<length(); i++)
	{
		CTrigger
			*Trigger = (CTrigger*)get(i);
		if( strcasecmp( aTriggerName, Trigger->name() ) == 0 )
		{
			Trigger->handler();
		}
	}
}
