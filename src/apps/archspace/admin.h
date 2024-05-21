#if !defined(__ARCHSPACE_ADMIN_H__)
#define __ARCHSPACE_ADMIN_H__

#include "council.h"
#include "portal.h"

#define MIN_ADMIN_LEVEL      UL_ADMIN

extern TZone gDetachmentPlayerPlayerZone;

class CDetachmentPlayerPlayer: public CStore
{
	public:
		CDetachmentPlayerPlayer();
		virtual ~CDetachmentPlayerPlayer() {}

		virtual const char *table() { return "detachment_player_player"; }
		virtual CString &query();

		int get_id() { return mID; }
		void set_id(int aID) { mID = aID; }

		CPlayer *get_player1() { return mPlayer1; }
		void set_player1(CPlayer *aPlayer) { mPlayer1 = aPlayer; }

		CPlayer *get_player2() { return mPlayer2; }
		void set_player2(CPlayer *aPlayer) { mPlayer2 = aPlayer; }

		bool initialize(MYSQL_ROW aRow);

	private:
		int
			mID;
		CPlayer
			*mPlayer1,
			*mPlayer2;
};

class CDetachmentPlayerPlayerTable: public CSortedList
{
	public:
		CDetachmentPlayerPlayerTable() {}
		virtual ~CDetachmentPlayerPlayerTable();

		bool add_detachment(CDetachmentPlayerPlayer *aDetachment);
		bool remove_detachment(int aID);

		CDetachmentPlayerPlayer *get_by_player_player(CPlayer *aPlayer1, CPlayer *aPlayer2);

		int get_new_id();

		bool load(CMySQL *aMySQL);

		char *get_restricted_player_list_html(CPlayer *aPlayer);

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
};

extern TZone gDetachmentPlayerCouncilZone;

class CDetachmentPlayerCouncil: public CStore
{
	public:
		CDetachmentPlayerCouncil();
		virtual ~CDetachmentPlayerCouncil() {}

		virtual const char *table() { return "detachment_player_council"; }
		virtual CString &query();

		int get_id() { return mID; }
		void set_id(int aID) { mID = aID; }

		CPlayer *get_player() { return mPlayer; }
		void set_player(CPlayer *aPlayer) { mPlayer = aPlayer; }

		CCouncil *get_council() { return mCouncil; }
		void set_council(CCouncil *aCouncil) { mCouncil = aCouncil; }

		bool initialize(MYSQL_ROW aRow);

	private:
		int
			mID;
		CPlayer *
			mPlayer;
		CCouncil *
			mCouncil;
};

class CDetachmentPlayerCouncilTable: public CSortedList
{
	public:
		CDetachmentPlayerCouncilTable() {}
		virtual ~CDetachmentPlayerCouncilTable();

		bool add_detachment(CDetachmentPlayerCouncil *aDetachment);
		bool remove_detachment(int aID);

		CDetachmentPlayerCouncil *get_by_player_council(CPlayer *aPlayer, CCouncil *aCouncil);

		int get_new_id();

		bool load(CMySQL *aMySQL);

		bool remove_from_db_memory(CCouncil *aCouncil);

		char *get_restricted_council_list_html(CPlayer *aPlayer);
		char *get_restricted_player_list_html(CCouncil *aCouncil);

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
};

extern TZone gDetachmentCouncilCouncilZone;

class CDetachmentCouncilCouncil: public CStore
{
	public:
		enum ERestrictionType
		{
			TYPE_NONE = 0,
			TYPE_SUBMISSION,
			TYPE_MERGING,
			TYPE_ALLIANCE,
			TYPE_WAR
		};
	public:
		CDetachmentCouncilCouncil();
		virtual ~CDetachmentCouncilCouncil() {}

		virtual const char *table() { return "detachment_council_council"; }
		virtual CString &query();

		int get_id() { return mID; }
		void set_id(int aID) { mID = aID; }

		int get_type() { return mType; }
		bool set_type(int aType);

		CCouncil *get_council1() { return mCouncil1; }
		void set_council1(CCouncil *aCouncil) { mCouncil1 = aCouncil; }

		CCouncil *get_council2() { return mCouncil2; }
		void set_council2(CCouncil *aCouncil) { mCouncil2 = aCouncil; }

		bool initialize(MYSQL_ROW aRow);

	private:
		int
			mID;
		int
			mType;
		CCouncil
			*mCouncil1,
			*mCouncil2;
};

class CDetachmentCouncilCouncilTable: public CSortedList
{
	public:
		CDetachmentCouncilCouncilTable() {}
		virtual ~CDetachmentCouncilCouncilTable();

		bool add_detachment(CDetachmentCouncilCouncil *aDetachment);
		bool remove_detachment(int aID);

		CDetachmentCouncilCouncil *get_by_council_council(CCouncil *aCouncil1, CCouncil *aCouncil2);

		int get_new_id();

		bool load(CMySQL *aMySQL);

		bool remove_from_db_memory(CCouncil *aCouncil);

		char *get_restricted_council_list_html(CCouncil *aCouncil);

	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
};

class CAdminList : public CSortedList
{
	public:
		CAdminList();
		virtual ~CAdminList();

		bool add_admin(CPortalUser *aPortalUser);
		bool remove_admin(CPortalUser *aPortalUser);
		bool remove_admin(int aPortalID);
		bool load();
	protected :
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
};

class CAdminTool
{
	public:
		CAdminTool();
		~CAdminTool();

	public:
		bool initialize(CMySQL *aMySQL);

        bool has_access(CPlayer *aPlayer);
        bool has_access(CPlayer *aPlayer, EUserLevel aUserLevel);
        bool has_access(CPlayer *aPlayer, EDevLevel aDevLevel);
        
        bool set_data_file_directory(char *aDirectory);
		bool set_CS_mail_address(char *aAddress);
		bool set_accuse_mail_name(char *aName);

		bool add_new_player_log(char *aLogString);
		bool add_dead_player_log(char *aLogString);
		bool add_restarting_player_log(char *aLogString);
		bool add_login_player_log(char *aLogString);

		bool add_bankrupt_report_log(char *aLogString);

		bool add_player_relation_log(char *aLogString);
		bool add_spy_log(char *aLogString);

		bool add_speaker_log(char *aLogString);
		bool add_financial_aid_log(char *aLogString);
		bool add_donation_log(char *aLogString);
		bool add_emigration_log(char *aLogString);
		bool add_independence_log(char *aLogString);
		bool add_council_relation_log(char *aLogString);

		bool add_new_bid_log(char *aLogString);
		bool add_winning_bid_log(char *aLogString);

		bool add_siege_planet_log(char *aLogString);
		bool add_blockade_log(char *aLogString);
		bool add_raid_log(char *aLogString);
		bool add_privateer_log(char *aLogString);

		bool add_invade_magistrate_log(char *aLogString);
		bool add_invade_empire_planet_log(char *aLogString);
		bool add_invade_fortress_log(char *aLogString);
		bool add_invade_empire_capital_planet_log(char *aLogString);

		bool add_punishment_log(char *aLogString);
		bool add_admin_action_log(CPortalUser *aAdmin, char *aLogString);
		bool accuse_message(CDiplomaticMessage *aMessage);
		bool accuse_message(CCouncilMessage *aMessage);
		bool accuse_message(CAdmission *aAdmission);

		CDetachmentPlayerPlayerTable *get_detachment_player_player_table()
				{ return &mDetachmentPlayerPlayerTable; }
		CDetachmentPlayerCouncilTable *get_detachment_player_council_table()
				{ return &mDetachmentPlayerCouncilTable; }
		CDetachmentCouncilCouncilTable *get_detachment_council_council_table()
				{ return &mDetachmentCouncilCouncilTable; }

		bool are_they_restricted(CPlayer *aPlayer1, CPlayer *aPlayer2);
		bool are_they_restricted(CPlayer *aPlayer, CCouncil *aCouncil);
		bool are_they_restricted(CCouncilMessage::EType aType, CCouncil *aCouncil1, CCouncil *aCouncil2);

		CAdminList *get_admin_list() { return &mAdminList; }
		bool add_admin(CPortalUser *aAdmin);

		CIPList *get_banned_ip_list() { return &mBannedIPList; }
		bool load_banned_ip_list();
		bool save_banned_ip_list();
		bool add_banned_ip(char *aBannedIP);

		char *admin_list_html();
		char *banned_ip_list_html();

	protected :
		CDetachmentPlayerPlayerTable
			mDetachmentPlayerPlayerTable;
		CDetachmentPlayerCouncilTable
			mDetachmentPlayerCouncilTable;
		CDetachmentCouncilCouncilTable
			mDetachmentCouncilCouncilTable;

		CAdminList
			mAdminList;
		CIPList
			mBannedIPList;

		CString
			mDataFileDirectory,
			mCSMailAddress,
			mAccuseMailName;

	public :
		static char *
			mSEDPath;
};

#endif
