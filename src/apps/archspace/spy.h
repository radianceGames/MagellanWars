#if !defined(__ARCHSPACE_SPY_H__)
#define __ARCHSPACE_SPY_H__

#include "util.h"
#include "prerequisite.h"

class CPlayer;

class CSpy: public CPrerequisiteList
{
	public:
		enum ESpyStatus
		{
			TYPE_ACCEPTABLE = 0,
			TYPE_ORDINARY,
			TYPE_HOSTILE,
			TYPE_ATROCIOUS,
			TYPE_MAX
		};

	private:
		int
			mID;
		CString
			mName,
			mDescription;
		int
			mDifficulty;
		int
			mCost;
		int
			mPrerequisite;
		int
			mType;
		static int
			mHonorLossTable[][5];
		static char *
			mRelationChangeTable[][5];

	public:
		CSpy();
		~CSpy();

		int get_id() { return mID; }
		void set_id(int aID) { mID = aID; }

		char *get_name() { return (char *)mName; }
		void set_name(char *aName) { mName = aName; }

		char *get_description() { return (char *)mDescription; }
		void set_description(char *aDescription) { mDescription = aDescription; }

		int get_difficulty() { return mDifficulty; }
		void set_difficulty(int aDifficulty) { mDifficulty = aDifficulty; }

		int get_cost() { return mCost; }
		void set_cost(int aCost) { mCost = aCost; }

		int get_type() { return mType; }
		char *get_type_name();
		void set_type(int aType) { mType = aType; }

		int get_prerequisite() { return mPrerequisite; }
		void set_prerequisite(int aPrerequisite)
			{ mPrerequisite = aPrerequisite; }

		static int get_honor_loss(CPlayer *aSpyPlayer,
									CPlayer *aTargetPlayer,
									int aSpyType);
		static int get_relation_change(CPlayer *aSpyPlayer,
									CPlayer *aTargetPlayer,
									int aSPyType);
		static int get_security_status(int aSecurityLevel);
		static char *get_security_level_description(int aSecurityLevel);
		static int get_security_upkeep(int aSecurityLevel, int aIncome);

		char *perform(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer, bool *aSuccess);

		static char *security_level_select_html();

	protected:
		// ID : 8001
		char *general_information_gathering(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		// ID : 8002
		char *detailed_information_gathering(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		// ID : 8003
		char *steal_secret_info(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		// ID : 8004
		char *computer_virus_infiltration(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		// ID : 8005
		char *devastating_network_worm(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		// ID : 8006
		char *sabotage(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		// ID : 8007
		char *incite_riot(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		// ID : 8008
		char *steal_common_technology(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		char *steal_important_technology(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		char *steal_secret_technology(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		// ID : 8009
		char *artificial_disease(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		// ID : 8010
		char *red_death(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		// ID : 8011
		char *strike_base(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		// ID : 8012
		char *meteor_strike(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		// ID : 8013
		char *emp_storm(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		// ID : 8014
		char *stellar_bombardment(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
		// ID : 8015
		char *assassination(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer);
};

class CSpyTable: public CSortedList
{
	public:
		CSpyTable();
		virtual ~CSpyTable();

		int add_spy(CSpy* aSpy);
		bool remove_spy(int aSpyID);

		CSpy *get_by_id(int aSpyID);

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
};

#endif
