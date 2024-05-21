#include "net.h"
#include <cstdlib>
#include <netinet/in.h>

CServer::CServer(int aMaxConnection):CList(256, 256)
{
	FD_ZERO(&mReads);
	FD_ZERO(&mWrites);
	FD_ZERO(&mExceptions);

	mMaxDescription = 0;
	mMaxConnection = aMaxConnection;
}

CServer::~CServer()
{
	remove_all();
	shutdown(mSocket, 2);
	system_log("close mother socket %d", mSocket);
	close();
}

bool
CServer::free_item(TSomething aItem)
{
	CSocket
		*Connection = (CSocket*)aItem;

	delete Connection;
	return true;
}

int
CServer::prepare()
{
	int 
		Max,
		Socket;

	CSocket
		*Connection;

	Max = 0;

	FD_ZERO(&mReads);
	FD_ZERO(&mWrites);
	FD_ZERO(&mExceptions);

	FD_SET(mSocket, &mReads);

	for(int i=0; i<mCount; i++)
	{
		Connection = (CSocket*)CList::get(i);	
		Socket = Connection->get();
		FD_SET(Socket, &mReads);
		FD_SET(Socket, &mWrites);
		FD_SET(Socket, &mExceptions);
		if (Socket > Max) Max = Socket;
	}

	if (mSocket > Max) Max = mSocket;
	mMaxDescription = Max;

	return Max;
}

void
CServer::select()
{	
	static struct timeval 
		NullTime = { 0, 0 };

	if (::pth_select(mMaxDescription+1, &mReads, &mWrites, &mExceptions, 
			&NullTime) < 0)
	{
		exit(0);
	}
}

int 
CServer::accept()
{
	int 
		Size,
		Socket;

	struct sockaddr_in
		SocketAddress;

	if (!FD_ISSET(mSocket, &mReads)) return 0; 

//	system_log("Start accept");

	Size = sizeof(SocketAddress);

	getsockname(mSocket, (struct sockaddr*)&SocketAddress, 
							(socklen_t*)&Size);
	if ((Socket = ::pth_accept(mSocket, (struct sockaddr*)&SocketAddress, (socklen_t*)&Size)) < 0)
//	if ((Socket = ::accept(mSocket, (struct sockaddr*)&SocketAddress, (socklen_t*)&Size)) < 0)
		return -1;

	nonblock(Socket);

	if (mCount >= mMaxConnection-1) 
	{
		::close(Socket);
		return -1;
	}

//	system_log("Accept new connection(%d)", Socket);
	
	CSocket 
		*Connection = new_connection(Socket);
	
	if (Connection) 
	{
		push(Connection);
	
		return Socket;
	}

	return -1;
}

int 
CServer::open(int aPort)
{
	struct sockaddr_in 
		SocketAddress;
	struct linger
		Linger;
	int
		Opt = 0;
	char
		*Option;

	memset(&SocketAddress, 0, sizeof(struct sockaddr_in));

	mSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (mSocket < 0)
	{
		mSocket = -1;
		system_log("could not create mother socket");
		return -1;
	}

	if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&Option, 
			sizeof(Option)) < 0)
	{
		mSocket = -1;
		system_log("could not set REUSEADDR socket option");
		return -1;
	}

	if (setsockopt(mSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&Opt, 
			sizeof(Opt)) < 0)
	{
		mSocket = -1;
		system_log("could not set KEEPALIVE socket option");
		return -1;
	}

	Linger.l_onoff = 0;
	Linger.l_linger = 0;
	if (setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char*)&Linger,
			sizeof(Linger)) < 0)
	{
		mSocket = -1;
		system_log("could not set LINGER socket option");
		return -1;
	}

	SocketAddress.sin_family= AF_INET;
	SocketAddress.sin_port = htons(aPort);
	int 
		Count = 0;

	while(1)
	{
		if (bind(mSocket, (struct sockaddr *)&SocketAddress, 
				sizeof(SocketAddress)) < 0)
		{
			system_log("could not bind mother socket(%d),"
						" wait 20 second more", aPort);
			if (Count++ > 10)
			{
				pth_nap((pth_time_t){20, 0});
				close();
				return -1;
			}
		} else break;
		pth_nap((pth_time_t){20, 0});
	}

	nonblock();
	listen(mSocket, 100);

	mMaxDescription = mSocket;

	return mSocket;
}
