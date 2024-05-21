#if !defined(__ARCHSPACE_UNIVERSE_H__)
#define __ARCHSPACE_UNIVERSE_H__

#include "store.h"

class CPlayerList;
class CCouncilList;
class CSortedPlanetList;
class CEventInstanceList;
class CPlanet;
class CCouncil;
class CPlayer;

//-------------------------------------------------------- CCluster

extern TZone gClusterZone;
/**
*/
class CCluster: public CStore
{
	private:
		int mID;
		CString	mName;
		CString	mNick;
		bool mAcceptNewPlayer;
		int	mWeight;

		CPlayerList *mPlayerList;
		CCouncilList *mCouncilList;
		CSortedPlanetList *mPlanetList;
		CEventInstanceList *mEventList;

	public:
		CCluster();
		/* For Empire */
		CCluster(int aClusterID);
		CCluster(MYSQL_ROW aRow);
		virtual ~CCluster();

		virtual const char *table() { return "cluster"; }

		virtual CString& query();

	public: // get
		inline int get_id();
		inline const char* get_name();
		char *get_nick() { return (char *)mNick; }

		CSortedPlanetList *get_planet_list() { return mPlanetList; }

		CPlanet *find_planet(int aPlanetID);
		CPlayer *find_player(int aPlayerID);
		CCouncil *find_council(int aCouncilID);

		int get_planet_count();
		int get_player_count();
		int get_council_count();

		inline bool get_accept_new_player();
		inline int get_weight();
		inline int get_max_planet_name_number();

		CCouncilList* get_council_list(); 
		CEventInstanceList *get_event_list();
		
	public: // set
		inline void set_id(int aID);
		inline void set_name(const char* aName);

		void add_planet(CPlanet* aPlanet);
		void add_player(CPlayer* aPlayer);
		void add_council(CCouncil* aCouncil);

		inline void set_accept_new_player(bool aBool);
		inline void set_weight(int aWeight);
		void set_max_planet_name_number(int aMaxPlanetNameNumber);

	public: // etc
		void refresh_nick();

		bool remove_planet(int aPlanetID);
		bool remove_player(int aPlayerID);
		bool remove_council(int aCouncilID);

		const char *get_new_planet_name();
		char *info_council_list_html();

	RECYCLE(gClusterZone);
};

inline int
CCluster::get_id()
{
	return mID;
}

inline const char*
CCluster::get_name()
{
	return (char*)mName;
}

inline bool 
CCluster::get_accept_new_player()
{
	return mAcceptNewPlayer;
}

inline int 
CCluster::get_weight()
{
	return mWeight;
}

inline void 
CCluster::set_id(int aID)
{
	mID = aID;
}

inline void
CCluster::set_name(const char* aName)
{
	mName = aName;
	refresh_nick();
}

inline void 
CCluster::set_accept_new_player(bool aBool)
{
	mAcceptNewPlayer = aBool;
}

inline void 
CCluster::set_weight(int aWeight)
{
	mWeight = aWeight;
}

//------------------------------------------- CClusterList
/**
*/
class CClusterList: public CSortedList
{
	public:
		CClusterList();
		virtual ~CClusterList();

		int add_cluster(CCluster* aCluster);
		bool remove_cluster(int aCluster);

		CCluster* get_by_id(int aID);

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "cluster list"; }
};

//------------------------------------------- CClusterNameList
/**
*/
class CClusterNameList: public CSortedList
{
	public:
		CClusterNameList();
		virtual ~CClusterNameList();

		void add_name(const char* aName);
		const char *get_random_name();
	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "cluster name list"; }
};

//------------------------------------------- CUniverse
/**
*/
class CUniverse : public CSortedList
{
	private:
		int mMaxClusterID;

		static CClusterNameList *mClusterNameList;

	public:
		CUniverse();
		virtual ~CUniverse();

		int add_cluster(CCluster *aCluster);
		bool remove_cluster(int aID);

		CCluster *get_by_id(int aID);

		CCluster *new_cluster();

		bool load(CMySQL &aMySQL);
		static bool load_name(const char* aFilename);

		char *info_cluster_list_html();
	protected:
		const char *new_cluster_name();

		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "universe"; }
};

#endif
