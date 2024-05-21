#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "../pages.h"
#include "../relay.h"

bool
CPageLogin::handle(CConnection &aConnection)
{
	system_log("start handler %s", get_name());
	CQueryList
		Conversion = get_conversion(aConnection);

	CWebConnection
		*Connection = (CWebConnection*)&aConnection;

	int 
		PortalID = Connection->get_portal_id();

	CRelay
		*Relay = (CRelay*)Connection->application();

	CUserTable *
		UserTable = Relay->get_user_table();

	CUser *
		User = UserTable->get_by_portal_id(PortalID);

	if (!User)
	{
		message_page(aConnection,
				GETTEXT("<A HREF=\"/create.as\">"
					"Move to create a charactor</A>"));
		return false;
	}

	CGameServerList*
		ServerList = Relay->get_game_server_list();
	int 
		ServerID = User->get_server();
	int 
		Pos = ServerList->find_sorted_key((TSomething)ServerID);
	if (Pos < 0)
	{
		message_page(aConnection, 
				"Your game server is not available.");
		return false;
	}
	CGameServer 
		*GameServer = (CGameServer*)ServerList->get(Pos);

	system_log("login charactor");

	Conversion.set_value("PORTALID", (char*)format("%d", PortalID));
	Conversion.set_value("GAMEID", 
			(char*)format("%d", User->get_game_id()));
	Conversion.set_value("NAME", User->get_name());
	Conversion.set_value("SERVER", 
			(char*)format("%d.%s:%d", ServerID,
					GameServer->get_name(), 
					GameServer->get_port()));

	Connection->cookies().set_value("ARCHSPACE_HOST", 
				GameServer->get_name());
	Connection->cookies().set_value("ARCHSPACE_PORT", 
				(char*)format("%d", GameServer->get_port()));

	Connection->id_strings().set_value("ARCHSPACE_ID", 
				(char*)format("%d", User->get_game_id()));

	CAdminTable 
		*AdminTable = Relay->get_admin_table();

	const char
		*authority = AdminTable->has(PortalID);

	if (authority)
		Connection->id_strings().set_value("ADMIN", authority); 

	if (!output("login.html", Conversion))
		return false;
	
	return true;
}
