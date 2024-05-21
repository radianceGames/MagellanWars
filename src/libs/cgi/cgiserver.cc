#include "cgi.h"

CCGIServer::CCGIServer(int aLoopPerSec, int aMaxConnection):
	CLoopServer(aLoopPerSec, aMaxConnection)
{
}

CCGIServer::~CCGIServer()
{
	remove_all();

	CPageStation 
		*PS = (CPageStation*)CConnection::get_page_station();

	if (PS)
	{
		delete PS;
		CConnection::set_page_station(NULL);
	}
}

CSocket*
CCGIServer::new_connection(int aSocket)
{
	CConnection 
		*Connection = new CConnection();

	Connection->set(aSocket);

	return Connection;
}

bool
CCGIServer::free_item(TSomething aItem)
{
	CConnection
		*Connection = (CConnection*)aItem;
	if (!Connection) 
		return false;

	delete Connection;

	return true;
}

void
CCGIServer::handle_message()
{
	for(int i=length()-1; i>=0; i--)
	{
		CConnection
			*Socket = (CConnection*)CList::get(i);
		if (!Socket->analysis())
		{
			system_log("message fault");
			remove(i);
			continue;
		}
	}
}

