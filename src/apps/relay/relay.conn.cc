#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "relay.h"
#include "protocol.h"

CRelayConnection::CRelayConnection(CApplication* aApplication):
		CSocket()
{
	mApplication = aApplication;
	mServerID = -1;
}

CRelayConnection::~CRelayConnection()
{
	if (mServerID >= 0)
	{
		CRelay
			*Relay = (CRelay*)mApplication;
		Relay->get_game_server_list()->remove(mServerID);
	}
	close();
}

int 
CRelayConnection::read()
{
	CPacket
		*Packet = new CPacket();
	if (!Packet) return -1;
	int
		Count;
	if ((Count = read_packet(Packet)) > 0)
	{
		CMessage
			Message;

		Message.set_packet(Packet->get(), MESSAGE_RECEIVE);
		system_log("read packet %04X", Message.server_ID());
		if ((Message.server_ID() & WEB_SERVER_SERIES)) 
		{
			system_log("change web connection");
			CRelay
				*Relay = (CRelay*)mApplication;
			Relay->set_web_connection(*this, Packet);
			system_log("change web in RelayConnect");
			return -1; // for remove connection
		} else mInput.append(Packet);
	} else {
		delete Packet;
		return -1;
	}
	return Count;
}

int 
CRelayConnection::write()
{
	CPacket
		*Packet;
	if ((Packet = (CPacket*)mOutput.first()))
	{
		int
			Result = write_packet(Packet);
		if (Result == -1)
			return -1;
		if (Result != -2)
			mOutput.remove(Packet);
	}
	return 0;
}

bool 
CRelayConnection::analysis()
{
	CMessage
		Message;

	while(!mInput.empty())
	{
		CPacket
			*Packet;

		Packet = (CPacket*)mInput.first();
		
		if (!Message.set_packet(Packet->get(), MESSAGE_RECEIVE))
		{
			system_log("message analysis error");
			return false;
		}

		system_log("inter communication message %04X", Message.type());
		
		int 
			Type = Message.type();
		bool 
			Even = ((Type % 2) == 0) ? true:false; 

		if (((Type & 0xF000) == 0xA000) && Even)
		{
			int 
				Socket,
				PortalID;
			CWebConnection
				*Connection;

			if (!Message.get_item(Socket)) 
				Socket = -1;
			if (!Message.get_item(PortalID)) 
				PortalID = -1;

			CRelay 
				*Relay = (CRelay*)mApplication;
			Connection = 
				Relay->find_web_connection_by_socket_and_portal_id(
							Socket, PortalID);
			
			if (Connection) 
			{
				Connection->add_packet(Packet);
				mInput.remove_without_free(Packet);
			} else mInput.remove(Packet);
		} else if (((Type & 0xF000) == 0xB000) && !Even)
		{
			switch(Type)
			{
				case MT_COMMIT_SUICIDE_REQ:
					receive_commit_suicide(Message);
					break;
			}
			mInput.remove(Packet);
		} else if ((Type & 0xF000) == 0xC000)
		{
			switch(Type)
			{
				case MT_GAME_SERVER_INFO_REQ:
					receive_game_server_info(Message);
					break;
			}
			mInput.remove(Packet);
		} else {
			system_log("known type packet");
			mInput.remove(Packet);
		}
	}

	return true;
}

void
CRelayConnection::receive_game_server_info(CMessage &aMessage)
{
	system_log("game server info");
	int 
		ServerID;
	aMessage.get_item(ServerID);
	system_log("\t Server ID:%d", ServerID);
	int 
		Population;	
	aMessage.get_item(Population);
	system_log("\t Population:%d", 
			Population);
	int 
		LimitPopulation;	
	aMessage.get_item(LimitPopulation);
	system_log("\t LimitPopulation:%d", 
			LimitPopulation);

	char*
		Buffer;
	int 
		Length;
	aMessage.get_item(Buffer, Length, true);
	CString
		Host;
	Host.append(Buffer, Length);

	int 
		Port;
	aMessage.get_item(Port);

	system_log("\t Game Server Name %s:%d", (char*)Host, Port);


	CPacket 
		*Packet = new CPacket;

	CMessage
		Message;

	Message.set_packet(Packet->get(), MESSAGE_SEND);
	Message.type(MT_GAME_SERVER_INFO_ACK);
	system_log("send GAME_SERVER_INFO_ACK");
	mOutput.append(Packet);

	CRelay 
		*Relay = (CRelay*)application();

	CGameServer
		*GameServer = new CGameServer();

	GameServer->initialize(ServerID, (char*)Host, Port, 
				Population, LimitPopulation);

	Relay->get_game_server_list()->insert_sorted(GameServer);

	mServerID = ServerID;
}

void
CRelayConnection::send_create_new_player(CWebConnection& aConnection,
		int aPortalID, const char *aName, int aRace, int aGameID)
{
	CPacket 
		*Packet = new CPacket();

	CMessage
		Message;

	Message.set_packet(Packet->get(), MESSAGE_SEND);
	Message.type(MT_CREATE_NEW_PLAYER_REQ);
	Message.set_item(aConnection.get());		
	Message.set_item(aPortalID);
	Message.set_item(aName);
	Message.set_item(aRace);
	Message.set_item(aGameID);

	system_log("send_create_new_player "
				"PortalID %d, Name %s, Race: %d, GameID:%d", 
				aPortalID, aName, aRace, aGameID);

	mOutput.append(Packet);
}

void
CRelayConnection::receive_commit_suicide(CMessage &aMessage)
{
	system_log("commit suicide");

	int
		Socket,
		PortalID;

	if (!aMessage.get_item(Socket)) 
	{
		system_log("Could not read a socket from message");
		return;
	}

	if (!aMessage.get_item(PortalID)) 
	{
		system_log("Could not read a portal id from message");
		return;
	}

	system_log("delete portal ID %d", PortalID);

	CPacket 
		*Packet = new CPacket;

	CMessage
		Message;

	Message.set_packet(Packet->get(), MESSAGE_SEND);
	Message.type(MT_COMMIT_SUICIDE_ACK);
	Message.set_item(Socket);
	Message.set_item(PortalID);
	system_log("send COMMIT_SUICIDE_ACK");
	mOutput.append(Packet);
}
