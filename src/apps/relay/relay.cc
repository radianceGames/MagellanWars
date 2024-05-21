#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "debug.h"
#include "util.h"
#include "net.h"
#include "frame.h"
#include "runtime.h"
#include "database.h"
#include "script.h"

#include "relay.h"

CStoreCenter
	CRelay::mStoreCenter;
bool
	CRelay::mServerLoop = true;

CRelay::CRelay():CApplication(), CLoopServer(100)
{
	memset(&mKeyServerInfo, 0, sizeof(SKeyServerInfo));
	mBannerCenter = NULL;
}

CRelay::~CRelay()
{
	if (mBannerCenter) delete mBannerCenter;
	remove_all();
}

bool
CRelay::free_item(TSomething aItem)
{
	CSocket 
		*Socket = (CSocket*)aItem;

	system_log("type %x", Socket);

	if (Socket->type() == SOCKET_RELAY_CONNECTION)
	{
		CRelayConnection 
			*Connection = (CRelayConnection*)aItem;
		system_log("remove relay connection(%d)", 
				Connection->CSocket::get());
		delete Connection;
	} else {
		CWebConnection
			*Connection = (CWebConnection*)aItem;
		system_log("remove web connection(%d)", 
				Connection->CSocket::get());
		delete Connection;
	}
	return true;
}

CSocket* 
CRelay::new_connection(int aSocket)
{
	struct sockaddr_in 
		Address;
	int 
		Size;

	if (getsockname(aSocket, (struct sockaddr*)&Address, 
												(socklen_t*)&Size))
	{
		::close(aSocket);
		return NULL;
	}

	system_log("Address %04X", Address.sin_addr.s_addr);
	
	if (!mIPAllowList.has(Address.sin_addr.s_addr))	
	{
		::close(aSocket);
		return NULL;
	}

	CRelayConnection
		*Connection = new CRelayConnection(this);

	Connection->set(aSocket);

	return Connection;
}

int
CRelay::prepare()
{
	int 
		MaxFD = CLoopServer::prepare();

	int
		DBCon = mStoreCenter.CFIFO::get();

	if (DBCon > 0)
	{
		FD_SET(DBCon, &mWrites);
		FD_SET(DBCon, &mExceptions);

		mMaxDescription = (DBCon > MaxFD) ? DBCon : MaxFD;

	}
	if (mKeyServerInfo.pipe[0] > 0)
	{
		FD_SET(mKeyServerInfo.pipe[0], &mReads);
		FD_SET(mKeyServerInfo.pipe[0], &mExceptions);

		mMaxDescription = (mKeyServerInfo.pipe[0] > MaxFD) ? 
				mKeyServerInfo.pipe[0] : MaxFD;
	}

	return mMaxDescription;
}

void
CRelay::handle_exception()
{
	CLoopServer::handle_exception();

	int 
		DBCon = mStoreCenter.CFIFO::get();

	if (FD_ISSET(DBCon, &mExceptions))
	{
		system_log("On exception in database connection");
		kill(getpid(), SIGTERM);
	}

	if (FD_ISSET(mKeyServerInfo.pipe[0], &mExceptions))
	{
		system_log("On exception in key thread pipe");
		kill(getpid(), SIGTERM);
	}
}

void
CRelay::handle_output()
{
	CLoopServer::handle_output();

	int 
		DBCon = mStoreCenter.CFIFO::get();

	if (FD_ISSET(DBCon, &mWrites)) 
	{
		if (mStoreCenter.write())
		{
			system_log("Could not write database connection");
			kill(getpid(), SIGTERM);
		}
	}
}

void
CRelay::handle_input()
{
	CLoopServer::handle_input();

	if (FD_ISSET(mKeyServerInfo.pipe[0], &mReads))
		get_key(mKeyServerInfo.pipe[0]);
}

void 
CRelay::handle_message()
{
	for(int i=length()-1; i>=0; i--)
	{
		CSocket
			*Socket = (CSocket*)CList::get(i);
		if (Socket->type() == SOCKET_RELAY_CONNECTION)
		{
			CRelayConnection
				*RelayConnection = (CRelayConnection*)Socket;
			if (!RelayConnection->analysis())
			{
				system_log("message fault");
				remove(i);
				continue;
			}
		} else {
			CWebConnection
				*WebConnection = (CWebConnection*)Socket;
			if (!WebConnection->analysis())
			{
				system_log("message fault");
				remove(i);
				continue;
			}
		}
	}
}

void
CRelay::signal_stop(int aSignalNumber)
{
	system_log("server stop by signal(%d)", aSignalNumber);
	mServerLoop = false;
}

void
CRelay::set_signalset()
{
	sigemptyset(&mSignalSet);
//	sigaddset(&mSignalSet, SIGSEGV);
	sigaddset(&mSignalSet, SIGTERM);
}

void
CRelay::set_signalfunction()
{
	signal(SIGTERM, signal_stop);
	signal(SIGPIPE, signal_ignore);
	signal(SIGBUS, signal_exits);
	signal(SIGINT, signal_ignore);
	signal(SIGHUP, signal_exits);
	signal(SIGQUIT, signal_exits);
	signal(SIGILL, signal_exits);
	signal(SIGTRAP, signal_exits);
	signal(SIGIOT, signal_exits);
	signal(SIGFPE, signal_ignore);
	signal(SIGALRM, signal_ignore);
	signal(SIGUSR1, signal_ignore);
	signal(SIGUSR2, signal_ignore);
	signal(SIGTSTP, signal_ignore);
	signal(SIGTTIN, signal_ignore);
	signal(SIGTTOU, signal_ignore);
	signal(SIGIO, signal_exits);
	signal(SIGXCPU, signal_exits);
	signal(SIGVTALRM, signal_exits);
	signal(SIGWINCH, signal_ignore);
	signal(SIGURG, signal_ignore);
}

bool
CRelay::initialize(int Argc, char** Argv)
{
	bool 
		Res;

	Res = CApplication::initialize(Argc, Argv);

	if (!Res)
	{
		system_log("Could not initialize CApplication");
		return false;
	}

	char 
		*SystemLog = 
				mConfiguration.get_string("Server", 
									"SystemLogFile", "./systemlog");
	strcpy(SYSTEM_LOGFILE, SystemLog);

	system_log("start to initalize game server");

	if (!make_pid_file())
	{
		system_log("Could not make pid file.");
		return false;
	}

	

	if (!connect_database())
	{
		system_log("Could not make database connection.");
		return false;
	}

	if (!database_table())
	{
		system_log("Could not read database table.");
		return false;
	}

	if (!load_banner())
	{
		system_log("load banner data failed!");
		return false;
	}

	if (!setup_page_handler())
	{
		system_log("Could not setup page handler.");
		return false;
	}

	if (!script_table())
	{
		system_log("Could not load script table.");
		return false;
	}

	if (!set_ban_list())
	{
		system_log("Could not set IP ban list.");
		return false;
	}

	if (!set_allow_list())
	{
		system_log("Could not set IP allow list.");
		return false;
	}

	if (!set_key_thread())
	{
		system_log("Could not set key thread.");
		return false;
	}

	if (!open_server())
	{
		system_log("Could not open mother socket.");
		return false;
	}

	return Res;
}

bool
CRelay::make_pid_file()
{
	char
		*PIDFilename = 
				mConfiguration.get_string("Server", 
					"PIDLogfile", "./pidfile");

	if (!PIDFilename)
	{
		system_log("Could not get PIDLogfilename");
		return false;
	}

	system_log("Write PID file(%s)", PIDFilename);

	FILE
		*File;
	File = fopen(PIDFilename, "w");
	if (File)
	{
		pid_t 
			PID;
		PID = getpid();
		fprintf(File, "%d", PID);
		fclose(File);
	} else {
		system_log("Could not make a PID Logfile (%s)", PIDFilename);
		return false;
	}

	return true;
}

bool
CRelay::connect_database()
{
	char
		*DBConnection = mConfiguration.get_string(
				"Database", "Connection", NULL);

	if (!DBConnection)
	{
		system_log("Could not get DBConnection");
		return false;
	}
	system_log("Database connection %s", DBConnection);

	if (!mStoreCenter.initialize(DBConnection))
	{
		system_log("Could not initalize StoreCenter");
		return false;
	}

	return true;
}

bool
CRelay::database_table()
{
	CMySQL
		MySQL;

	char    
		*Host = mConfiguration.get_string("Database", "Host", NULL ), 
		*User = mConfiguration.get_string("Database", "User", NULL ), 
		*Password = mConfiguration.get_string( 
								"Database", "Password", NULL ), 
		*Database = mConfiguration.get_string(
								"Database", "Database", NULL ); 

	if( !Host || !User || !Password || !Database ) return false;

	system_log( "open mysql" );
	if( !MySQL.open(Host, User, Password, Database) ) return false;

	if (!mUserTable.load(MySQL))
		return false;
	system_log("load user #%d", mUserTable.length());

	if (!mAdminTable.load(MySQL))
		return false;
	system_log("load admin #%d", mAdminTable.length());

	MySQL.close();

	return true;
}

bool
CRelay::load_banner()
{
	mBannerCenter = new CBannerCenter(this);

	if (!mBannerCenter)
	{
		system_log("Could not allocate memory for BannerCenter");
		return false;
	}
	if (!mBannerCenter->initialize())
	{
		system_log("Could not initalize BannerCenter");
		return false;
	}
	system_log("load banner data completed!");
	return true;
}

bool
CRelay::setup_page_handler()
{
	char 
		*HTMLPath = mConfiguration.get_string("Web", "Path", NULL);

	if (!HTMLPath)
	{
		system_log("Could not get game web data directory");
		return false;
	}
	CRelayPageStation
		*PageStation = new CRelayPageStation();
	PageStation->initialize(HTMLPath);
	CConnection::set_page_station(PageStation);
	system_log("PageStation is initalized");

	return true;
}

bool
CRelay::script_table()
{
	char*
		RaceScript = mConfiguration.get_string(
								"Relay", "RaceScript", NULL);
	if (!RaceScript)
	{
		system_log("Could not found race script filename");
		return false;
	}

	CRaceScript
		RScript;

	if (!RScript.load(RaceScript))
	{
		system_log("Could not read race script filename:%s", RaceScript);
		return false;
	}

	if (!RScript.get(mRaceList))
	{
		system_log("Could not get informattion from race script");
		return false;
	}

	return true;
}

bool
CRelay::set_key_thread()
{
	char
		*KeyServer = mConfiguration.get_string("Key", "Server", NULL);
	if (!KeyServer)
	{
		system_log("Could not get info(KeyServer)");
		return false;
	}

	int 
		KeyServerPort = mConfiguration.get_integer("Key", "Port", -1);
	if (KeyServerPort < 0) 
	{
		system_log("Could not get info(KeyServerPort)");
		return false;
	}

	system_log("key server %s %d", KeyServer, KeyServerPort);

	strcpy(mKeyServerInfo.name, KeyServer);
	mKeyServerInfo.port = KeyServerPort;

	if (pipe(mKeyServerInfo.pipe))
	{
		system_log("Could not open pipe to connect key thread");
		return false;
	}

	int
		Result = pthread_create(&mKeyThread, NULL, 
				CRelay::key_thread, (void*)&mKeyServerInfo);

	if (Result != 0)
	{
		system_log("Could not start to get key thread");
		return false;
	}

	return true;
}

bool
CRelay::set_ban_list()
{
	char
		*IPBanFile = mConfiguration.get_string(
				"Relay", "IPBanFile", NULL);
	if (IPBanFile)
	{
		system_log("Load IP BanFile %s", IPBanFile);
		
		if (!mIPBanList.load(IPBanFile))
			system_log("Could not load IP Ban File");
	}

	return true;
}

bool
CRelay::set_allow_list()
{
	char
		*IPAllowFile = mConfiguration.get_string(
				"Relay", "IPAllowFile", NULL);
	if (IPAllowFile)
	{
		system_log("Load IP AllowFile %s", IPAllowFile);
		
		if (!mIPAllowList.load(IPAllowFile))
			system_log("Could not load IP Allow File");
	}

	return true;
}

bool
CRelay::open_server()
{
	int 
		Port = mConfiguration.get_integer("Server", "Port", 12345);

	system_log("try to open a relay server... port %d", Port);

	if (CServer::open(Port) < 0)
	{
		system_log("Could not open relay server system, port(%d)", 
				Port);	
		return false;
	} else 
		system_log("Relay open mother socket (port:%d)", Port);

	return true;
}


int 
CRelay::run()
{
	time_t 
		PushTime = time(0);

	while(mServerLoop)
	{
		prepare();
		get_remain_time();

		signal_masking();
		select();
		if (mServerLoop) wait_cycle();
		signal_unmasking();

		handle_exception();
		handle_input();
		handle_output();

		handle_message();

		accept();	
		// for time trigger

		mStoreCenter.send();

		if (time(0)-PushTime > 360)
		{
			PushTime = time(0);
			mStoreCenter.push();
		}
	}
	mStoreCenter.push();
	
	return 0;
}

int
CRelay::get_key(int aPIPE)
{
	CString
			Buffer;
	Buffer.set_size(MAX_KEY_RECEIVE_BUFFER);
	int
		NRead;
	NRead = ::read(aPIPE, Buffer.get_data(), 
			MAX_KEY_RECEIVE_BUFFER);
	Buffer.set_size(NRead);

	CQueryList 
		QueryList;

	QueryList.set(Buffer);

	char 
		*OldKey = QueryList.get_value("OLDKEY"),
		*CurKey = QueryList.get_value("CURKEY"),
		*FutKey = QueryList.get_value("FUTKEY");

	if (!OldKey || !CurKey || !FutKey)
	{
		system_log("Could not parsing a key string");
		return -1;
	}

	CCryptConnection::set_key(CurKey, OldKey, FutKey);

	system_log("CurrentKey %s, OldKey %s, FutureKey %s", 
				CurKey, OldKey, FutKey);
	return 0;
}

void*
CRelay::key_thread(void *aArg)
{
	SKeyServerInfo* 
		ServerInfo;

	ServerInfo = (SKeyServerInfo*)aArg;

	CKeyServer
		Client;

	while(mServerLoop)
	{
		if (Client.open(ServerInfo->name, ServerInfo->port) < 0)
		{
			system_log("thread: cannot connect to key server");
			pthread_exit(NULL);	
			return NULL;
		}

		Client.write();
		sleep(1);
		Client.read();

		Client.close();

//		system_log("send_to_server");
		Client.send_to_server(ServerInfo->pipe[1]);
		int 
			Count = 0;
		while (Count++ < 5*60 && mServerLoop) sleep(1); 
	}

	pthread_exit(NULL);
	return NULL;
}

void 
CRelay::wait_key_thread()
{
	int 
		Res;
	void*
		ThreadResult;

	Res = pthread_join(mKeyThread, &ThreadResult);

	if (Res)
	{
		system_log("Key thread join failed!");
	}
}

void
CRelay::set_web_connection(CRelayConnection &aConnection,
			CPacket* aPacket)
{
	int 
		Pos = CList::find(&aConnection);
	int 
		Socket = aConnection.CSocket::get();

	system_log("POS(%d), Socket(%d)", Pos, Socket);

	if (Pos < 0) return;

	// Do not close connection
	system_log("set socket = -1");
	aConnection.CSocket::set(-1);

	system_log("create a web connection");
	CWebConnection	
		*WebConnection = new CWebConnection(this);

	system_log("set socket to web connection");
	WebConnection->set(Socket);
	system_log("save a packet");
	WebConnection->mInput.append(aPacket);

	system_log("link the web connection");
	push(WebConnection);
	system_log("complete chage web connection");
}


CWebConnection*
CRelay::find_web_connection_by_socket_and_portal_id(
		int aSocket, int aPortalID)
{
	for(int i=0; i<length(); i++)
	{
		CSocket
			*Socket = (CSocket*)CList::get(i);
		if (Socket->type() == SOCKET_WEB_CONNECTION)
		{
			CWebConnection
				*Connection = (CWebConnection*)Socket;
			if ((Connection->get() == aSocket) && 
					(Connection->get_portal_id() == aPortalID))
				return Connection;
		}
	}
	return NULL;	
}

CRelayConnection*
CRelay::find_game_connection_by_server_id(int aServerID)
{
	for(int i=0; i<length(); i++)
	{
		CSocket
			*Socket = (CSocket*)CList::get(i);
		if (Socket->type() == SOCKET_RELAY_CONNECTION)
		{
			CRelayConnection
				*Connection = (CRelayConnection*)Socket;
			if (Connection->get_server_id() == aServerID)
				return Connection;
		}
	}
	return NULL;	
}
