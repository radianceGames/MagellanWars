#if !defined(__ARCHSPACE_COUNCIL_H__)
#define __ARCHSPACE_COUNCIL_H__

#include "store.h"
#include "player.h"

class CPlayer;
class CCouncil;

extern TZone gCouncilTableZone;
extern TZone gAdmissionZone;

//----------------------------------------------------- CAdmission

/**
*/
class CAdmission: public CStore
{
	private:
		int mPlayerID;
		int mCouncilID;
		time_t mTime;
		CString mContent;

	public:
		CAdmission();
		CAdmission(MYSQL_ROW aRow);
		virtual ~CAdmission();

		virtual const char *table() { return "admission"; }
		virtual CString& query();

	public:
		CPlayer *get_player();
		CCouncil *get_council();
		inline time_t get_time();
		inline char *get_content();
		const char* get_time_string();

	public:
		void set_player(CPlayer* aPlayer);
		void set_council(CCouncil* aCouncil);
		inline void set_time(time_t aItem);
		inline void set_content(const char *aString);
};

inline time_t
CAdmission::get_time()
{
	return mTime;
}

inline char *
CAdmission::get_content()
{
	return (char *)mContent;
}

inline void
CAdmission::set_time(time_t aTime)
{
	mTime = aTime;
}

inline void
CAdmission::set_content(const char* aContent)
{
//	assert(aContent);
	mContent = aContent;
}

//------------------------------------------------- CAdmissionList
/**
*/
class CAdmissionList : public CSortedList
{
	public :
		CAdmissionList();
		virtual ~CAdmissionList();

		int add_admission(CAdmission* aAdmission);
		bool remove_admission(int aPlayerID);
		CAdmission *get_by_player_id(int aPlayerID);

		const char *html_table();
	protected:

		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "admission list"; }
};

//-------------------------------------------------- CCouncil
extern TZone gCouncilZone;
/**
*/
class CCouncil: public CStore
{
	class CMemberRankList: public CPlayerList
	{
		public:
			virtual ~CMemberRankList();

		protected:
			virtual const char *debug_info() const { return "ranklist"; }
			virtual int compare(TSomething aItem1, TSomething aItem2) const;
	};
	private:
		static int mMaxID;
		enum {
			STORE_NAME = 0,
			STORE_SLOGAN,
			STORE_SPEAKER,
			STORE_HONOR,
			STORE_AUTO_ASSIGN,
			STORE_PRODUCTION,
			STORE_MERGE_PENALTY_TIME,
			STORE_SECONDARY_SPEAKER
		};

		CString
			mName,
			mNick,
			mSlogan;
		int
			mSpeakerID;
		int
			mSecondarySpeakerID;
		int
			mProduction;
		int
			mHonor;
		int
			mPower;
		int
			mID;
		int
			mHomeClusterID;
		time_t mMergePenaltyTime;
		CCouncil *mSupremacyCouncil;

		bool mAutoAssign;

		CPlayerList mMembers;
		CClusterList mClusterList;
		CAdmissionList mAdmissionList;
		CProjectList mProjectList;
		CPurchasedProjectList mPurchasedProjectList;
		CCouncilRelationList mRelationList;
		CCouncilActionList mActionList;
		CCouncilMessageBox mMessageBox;

		CControlModel mControlModel;

	public:
		CCouncil();
		CCouncil(MYSQL_ROW aRow);
		virtual ~CCouncil();

		virtual const char *table() { return "council"; }
		virtual CString& query();

	public: // get
		inline int get_id() const;
		inline bool get_auto_assign();

		char *get_name() { return (char *)mName; }
		bool set_name(char *aName);

		char *get_nick() { return (char *)mNick; }
		void refresh_nick();

		inline const char *get_slogan();
		inline int get_production();
		CPlayer *get_speaker();
		CPlayer *get_secondary_speaker();
		inline int get_speaker_id();
		inline int get_secondary_speaker_id();
		inline int get_honor();
		inline int get_empire_relation();
		int get_power() { return mPower; }
		bool refresh_power();

		int get_home_cluster_id() { return mHomeClusterID; }

		const char* get_honor_description();
		const char *get_cluster_names();

		CPlayerList *get_members() { return &mMembers; }
		CClusterList *get_cluster_list() { return &mClusterList; }
		CControlModel &get_control_model() { return mControlModel; }

		inline CPlayer* get_member_by_game_id(int aID);
		CAdmissionList *get_admission_list() { return &mAdmissionList; }
		CAdmission *get_admission_by_index(int aIndex);
		CAdmission *get_admission_by_time(time_t aTime);

	public: // set
		inline void set_id(int aID);
		inline void set_auto_assign(bool aAssign);
		inline void set_slogan(const char *aSlogan);

		void set_home_cluster_id(int aHomeClusterID)
			{ mHomeClusterID = aHomeClusterID; }

		bool set_speaker(int aGameID);
		bool set_secondary_speaker(int aGameID);

		bool set_production(int aProduction);
		void change_production(int aProduction);

		void set_honor(int aHonor);
		void change_honor(int aChange);

	public: // etc
		const char* buy_council_project(int aProjectID);
		int can_buy_any_project();

		int max_member();
		inline int get_number_of_members();

		bool add_member(CPlayer *aPlayer);
		bool remove_member(int aID);

		inline bool remove_admission(CPlayer *aOwner);
		inline int add_admission(CAdmission *aAdmission);

		inline bool add_project(CPurchasedProject *aProject);

		void set_project_list();
		int distribute(int aAmount, CString& List);

		int process_vote();

		bool has_cluster();
		bool has_cluster(int aClusterID);
		bool add_cluster(CCluster* aCluster);
		bool remove_cluster(int aClusterID);

		void rearrange_cluster(bool aAll=false);

		void update_by_tick();
		void update_by_time();

		void add_relation(CCouncilRelation* aRelation);
		void remove_relation(int aRelationID);
		CCouncilRelation* get_relation(CCouncil* aCouncil);
		CCouncilRelationList *get_relation_list() { return &mRelationList; }

		CPurchasedProjectList *get_purchased_project_list()
			{ return &mPurchasedProjectList; }
		inline bool has_project(int aProjectID);
		CCouncilActionList *get_council_action_list()
				{ return &mActionList; }
		CCouncilMessageBox *get_council_message_box()
				{ return &mMessageBox; }

		void add_action(CCouncilAction *aAction);
		void remove_action(int aActionID);

		CCouncil *relation_council(int aCouncilID);

		void add_council_message(CCouncilMessage *aMessage);
		CCouncilMessage *get_council_message(int aMessageID);
		inline void remove_council_message(int aMessageID);

		char *check_council_message(CCouncilMessage::EType aMessageType,
												CCouncil* aCouncil, CCouncil *aActiveCouncil);
		bool check_duplication_diplomatic_message(
								CCouncilMessage::EType aMessageType,
												CCouncil* aCouncil);

		void declare_war(CCouncil* aCouncil);
		void declare_total_war(CCouncil* aCouncil);
		void break_pact(CCouncil* aCouncil);
		void break_ally(CCouncil* aCouncil);
		void set_free_subordinary(CCouncil* aCouncil);
		const char* improve_relationship(CCouncilMessage::EType aType,
												CCouncil* aCouncil);

//		const char* try_council_fusion(aCouncil);

		CCouncilAction* check_declare_total_war_effect_timer(CCouncil *aCouncil);
		void set_declare_total_war_effect_timer(CCouncil *aCouncil);

		CCouncilAction* check_declare_war_effect_timer(CCouncil *aCouncil);
		void set_declare_war_effect_timer(CCouncil *aCouncil);


		CCouncilAction* check_break_pact_effect_timer(CCouncil *aCouncil);
		void set_break_pact_effect_timer(CCouncil *aCouncil);
		CCouncilAction* check_break_ally_effect_timer(CCouncil *aCouncil);
		void set_break_ally_effect_timer(CCouncil *aCouncil);
		CCouncilAction* check_improve_relation_effect_timer(CCouncil *aCouncil);
		void set_improve_relation_effect_timer(CCouncil *aCouncil);
		void clear_improve_relation_effect_timer(CCouncil *aCouncil);

		void change_honor_with_news(int aChange);

		CCouncil* get_supremacy_council();
		void set_supremacy_council(CCouncil* aCouncil);

		bool check_total_war();
		bool check_war();

		int get_member_total_turn_production();

		bool process_merge_with(CCouncil *aCouncil, CCouncil *aActiveCouncil);
		char *can_merge_with(CCouncil *aCouncil, CCouncil *aActiveCouncil);
		bool can_merge();
		void disable_merge();
		void disable_merge(time_t aTime);
		void enable_merge();
		const int get_merge_penalty_time();
		char *get_dismissable_html();

	public:
		const char *select_member_html();
		const char *select_project_html();
		const char *vote_member_list_html(CPlayer* aPlayer);
		inline const char *admission_table_html();
		const char *achieved_project_html();
		const char *select_member_except_player_html(CPlayer *aPlayer);
		const char *player_relation_table_html(CPlayer *aPlayer);
		const char *council_relation_table_html();
		const char *select_council_html();
		const char *top_three_list_html();
		const char *council_message_select_html(CCouncil *aCouncil);
		const char *council_unread_message_table_html();
		const char *get_message_list_html(int aFirst, bool aIsForAdmin);
		static char *get_message_list_by_sender_html(CCouncil *aSender, bool aIsForAdmin = false);
		static int mFusionTimeLimit;
		static int mIDOffset;
		void send_time_news_to_all_member(const char *aNews);

		char *info_members_information_html();
		char *council_war_status_information_html();
	protected:
		int collect_tax();
		double total_vote_power();

		void set_commerce_with(CCouncil* aCouncil);
		void clear_commerce_with(CCouncil* aCouncil);

	RECYCLE(gCouncilZone);
};

inline int
CCouncil::get_id() const
{
	return mID;
}

inline bool
CCouncil::get_auto_assign()
{
	return mAutoAssign;
}

inline const char*
CCouncil::get_slogan()
{
	static CString Slogan;
	Slogan = mSlogan;
	Slogan.htmlspecialchars();
	Slogan.nl2br();
	return (char*)Slogan;
}

inline int
CCouncil::get_production()
{
	return mProduction;
}

inline int
CCouncil::get_speaker_id()
{
	return mSpeakerID;
}

inline int
CCouncil::get_secondary_speaker_id()
{
	return mSecondarySpeakerID;
}

inline int
CCouncil::get_honor()
{
	return mHonor;
}

inline int
CCouncil::get_empire_relation()
{
	if (get_speaker() == NULL) return 0;
	return get_speaker()->get_empire_relation();
}

inline CPlayer*
CCouncil::get_member_by_game_id(int aID)
{
	if (!mMembers.length()) return NULL;
	return (CPlayer*)mMembers.get_by_game_id(aID);
}

inline void
CCouncil::set_id(int aID)
{
	mID = aID;
}

inline void
CCouncil::set_auto_assign(bool aAssign)
{
	if (mAutoAssign != aAssign)
		mStoreFlag += STORE_AUTO_ASSIGN;
	mAutoAssign = aAssign;
}

inline void
CCouncil::set_slogan(const char *aSlogan)
{
	mSlogan = aSlogan;
	mStoreFlag += STORE_SLOGAN;
}

inline int
CCouncil::get_number_of_members()
{
	return mMembers.length();
}

inline bool
CCouncil::remove_admission(CPlayer *aOwner)
{
	return mAdmissionList.remove_admission(aOwner->get_game_id());
}

inline int
CCouncil::add_admission(CAdmission *aAdmission)
{
	return mAdmissionList.add_admission(aAdmission);
}

inline const char*
CCouncil::admission_table_html()
{
	return mAdmissionList.html_table();
}

inline bool
CCouncil::add_project(CPurchasedProject *aProject)
{

	if (mPurchasedProjectList.add_project(aProject) >= 0)
	{
		mControlModel += aProject->get_effect();
		return true;
	}

	return false;
}

inline bool
CCouncil::has_project(int aProjectID)
{
	if(mPurchasedProjectList.get_by_id(aProjectID)) return true;
	return false;
}

//------------------------------------------------- CCouncilList
extern TZone gCouncilListZone;
/**
*/
class CCouncilList: public CSortedList
{
	public:
		virtual ~CCouncilList();

		CCouncil* get_by_id(int aID);
		int add_council(CCouncil* aCouncil);
		bool remove_council(int aCouncilID);

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "council list"; }
	RECYCLE(gCouncilListZone);
};

//------------------------------------------------- CCouncilTable
/**
*/
class CCouncilTable: public CSortedList
{
	public:
		CRankTable
			mOverallRankTable,
			mFleetRankTable,
			mPlanetRankTable,
			mDiplomaticRankTable;

	public:
		CCouncilTable();
		virtual ~CCouncilTable();

		int add_council(CCouncil* aCouncil);
		bool remove_council(int aID);

		CCouncil* get_by_id(int aID);
		CCouncil* get_by_name(char *aName);

		bool load(CMySQL &aMySQL);

		const char *html_select_council_except(CCouncil *aCouncil = NULL);

		void refresh_rank_table();

		void add_council_rank(CCouncil *aCouncil);
		void remove_council_rank(int aCouncilID);

		CRankTable *get_overall_rank_table() { return &mOverallRankTable; }
		CRankTable *get_fleet_rank_table() { return &mFleetRankTable; }
		CRankTable *get_planet_rank_table() { return &mPlanetRankTable; }
		CRankTable *get_diplomatic_rank_table() { return &mDiplomaticRankTable; }

		const char* offer_select_html(CPlayer* aPlayer);
	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "council table"; }

	RECYCLE(gCouncilTableZone);
};

#endif
