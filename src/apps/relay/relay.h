#if !defined(__ARCHSPACE_H__)
#define __ARCHSPACE_H__

#include "net.h"
#include "frame.h"
#include "cgi.h"
#include "runtime.h"
#include "store.h"
#include "database.h"
#include "key.h"
#include "pages.h"
#include "banner.h"

#include "data.h"

#include "protocol.h"

#include <pthread.h>

#define WEB_SERVER_SERIES 0x0400

#define SOCKET_RELAY_CONNECTION			0x0201
#define SOCKET_WEB_CONNECTION			0x0202

class CRelay;

class CRelayPageStation: public CPageStation
{
	public:
		CRelayPageStation():CPageStation() {}
		virtual ~CRelayPageStation() {}

	protected:
		virtual bool registration();
};

class CRelayDatabase: public CDatabase
{
	public:
		CRelayDatabase(): CDatabase() {}
		virtual ~CRelayDatabase() {}

		bool initialize(int aArgc, char **aArgv);
};

class CWebConnection;

class CRelayConnection: public CSocket
{
	friend class CRelay;
	public:
		CRelayConnection(CApplication *aApplication);
		virtual ~CRelayConnection();

		virtual int read();
		virtual int write();

		virtual bool analysis();

		CApplication* application() 
				{ return (CApplication*)mApplication; }

		int get_server_id() { return mServerID; }
	protected:
		virtual int type() { return SOCKET_RELAY_CONNECTION; }
		CApplication
			*mApplication;
		int 
			mServerID;

	public:
		void send_create_new_player(CWebConnection& aConnection,
				int aPortalID, const char *aName, int aRace, 
				int aGameID);
	protected:
		void receive_game_server_info(CMessage &aMessage);
		void receive_commit_suicide(CMessage &aMessage);
};

class CWebConnection: public CCryptConnection
{
	friend class CRelay;
	public:
		CWebConnection(CApplication *aApplication);
		virtual ~CWebConnection();

		CApplication* application() 
			{ return (CApplication*)mApplication; }

		void message_page(const char *aMessage);
		int get_portal_id() { return mPortalID; }
		int get_status() { return mStatus; }
		void inc_status() { mStatus++; }

		void add_packet(CPacket* aPacket) { mPackets.append(aPacket); }

		CAcknowlegde* acknowlegde() { return mAcknowlegde; }

		virtual bool analysis();

		CQueryList& cookies() { return mCookies; }
		CQueryList& id_strings() { return mIDStrings; }
	protected:
		virtual int type() { return SOCKET_WEB_CONNECTION; }

		CString& decrypt_id_string(const char *aString);

		virtual bool make_page();

		void timeout();

		int 
			mPortalID;

		CQueryList
			mCookies,
			mIDStrings;
		CApplication 
			*mApplication;
		int 
			mStatus;
		time_t 
			mLastTime;
		bool
			mDecrypt;

		CCollection
			mPackets;

		CAcknowlegde*
			mAcknowlegde;
	protected:
		void acknowlegde(CAcknowlegde* aAcknowlegde)
		{
			if (mAcknowlegde) delete mAcknowlegde;
			mAcknowlegde = aAcknowlegde;
		}
		bool receive_create_new_player(CMessage& aMessage);	
};

class CRelay: public CApplication, public CLoopServer
{
	public:
		CRelay();
		virtual ~CRelay();

		bool initialize(int Argc, char** Argv);
		int run();

		static CStoreCenter
			mStoreCenter;

		void wait_key_thread();

		void set_web_connection(CRelayConnection &aConnection,
						CPacket* aPacket);
		CWebConnection* find_web_connection_by_socket_and_portal_id(
											int aSocket, int aPortalID);
		CRelayConnection* find_game_connection_by_server_id(
												int aServerID);


		CRaceList *get_race_list() { return &mRaceList; }
		CUserTable *get_user_table() { return &mUserTable; }

		CIPList* get_ip_ban_list() { return &mIPBanList; }
		CIPList* get_ip_allow_list() { return &mIPAllowList; }
		CAdminTable* get_admin_table() { return &mAdminTable; }
		CGameServerList* get_game_server_list() 
				{ return &mGameServerList; }
		CBannerCenter* get_banner_center() { return mBannerCenter; }
	protected:
		bool make_pid_file();
		bool connect_database();
		bool script_table();
		bool database_table();
		bool load_banner();
		bool open_server();
		bool setup_page_handler();
		bool set_key_thread();
		bool set_ban_list();
		bool set_allow_list();

		virtual bool free_item(TSomething aItem);
		virtual CSocket* new_connection(int aSocket);
		virtual int prepare();
		virtual void handle_exception();
		virtual void handle_output();
		virtual void handle_input();
		virtual void handle_message();

		static void signal_stop(int aSignalNumber);

		virtual void set_signalset();
		virtual void set_signalfunction();

		int get_key(int aPIPE);
		pthread_t 
			mKeyThread;
		static void* key_thread(void *aArg);
		SKeyServerInfo
			mKeyServerInfo;

		static bool 
			mServerLoop;

	protected:
		CBannerCenter
			*mBannerCenter;
		CUserTable
			mUserTable;
		CRaceList
			mRaceList;
		CIPList
			mIPBanList;
		CIPList
			mIPAllowList;
		CAdminTable
			mAdminTable;
		CGameServerList
			mGameServerList;
};

#endif
