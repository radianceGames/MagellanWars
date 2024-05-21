#if !defined(__ARCHSPACE_RANK_H__)
#define __ARCHSPACE_RANK_H__

#include "cgi.h"

class CCouncil;
class CPlayer;

extern TZone gRankZone;

class CRank
{
	protected:
		int
			mID;
		int
			mPower;
		int
			mNumber;
	public:
		enum ERankType
		{
			PLAYER_OVERALL = 0,
			PLAYER_FLEET,
			PLAYER_PLANET,
			PLAYER_TECH,
			PLAYER_RATING,
			COUNCIL_OVERALL,
			COUNCIL_FLEET,
			COUNCIL_PLANET,
			COUNCIL_DIPLOMATIC
		};

	public:
		CRank();
		~CRank();

		int get_id() { return mID; }
		int get_power() { return mPower; }
		int get_number() { return mNumber; }

		void set_id(int aID) { mID = aID; }
		void set_power(int aPower) { mPower = aPower; }
		void set_number(int aNumber) { mNumber = aNumber; }

		bool change_power(int aPower);
		void change_number(int aNumber) { mNumber += aNumber; }

		bool initialize_from_player(CPlayer *aPlayer, int aType);
		bool initialize_from_council(CCouncil *aCouncil, int aType);

};

class CRankTable: public CSortedList
{
	private:
		int
			mRankType;
		time_t
			mUpdateTime;

	public:
		CRankTable();
		virtual ~CRankTable();

		int add_rank(CRank *aRank);
		bool remove_rank(int aID);

		CRank *get_by_id(int aID);
		int get_rank_by_id(int aID);

		int get_rank_type() { return mRankType; }
		time_t get_time() { return mUpdateTime; }

		void set_rank_type(int aType);
		void set_time(time_t aTime) { mUpdateTime = aTime; }

		char *top_ten_info_html(int aID);
		char *around_me_info_html(int aID);
		
		int get_top_number_of_techs();

	protected:
		CQueryList
			mConversion;

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;

		char *get_race_symbol_url(int aRace);
};

#endif
