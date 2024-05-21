#if !defined(__ARCHSPACE_RELAY_DATA_H__)
#define __ARCHSPACE_RELAY_DATA_H__

#include "util.h"
#include "store.h"

class CGameServer
{
	public:
		CGameServer();
		virtual ~CGameServer();

		void initialize(int aID, const char* aName, int aPort, 
				int aPopulation, int aLimitPopulation);

		int get_id() { return mID; }
		const char* get_name() { return (char*)mName; }
		int get_port() { return mPort; }
		int get_population() { return mPopulation; }
		int get_limit_population() { return mLimitPopulation; }
	protected:
		int 
			mID; 
		CString 
			mName;
		int 
			mPort;
		int 
			mPopulation;
		int 
			mLimitPopulation;
};

class CGameServerList: public CSortedList
{
	public:
		CGameServerList();
		virtual ~CGameServerList();

		bool remove(int aGameServerID);
		int get_available_server();
	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2, 
													int aMode) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey,
													int aMode) const;
};

class CAdmin: public CStore
{
	public:
		CAdmin();
		CAdmin(MYSQL_ROW aRow);
		virtual ~CAdmin();

		virtual const char *table() { return "Admin"; }
		virtual CString& query();

		bool initialize(int aPortalID, 
							char *aName, char *aAuthority);

		int get_portal_id() { return mPortalID; }
		const char *get_name() { return (char*)mName; }
		const char *get_authority() { return (char*)mAuthority; }
	protected:
		int 
			mPortalID;

		CString
			mName,
			mAuthority;
};

class CAdminTable: public CSortedList
{
	public:
		CAdminTable();
		virtual ~CAdminTable();

		bool load(CMySQL &aMySQL);

		const char *has(int aPortalID);
	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2,
							int aMode) const;
		virtual int compare_key(TSomething aItem, 
							TConstSomething aKey, int aMode) const;
};

class CRace
{
	public:
		CRace();
		virtual ~CRace();

		bool initialize(const char *aName, 
							int aID, const char *aDescription);

		const char *get_name() { return (char*)mName; }
		int get_id() { return mID; }
		const char *get_description() { return (char*)mDescription; }
	protected:
		CString 
			mName;
		int 
			mID;
		CString
			mDescription;
};

class CRaceList: public CSortedList
{
	public:
		CRaceList(): CSortedList(10, 10) {}
		virtual ~CRaceList() { remove_all(); }

	protected:
		inline virtual bool free_item(TSomething aItem);

		inline virtual int compare(TSomething aItem1, 
					TSomething aItem2, int aMode) const;
		inline virtual int compare_key(TSomething aItem, 
					TConstSomething aKey, int aMode) const;
};

inline bool 
CRaceList::free_item(TSomething aItem)
{
	CRace 
		*Race = (CRace*)aItem;	

	if (!Race) return false;
	delete Race;
	return true;
}

inline int
CRaceList::compare(TSomething aItem1, TSomething aItem2, 
				int aMode) const
{
	(void)aMode;
	CRace 
		*Race1 = (CRace*)aItem1,
		*Race2 = (CRace*)aItem2;	

	if (Race1->get_id() > Race2->get_id()) return 1;
	if (Race1->get_id() < Race2->get_id()) return -1;
	return 0;	
}

inline int
CRaceList::compare_key(TSomething aItem, TConstSomething aKey,
				int aMode) const
{
	(void)aMode;
	CRace 
		*Race = (CRace*)aItem;

	if (Race->get_id() > (int)aKey) return 1;
	if (Race->get_id() < (int)aKey) return -1;
	return 0;	
}

class CUser: public CStore
{
	public:
		CUser();
		CUser(MYSQL_ROW aRow);
		virtual ~CUser();
	
		virtual const char* table() { return "User"; }

		virtual CString& query();

		int get_game_id() { return mGameID; }
		int get_portal_id() { return mPortalID; }
		int get_server() { return mServer; }

		const char *get_name() { return (char*)mName; }
		int get_race() { return mRace; }

		time_t get_create_time() { return mCreateTime; }
		time_t get_last_login() { return mLastLogin; }

		bool create(int aGameID, int aPortalID, const char *aName, 
						int aRace, int aServer, time_t aCreateTime);
	protected:
		int 
			mGameID,
			mPortalID,
			mServer;
		CString 
			mName;
		int
			mRace;
		time_t
			mCreateTime,
			mLastLogin;

		friend CStoreCenter& operator << (CStoreCenter &aStoreCenter,
										CUser &aUser);
};

#define MAX_HASHLIST 256

class CUserTable: public CList
{
	class CPortalIndex: public CSortedList
	{
		public:
			CPortalIndex();
			virtual ~CPortalIndex();

		protected:
			virtual bool free_item(TSomething aItem)
			{
				(void)aItem;
				return true;
			}
			virtual int compare(TSomething aItem1, TSomething aItem2,
									int aMode) const;
			virtual int compare_key(TSomething aItem, 
						TConstSomething aKey, int aMode) const;
	};

	class CNameIndex: public CSortedList
	{
		public:
			CNameIndex();
			virtual ~CNameIndex();

		protected:
			virtual bool free_item(TSomething aItem)
			{
				(void)aItem;
				return true;
			}
			virtual int compare(TSomething aItem1, TSomething aItem2,
									int aMode) const;
			virtual int compare_key(TSomething aItem, 
						TConstSomething aKey, int aMode) const;
	};

	class CUnusedIDList: public CSortedList
	{
		public:
			CUnusedIDList(): CSortedList(100, 100) {}
			virtual ~CUnusedIDList() {}

		protected:
			virtual bool free_item(TSomething aItem)
			{
				(void)aItem;
				return true;
			}

			virtual int compare(TSomething aItem1, TSomething aItem2,
									int aMode) const
			{
				(void)aMode;
				if ((int)aItem1 > (int)aItem2) return 1;
				if ((int)aItem1 < (int)aItem2) return -1;
				return 0;
			}

			virtual int compare_key(TSomething aItem, 
							TConstSomething aKey, int aMode) const
			{
				(void)aMode;
				if ((int)aItem > (int)aKey) return 1;
				if ((int)aItem < (int)aKey) return -1;
				return 0;
			}
	};

	public:
		CUserTable();
		virtual ~CUserTable();

		bool load(CMySQL &aMySQL);

		CUserTable& operator+=(CUser& aUser);
		CUserTable& operator+=(CUser* aUser);
		CUserTable& operator-=(CUser& aUser);
		CUserTable& operator-=(CUser* aUser);
		CUserTable& operator-=(int aGameID);

		int get_unused_game_id();
		int get_max_game_id() { return mMaxGameID; }

		CUser* get_by_portal_id(int aPortalID);
		CUser* get_by_name(const char *aName);
	protected:
		bool add(CUser& aUser);
		bool remove(int aGameID);
		virtual bool free_item(TSomething aItem);

		CPortalIndex
			mPortalIndex[MAX_HASHLIST];
		CNameIndex
			mNameIndex[MAX_HASHLIST];

		CUnusedIDList
			mUnusedIDList;
		int
			mMaxGameID;
};

class CPlayer: public CStore
{
	public:
		CPlayer();
		virtual ~CPlayer();

		virtual const char* table() { return "Player"; }

		virtual CString& query();
	protected:
		int 
			mID;
		CString	
			mName;
		int 
			mServer;
};

class CPlayerTable: protected CList
{
	public:
		CPlayerTable();
		virtual ~CPlayerTable();

		int add(CPlayer *aPlayer);
		bool remove(int aID);

	protected:
		virtual bool free_item(TSomething aItem);
};

#endif
