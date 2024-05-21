#include "../triggers.h"
#include "../archspace.h"

void
CCronTabDatabasePush::handler()
{
//	system_log("run database update");

	STORE_CENTER->write(STORE_CENTER);
}
