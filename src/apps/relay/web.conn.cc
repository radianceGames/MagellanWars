#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "relay.h"
#include "protocol.h"

CWebConnection::CWebConnection(CApplication *aApplication)
		:CCryptConnection()
{
	mApplication = aApplication;
	mStatus = 0;
	mLastTime = 0;
	mDecrypt = false;
	mAcknowlegde = NULL;
}

CWebConnection::~CWebConnection()
{
	if (mAcknowlegde) 
		delete mAcknowlegde;

	close();
}

bool decrypt_success(const char *aString, int aLength)
{
	const char
		Ascii[] = "1234567890=&%;,.+-_"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";

	for(int i=0; i<aLength; i++)
		if (!strchr(Ascii, aString[i]))
			return false;
	return true;
}

CString&
CWebConnection::decrypt_id_string(const char *aString)
{
	static CString 
		Buffer;

	int 
		Length = (strlen(aString)-16) / 2;

	decrypt(CRYPT_CURRENT_KEY, aString, Buffer);
	if (!decrypt_success((char*)Buffer, Length)) 
	{
		Buffer.clear();
		decrypt(CRYPT_OLD_KEY, aString, Buffer);
		if (!decrypt_success((char*)Buffer, Length))
		{
			Buffer.clear();
			decrypt(CRYPT_FUTURE_KEY, aString, Buffer);
			if (!decrypt_success((char*)Buffer, Length))
			{
				Buffer.clear();
			}
		} 
	}
	return Buffer;	
}

void
CWebConnection::message_page(const char *aMessage)
{
	CQueryList
		Conversion;

	Conversion.set_value("MESSAGE", aMessage);

	CString
		Output;
	CPage::get_html_station()->get_html(Output,
			"sys/message.html", &Conversion);
	set_content(Output);
	send_terminate();
}

bool
CWebConnection::make_page()
{
	CRelay
		*Relay = (CRelay*)mApplication;

	if (Relay->get_ip_ban_list()->has(inet_addr(get_connection())))
	{
		system_log("Ban IP %s", get_connection());
		message_page("Your IP is not allowed");
		return true;
	}

	system_log("make page");
	

	if (!mStatus)
	{
		mCookies.set(mCookie, ';');

		// get info from Cookie
		char
			*IDString;

		IDString = mCookies.get_value("ID_STRING");
		system_log("cookie %s", IDString);
	

		if (IDString)
		{
			CString
				String;

			String = decrypt_id_string(IDString);
			system_log("descrypt");

			if (String.length())
			{
				system_log("Success decrypt ID_STRING(%s)", 
						(char*)String);
			
				mIDStrings.set(String, '&');
		
			// get info from ID_STRING
				mPortalID = atoi(mIDStrings.get_value("ID"));

				mDecrypt = true;

				CString
					*Page = mPageStation->get_page(*this);
				
				if (Page && Page->length() > 0)
				{
					CString 
						TString;

					set_content(*Page);

					// encrypt ID_STRING
					String = mIDStrings.get('&');
					system_log("new ID_STRING = %s", (char*)String);
					encrypt(CRYPT_CURRENT_KEY, (char*)String, TString);
					mCookies.replace_value("ID_STRING", 
													(char*)TString);

					// binding cookie infos
					TString = mCookies.get(';');
					set_cookie(TString);

					system_log("Set-Cookie:%s", (char*)TString);

					send_terminate();
				} else {
					mLastTime = time(0);
					mStatus++;
				}
				return true;
			} else {
				system_log("Could not decrypt ID_STRING");
				message_page("인증 시간 초과");
				return true;
			}
		} else {
			system_log("Could not found ID_STRING");
			message_page("포탈에 먼저 로긴하십시요");
			return true;
		}
	} else {
		if (!mDecrypt) 
		{
			system_log("Could not decrypt ID_STRING");
			message_page("인증 시간 초과");
			return true;
		}

		CString
			*Page = mPageStation->get_page(*this);
				
		if (Page && Page->length() > 0)
		{
			CString 
				TString,
				String;

			set_content(*Page);
			// encrypt ID_STRING
			String = mIDStrings.get('&');
			system_log("new ID_STRING = %s", (char*)String);
			encrypt(CRYPT_CURRENT_KEY, (char*)String, TString);
			mCookies.replace_value("ID_STRING", 
									(char*)TString);

			// binding cookie infos
			TString = mCookies.get(';');
			set_cookie(TString);

			send_terminate();
		} else {
			mLastTime = time(0);
			mStatus++;
		}
	}
	return true;
}

bool
CWebConnection::analysis()
{
	if (mLastTime)
	{
		if (time(0)-mLastTime > INTER_PROTOCOL_TIMEOUT)
		{
			mLastTime = 0;
			timeout();

			return true;
		}
	}

	CMessage
		Message;

	while(!mPackets.empty())
	{
		CPacket
			*Packet;

		Packet = (CPacket*)mPackets.first();

		if (!Message.set_packet(Packet->get(), MESSAGE_RECEIVE))
		{
			system_log("message analysis error");
			return false;
		}

		if (Message.type() == MT_CREATE_NEW_PLAYER_ACK)
			receive_create_new_player(Message);
		
		mPackets.remove(Packet);
	}

	return CConnection::analysis();	
}

void 
CWebConnection::timeout()
{
	system_log("timeout connection %s", get_uri());
	message_page("처리할 수 있는 시간을 넘어섰습니다.");
}

bool
CWebConnection::receive_create_new_player(CMessage& aMessage)
{
	int 
		Dummy;
	if (!aMessage.get_item(Dummy)) return false;

	int 
		PortalID;

	if (!aMessage.get_item(PortalID)) return false;

	char 
		*TName;
	int 
		TNameLen;
	if (!aMessage.get_item(TName, TNameLen, true)) return false;
	CString
		Name;
	Name.append(TName, TNameLen);

	int 
		Race;
	if (!aMessage.get_item(Race)) return false;

	int 
		GameID;
	if (!aMessage.get_item(GameID)) return false;

	int 
		ServerID = aMessage.server_ID() & 0xFF;

	CCreateNewPlayerAck
		*Ack = new CCreateNewPlayerAck();

	Ack->initialize(PortalID, (char*)Name, GameID, Race, ServerID);

	acknowlegde(Ack);

	system_log("Ack PortalID :%d, Name:%s, Server %d", 
			PortalID, (char*)Name, ServerID);

	make_page();

	return true;
}
