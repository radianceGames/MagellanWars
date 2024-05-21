#if !defined(__ARCHSPACE_RELATION_H__)
#define __ARCHSPACE_RELATION_H__

#include "store.h"

class CPlayer;
class CCouncil;

//----------------------------------------------------- CRelation
/**
*/
class CRelation: public CStore
{
	public:
		enum ERelation
		{
			RELATION_NONE = 0,
			RELATION_SUBORDINARY,
			RELATION_ALLY,
			RELATION_PEACE,
			RELATION_TRUCE,
			RELATION_WAR,
			RELATION_TOTAL_WAR,
			RELATION_BOUNTY,
			RELATION_HOSTILE
		};

	protected:
		int mID;
		ERelation mRelation;
		time_t mTime;

	public:
		CRelation();
		virtual ~CRelation() {}

		virtual const char *table() = 0;
		virtual CString& query() = 0;

	public:
		inline int get_id();
		inline ERelation get_relation();
		inline time_t get_time();

		inline void set_id(int aID);
		inline void set_relation(ERelation aRelation);
		inline void set_time(time_t aTime);

		const char* get_relation_description();
		static const char*
				get_relation_description(ERelation aRelation);
};

inline int
CRelation::get_id()
{
	return mID;
}

inline CRelation::ERelation
CRelation::get_relation()
{
	return mRelation;
}

inline time_t
CRelation::get_time()
{
	return mTime;
}

inline void
CRelation::set_id(int aID)
{
	mID = aID;
}

inline void
CRelation::set_relation(ERelation aRelation)
{
	mRelation = aRelation;
}

inline void
CRelation::set_time(time_t aTime)
{
	mTime = aTime;
}

//-------------------------------------------------- CPlayerRelation
extern TZone gPlayerRelationZone;
/**
*/
class CPlayerRelation: public CRelation
{
	public :
		enum EStoreFlag
		{
			STORE_ID = 0,
			STORE_PLAYER_1,
			STORE_PLAYER_2,
			STORE_RELATION,
			STORE_TIME
		};

	private:
		CPlayer *mPlayer1, *mPlayer2;

		static int mMaxID;
	public:
		CPlayerRelation();
		CPlayerRelation(MYSQL_ROW aRow);
		CPlayerRelation(CPlayer *aPlayer1, CPlayer* aPlayer2,
						ERelation aRelation);
		virtual ~CPlayerRelation();

		virtual const char *table() { return "player_relation"; }

		virtual CString& query();

	public:
		CPlayer* get_player1();
		CPlayer* get_player2();

	public:
		void set_player1_id(int aPlayerID);
		void set_player2_id(int aPlayerID);
		void set_player1(CPlayer *aPlayer);
		void set_player2(CPlayer *aPlayer);

	RECYCLE(gPlayerRelationZone);
};

//------------------------------------------------- CCouncilRelation
extern TZone gCouncilRelationZone;
/**
*/
class CCouncilRelation: public CRelation
{
	public :
		enum EStoreFlag
		{
			STORE_ID = 0,
			STORE_COUNCIL_1,
			STORE_COUNCIL_2,
			STORE_RELATION,
			STORE_TIME
		};

	private:
		CCouncil *mCouncil1, *mCouncil2;

		static int mMaxID;
	public:
		CCouncilRelation();
		CCouncilRelation(MYSQL_ROW aRow);
		CCouncilRelation(CCouncil* aCouncil1, CCouncil* aCouncil2,
				ERelation aRelation);
		virtual ~CCouncilRelation();

		virtual const char *table() { return "council_relation"; }

		virtual CString& query();

	public: // get
		CCouncil* get_council1();
		CCouncil* get_council2();

	public: // set
		void set_council1_id(int aCouncilID);
		void set_council2_id(int aCouncilID);
		void set_council1(CCouncil* aOwner);
		void set_council2(CCouncil* aTarget);

	RECYCLE(gCouncilRelationZone);
};

//-------------------------------------------- CRelationList
/**
*/
class CRelationList: public CSortedList
{
	public:
		CRelationList();
		virtual ~CRelationList();

		CRelation* find_by_id(int aID);

		bool add_relation(CRelation *aRelation);
		bool remove_relation(int aID);

		bool is_there_relation(int aRelation);
	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "relation list"; }

};

//------------------------------------------- CPlayerRelationList
/**
*/
class CPlayerRelationList: public CRelationList
{
	private:
		CPlayer* mOwner;

	public:
		CPlayerRelationList();
		virtual ~CPlayerRelationList();

		void set_owner(CPlayer *aOwner);

		CPlayerRelation* find_by_target_id(int aID);
		bool remove_player_relation_by_target_id(int aTargetID);

		char *select_player_by_relation_html(CPlayer *aPlayer, int aRelation);
		virtual const char *debug_info() const { return "player relation list"; }
};

//------------------------------------------- CCouncilRealtionList
/**
*/
class CCouncilRelationList: public CRelationList
{
	private:
		CCouncil* mOwner;

	public:
		CCouncilRelationList();
		virtual ~CCouncilRelationList();

		void set_owner(CCouncil *aOwner);

		CCouncilRelation* find_by_target_id(int aID);
		CCouncil* get_supremacy();

		bool remove_council_relation_by_target_id(int aTargetID);
		virtual const char *debug_info() const { return "council relation list"; }
};

//-------------------------------------------- CPlayerRelationTable
/**
*/
class CPlayerRelationTable: public CRelationList
{
	public:
		virtual ~CPlayerRelationTable();

		bool load(CMySQL &aMysql);

		void timeout();
	protected:
		virtual bool free_item(TSomething aItem);
		virtual const char *debug_info() const { return "player relation table"; }
};

//-------------------------------------------- CCouncilRelationTable
/**
*/
class CCouncilRelationTable: public CRelationList
{
	public:
		virtual ~CCouncilRelationTable();

		bool load(CMySQL &aMysql);

		void timeout();
	protected:
		virtual bool free_item(TSomething aItem);
		virtual const char *debug_info() const { return "council relation table"; }
};

#endif
