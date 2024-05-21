#if !defined(__ARCHSPACE_BOUNTY_H__)
#define __ARCHSPACE_BOUNTY_H__

#include "store.h"
#include "relation.h"
//#include "player.h"
#include "util.h"
#include "define.h"
#include <list>

using namespace std;

class CPlayer;

extern TZone gBountyZone;
/**
*/
class CBounty: public CStore
{
	protected:
		CPlayer
			*mSourcePlayer,
			*mTargetPlayer;

		int mEmpirePoints;
		int mID;

		time_t mExpireTime;

		enum
		{
			FIELD_ID = 0,//id = mPlayerRelationID as well
			FIELD_SOURCE_PLAYER,
			FIELD_TARGET_PLAYER,
			FIELD_EMPIRE_POINTS,
			FIELD_EXPIRE_TIME
		};

	public:
		CBounty(CPlayer *aSourcePlayer, CPlayer *aTargetPlayer);
		CBounty(MYSQL_ROW aRow);

		virtual ~CBounty();

		virtual const char *table() {return "bounty";}
		virtual CString& query();

	public:
		inline int get_id();
		inline time_t get_time();
		inline CPlayer* get_source_player();
		inline CPlayer* get_target_player();
		inline int get_empire_points();

		void set_relation(int aID);
		void set_source_player(int aID);
		inline void set_source_player(CPlayer *aPlayer);
		void set_target_player(int aID);
		inline void set_target_player(CPlayer *aPlayer);
		inline void set_time(time_t aTime);
		void set_empire_points(int aPoints);
		void change_empire_points(int aPoints);

		bool is_expired();
		bool is_valid();//was unable to add bounty
		void expire();

	RECYCLE(gBountyZone);
};

inline int
CBounty::get_id()
{
	return mID;
}

inline time_t
CBounty::get_time()
{
	return mExpireTime;
}

inline CPlayer*
CBounty::get_source_player()
{
	return mSourcePlayer;
}

inline CPlayer*
CBounty::get_target_player()
{
	return mTargetPlayer;
}

inline int
CBounty::get_empire_points()
{
	return mEmpirePoints;
}

inline void
CBounty::set_source_player(CPlayer* aPlayer)
{
	mSourcePlayer = aPlayer;
}

inline void
CBounty::set_target_player(CPlayer* aPlayer)
{
	mTargetPlayer = aPlayer;
}

inline void
CBounty::set_time(time_t aTime)
{
	mExpireTime = aTime;
}

class COfferedBounty
{
	private:
		CPlayer *mPlayer;
		unsigned char mHunters;
	public:
		COfferedBounty(CPlayer *aTargetPlayer);
		inline CPlayer *get_player();
		inline unsigned char get_active_hunters();
		void add_hunter(CPlayer *aPlayer);
		inline char compare(int aID);
		inline char compare(COfferedBounty *aBounty);
		int get_id();
};

inline char
COfferedBounty::compare(int aID)
{
	if (aID < get_id()) return (char)-1;
	if (aID > get_id()) return (char)1;
	return (char)0;
}

inline char
COfferedBounty::compare(COfferedBounty *aBounty)
{
	return compare(aBounty->get_id());
}

inline CPlayer*
COfferedBounty::get_player()
{
	return mPlayer;
}

inline unsigned char
COfferedBounty::get_active_hunters()
{
	return mHunters;
}

class COfferedBountyTable
{
	private:
		std::list<COfferedBounty*> mBounties;

	public:
		COfferedBountyTable();
		void new_bounty(CPlayer *aTargetPlayer);
		void expire_bounties();
		void refresh_bounties();
		void remove_bounty_on_player(int aID);
		COfferedBounty* get_by_id(int aID);
		std::list<COfferedBounty*> *get_available_bounties(CPlayer *aPlayer);
};

class CBountyList
{
	private:
		std::list<CBounty*> mBounties;

	public:
		bool accept_bounty(COfferedBounty* aBounty, CPlayer *aPlayer);
		bool insert_bounty(CBounty* aBounty);
		void expire_bounties();
		void remove_bounties();
		void remove_bounty(int aID);
		void remove_bounties_on_player(int aID);
		CBounty* get_bounty_by_player_id(int aID);
		inline std::list<CBounty*> *get_current_bounties();
		inline int size();
};

inline std::list<CBounty*> *
CBountyList::get_current_bounties()
{
	return &mBounties;
}

inline int
CBountyList::size()
{
	return mBounties.size();
}

class CBountyTable
{
	private:
		std::list<CBounty*> mBounties;

	public:
		~CBountyTable();
		bool load(CMySQL &aMySQL);
		CBounty* get_by_id(int aID);
		CBounty* get_by_target_id(int aID);
		void remove(int aID);
		void remove(CBounty* aBounty);
		void remove_by_target_id(int aID);
		void add(CBounty* aBounty);
		void new_bounty(COfferedBounty *aOfferedBounty, CPlayer *aHunter);
		void new_bounty(MYSQL_ROW aRow);
};

#endif

