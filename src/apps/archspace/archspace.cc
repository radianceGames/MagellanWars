#include <cerrno>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "archspace.h"
#include "banner.h"
#include "council.h"
#include <cstdlib>

#define _LINUX_THREADS

bool CArchspace::mServerLoop = true;
time_t CArchspace::mServerShutdownTime = 0;

CArchspace::CArchspace():CApplication(), CCGIServer(10)
{
	memset(&mKeyServerInfo, 0, sizeof(SKeyServerInfo));
	mBannerCenter = NULL;
	mTriggerStation = NULL;
	mStoreCenter = NULL;
	mGame = NULL;
}

CArchspace::~CArchspace()
{
	if (mGame)
		delete mGame;

	if (mTriggerStation)
		delete mTriggerStation;

	if (mBannerCenter)
		delete mBannerCenter;

	if (mStoreCenter)
		delete mStoreCenter;
}

bool
CArchspace::free_item(TSomething aItem)
{
	CArchspaceConnection
		*Connection = (CArchspaceConnection*)aItem;

	if (!Connection) return false;
	delete Connection;

	return true;
}

CSocket*
CArchspace::new_connection(int aSocket)
{
	CArchspaceConnection
		*Connection = new CArchspaceConnection(this);

	Connection->set(aSocket);

	return Connection;
}

int
CArchspace::prepare()
{
	int
		MaxFD = CCGIServer::prepare();

	if (mKeyServerInfo.pipe[0] > 0)
	{
		FD_SET(mKeyServerInfo.pipe[0], &mReads);
		FD_SET(mKeyServerInfo.pipe[0], &mExceptions);

		mMaxDescription = (mKeyServerInfo.pipe[0] > MaxFD) ?
				(MaxFD = mKeyServerInfo.pipe[0]) : MaxFD;
	}

	return mMaxDescription;
}

void
CArchspace::handle_exception()
{
	CCGIServer::handle_exception();
}

void
CArchspace::handle_output()
{
	CCGIServer::handle_output();
}

void
CArchspace::signal_stop(int aSignalNumber)
{
	SLOG("server stop by signal(%d %d)", getpid(), aSignalNumber);

	mServerLoop = false;
}

void
CArchspace::set_signalset()
{
	sigemptyset(&mSignalSet);
	sigaddset(&mSignalSet, SIGSEGV);
	sigaddset(&mSignalSet, SIGTERM);
}

void
CArchspace::set_signalfunction()
{
	signal(SIGHUP, signal_exits);
	signal(SIGINT, signal_ignore);
	signal(SIGQUIT, signal_exits);
	signal(SIGILL, signal_exits);
	signal(SIGTRAP, signal_exits);

//	signal(SIGFPE, signal_exits);

//	signal(SIGBUS, signal_exits);
//	signal(SIGSEGV, signal_stop);

	signal(SIGPIPE, signal_ignore);
	signal(SIGTERM, signal_stop);
	signal(SIGIOT, signal_exits);
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
CArchspace::shutdown_server()
{
	mServerShutdownTime = time(NULL) + SERVER_SHUTDOWN_DELAY;

	return true;
}

bool
CArchspace::initialize(int Argc, char** Argv)
{
	gApplication = this;

	bool
		Res;

	Res = CApplication::initialize(Argc, Argv);

	if (!Res)
	{
		SLOG("ERROR : Could not initialize CApplication");
		return false;
	}

	char
		*SystemLog =
				mConfiguration.get_string("Server",
									"SystemLogFile", "./systemlog");
	strcpy(SYSTEM_LOGFILE, SystemLog);

	SLOG("start to initialize game server");

	char
		EnvString[32];

	char *
		Language = mConfiguration.get_string("Game", "Language", NULL);
	if (Language == NULL)
	{
		SLOG("ERROR : No language information specified!");
		return false;
	}
	else
	{
		if (!strcmp(Language, "en"))
		{
			CGame::mLanguage = CGame::LANG_EN;
			sprintf( EnvString, "LANG=%s", Language);
			putenv( EnvString );
		}
		else if (!strcmp(Language, "ko"))
		{
			CGame::mLanguage = CGame::LANG_KO;
			sprintf( EnvString, "LANG=%s", Language);
			putenv( EnvString );
		}
	}

	if (!set_server_id())
	{
		SLOG("ERROR : Could not set server id");
		return false;
	}

	if (!make_pid_file())
	{
		SLOG("ERROR : Could not make pid file");
		return false;
	}

	char *
		BannerPath = mConfiguration.get_string("Banner",
												"BannerPath",
												"/etc/archspace/banner");
	char
		*TopBanner = mConfiguration.get_string("Banner",
												"TopBanner",
												"NO"),
		*BottomBanner = mConfiguration.get_string("Banner",
												"BottomBanner",
												"NO");
	if (!load_banner(BannerPath, TopBanner, BottomBanner))
	{
		SLOG("ERROR : Couldn't load banner code!");
		return false;
	}

	if (!connect_database())
	{
		SLOG("ERROR : Could not connect database");
		return false;
	}

	if (!setup_page_and_trigger())
	{
		SLOG("ERROR : Could not setup pages and time triggers handler");
		return false;
	}

	if (!initialize_game())
	{
		SLOG("ERROR : Error game initialize");
		return false;
	}

/*	if (!set_key_server())
	{
		SLOG("ERROR : Could not set key server");
		return false;
	}*/

	if (!open_server())
	{
		SLOG("ERROR : Could not open server");
		return false;
	}

	return Res;
}

bool
CArchspace::set_server_id()
{
	int
		ServerID = 1;
//			mConfiguration.get_integer(
//				"Server", "GameServerID", -1);
	if (ServerID < 0)
	{
		SLOG("ERROR : Could not found GameServerID in configuration file");
		return false;
	}
	CMessage::host_server_ID((int)(GAME_SERVER_SERIES + ServerID));

	SLOG("set server id = %04X", GAME_SERVER_SERIES + ServerID);

	return true;
}

bool
CArchspace::make_pid_file()
{
	char
		*PIDFilename =
				mConfiguration.get_string("Server",
					"PIDLogfile", "./pidfile");

	if (!PIDFilename)
	{
		SLOG("ERROR : Could not get PIDLogfilename");
		return false;
	}

	FILE
		*File;
	File = fopen(PIDFilename, "w");
	if (File)
	{
		pid_t
			PID;
		PID = getpid();
		fprintf(File, "%d", (int)PID);
		fclose(File);
	}
	else
	{
		SLOG("ERROR : Could not make a PID Logfile (%s)", PIDFilename);
		return false;
	}

	SLOG("Write PID file(%s)", PIDFilename);

	return true;
}

bool
CArchspace::load_banner(char *aBannerPath, char *aTopBanner, char *aBottomBanner)
{
	mBannerCenter = new CBannerCenter();

	if (!mBannerCenter)
	{
		SLOG("ERROR : Could not allocate BannerCenter");
		return false;
	}

	if (!mBannerCenter->load(aBannerPath, aTopBanner, aBottomBanner))
	{
		SLOG("ERROR : Could not initialize BannerCenter");
		return false;
	}

	SLOG("load banner data completed!");

	return true;
}

bool
CArchspace::setup_page_and_trigger()
{
	char
		*HTMLPath = mConfiguration.get_string("Game", "WebPath", NULL);

	if (!HTMLPath)
	{
		SLOG("ERROR : Could not found web file path");
		return false;
	}

	CArchspacePageStation
		*PageStation = new CArchspacePageStation();
	PageStation->initialize(HTMLPath);
	CConnection::set_page_station(PageStation);

	SLOG("PageStation is initalized");


	CArchspaceTriggerStation
		*ArchspaceTriggerStation = new CArchspaceTriggerStation();
	if (!ArchspaceTriggerStation->initialize())
	{
		SLOG("ERROR : Could not initialize runtime triggers");
		return false;
	}
	mTriggerStation = (CTriggerStation*)ArchspaceTriggerStation;

	SLOG("TriggerStation is initialized");

	return true;
}

bool
CArchspace::connect_database()
{
	CMySQL *DBConnection = new CMySQL();
  	char
		*Host = mConfiguration.get_string("Database", "Host", NULL),
		*User = mConfiguration.get_string("Database", "User", NULL),
		*Password = mConfiguration.get_string("Database", "Password", NULL),
		*Database = mConfiguration.get_string("Database", "Database", NULL);

	if (!DBConnection->open(Host, User, Password, Database))
	{
		SLOG("ERROR : Could not create DBConnection");
		return false;
	}
	SLOG("DBConnection Created!");
//	SLOG("Try to connect %s", DBConnection);

	mStoreCenter = new CArchspaceStoreCenter();
	if (!mStoreCenter)
	{
		SLOG("ERROR : Could not allocate StoreCenter");
		return false;
	}

	if (!mStoreCenter->initialize(DBConnection))
	{
		SLOG("ERROR : Could not initalize StoreCenter");
		return false;
	}

	SLOG("StoreCenter created!");

	CGameStatus::load(DBConnection);
	SLOG("CGameStatus: %d Down_Time: %d CALC: %d", CGameStatus::mLastGameTime, CGameStatus::get_down_time(), CGame::mServerStartTime - CGameStatus::mLastGameTime);

	return true;
}

bool
CArchspace::initialize_game()
{
	mGame = new CGame();

	if (!mGame)
	{
		SLOG("ERROR : Could not allocate game class");
		return false;
	}

	return mGame->initialize(&mConfiguration);
}

bool
CArchspace::set_key_server()
{
	char
		*KeyServer = mConfiguration.get_string("Key", "Server", NULL);
	if (!KeyServer)
	{
		SLOG("ERROR : Could not get info(KeyServer)");
		return false;
	}

	int
		KeyServerPort = mConfiguration.get_integer("Key", "Port", -1);
	if (KeyServerPort < 0)
	{
		SLOG("ERROR : Could not get info(KeyServerPort)");
		return false;
	}

	strcpy(mKeyServerInfo.name, KeyServer);
	mKeyServerInfo.port = KeyServerPort;

	SLOG("Connect to key server %s %d", KeyServer, KeyServerPort);

	return get_key();
}

bool
CArchspace::open_server()
{
	int
		Port = mConfiguration.get_integer("Server", "Port", 12345);

	SLOG("try to open a game server... port %d", Port);

	if (CServer::open(Port) < 0)
	{
		SLOG("ERROR : Could not open archspace server system, port(%d)", Port);
		return false;
	}

	SLOG("Archspace open mother socket (port:%d)", Port);

	return true;
}

extern TZone gInnerString32KZone;
extern TZone gInnerString64KZone;
extern TZone gInnerString128KZone;
extern TZone gInnerString256KZone;
extern TZone gInnerString512KZone;
extern TZone gInnerString2MZone;


int
CArchspace::run()
{
	CArchspaceStoreCenter::spawn_database_sql_thread();

	CGame::spawn_update_thread();
	spawn_accept_thread();

//	CArchspaceStoreCenter::spawn_database_packet_thread();

/*#ifdef MEMORY_DEBUG
	FILE *File = fopen("32Kstring.log", "w");
	if (File)
	{
		TChunk *Chunk;
		Chunk = (TChunk*)gInnerString32KZone.recycle;
		while(Chunk)
		{
			char *Data = ((char*)Chunk)+sizeof(TChunk);
			fprintf(File, "[START]\n%s\n[END]\n", Data);
			Chunk = Chunk->next;
		}
		fclose(File);
	}

	File = fopen("64Kstring.log", "w");
	if (File)
	{
		TChunk *Chunk;
		Chunk = (TChunk*)gInnerString64KZone.recycle;
		while(Chunk)
		{
			char *Data = ((char*)Chunk)+sizeof(TChunk);
			fprintf(File, "[START]\n%s\n[END]\n", Data);
			Chunk = Chunk->next;
		}
		fclose(File);
	}

	File = fopen("128Kstring.log", "w");
	if (File)
	{
		TChunk *Chunk;
		Chunk = (TChunk*)gInnerString128KZone.recycle;
		while(Chunk)
		{
			char *Data = ((char*)Chunk)+sizeof(TChunk);
			fprintf(File, "[START]\n%s\n[END]\n", Data);
			Chunk = Chunk->next;
		}
		fclose(File);
	}

	File = fopen("256Kstring.log", "w");
	if (File)
	{
		TChunk *Chunk;
		Chunk = (TChunk*)gInnerString256KZone.recycle;
		while(Chunk)
		{
			char *Data = ((char*)Chunk)+sizeof(TChunk);
			fprintf(File, "[START]\n%s\n[END]\n", Data);
			Chunk = Chunk->next;
		}
		fclose(File);
	}

	File = fopen("512Kstring.log", "w");
	if (File)
	{
		TChunk *Chunk;
		Chunk = (TChunk*)gInnerString512KZone.recycle;
		while(Chunk)
		{
			char *Data = ((char*)Chunk)+sizeof(TChunk);
			fprintf(File, "[START]\n%s\n[END]\n", Data);
			Chunk = Chunk->next;
		}
		fclose(File);
	}

	File = fopen("2Mstring.log", "w");
	if (File)
	{
		TChunk *Chunk;
		Chunk = (TChunk*)gInnerString2MZone.recycle;
		while(Chunk)
		{
			char *Data = ((char*)Chunk)+sizeof(TChunk);
			fprintf(File, "[START]\n%s\n[END]\n", Data);
			Chunk = Chunk->next;
		}
		fclose(File);
	}
#endif
*/
	mTriggerStation->fire("debug");

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

		CGame::lock();
		handle_message();
		CGame::unlock();

		CGame::lock();
		if (mTriggerStation)
			mTriggerStation->fire();
		CGame::unlock();

		if (mServerShutdownTime == time(NULL)) mServerLoop = false;
	}

	CGame::kill_update_thread();
	kill_accept_thread();
	CArchspaceStoreCenter::kill_database_sql_thread();

	SLOG("START TO PREVENT ROLL BACK");

	int Ret = 0;

	int
		CouncilCount = 0,
		PlayerCount = 0;

	for(int i=0; i<COUNCIL_TABLE->length(); i++)
	{
		CCouncil *Council = (CCouncil*)COUNCIL_TABLE->get(i);
		Council->update_by_tick();
		CouncilCount++;
	}

	for (int i=0 ; i<PLAYER_TABLE->length() ; i++)
	{
		CPlayer *
			Player = (CPlayer *)PLAYER_TABLE->get(i);
		if (Player->get_game_id() == EMPIRE_GAME_ID) continue;
		if (Player->is_changed() == false) continue;
		Player->store();
		PlayerCount++;
	}

	while(Ret == 0)
	{
		Ret = STORE_CENTER->CStoreCenter::write();
	}
	SLOG("END TO PREVENT ROLL BACK : %d Council, %d Player", CouncilCount, PlayerCount );

	return 0;
}

bool
CArchspace::get_key()
{
	CKeyServer Client;

	if (Client.open(mKeyServerInfo.name, mKeyServerInfo.port) < 0)
	{
		SLOG("ERROR : Cannot connect to key server");
		return false;
	}

	Client.write();
	pth_nap((pth_time_t){0, 500});
	Client.read();
	Client.close();

	if (!strlen(Client.get_buffer())) return false;

	CQueryList
		QueryList;

	QueryList.set(Client.get_buffer());

	char
		*OldKey = QueryList.get_value("OLDKEY"),
		*CurKey = QueryList.get_value("CURKEY"),
		*FutKey = QueryList.get_value("FUTKEY");

	if (!OldKey || !CurKey || !FutKey)
	{
		SLOG("ERROR : Could not parse a key string");
		return -1;
	}

	CCryptConnection::set_key(CurKey, OldKey, FutKey);

	SLOG("CurrentKey %s, OldKey %s, FutureKey %s",
				CurKey, OldKey, FutKey);
	return true;
}

void
CArchspace::as_accept()
{
	fd_set
		NewConnection;
	static struct timeval
		NullTime = { 0, 0 };
	int
		Size,
		Socket;
	struct sockaddr_in
		SocketAddress;

	while( mServerLoop ){
		FD_ZERO( &NewConnection );
		FD_SET( mSocket, &NewConnection );

		if( ::pth_select(mSocket+1, &NewConnection, NULL, NULL, &NullTime) < 0 ){
			exit(0);
		}

		if( !FD_ISSET(mSocket, &NewConnection) ){
			pth_nap((pth_time_t){0, 500});
			continue;
		}

		Size = sizeof(SocketAddress);
		getsockname(mSocket, (struct sockaddr*)&SocketAddress, (socklen_t*)&Size);
		if( (Socket = ::pth_accept(mSocket, (struct sockaddr*)&SocketAddress, (socklen_t*)&Size)) < 0 ){
			continue;
		}

		nonblock(Socket);

		if( mCount >= mMaxConnection-1 ){
			::close(Socket);
			continue;
		}

		CSocket
			*Connection = new_connection(Socket);
		push(Connection);
	}
}

void *
as_accept_wrapper( void *aArg )
{
	((CArchspace*)gApplication)->as_accept();

	return NULL;
}

void
CArchspace::spawn_accept_thread()
{
	mAcceptThread = pth_spawn(PTH_ATTR_DEFAULT, as_accept_wrapper, NULL);

	if (mAcceptThread == NULL)
	{
		SLOG("ERROR : Could not spawn accept thread");
	}
	else
	{
		SLOG("Spawn Accept Thread");
	}
}

void
CArchspace::kill_accept_thread()
{
	pth_cancel( mAcceptThread );
}

