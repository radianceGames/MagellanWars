#if !defined(__ARCHSPACE_H__)
#define __ARCHSPACE_H__

//#include "bsdport.h"
#include "triggers.h"
#include "database.h"
#include "key.h"
#include "game.h"

class CBannerCenter;

extern TZone gArchspaceTriggerStationZone;

#define GAME_SERVER_SERIES 0x0800

#define SERVER_SHUTDOWN_DELAY 10	// in secs

/**
*/
class CArchspacePageStation: public CPageStation
{
	public:
		CArchspacePageStation():CPageStation() {}
		virtual ~CArchspacePageStation() {}

	protected:
		virtual bool registration();
};

/**
*/
class CArchspaceTriggerStation: public CTriggerStation
{
    public:
		CArchspaceTriggerStation():CTriggerStation() {}
		virtual ~CArchspaceTriggerStation() {}
	protected:
		virtual bool registration();
		virtual const char *debug_info() const { return "archspace trigger station"; }

	RECYCLE(gArchspaceTriggerStationZone);
};

/**
*/
class CArchspaceStoreCenter: public CStoreCenter
{
	public:
		CArchspaceStoreCenter():CStoreCenter() {}
		virtual ~CArchspaceStoreCenter() {}

		bool store(CStore &aStore);
		bool query (const char *aTable, const char *aQuery);

		static void *write(void *aArg);
		static void lock_sql();
		static void unlock_sql();
		static void spawn_database_sql_thread();
		static void kill_database_sql_thread();

    protected:
        static pth_mutex_t mSQLMutex;
        static pth_t mSQLThread;
};


extern TZone gArchspaceConnectionZone;
/**
*/
class CArchspaceConnection: public CCryptConnection
{
	public:
		CArchspaceConnection(CApplication *aApplication);
		virtual ~CArchspaceConnection();

		void message_page(const char *aMessage);
		void portal_login_message_page(const char *aMessage);
		void page(const char *aMessage);


		int get_portal_id() { return mPortalID; }
		int get_game_id() { return mGameID; }

		CQueryList& cookies() { return mCookies; }
		CQueryList& id_string() { return mIDString; }
		CQueryList& as_string() { return mASString; }
	protected:
		CString &decrypt_id_string(const char *aString);

		virtual bool make_page();

		CQueryList
			mCookies;
		CQueryList
			mIDString;
		CQueryList
			mASString;

		int
			mPortalID;
		int
			mGameID;

	RECYCLE(gArchspaceConnectionZone);
};

/**
  아크 스페이스 어플리케이션 클레스
*/
class CArchspace: public CApplication, public CCGIServer
{
	public:
		CArchspace();
		virtual ~CArchspace();

		bool initialize(int Argc, char** Argv);
		bool set_server_id();
		bool make_pid_file();
		bool load_banner(char *aBannerPath, char *aTopBanner, char *aBottomBanner);
		bool setup_page_and_trigger();
		bool connect_database();
		bool initialize_game();
		bool set_key_server();
		bool open_server();

		int run();

		void save_database();

		bool get_key();

		void as_accept( );
		void spawn_accept_thread();
		void kill_accept_thread();

	protected:
		virtual bool free_item(TSomething aItem);
		virtual CSocket* new_connection(int aSocket);
		virtual int prepare();
		virtual void handle_exception();
		virtual void handle_output();

		static void signal_stop(int aSignalNumber);

		virtual void set_signalset();
		virtual void set_signalfunction();

		static bool
			mServerLoop;
		static time_t
			mServerShutdownTime;

		CTriggerStation
			*mTriggerStation;

		SKeyServerInfo
			mKeyServerInfo;

		pth_t mAcceptThread;

	public:
		CBannerCenter *banner_center() const { return mBannerCenter; }
        	CArchspaceStoreCenter *store_center() const { return mStoreCenter; }
		CGame *game() const { return mGame; }
		inline static bool get_server_loop() { return mServerLoop; }
		static bool shutdown_server();

	protected:
		CBannerCenter *
			mBannerCenter;
		CArchspaceStoreCenter *
			mStoreCenter;
		CGame *
			mGame;
};

#endif
