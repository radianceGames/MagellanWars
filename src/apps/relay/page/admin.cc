#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "../pages.h"
#include "../relay.h"

bool
CPageAdmin::handle(CConnection &aConnection)
{
	system_log("start handler %s", get_name());

	CWebConnection
		*Connection = (CWebConnection*)&aConnection;

	int 
		PortalID = Connection->get_portal_id();

	CRelay
		*Relay = (CRelay*)Connection->application();

	const char 
		*Authority = Relay->get_admin_table()->has(PortalID);

	if (!Authority)
	{
		message_page(aConnection, "This page can aceess admin");
		return false;
	}

	return admin_handle(aConnection, Authority);
}
