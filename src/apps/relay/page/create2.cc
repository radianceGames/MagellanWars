#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "../pages.h"
#include "../relay.h"

bool
CPageCreate2::handle(CConnection &aConnection)
{
	system_log("start handler %s", get_name());

	CWebConnection
		*Connection = (CWebConnection*)&aConnection;

	if (Connection->get_status() == 0)	
	{
		CQueryList
			Post;
	
		Post.set(aConnection.get_query(), '&');

		const char *
			Name = Post.get_value("NAME");
		const char *
			Race = Post.get_value("RACE");

		if (!Name || !strlen(Name))
		{
			message_page(aConnection, "Could not found player name");
			return false;
		}

		if (!Race)
		{
			message_page(aConnection, "Could not found player race");
			return false;
		} 


		int 
			PortalID = Connection->get_portal_id();

		CRelay
			*Relay = (CRelay*)Connection->application();

		CUserTable *
			UserTable = Relay->get_user_table();

		CUser 
			*User = UserTable->get_by_name(Name);

		if (User)
		{
			message_page(aConnection,
					(char*)format("This name is already exists", 
							User->get_name(), User->get_game_id()));
			return false;
		}

		User = UserTable->get_by_portal_id(PortalID);

		if (User)
		{
			message_page(aConnection,
				(char*)format("You already have your player(%s:#%d)", 
						User->get_name(), User->get_game_id()));
			return false;
		}

		CGameServerList*
			ServerList = Relay->get_game_server_list();
		int
			ServerID = ServerList->get_available_server();
		if (ServerID < 0)
		{
			message_page(aConnection,
					"Could get available game server.");
			return false;
		}

		CRaceList
			*RaceList = Relay->get_race_list();

		int 
			Index;
		Index =	RaceList->find_sorted_key((TSomething)atoi(Race));

		if (Index < 0)
		{
			system_log("could not found race type (%s)", Race);
			message_page(aConnection,
					(char*)format("unknown race type(%s)", Race));
			return false;
		}

		CRace
			*R = (CRace*)RaceList->get(Index);
		int 
			GameID = UserTable->get_unused_game_id();

		CRelayConnection* 
			Con = Relay->find_game_connection_by_server_id(ServerID);

		if (!Con)
		{
			system_log("could not access game server %d", ServerID);
			message_page(aConnection,
				(char*)format("not available server %d", ServerID));
			return false;
		}

		Con->send_create_new_player(*Connection, 
					PortalID, Name, R->get_id(), GameID);

		Connection->inc_status();	
	} else {
		if (!Connection->acknowlegde()) 
		{
			system_log("could not understand a acknowlegde");
			message_page(aConnection, "Don't understand a acknowlegde");
			return false;
		}
		if (Connection->acknowlegde()->type() 
					!= MT_CREATE_NEW_PLAYER_ACK)
		{
			system_log("East Question, West Answer.");
			message_page(aConnection, "East Question, West Answer.");
			return false;

		}
	
		CCreateNewPlayerAck
			*Ack = (CCreateNewPlayerAck*)Connection->acknowlegde();

		CRelay
			*Relay = (CRelay*)Connection->application();

		CRaceList
			*RaceList = Relay->get_race_list();

		int 
			Index;
		Index =	RaceList->find_sorted_key((TSomething)Ack->get_race());

		if (Index < 0)
		{
			system_log("could not found race type (%d)", 
											Ack->get_race());
			message_page(aConnection,
					(char*)format("unknown race type(%d)", 
					Ack->get_race()));
			return false;
		}

		CRace
			*Race = (CRace*)RaceList->get(Index);


		CQueryList
			Conversion = get_conversion(aConnection);

		Conversion.set_value("NAME", Ack->get_name());
		Conversion.set_value("RACE", Race->get_name());
		Conversion.set_value("PORTALID", 
				format("%d", Ack->get_portal_id()));
		Conversion.set_value("GAMEID", 
				format("%d", Ack->get_game_id()));
		Conversion.set_value("SERVERID", 
				format("%d", Ack->get_server()));



		CUserTable *
			UserTable = Relay->get_user_table();

		CUser
			*User;
		User = new CUser();
		User->create(Ack->get_game_id(), 
				Ack->get_portal_id(),
				Ack->get_name(),
				Ack->get_race(),
				Ack->get_server(),
				time(0));
		User->type(QUERY_INSERT);

		system_log("create new charactor %s", Ack->get_name());

		*UserTable += *User;

		CRelay::mStoreCenter << *User;

		if (!output("create2.html", Conversion))
			return false;
	}	
	return true;
}
