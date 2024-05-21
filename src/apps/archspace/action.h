#if !defined(__ARCHSPACE_ACTION_H__)
#define __ARCHSPACE_ACTION_H__

#include "store.h"

class CPlayer;
class CCouncil;

//------------------------------------------------ CAction

/**
*/
class CAction : public CStore
{
	public:
		enum EActionType
		{
			ACTION_NONE = 0,
			ACTION_PLAYER_COUNCIL_DONATION,
			ACTION_PLAYER_BREAK_ALLY,
			ACTION_PLAYER_BREAK_PACT,
			ACTION_COUNCIL_DECLARE_TOTAL_WAR,
			ACTION_COUNCIL_DECLARE_WAR,
			ACTION_COUNCIL_BREAK_ALLY,
			ACTION_COUNCIL_BREAK_PACT,
			ACTION_COUNCIL_IMPROVE_RELATION,
			ACTION_EMPIRE_BRIBE,
			ACTION_COUNCIL_MERGING_OFFER,
			ACTION_PLAYER_SIEGE_BLOCKADE_RESTRICTION,
			ACTION_PLAYER_SIEGE_BLOCKADE_PROTECTION,
			ACTION_PLAYER_EMPIRE_INVASION_HISTORY,
			ACTION_PLAYER_DECLARE_HOSTILE
		};

	protected:
		int mID;
		time_t mStartTime;
		int mArgument;

	public:
		CAction()
		{
			mID = 0;
			mStartTime = 0;
			mArgument = 0;
		}

		virtual ~CAction() {}

		virtual const char *table() = 0;
		virtual EActionType get_type() = 0;

		virtual CString &query() = 0;

	public:
		inline int get_id();
		inline time_t get_start_time();
		virtual time_t get_period() = 0;
		inline int get_argument();


		inline void set_start_time(time_t aTime);
		inline void set_argument(int aArgument);
		virtual void feedback() = 0;

		static int mPeriodPlayerCouncilDonation;
        static int mPeriodPlayerDeclareHostile;
        static int mPeriodPlayerBreakPact;
		static int mPeriodPlayerBreakAlly;
		static int mPeriodCouncilDeclareTotalWar;
		static int mPeriodCouncilDeclareWar;
		static int mPeriodCouncilBreakPact;
		static int mPeriodCouncilBreakAlly;
		static int mPeriodCouncilImproveRelation;
		static int mPeriodCouncilMergingOffer;
};


inline int 
CAction::get_id()
{
	return mID;
}

inline time_t 
CAction::get_start_time()
{
	return mStartTime;
}

inline int 
CAction::get_argument()
{
	return mArgument;
}

inline void 
CAction::set_start_time(time_t aTime)
{
	mStartTime = aTime;
}

inline void
CAction::set_argument(int aArgument)
{
	mArgument = aArgument;
}
//--------------------------------------------- CPlayerAction

extern TZone gPlayerActionZone;
/**
*/

class CPlayerAction: public CAction
{
	protected:
		CPlayer *mOwner;

		static int mMaxID;
	public:
		CPlayerAction();
		virtual ~CPlayerAction();

		void load_from_database(MYSQL_ROW aRow);

		virtual const char *table() { return "player_action"; }

		virtual CString &query();
	public:
		inline CPlayer* get_owner();
		inline void set_owner(CPlayer *aOwner);

	RECYCLE(gPlayerActionZone);
};

inline CPlayer*
CPlayerAction::get_owner()
{
	return mOwner;
}

inline void 
CPlayerAction::set_owner(CPlayer *aOwner)
{
	mOwner = aOwner;
}

//----------------------------------CPlayerActionCouncilDonation
/**
*/
class CPlayerActionCouncilDonation:public CPlayerAction
{
	public:
		virtual ~CPlayerActionCouncilDonation() {}

		virtual EActionType get_type() 
					{ return ACTION_PLAYER_COUNCIL_DONATION; }
		virtual time_t get_period();

	protected:
		virtual void feedback();
};

//----------------------------------CPlayerActionBreakAlly
/**
*/
class CPlayerActionBreakAlly:public CPlayerAction
{
	public:
		virtual ~CPlayerActionBreakAlly() {}

		virtual EActionType get_type() 
					{ return ACTION_PLAYER_BREAK_ALLY; }
		virtual time_t get_period();

	protected:
		virtual void feedback();
};

//----------------------------------CPlayerActionBreakPact
/**
*/
class CPlayerActionBreakPact:public CPlayerAction
{
	public:
		virtual ~CPlayerActionBreakPact() {}

		virtual EActionType get_type() 
					{ return ACTION_PLAYER_BREAK_PACT; }
		virtual time_t get_period();

	protected:
		virtual void feedback();
};

class CPlayerActionSiegeBlockadeRestriction : public CPlayerAction
{
	public:
		virtual ~CPlayerActionSiegeBlockadeRestriction() {}

		virtual EActionType get_type() 
					{ return ACTION_PLAYER_SIEGE_BLOCKADE_RESTRICTION; }
		virtual time_t get_period();

	protected:
		virtual void feedback();
};

class CPlayerActionSiegeBlockadeProtection : public CPlayerAction
{
	public:
		virtual ~CPlayerActionSiegeBlockadeProtection() {}

		virtual EActionType get_type() 
					{ return ACTION_PLAYER_SIEGE_BLOCKADE_PROTECTION; }
		virtual time_t get_period();

	protected:
		virtual void feedback();
};

class CPlayerActionEmpireInvasionHistory : public CPlayerAction
{
	public:
		virtual ~CPlayerActionEmpireInvasionHistory() {}

		virtual EActionType get_type()
				{ return ACTION_PLAYER_EMPIRE_INVASION_HISTORY; }
		virtual time_t get_period();

	protected:
		virtual void feedback();
};

class CPlayerActionDeclareHostile : public CPlayerAction
{
	public:
		virtual ~CPlayerActionDeclareHostile() {}

		virtual EActionType get_type()
				{ return ACTION_PLAYER_DECLARE_HOSTILE; }
		virtual time_t get_period();

	protected:
		virtual void feedback();
};

//----------------------------------------------- CCouncilAction
/**
*/

extern TZone gCouncilActionZone;

class CCouncilAction: public CAction
{
	protected:
		CCouncil *mOwner;

		static int mMaxID;
	public:
		CCouncilAction();
		virtual ~CCouncilAction();

		void load_from_database(MYSQL_ROW aRow);

		virtual const char *table() { return "council_action"; }

		virtual CString &query();
	public:
		inline CCouncil* get_owner();

		inline void set_owner(CCouncil *aOwner);

	RECYCLE(gCouncilActionZone);
};

inline CCouncil*
CCouncilAction::get_owner()
{
	return mOwner;
}

inline void 
CCouncilAction::set_owner(CCouncil *aOwner)
{
	mOwner = aOwner;
}

//----------------------------------CCouncilActionBreakPact
/**
*/
class CCouncilActionBreakPact:public CCouncilAction
{
	public:
		virtual ~CCouncilActionBreakPact() {}

		virtual EActionType get_type() 
					{ return ACTION_COUNCIL_BREAK_PACT; }
		virtual time_t get_period();

	protected:
		virtual void feedback();
};

//----------------------------------CCouncilActionBreakAlly
/**
*/
class CCouncilActionBreakAlly:public CCouncilAction
{
	public:
		virtual ~CCouncilActionBreakAlly() {}

		virtual EActionType get_type() 
					{ return ACTION_COUNCIL_BREAK_ALLY; }
		virtual time_t get_period();

	protected:
		virtual void feedback();
};

//----------------------------------CCouncilActionDeclareTotalWar
/**
*/
class CCouncilActionDeclareTotalWar:public CCouncilAction
{
	public:
		virtual ~CCouncilActionDeclareTotalWar() {}

		virtual EActionType get_type() 
					{ return ACTION_COUNCIL_DECLARE_TOTAL_WAR; }
		virtual time_t get_period();

	protected:
		virtual void feedback();
};

//----------------------------------CCouncilActionDeclareTotalWar
/**
*/
class CCouncilActionDeclareWar:public CCouncilAction
{
	public:
		virtual ~CCouncilActionDeclareWar() {}

		virtual EActionType get_type() 
					{ return ACTION_COUNCIL_DECLARE_WAR; }
		virtual time_t get_period();

	protected:
		virtual void feedback();
};

//----------------------------------CCouncilActionImproveRelation
/**
*/
class CCouncilActionImproveRelation:public CCouncilAction
{
	public:
		virtual ~CCouncilActionImproveRelation() {}

		virtual EActionType get_type() 
					{ return ACTION_COUNCIL_IMPROVE_RELATION; }
		virtual time_t get_period();

	protected:
		virtual void feedback();
};


class CCouncilActionMergingOffer:public CCouncilAction
{
	public:
		virtual ~CCouncilActionMergingOffer() {}

		virtual EActionType get_type() 
					{ return ACTION_COUNCIL_MERGING_OFFER; }
		virtual time_t get_period();

	protected:
		virtual void feedback();
};

//---------------------------------------------- CActionList
/**
*/
class CActionList : public CSortedList
{
	public :
		CActionList();
		virtual ~CActionList();

		bool add_action(CAction* aAction);
		bool remove_action(int aID);

		CAction* get_by_id(int aID);
	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "action list"; }
};

//---------------------------------------------- CPlayerActionList
/**
*/
class CPlayerActionList: public CActionList
{
	public :
		virtual ~CPlayerActionList();

		CPlayerAction *get_council_donation_action();
		CPlayerAction *get_break_pact_action(int mPlayerID);
		CPlayerAction *get_break_ally_action(int mPlayerID);
        CPlayerAction *get_declare_hostile_action(int mPlayerID);
        
		bool add_action(CPlayerAction* aAction);
		virtual const char *debug_info() const { return "player action list"; }
};

//---------------------------------------------- CCouncilActionList
/**
*/
class CCouncilActionList: public CActionList
{
	public :
		virtual ~CCouncilActionList();

		CCouncilAction *get_break_pact_action(int mCouncilID);
		CCouncilAction *get_break_ally_action(int mCouncilID);
		CCouncilAction *get_declare_war_action(int mCouncilID);
		CCouncilAction *get_declare_total_war_action(int mCouncilID);
		CCouncilAction *get_improve_relation_action(int mCouncilID);

		bool add_action(CCouncilAction* aAction);
		virtual const char *debug_info() const { return "council player action list"; }
};

//-------------------------------------------- CActionTable
/**
*/
class CActionTable: public CActionList
{
	class CRecentlyIndex: public CSortedList
	{
		public :
			CRecentlyIndex();
			virtual ~CRecentlyIndex();
		protected:
			virtual bool free_item(TSomething aItem);
			virtual int compare(TSomething aItem1, TSomething aItem2) const;
			virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
			virtual const char *debug_info() const
					{ return "action table recently index"; }
	};

	protected:
		CRecentlyIndex mRecentlyIndex;	

	public:
		CActionTable();
		virtual ~CActionTable();

		void expire();

		bool add_action(CAction *aAction);
		bool remove_action(int aActionID);

	protected:
		virtual bool process_expire(CAction* aAction) = 0;
		virtual bool free_item(TSomething aItem);
		virtual const char *debug_info() const { return "action table"; }
};

//--------------------------------------------- CPlayerActionTable
/**
*/
class CPlayerActionTable : public CActionTable
{
	public:
		CPlayerActionTable();
		virtual ~CPlayerActionTable();

		CPlayerAction *get_by_owner_type(CPlayer *aPlayer, int aType);

		void load(CMySQL &aMySQL);
	protected:
		virtual bool process_expire(CAction *aAction);
		virtual const char *debug_info() const { return "player action table"; }
};

//--------------------------------------------- CCouncilActionTable
/**
*/
class CCouncilActionTable : public CActionTable
{
	public:
		CCouncilActionTable();
		virtual ~CCouncilActionTable();

		CCouncilAction *get_by_owner_type(CCouncil *aCouncil, int aType);

		void load(CMySQL &aMySQL);
	protected:
		virtual bool process_expire(CAction *aAction);
		virtual const char *debug_info() const { return "council action table"; }
};

#endif
