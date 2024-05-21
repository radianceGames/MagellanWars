#include "cgi.h"

CPageStation*
	CConnection::mPageStation = NULL;

CConnection::CConnection():
		CSocket()
{
	mURI = "";
	mMethod = "";
	mReferer = "";
	mCookie = "";
	mEncoding = "";
	mLanguage = "";
	mAgent = "";
	mHostName = "";
	mConnection = "";

	mQuery = "";
}

CConnection::~CConnection()
{
	close();
}

int 
CConnection::read()
{
	CPacket 
		*Packet = new CPacket();
	if (!Packet) return -1;

	int 
		Count;

	if ((Count = read_packet(Packet)) > 0)
	{
		mInput.append(Packet);	
	} else {
		delete Packet;
		return -1;
	}

	return Count;
}

int 
CConnection::write()
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
		{
			mOutput.remove(Packet);
		}
	}
	return 0;
}

bool
CConnection::get_uri(CMessage& aMessage)
{
	if (aMessage.type() != MT_URL_SEND)
		return false;

	char 
		*URI;
	int 
		Length;
	unsigned char
		Count;

	if (!aMessage.get_item(Count))
	{
		system_log("could not get block count");
		return false;
	}

	if (!aMessage.get_item(URI, Length, true))
	{
		system_log("could not get string item");
		return false;
	}

	mURI.append(URI, Length);

//	system_log("MT_URL_SEND block:%d (%s:%d)", 
//			Count, mURI.get_data(), Length);

	return true;
}

bool
CConnection::get_method(CMessage& aMessage)
{
	if (aMessage.type() != MT_METHOD_SEND)
		return false;

	char 
		*Method;
	int 
		Length;
	unsigned char
		Count;

	if (!aMessage.get_item(Count))
	{
		system_log("could not get block count");
		return false;
	}

	if (!aMessage.get_item(Method, Length, true))
	{
		system_log("could not get string item");
		return false;
	}

	mMethod.append(Method, Length);

//	system_log("MT_METHOD_SEND block:%d (%s:%d)", 
//			Count, (char*)mMethod, Length);

	return true;
}



bool
CConnection::get_referer(CMessage &aMessage)
{
	if (aMessage.type() != MT_REFERER_SEND)
		return false;

	char 
		*Referer;
	int 
		Length;
	unsigned char
		Count;

	if (!aMessage.get_item(Count))
	{
		system_log("could not get block count");
		return false;
	}

	if (!aMessage.get_item(Referer, Length, true))
	{
		system_log("could not get string item");
		return false;
	}

	mReferer.append(Referer, Length);

//	system_log("MT_REFERER_SEND block:%d (%s:%d)", 
//			Count, mReferer.get_data(), Length);

	return true;
}

bool
CConnection::get_cookie(CMessage &aMessage)
{
	if (aMessage.type() != MT_COOKIE_SEND)
		return false;

	char 
		*Cookie;
	int 
		Length;
	unsigned char
		Count;

	if (!aMessage.get_item(Count))
	{
		system_log("could not get block count");
		return false;
	}
	if (!aMessage.get_item(Cookie, Length, true))
	{
		system_log("could not get string item");
		return false;
	}

	mCookie.append(Cookie, Length);

//	system_log("MT_COOKIE_SEND block:%d (%s:%d)", 
//			Count, mCookie.get_data(), Length);

	return true;
}

bool
CConnection::get_encoding(CMessage &aMessage)
{
	if (aMessage.type() != MT_ACCEPT_ENCODING_SEND)
		return false;

	char 
		*Encoding;
	int 
		Length;
	unsigned char
		Count;

	if (!aMessage.get_item(Count))
	{
		system_log("could not get block count");
		return false;
	}
	if (!aMessage.get_item(Encoding, Length, true))
	{
		system_log("could not get string item");
		return false;
	}

	mEncoding.append(Encoding, Length);

//	system_log("MT_ACCEPT_ENCODING_SEND block:%d (%s:%d)", 
//			Count, mEncoding.get_data(), Length);

	return true;
}

bool
CConnection::get_language(CMessage &aMessage)
{
	if (aMessage.type() != MT_ACCEPT_LANGUAGE_SEND)
		return false;

	char 
		*Language;
	int 
		Length;
	unsigned char
		Count;

	if (!aMessage.get_item(Count))
	{
		system_log("could not get block count");
		return false;
	}
	if (!aMessage.get_item(Language, Length, true))
	{
		system_log("could not get string item");
		return false;
	}

	mLanguage.append(Language, Length);

//	system_log("MT_ACCEPT_LANGUAGE_SEND block:%d (%s:%d)", 
//			Count, mLanguage.get_data(), Length);

	return true;
}

bool
CConnection::get_agent(CMessage &aMessage)
{
	if (aMessage.type() != MT_USER_AGENT_SEND)
		return false;

	char 
		*Agent;
	int 
		Length;
	unsigned char
		Count;

	if (!aMessage.get_item(Count))
	{
		system_log("could not get block count");
		return false;
	}
	if (!aMessage.get_item(Agent, Length, true))
	{
		system_log("could not get string item");
		return false;
	}

	mAgent.append(Agent, Length);

//	system_log("MT_USER_AGENT_SEND block:%d (%s:%d)", 
//			Count, mAgent.get_data(), Length);

	return true;
}

bool
CConnection::get_host_name(CMessage &aMessage)
{
	if (aMessage.type() != MT_HOST_NAME_SEND) return false;

	char *
		HostName;
	int
		Length;
	unsigned char
		Count;

	if (!aMessage.get_item(Count))
	{
		system_log("could not get block count");
		return false;
	}
	if (!aMessage.get_item(HostName, Length, true))
	{
		system_log("could not get string item");
		return false;
	}

	mHostName.append(HostName, Length);

	return true;
}

bool
CConnection::get_connection(CMessage &aMessage)
{
	if (aMessage.type() != MT_CONNECTION_INFO_SEND)
		return false;

	char 
		*Connection;
	int 
		Length;
	unsigned char
		Count;

	if (!aMessage.get_item(Count))
	{
		system_log("could not get block count");
		return false;
	}
	if (!aMessage.get_item(Connection, Length, true))
	{
		system_log("could not get string item");
		return false;
	}

	mConnection.append(Connection, Length);

//	system_log("MT_CONNECTION_INFO_SEND block:%d (%s:%d)", 
//			Count, mConnection.get_data(), Length);

	return true;
}


bool
CConnection::get_query(CMessage &aMessage)
{
	if (aMessage.type() != MT_QUERY_SEND)
		return false;

	char 
		*Query;
	int 
		Length;

	unsigned char 
		Count;
		
	if (!aMessage.get_item(Count))
	{
		system_log("could not get block count");
		return false;
	}

	if (!aMessage.get_item(Query, Length, true))
	{
		system_log("could not get string item");
			
		return false;
	}
	mQuery.append(Query, Length);

//	system_log("MT_QUERY_SEND %d (%s)", Count, mQuery.get_data());

	return true;
}

bool
CConnection::get_page_request(CMessage &aMessage)
{
	if (aMessage.type() != MT_GET_PAGE_REQUEST)
		return false;

	if (!make_page()) return false;

	return true;
}

bool
CConnection::make_page()
{
	static time_t t = time(0);
	static int count = 0;

	if( t <= time(0)-60 ){
		system_log( "page handler call %d during %d seconds", count, time(0)-t );
		count = 0;
		t = time(0);
	}

	count++;

	// get user info
	CString*
		Page = mPageStation->get_page(*this);

	set_content(*Page);	

	send_terminate();

	return true;
}

bool
CConnection::send_string_packet(int aType, CString &aString)
{
//	system_log("send_string_packet %s", aString.get_data());

	if (!aString.length()) return false;

	int 
		Done = 0,
		Length = aString.length();

	CMessage
		Message;
	char 
		Part[STRING_DATA_BLOCK_SIZE+1];
	unsigned char
		Count = 0;

//	system_log("string size : %d", Length);

	while(Done < Length)
	{
		int 
			Size = ((Length - Done) > STRING_DATA_BLOCK_SIZE) ?
					STRING_DATA_BLOCK_SIZE : Length-Done;
//		system_log("send string %d block size: %d", Count, Size);
		CPacket 
			*Packet = new CPacket();

		Message.set_packet(Packet->get(), MESSAGE_SEND);
		Message.type(aType);
		memcpy(Part, aString.get_data()+Done, Size);
		Part[Size] = '\0';
		if (!Message.set_item(Count++)) return false;
		if (!Message.set_item(Part)) return false;

//		system_log("send string packet part %s", Part);
//		system_log("send string packet size %d", Packet->count());
		mOutput.append(Packet);

		Done += Size;
	}

	return true;
}

void 
CConnection::set_cookie(CString &aCookie)
{
	send_string_packet(MT_SET_COOKIE_SEND, aCookie);
}

void
CConnection::set_content(CString &aPage)
{
	send_string_packet(MT_CONTENT_SEND, aPage);
}

void
CConnection::send_terminate()
{
	CPacket 
		*Packet = new CPacket();

	CMessage
		Message;

	Message.set_packet(Packet->get(), MESSAGE_SEND);
	Message.type(MT_TERMINATE_REQUEST);

//	system_log("terminate request");
	mOutput.append(Packet);
}

bool
CConnection::analysis()
{
	CMessage
		Message;

	while(!mInput.is_empty())
	{
		CPacket
			*Packet;

		Packet = (CPacket*)mInput.first();
		
		if (!Message.set_packet(Packet->get(), MESSAGE_RECEIVE))
			return false;

//		system_log("message type:%04X, size:%d", Message.type(), 
//							Message.get_packet_size());
		switch(Message.type())
		{
			case MT_URL_SEND:
				if (!get_uri(Message)) return false;
				break;
			case MT_METHOD_SEND:
				if (!get_method(Message)) return false;
				break;
			case MT_REFERER_SEND:
				if (!get_referer(Message)) return false;
				break;
			case MT_COOKIE_SEND:
				if (!get_cookie(Message)) return false;
				break;
			case MT_ACCEPT_ENCODING_SEND:
				if (!get_encoding(Message)) return false;
				break;
			case MT_ACCEPT_LANGUAGE_SEND:
				if (!get_language(Message)) return false;
				break;
			case MT_USER_AGENT_SEND:
				if (!get_agent(Message)) return false;
				break;
			case MT_HOST_NAME_SEND:
				if (!get_host_name(Message)) return false;
				break;
			case MT_CONNECTION_INFO_SEND:
				if (!get_connection(Message)) return false;
				break;
			case MT_QUERY_SEND:
				if (!get_query(Message)) return false;
				break;
			case MT_GET_PAGE_REQUEST:
				if (!get_page_request(Message)) return false;
				break;
			default:
				return false;
		}
		mInput.remove(Packet);
	}

	return true;
}
