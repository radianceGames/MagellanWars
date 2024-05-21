#include "../triggers.h"
#include "../archspace.h"

bool
CTriggerGetKey::handler()
{
	system_log("get key trigger");

	ARCHSPACE->get_key();

	return true;
}
