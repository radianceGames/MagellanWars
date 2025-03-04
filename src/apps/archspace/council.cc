#include "common.h"
#include "util.h"
#include "council.h"
#include <cstdlib>
#include "archspace.h"
#include "game.h"
#include <libintl.h>
#include "race.h"
#include "admin.h"
#include "action.h"

TZone gCouncilZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CCouncil),
	0,
	0,
	NULL,
	"Zone CCouncil"
};

TZone gAdmissionZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CAdmission),
	0,
	0,
	NULL,
	"Zone CAdmission"
};

int CCouncil::mMaxID = 0;
int CCouncil::mIDOffset = 0;
int CCouncil::mFusionTimeLimit = 12*60*60;

CCouncil::CMemberRankList::~CMemberRankList()
{
	remove_all();
}

int
	CCouncil::CMemberRankList::compare(TSomething aItem1, TSomething aItem2) const
{
	CPlayer *Player1 = (CPlayer*)aItem1;
	CPlayer *Player2 = (CPlayer*)aItem2;

	assert(Player1);
	assert(Player2);

	if (Player1->get_power() < Player2->get_power()) return 1;
	if (Player1->get_power() > Player2->get_power()) return -1;

	return 0;
}

CCouncil::CCouncil()
{
	mMaxID++;
	mMaxID = (mMaxID <= 0) ? 1:mMaxID;

	mID = mMaxID + mIDOffset;   // TODO/NOTE: THIS IS BAD. IGNORES mMaxID.
	mSpeakerID = 0;
	mSecondarySpeakerID = 0;
	mProduction = 0;
	mHonor = 50;
	mAutoAssign = true;
	mName = GETTEXT("Untitled");
	mSlogan = GETTEXT("None");
	mSupremacyCouncil = NULL;
	mPower = 0;
	mHomeClusterID = 0;

	mSupremacyCouncil = NULL;

	refresh_nick();

	mRelationList.set_owner(this);
	mMergePenaltyTime = -1;
	set_project_list();

	CMySQL
		MySQL;
	char
		*Host = CONFIG->get_string("Database", "Host", NULL),
	*User = CONFIG->get_string("Database", "User", NULL),
	*Password = CONFIG->get_string("Database", "Password", NULL),
	*Database = CONFIG->get_string("Database", "PortalUserDB", NULL);
	if ( (Host != NULL && User != NULL && Password != NULL && Database != NULL) && MySQL.open(Host, User, Password, Database))
	{
		CString
			Query;
		Query.clear();
		Query.format( "INSERT INTO %s.asbb_groups "
			"(group_type, group_name, group_description,"
			" group_moderator, group_single_user)"
			" VALUES (2, '%d', 'Council #%d', 2, 0)", Database, mID, mID);
		MySQL.query( (char*)Query );
		MySQL.use_result();
		MySQL.free_result();
		Query.clear();

		Query.format( "SELECT group_id FROM %s.asbb_groups WHERE group_name='%d'", Database, mID);
		MySQL.query( (char*)Query );
		MySQL.use_result();
		if (MySQL.fetch_row())
		{
			MYSQL_ROW aRow = MySQL.row();
			char *aGroupIDString = aRow[0];
			int aGroupID = -1;
			if (aGroupIDString && (aGroupID = atoi(aGroupIDString)))
			{
				MySQL.free_result();
				Query.clear();

				Query.format( "INSERT INTO %s.asbb_forums (forum_id, cat_id, forum_name,"
					" forum_desc, forum_status, forum_order, forum_posts, "
					"forum_topics, forum_last_post_id, prune_next, prune_enable, "
					"auth_view, auth_read, auth_post, auth_reply, auth_edit, "
					"auth_delete, auth_sticky, auth_announce, auth_vote, "
					"auth_pollcreate, auth_attachments) VALUES (%d, 4, '%d',"
					" 'Council #%d', 0, 10, 0, 0, 0, NULL, 0, 2, 2, 2,"
					" 2, 2, 2, 2, 3, 2 ,2 ,0)", Database, mID+1000, mID, mID);
				MySQL.query( (char*)Query );
				MySQL.use_result();
				MySQL.free_result();
				Query.clear();

				Query.format( "SELECT forum_id FROM %s.asbb_forums where forum_name='%d'", Database, mID);
				MySQL.query( (char*)Query);
				MySQL.use_result();
				if (MySQL.fetch_row())
				{
					aRow = MySQL.row();
					char *aForumIDString = aRow[0];
					int aForumID = -1;
					if (aForumIDString && (aForumID = atoi(aForumIDString)))
					{
						MySQL.free_result();
						Query.clear();
						Query.format( "INSERT INTO %s.asbb_auth_access "
							"(group_id, forum_id, auth_view, auth_read, "
							"auth_post, auth_reply, auth_edit, auth_delete, "
							"auth_sticky, auth_announce, auth_vote, auth_pollcreate, "
							"auth_attachments, auth_mod) VALUES (%d, %d, "
							"1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0)", Database, aGroupID, aForumID);
						MySQL.query( (char*)Query );
						MySQL.use_result();
						MySQL.free_result();
						Query.clear();
					} else { SLOG("ERROR: Could not create council %d forums - 2", mID); }
				} else { SLOG("ERROR: Could not create council %d forums", mID); }
			} else { SLOG("ERROR: Could not create council %d group - 2", mID); }
		}
		else { SLOG("ERROR: Could not create council %d group", mID); }
		MySQL.close();
	}
	else { SLOG("ERROR: Could not create council %d DB conn forums", mID); }


	char *IRCPHPScript = CONFIG->get_string("Game", "IRCPHPScript", NULL);
	if (IRCPHPScript != NULL)
		system((char*)format("php %s START %d", IRCPHPScript, mID));
}

CCouncil::CCouncil(MYSQL_ROW aRow)
{
	enum
	{
		FIELD_ID = 0,
		FIELD_SPEAKER_ID,
		FIELD_NAME,
		FIELD_SLOGAN,
		FIELD_PRODUCTION,
		FIELD_HONOR,
		FIELD_AUTO_ASSIGN,
		FIELD_HOME_CLUSTER_ID,
		FIELD_MERGE_PENALTY_TIME
		//FIELD_SECONDARY_SPEAKER
	};

	mID = atoi(aRow[FIELD_ID]);
	mSpeakerID = atoi(aRow[FIELD_SPEAKER_ID]);
	mName = aRow[FIELD_NAME];
	mSlogan = aRow[FIELD_SLOGAN];
	mProduction = atoi(aRow[FIELD_PRODUCTION]);
	mHonor = atoi(aRow[FIELD_HONOR]);
	mAutoAssign = (bool)atoi(aRow[FIELD_AUTO_ASSIGN]);
	mHomeClusterID = as_atoi(aRow[FIELD_HOME_CLUSTER_ID]);
    mMergePenaltyTime = as_atoi(aRow[FIELD_MERGE_PENALTY_TIME]);
	//mMergePenaltyTime = as_atoi(aRow[FIELD_MERGE_PENALTY_TIME]) + CGameStatus::get_down_time();

	mStoreFlag += STORE_MERGE_PENALTY_TIME;
	type(QUERY_UPDATE);
	STORE_CENTER->store(*this);

	mSecondarySpeakerID = 0;//as_atoi(aRow[FIELD_SECONDARY_SPEAKER]);
//	SLOG("Council Record %d, %s", mID, (char*)mName);

	if (mID > mMaxID) mMaxID = mID;

	mSupremacyCouncil = NULL;
	mPower = 0;

	refresh_nick();

	mRelationList.set_owner(this);

	CMySQL
		MySQL;
	char
		*Host = CONFIG->get_string("Database", "Host", NULL),
	*User = CONFIG->get_string("Database", "User", NULL),
	*Password = CONFIG->get_string("Database", "Password", NULL),
	*Database = CONFIG->get_string("Database", "PortalUserDB", NULL);
	if ( (Host != NULL && User != NULL && Password != NULL && Database != NULL) && MySQL.open(Host, User, Password, Database))
	{
		CString
			Query;
		Query.clear();

		Query.format( "SELECT group_id FROM %s.asbb_groups WHERE group_name='%d'", Database, mID);
		MySQL.query( (char*)Query );
		MySQL.use_result();
		if (MySQL.fetch_row())
		{
			MYSQL_ROW aRow = MySQL.row();
			char *aGroupIDString = aRow[0];
			int aGroupID = -1;
			if (aGroupIDString && (aGroupID = atoi(aGroupIDString)))
			{
				Query.clear();
				MySQL.free_result();
				Query.format( "SELECT forum_id FROM %s.asbb_forums where forum_name='%d'", Database, mID);
				MySQL.query( (char*)Query);
				MySQL.use_result();
				if (MySQL.fetch_row())
				{
					aRow = MySQL.row();
					char *aForumIDString = aRow[0];
					int aForumID = -1;
					if (aForumIDString && (aForumID = atoi(aForumIDString)))
					{
						MySQL.free_result();
						Query.clear();
						Query.format( "INSERT INTO %s.asbb_auth_access "
							"(group_id, forum_id, auth_view, auth_read, "
							"auth_post, auth_reply, auth_edit, auth_delete, "
							"auth_sticky, auth_announce, auth_vote, auth_pollcreate, "
							"auth_attachments, auth_mod) VALUES (%d, %d, "
							"1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0)", Database, aGroupID, aForumID);
						MySQL.query( (char*)Query );
						MySQL.use_result();
						MySQL.free_result();
						Query.clear();
					} else {
						SLOG("ERROR: Could not find council %d forums - 2 attempt creation", mID);
						MySQL.free_result();
						Query.clear();

						Query.format( "INSERT INTO %s.asbb_forums (forum_id, cat_id, forum_name,"
							" forum_desc, forum_status, forum_order, forum_posts, "
							"forum_topics, forum_last_post_id, prune_next, prune_enable, "
							"auth_view, auth_read, auth_post, auth_reply, auth_edit, "
							"auth_delete, auth_sticky, auth_announce, auth_vote, "
							"auth_pollcreate, auth_attachments) VALUES (%d, 4, '%d',"
							" 'Council #%d', 0, 10, 0, 0, 0, NULL, 0, 2, 2, 2,"
							" 2, 2, 2, 2, 3, 2 ,2 ,0)", Database, 1000+mID, mID, mID);
						MySQL.query( (char*)Query );
						MySQL.use_result();
						MySQL.free_result();
						Query.clear();
					}
				} else {
					SLOG("ERROR: Could not find council %d forums -- attempt creation", mID);
					MySQL.free_result();
					Query.clear();

					Query.format( "INSERT INTO %s.asbb_forums (forum_id, cat_id, forum_name,"
						" forum_desc, forum_status, forum_order, forum_posts, "
						"forum_topics, forum_last_post_id, prune_next, prune_enable, "
						"auth_view, auth_read, auth_post, auth_reply, auth_edit, "
						"auth_delete, auth_sticky, auth_announce, auth_vote, "
						"auth_pollcreate, auth_attachments) VALUES (%d, 4, '%d',"
						" 'Council #%d', 0, 10, 0, 0, 0, NULL, 0, 2, 2, 2,"
						" 2, 2, 2, 2, 3, 2 ,2 ,0)", Database, 1000+mID, mID, mID);
					MySQL.query( (char*)Query );
					MySQL.use_result();
					MySQL.free_result();
					Query.clear();
				}
			} else {
				SLOG("ERROR: Could not find council %d group - 2 attempt creation", mID);
				Query.clear();
				Query.format( "INSERT INTO %s.asbb_groups "
					"(group_type, group_name, group_description,"
					" group_moderator, group_single_user)"
					" VALUES (2, '%d', 'Council #%d', 2, 0)", Database, mID, mID);
				MySQL.query( (char*)Query );
				MySQL.use_result();
				MySQL.free_result();

				Query.clear();

				Query.format( "INSERT INTO %s.asbb_forums (forum_id, cat_id, forum_name,"
					" forum_desc, forum_status, forum_order, forum_posts, "
					"forum_topics, forum_last_post_id, prune_next, prune_enable, "
					"auth_view, auth_read, auth_post, auth_reply, auth_edit, "
					"auth_delete, auth_sticky, auth_announce, auth_vote, "
					"auth_pollcreate, auth_attachments) VALUES (%d, 4, '%d',"
					" 'Council #%d', 0, 10, 0, 0, 0, NULL, 0, 2, 2, 2,"
					" 2, 2, 2, 2, 3, 2 ,2 ,0)", Database, mID+1000, mID, mID);
				MySQL.query( (char*)Query );
				MySQL.use_result();
				MySQL.free_result();

				Query.clear();
			}
		}
		else {
			Query.clear();
			Query.format( "INSERT INTO %s.asbb_groups "
				"(group_type, group_name, group_description,"
				" group_moderator, group_single_user)"
				" VALUES (2, '%d', 'Council #%d', 2, 0)", Database, mID, mID);
			MySQL.query( (char*)Query );
			MySQL.use_result();
			MySQL.free_result();

			Query.clear();

			Query.format( "INSERT INTO %s.asbb_forums (forum_id, cat_id, forum_name,"
				" forum_desc, forum_status, forum_order, forum_posts, "
				"forum_topics, forum_last_post_id, prune_next, prune_enable, "
				"auth_view, auth_read, auth_post, auth_reply, auth_edit, "
				"auth_delete, auth_sticky, auth_announce, auth_vote, "
				"auth_pollcreate, auth_attachments) VALUES (4, '%d',"
				" 'Council #%d', 0, 10, 0, 0, 0, NULL, 0, 2, 2, 2,"
				" 2, 2, 2, 2, 3, 2 ,2 ,0)", Database, mID+1000, mID, mID);
			MySQL.query( (char*)Query );
			MySQL.use_result();
			MySQL.free_result();

			Query.clear();
			SLOG("ERROR: Could not find council %d group - attempt creation", mID); }
		MySQL.close();
	}
	else { SLOG("ERROR: Could not create council %d DB conn forums", mID); }


}

CCouncil::~CCouncil()
{
	char *IRCPHPScript = CONFIG->get_string("Game", "IRCPHPScript", NULL);
	if (IRCPHPScript != NULL)
		system((char*)format("php %s CLEAR %d", IRCPHPScript, mID));
}

bool
	CCouncil::refresh_power()
{
	mPower = 0;
	for(int i=0 ; i<mMembers.length() ; i++)
	{
		CPlayer *
		Member = (CPlayer *)mMembers.get(i);

		int
			AvailablePower = MAX_POWER - mPower;
		if (Member->get_power() > AvailablePower)
		{
			mPower = MAX_POWER;
			break;
		}
		else
		{
			mPower += Member->get_power();
		}
	}

	return true;
}

bool
	CCouncil::add_member(CPlayer *aPlayer)
{
	if (!aPlayer) return false;
	if (mMembers.get_by_game_id(aPlayer->get_game_id())) return false;

	mMembers.add_player(aPlayer);

	char *IRCPHPScript = CONFIG->get_string("Game", "IRCPHPScript", NULL);
	if (IRCPHPScript != NULL)
		system((char*)format("php %s ADD %d %d", IRCPHPScript, mID, aPlayer->get_game_id()));
	rearrange_cluster();

	CMySQL
		MySQL;
	char
		*Host = CONFIG->get_string("Database", "Host", NULL),
	*User = CONFIG->get_string("Database", "User", NULL),
	*Password = CONFIG->get_string("Database", "Password", NULL),
	*Database = CONFIG->get_string("Database", "PortalUserDB", NULL);
	if ( (Host != NULL && User != NULL && Password != NULL && Database != NULL) && MySQL.open(Host, User, Password, Database))
	{
		CString
			Query;
		Query.clear();
		Query.format( "SELECT group_id FROM %s.asbb_groups WHERE group_name='%d'", Database, mID);
		MySQL.query( (char*)Query );
		MySQL.use_result();
		if (MySQL.fetch_row())
		{
			MYSQL_ROW aRow = MySQL.row();
			char *aGroupIDString = aRow[0];
			int aGroupID = -1;
			if (aGroupIDString && (aGroupID = atoi(aGroupIDString)))
			{
				MySQL.free_result();
				Query.clear();
				Query.format("SELECT group_id FROM %s.asbb_user_group WHERE group_id=%d and user_id=%d", Database, aGroupID, aPlayer->get_portal_id());
				MySQL.query((char*)Query);
				MySQL.use_result();
				if (!MySQL.fetch_row())
				{
					MySQL.free_result();
					Query.clear();
					Query.format("INSERT INTO %s.asbb_user_group (group_id, user_id, user_pending)"
						" VALUES (%d, %d, 0)", Database, aGroupID, aPlayer->get_portal_id());
					MySQL.query((char*)Query);
					MySQL.use_result();
				}
				MySQL.free_result();
				Query.clear();
			} //else { SLOG("ERROR: Could not create council %d group - 2", mID) }
		}
//       else { SLOG("ERROR: Could not create council %d group", mID) }
		MySQL.close();
	}
//	else { SLOG("ERROR: ", mID) }

	return true;
}

bool
	CCouncil::remove_member(int aGameID)
{
	if (!mMembers.get_by_game_id(aGameID)) return false;

	if (mSpeakerID == aGameID)
	{
		mStoreFlag += STORE_SPEAKER;
		mSpeakerID = NONE;
	}

	char *IRCPHPScript = CONFIG->get_string("Game", "IRCPHPScript", NULL);
	if (IRCPHPScript != NULL)
		system((char*)format("php %s REMOVE %d %d", IRCPHPScript, mID, aGameID));

	for(int i=0; i<mMembers.length(); i++)
	{
		CPlayer
			*Player = (CPlayer*)mMembers.get(i);
		if (Player->get_council_vote() == aGameID)
			Player->set_council_vote(NONE);
	}

	CPlayer *
	Player = mMembers.get_by_game_id(aGameID);

	Player->set_council_vote(NONE);
	Player->set_council_production(NONE);
	Player->set_council_donation(NONE);

	CFleetList *
	FleetList = Player->get_fleet_list();
	CAllyFleetList *
	AllyFleetList = Player->get_ally_fleet_list();

	for (int i=0 ; i<FleetList->length() ; i++)
	{
		CFleet *
		Fleet = (CFleet *)FleetList->get(i);
		if (Fleet->get_status() != CFleet::FLEET_UNDER_MISSION) continue;
		CMission &
		Mission = Fleet->get_mission();
		if (Mission.get_mission() == CMission::MISSION_DISPATCH_TO_ALLY)
		{
			Fleet->end_mission();

			Fleet->type(QUERY_UPDATE);
			STORE_CENTER->store(*Fleet);
		}
	}

	for (int i=AllyFleetList->length()-1 ; i>=0 ; i--)
	{
		CFleet *
		Fleet = (CFleet *)AllyFleetList->get(i);
		Fleet->end_mission();

		Fleet->type(QUERY_UPDATE);
		STORE_CENTER->store(*Fleet);
	}

	Player->remove_relation_non_bounty_all();

	//clear commerce on all planets
	CPlanetList* tempyPlanetList  = Player->get_planet_list();
	CPlanet* tempyPlanet;
	for (int i = 0; i < tempyPlanetList->length(); i++)
	{
		tempyPlanet = tempyPlanetList->get_by_order(i);
		tempyPlanet->clear_commerce_all();
	}

	CMySQL
		MySQL;
	char
		*Host = CONFIG->get_string("Database", "Host", NULL),
	*User = CONFIG->get_string("Database", "User", NULL),
	*Password = CONFIG->get_string("Database", "Password", NULL),
	*Database = CONFIG->get_string("Database", "PortalUserDB", NULL);
	if ( (Host != NULL && User != NULL && Password != NULL && Database != NULL) && MySQL.open(Host, User, Password, Database))
	{
		CString
			Query;
		Query.clear();
		Query.format( "SELECT group_id FROM %s.asbb_groups WHERE group_name='%d'", Database, mID);
		MySQL.query( (char*)Query );
		MySQL.use_result();
		if (MySQL.fetch_row())
		{
			MYSQL_ROW aRow = MySQL.row();
			char *aGroupIDString = aRow[0];
			int aGroupID = -1;
			if (aGroupIDString && (aGroupID = atoi(aGroupIDString)))
			{
				MySQL.free_result();
				Query.clear();
				Query.format("DELETE FROM %s.asbb_user_group WHERE user_id=%d AND group_id=%d", Database, Player->get_portal_id(), aGroupID);
				MySQL.query((char*)Query);
				MySQL.use_result();
				MySQL.free_result();
				Query.clear();
			} //else { SLOG("ERROR: Could not create council %d group - 2", mID) }
		}
//       else { SLOG("ERROR: Could not create council %d group", mID) }
		MySQL.close();
	}
//	else { SLOG("ERROR: ", mID) }


	mMembers.remove_player(aGameID);

	rearrange_cluster(true);

	return true;
}

void
	CCouncil::set_project_list()
{
	for(int i=0; i<PROJECT_TABLE->length(); i++)
	{
		CProject *
		Project = (CProject *)PROJECT_TABLE->get(i);

		if (Project->get_type() == TYPE_COUNCIL)
		{
			int
				ProjectID = Project->get_id();
			if ( mPurchasedProjectList.get_by_id( ProjectID ) ) continue;
			mProjectList.add_project(Project);
		}
	}
}

bool
	CCouncil::set_speaker(int aGameID)
{
	if(!mMembers.get_by_game_id(aGameID))
		return false;
	char *IRCPHPScript = CONFIG->get_string("Game", "IRCPHPScript", NULL);
	if (IRCPHPScript != NULL && mSpeakerID != 0)
	{
		system((char*)format("php %s REMOVE %d %d", IRCPHPScript, mID, mSpeakerID));
		system((char*)format("php %s ADD %d %d", IRCPHPScript, mID, mSpeakerID));
	}

	mSpeakerID = aGameID;
	mStoreFlag += STORE_SPEAKER;

	if (mSpeakerID == 0)
	{
		ADMIN_TOOL->add_speaker_log(
			(char *)format("The council %s has no speaker now.", (char *)mNick));
	}
	else
	{
		CPlayer *
		Speaker = PLAYER_TABLE->get_by_game_id(mSpeakerID);
		if (IRCPHPScript != NULL)
			system((char*)format("php %s SPEAKER %d %d", IRCPHPScript, mID, aGameID));

		ADMIN_TOOL->add_speaker_log(
			(char *)format("Player %s has become a speaker in the council %s.",
				Speaker->get_nick(), (char *)mNick));
	}

	return true;
}
bool
	CCouncil::set_secondary_speaker(int aGameID)
{
	if(!mMembers.get_by_game_id(aGameID))
		return false;
	char *IRCPHPScript = CONFIG->get_string("Game", "IRCPHPScript", NULL);
	if (IRCPHPScript != NULL && mSecondarySpeakerID != 0)
	{
		system((char*)format("php %s REMOVE %d %d", IRCPHPScript, mID, mSecondarySpeakerID));
		system((char*)format("php %s ADD %d %d", IRCPHPScript, mID, mSecondarySpeakerID));
	}

	mSecondarySpeakerID = aGameID;
	mStoreFlag += STORE_SECONDARY_SPEAKER;

	if (mSecondarySpeakerID == 0)
	{
		ADMIN_TOOL->add_speaker_log(
			(char *)format("The council %s has no speaker now.", (char *)mNick));
	}
	else
	{
		CPlayer *
		SecondarySpeaker = PLAYER_TABLE->get_by_game_id(mSecondarySpeakerID);
		if (IRCPHPScript != NULL)
			system((char*)format("php %s SPEAKER %d %d", IRCPHPScript, mID, aGameID));

		ADMIN_TOOL->add_speaker_log(
			(char *)format("Player %s has become a secondary speaker in the council %s.",
				SecondarySpeaker->get_nick(), (char *)mNick));
	}

	return true;
}

CString &
CCouncil::query()
{
	static CString
		Query;

	Query.clear();

	CString
		Name,
		Slogan;
	Name = (char *)add_slashes((char *)mName);
	Slogan = (char *)add_slashes((char *)mSlogan);

	switch( type() )
	{
		case QUERY_INSERT:

			Query.format("INSERT INTO council (id, speaker, name, slogan, home_cluster_id) "
				"VALUES ( %d, %d, \"%s\", \"%s\", %d )",
				get_id(),
				get_speaker_id(),
				(char *)Name, (char *)Slogan, mHomeClusterID );
			break;
		case QUERY_UPDATE :
			Query.format("UPDATE council SET production = %d", get_production());

#define STORE(x, y, z) if (mStoreFlag.has(x)) Query.format(y, z)

			STORE(STORE_NAME, ", name = '%s'", (char *)Name);
			STORE(STORE_SLOGAN, ", slogan = '%s'", (char *)Slogan);
			STORE(STORE_SPEAKER, ", speaker = %d", get_speaker_id());
			STORE(STORE_HONOR, ", honor = %d", get_honor());
			STORE(STORE_AUTO_ASSIGN, ", auto_assign = %d", (int)get_auto_assign());
			STORE(STORE_PRODUCTION, ", production = %d", get_production());
			STORE(STORE_MERGE_PENALTY_TIME, ", merge_penalty_time = %d", mMergePenaltyTime);

			Query.format(" WHERE id = %d", get_id());

			break;

		case QUERY_DELETE :
			Query.format("DELETE FROM council WHERE id = %d", get_id());
			break;
	}

	mStoreFlag.clear();

	return Query;
}

bool
	CCouncil::set_name(char *aName)
{
	if (aName == NULL) return false;

	mName = aName;
	mStoreFlag += STORE_NAME;

	refresh_nick();

	return true;
}

const char *
	CCouncil::select_member_html()
{
	static CString
		Buf;
	Buf.clear();

	Buf = "<SELECT NAME = \"MEMBER\">\n";
	if (!mMembers.length())
	{
		Buf += "<OPTION SELECTED VALUE = \"0\">\nNone\n</OPTION>\n";
	}

	for(int i=0; i<mMembers.length(); i++){
		CPlayer
			*Player = (CPlayer*)mMembers.get(i);
		Buf.format( "<OPTION VALUE = \"%d\">\n%s\n</OPTION>\n",
			Player->get_game_id(), Player->get_nick() );
	}

	Buf += "</SELECT>\n";

	return (char*)Buf;
}

const char *
	CCouncil::select_project_html()
{
	static CString
		Buf;
	Buf.clear();

	CPlayer *
	Speaker = get_speaker();

	if (Speaker != NULL)
	{
		Buf = "<SELECT NAME = \"PROJECT\">\n";

		for (int i=0 ; i<PROJECT_TABLE->length() ; i++)
		{
			CProject *
			Project = (CProject *)PROJECT_TABLE->get(i);

			if ((Project->get_type() == TYPE_COUNCIL)&&(Project->evaluate(Speaker))&&(!has_project(Project->get_id())))
			{
				Buf.format("<OPTION VALUE = \"%d\">\n%s(%s %s)\n</OPTION>\n",
					Project->get_id(),
					Project->get_name(),
					dec2unit(Project->get_cost()),
					GETTEXT("PP"));
			}
		}
	}

	Buf += "</SELECT>\n";

	return (char*)Buf;
}

const char*
	CCouncil::buy_council_project(int aProjectID)
{
	CProject *
	Project = PROJECT_TABLE->get_by_id(aProjectID);

	if (!Project)
	{
		return GETTEXT("You seem to have purchased this project before.");
	}
	if (Project->get_type() != CProject::TYPE_COUNCIL)
	{
		return GETTEXT("The selected project is not a type of council project.");
	}

	if (Project->get_cost() > get_production())
		return GETTEXT("The council safe does not have enough stored PP.");

	CPlayer *
	Speaker = PLAYER_TABLE->get_by_game_id(mSpeakerID);
	if (!Project->evaluate(Speaker))
	{
		return GETTEXT("You don't have all the requirements for this council project.");
	}

	CPurchasedProject *BuyProject;
	BuyProject = new CPurchasedProject(Project);
	BuyProject->set_owner(mID);
	BuyProject->type(QUERY_INSERT);
	*STORE_CENTER << *BuyProject;

	add_project(BuyProject);

	for (int i=0 ; i<mMembers.length() ; i++)
	{
		CPlayer *
		Member = (CPlayer *)mMembers.get(i);
		if (Member->is_dead()) continue;

		Member->build_control_model();
	}

	change_production(-Project->get_cost());

	static CString
		Ret;

	Ret.clear();
	Ret.format(GETTEXT("You gained the project \"%1$s\" successfully."),
		Project->get_name());

	return (char*)Ret;
}

int
	CCouncil::can_buy_any_project()
{
	CPlayer *
	Speaker = get_speaker();
	if (Speaker == NULL) return -1;

	for (int i=0 ; i<PROJECT_TABLE->length() ; i++)
	{
		CProject *
		Project = (CProject *)PROJECT_TABLE->get(i);
		if (Project->evaluate(Speaker)) return 1;
	}
	return 0;
}

const char*
	CCouncil::vote_member_list_html(CPlayer *aPlayer)
{
	static CString Buffer;

	Buffer.clear();

	CString Vote;

	for(int i=0; i<mMembers.length(); i++)
	{
		CPlayer
			*Player = (CPlayer*)mMembers.get(i);
		if (Player->is_dead()) continue;

		if (Player->get_council_vote())
		{
			CPlayer* Support =
				get_member_by_game_id(Player->get_council_vote());
//			assert(Support);
			if(Support)
				Vote = Support->get_name();
			else
				Vote = GETTEXT("None");
		} else
			Vote = "None";

		if (aPlayer->get_game_id() == Player->get_game_id())
			Buffer.format(
			"<TR>\n"
			"<TD CLASS=\"white\" WIDTH=160>&nbsp;%s</TD>\n"
			"<TD WIDTH=127 CLASS=\"white\" ALIGN=CENTER>%s</TD>\n"
			"<TD WIDTH=127 CLASS=\"white\" ALIGN=CENTER>%s</TD>\n"
			"<TD CLASS=\"white\" WIDTH=75 ALIGN=CENTER>%d</TD>\n"
			"<TD CLASS=\"white\" WIDTH=49 ALIGN=CENTER>\n"
			"<INPUT TYPE=\"radio\" NAME=\"VOTE\" VALUE=\"%d\">\n"
			"</TD>\n"
			"</TR>\n",
			Player->get_nick(),
			Player->get_race_data()->get_name(),
			(char*)Vote,
			Player->get_council_vote_gain(),
			Player->get_game_id());
		else
			Buffer.format(
				"<TR>\n"
				"<TD CLASS=\"tabletxt\" WIDTH=160>&nbsp;%s</TD>\n"
				"<TD WIDTH=127 CLASS=\"tabletxt\" ALIGN=CENTER>%s</TD>\n"
				"<TD WIDTH=127 CLASS=\"tabletxt\" ALIGN=CENTER>%s</TD>\n"
				"<TD CLASS=\"tabletxt\" WIDTH=75 ALIGN=CENTER>%d</TD>\n"
				"<TD CLASS=\"tabletxt\" WIDTH=49 ALIGN=CENTER>\n"
				"<INPUT TYPE=\"radio\" NAME=\"VOTE\" VALUE=\"%d\">\n"
				"</TD>\n"
				"</TR>\n",
				Player->get_nick(),
				Player->get_race_data()->get_name(),
				(char*)Vote,
				Player->get_council_vote_gain(),
				Player->get_game_id());
	}

	return (char*)Buffer;
}

double
	CCouncil::total_vote_power()
{
	double
		TotalVote = 0;

	for( int i=0; i<mMembers.length(); i++)
	{
		CPlayer
			*Player = (CPlayer*)mMembers.get(i);
		TotalVote += Player->get_council_vote_power();
	}

	return TotalVote;
}

int
	CCouncil::process_vote()
{
	double
		ValidVote = 0,
	TotalVote = 0;

	for(int i=0; i<mMembers.length(); i++)
	{
		CPlayer *
		Player = (CPlayer *)mMembers.get(i);
		Player->set_council_vote_gain(NONE);
		TotalVote += Player->get_council_vote_power();
	}

	for(int i=0; i<mMembers.length(); i++)
	{
		CPlayer *
		Player = (CPlayer *)mMembers.get(i);
		CPlayer *
		Support = get_member_by_game_id(Player->get_council_vote());

		if (Support)
		{
			Support->change_council_vote_gain(Player->get_council_vote_power());
			ValidVote += Player->get_council_vote_power();
		}
	}

	int
		Speaker = 0;
	double
		SpeakerGain = 0;
	for(int i=0; i<mMembers.length(); i++)
	{
		CPlayer *
		Player = (CPlayer *)mMembers.get(i);

		if (Player->get_council_vote_gain() > SpeakerGain)
		{
			Speaker = Player->get_game_id();
			SpeakerGain = Player->get_council_vote_gain();
		}
	}

	//if (ValidVote > TotalVote/2 && SpeakerGain > ValidVote/2 &&( GAME->get_game_time()/GAME->mSecondPerTurn) > 250)
	if (ValidVote > TotalVote/2 && SpeakerGain > ValidVote/2)
		return Speaker;

	return NONE;
}

void
	CCouncil::refresh_nick()
{
	mNick.clear();
	mNick.format("%s (#%d)", (char *)mName, mID);
}

CPlayer *
CCouncil::get_speaker()
{
	if(mSpeakerID == EMPIRE_GAME_ID) return NULL;
	CPlayer *
	Speaker = PLAYER_TABLE->get_by_game_id(mSpeakerID);
	return Speaker;
}

CPlayer *
CCouncil::get_secondary_speaker()
{
	if(mSecondarySpeakerID == EMPIRE_GAME_ID) return NULL;
	CPlayer *
	SecondarySpeaker = PLAYER_TABLE->get_by_game_id(mSecondarySpeakerID);
	return SecondarySpeaker;
}

int
	CCouncil::collect_tax()
{
	int
		Tax = 0;

	for(int i=0; i<mMembers.length(); i++)
	{
		CPlayer
			*Player = (CPlayer*)mMembers.get(i);

		Tax += Player->get_council_production();
		Player->change_council_donation(Player->get_council_production());
		Player->set_council_production(NONE);
	}

	// take pping
	if (mSupremacyCouncil)
	{
		mSupremacyCouncil->change_production((int)(Tax/4));
		Tax = Tax * 3 / 4;
	}

	change_production( Tax );

	return Tax;
}


int
	CCouncil::max_member()
{
	int
		Max = 10;

	CPlayer *
	Speaker = mMembers.get_by_game_id(mSpeakerID);
	if (!Speaker) return Max;

	Max += Speaker->count_tech_by_category(CTech::TYPE_SOCIAL);

	if (Max > 40) Max = 40;

	return Max;
}

const char*
	CCouncil::get_honor_description()
{
	if (mHonor <= 5) return GETTEXT("Evil");
	else if (mHonor <= 10) return GETTEXT("Seed of Corruption");
	else if (mHonor <= 15) return GETTEXT("Sadistic");
	else if (mHonor <= 20) return GETTEXT("Black Hearted");
	else if (mHonor <= 25) return GETTEXT("Backstabber");
	else if (mHonor <= 30) return GETTEXT("Undependable");
	else if (mHonor <= 35) return GETTEXT("Untrustworthy");
	else if (mHonor <= 40) return GETTEXT("Unreliable");
	else if (mHonor <= 60) return GETTEXT("Average");
	else if (mHonor <= 65) return GETTEXT("Reliable");
	else if (mHonor <= 70) return GETTEXT("Honest");
	else if (mHonor <= 75) return GETTEXT("Faithful");
	else if (mHonor <= 80) return GETTEXT("Virtuous");
	else if (mHonor <= 85) return GETTEXT("Honorable");
	else if (mHonor <= 90) return GETTEXT("Incorruptible");
	else if (mHonor <= 95) return GETTEXT("Stainless");
	return GETTEXT("Saintly");
}

bool
	CCouncil::has_cluster()
{
	if (mClusterList.length()) return true;
	return false;
}

bool
	CCouncil::has_cluster(int aClusterID)
{
	int
		Index = mClusterList.find_sorted_key((TSomething)aClusterID);

	if (Index < 0) return false;

	return true;
}

bool
	CCouncil::add_cluster(CCluster* aCluster)
{
	int
		Temp = mClusterList.add_cluster(aCluster);
	if (Temp < 0) return false;

	aCluster->add_council(this);
	return true;
}

bool
	CCouncil::remove_cluster(int aClusterID)
{
	for (int i=0; i<mMembers.length(); i++)
	{
		CPlayer *
		Player = (CPlayer *)mMembers.get(i);

		if (Player->has_cluster(aClusterID)) return false;
	}

	if (!mClusterList.remove_cluster(aClusterID))
	{
		SLOG("WRONG: remove request with unknown cluster id(%d)",
			aClusterID);
		return false;
	}

	CCluster *Cluster = UNIVERSE->get_by_id(aClusterID);
	if (!Cluster)
	{
		SLOG("WRONG: could not found cluster id(%d)", aClusterID);
		return false;
	}

	Cluster->remove_council(get_id());

	return true;
}

void
	CCouncil::rearrange_cluster(bool aAll)
{
	// SLOG("YOSHIKI : CCouncil::rearrange_cluster()-1, mMembers.length() = %d", mMembers.length());
	mClusterList.remove_all();

	// SLOG("YOSHIKI : CCouncil::rearrange_cluster()-2, mMembers.length() = %d", mMembers.length());
	// regathering clusters
	for (int i=0 ; i<mMembers.length() ; i++)
	{
		CPlayer *
		Player = (CPlayer *)mMembers.get(i);
		if (aAll == true) Player->rearrange_cluster();

		//added by zbyte 2004-03-12
		for (int i=0 ; i<UNIVERSE->length() ; i++)
		{
			CCluster *
			Cluster = (CCluster *)UNIVERSE->get(i);
			if (Player->has_cluster(Cluster->get_id()))
			{
				if (mClusterList.get_by_id(Cluster->get_id()) == NULL)
				{
					add_cluster(Cluster);
				}
			}
		}
		//end of new code
	}

	for (int i=0 ; i<mMembers.length() ; i++)
	{
		CPlayer *
		Player = (CPlayer *)mMembers.get(i);
		Player->add_cluster_list(mClusterList);
	}
	// SLOG("YOSHIKI : CCouncil::rearrange_cluster()-3, mMembers.length() = %d", mMembers.length());
	// update cluster
	for (int i=0 ; i<UNIVERSE->length() ; i++)
	{
		CCluster *
		Cluster = (CCluster *)UNIVERSE->get(i);
		if (Cluster->find_council(mID) != NULL)
		{
			if (mClusterList.get_by_id(Cluster->get_id()) == NULL)
			{
				Cluster->remove_council(mID);
			}
		}
	}
	// SLOG("YOSHIKI : CCouncil::rearrange_cluster()-4, mMembers.length() = %d", mMembers.length());
}

const char*
	CCouncil::get_cluster_names()
{
	static CString Names;

	Names.clear();

	for(int i=0; i<mClusterList.length(); i++)
	{
		CCluster *Cluster = (CCluster*)mClusterList.get(i);
		if (Names.length()) Names += ", ";
		Names += Cluster->get_name();
	}

	return (char*)Names;
}

void
	CCouncil::send_time_news_to_all_member(const char* aNews)
{
	for(int i=0; i<mMembers.length(); i++)
	{
		CPlayer* Member = (CPlayer*)mMembers.get(i);
		Member->time_news(aNews);
	}
}

void
	CCouncil::update_by_time()
{
    // Just in case check this -- it occasionally seems to happen!!?
	CPlayer *CurrentSpeaker = PLAYER_TABLE->get_by_game_id(mSpeakerID);
	if (CurrentSpeaker == NULL)
		set_speaker(NONE);
	int
		NewSpeaker = process_vote();

	collect_tax();
	change_honor(1);

	if (NewSpeaker)
	{
		CPlayer *
		Speaker = get_member_by_game_id(NewSpeaker);
		assert(Speaker);

		// new speaker doesn't have an admission offer
		if (!Speaker->get_admission())
		{
			if (mSpeakerID != NewSpeaker)
			{
				set_speaker(NewSpeaker);
				// send news to council members.
				send_time_news_to_all_member(
					(char*)format(
						GETTEXT("%1$s became new council speaker."),
						Speaker->get_nick()));
			}
		} else
		{
			set_speaker(NONE);
			send_time_news_to_all_member(
				GETTEXT("Nobody has become new speaker<BR>because the winner of council election had an admission offer."));
		}
	} else
	{
		if (mSpeakerID == 0)
		{
			send_time_news_to_all_member(GETTEXT("Nobody has won the council election."));
		}
		set_speaker(NONE);
	}

	if (!mStoreFlag.is_empty())
	{
		type(QUERY_UPDATE);

		STORE_CENTER->store(*this);
	}

	time_t
	Now = time(0);

	// clear admission
	for(int i=mAdmissionList.length()-1 ; i>=0 ; i--)
	{
		CAdmission *
		Admission = (CAdmission *)mAdmissionList.get(i);
		CPlayer *
		Player = Admission->get_player();

		if (Now - Admission->get_time() > 2*ONE_DAY)
		{
			Player->reset_admission();
			Admission->type(QUERY_DELETE);
			STORE_CENTER->store(*Admission);
			mAdmissionList.remove_admission(Player->get_game_id());
		}
	}
}

void
	CCouncil::update_by_tick()
{
	if (!mStoreFlag.is_empty())
	{
		type(QUERY_UPDATE);

		STORE_CENTER->store(*this);
	}
}

int
	CCouncil::distribute(int aAmount, CString& List)
{
	assert(get_production() >= aAmount);

	double TotalVote = 0;
	int Total = 0;
	int AliveMember = 0;

	for(int i=0; i<mMembers.length(); i++)
	{
		CPlayer *Player = (CPlayer*)mMembers.get(i);
		if (Player->is_dead()) continue;

		TotalVote += Player->get_council_vote_power();
		AliveMember++;
	}

	List = " ";
	int HalfAmount = (int)(aAmount/2);

	int Each = (int)(HalfAmount/AliveMember);
	int Dist;
	if (Each > 0)
	{
		for(int i=0; i<mMembers.length(); i++)
		{
			CPlayer *Player = (CPlayer*)mMembers.get(i);
			if (Player->is_dead()) continue;

			Dist = Each;
			// add (double) to prevent overflow
			Dist += (int)((double)HalfAmount*Player->get_council_vote_power()/TotalVote);

			Player->change_reserved_production(Dist);
			change_production(-Dist);

			Player->time_news(
				(char*)format(GETTEXT("Your council distributed %1$s PP to you."),
					dec2unit(Dist)));
			Total += Dist;

			List += "<TR><TD CLASS=\"maintext\" WIDTH=183 VALIGN=top>\n";
			List.format("%s</TD>\n", Player->get_nick());
			List += "<TD CLASS=\"maintext\" WIDTH=74 VALIGN=top ALIGN=RIGHT>\n";
			List.format("%s</TD></TR>\n", dec2unit(Dist));
		}
	}

	return Total;
}

CAdmission *
CCouncil::get_admission_by_index(int aIndex)
{
	if (mAdmissionList.length() == 0) return NULL;
	if (aIndex >= mAdmissionList.length()) return NULL;

	return (CAdmission*)mAdmissionList.get(aIndex);
}

CAdmission *
CCouncil::get_admission_by_time(time_t aTime)
{
	if (mAdmissionList.length() == 0) return NULL;
	int
		Index = mAdmissionList.find_sorted_key((void *)aTime);
	if (Index < 0) return NULL;

	return (CAdmission *)mAdmissionList.get(Index);
}

void
	CCouncil::set_honor(int aHonor)
{
	mStoreFlag += STORE_HONOR;
	mHonor = (aHonor<0) ? 0: (aHonor>100) ? 100: aHonor;
}

void
	CCouncil::change_honor(int aChange)
{
	/*int Diplomacy = 0;
	if (mSpeakerID)
	{
		CPlayer* Speaker = get_member_by_game_id(mSpeakerID);
//		assert(Speaker);
		if( Speaker )
			Diplomacy = Speaker->get_control_model()->get_diplomacy();
	}

	int Change = aChange;
	if (Change < 0)
	{
		Change += (int) (Diplomacy / 3);
		if (Change < aChange*2) Change = aChange*2;
		if (Change >= 0) Change = -1;
	} else if (Change > 0)
	{
		Change += (int) (Diplomacy / 3);
		if (Change > aChange*2) Change = aChange*2;
		if (Change <= 0) Change = 1;
	} <- more sillyness. */

	/*if ((mHonor == 100) && (Change < 0))
		Change = (Change < -1) ? (int)(Change/2):-1;
	if ((mHonor == 0) && (Change > 0))
		Change = (Change > 1) ? (int)(Change/2):1; <- stupid? */

	set_honor(mHonor + aChange);
}

bool
	CCouncil::set_production(int aProduction)
{
	if (aProduction > MAX_COUNCIL_PP)
	{
		mProduction = MAX_PLAYER_PP;
	}
	else if (aProduction < 0)
	{
		mProduction = 0;
	}
	else
	{
		mProduction = aProduction;
	}

	mStoreFlag += STORE_PRODUCTION;
	return true;
}

void
	CCouncil::change_production(int aProduction)
{
	if (aProduction == 0) return;

	if (aProduction > 0)
	{
		if ((long long int)aProduction >
			(long long int)MAX_COUNCIL_PP - (long long int)mProduction)
		{
			mProduction = MAX_COUNCIL_PP;
		}
		else
		{
			mProduction += aProduction;
		}
	}
	else
	{
		if (mProduction + aProduction > MAX_COUNCIL_PP)
		{
			mProduction = MAX_COUNCIL_PP;
		}
		else if (mProduction < -aProduction)
		{
			mProduction = 0;
		}
		else
		{
			mProduction += aProduction;
		}
	}

	mStoreFlag += STORE_PRODUCTION;
}

const char*
	CCouncil::achieved_project_html()
{
	static CString
		HTML;
	HTML.clear();

	if (!mPurchasedProjectList.length())
		return GETTEXT("You don't have any achieved projects.");

	for(int i=0; i<mPurchasedProjectList.length(); i++)
	{
		CPurchasedProject *Project =
			(CPurchasedProject*)mPurchasedProjectList.get(i);
		HTML.format((HTML.length()) ? ", %s":"%s", Project->get_name());
	}

	return (char*)HTML;
}

void
	CCouncil::add_relation(CCouncilRelation* aRelation)
{
	if ((aRelation->get_relation() == CRelation::RELATION_SUBORDINARY)
		&& (aRelation->get_council2() == this))
		mSupremacyCouncil = aRelation->get_council1();

	mRelationList.add_relation(aRelation);
}

void
	CCouncil::remove_relation(int aRelationID)
{
	CCouncilRelation *Relation =
		(CCouncilRelation*)mRelationList.find_by_id(aRelationID);

	if ((Relation->get_relation() == CRelation::RELATION_SUBORDINARY)
		&& (Relation->get_council2() == this))
		mSupremacyCouncil = NULL;

	mRelationList.remove_relation(aRelationID);
}

const char *
	CCouncil::select_member_except_player_html(CPlayer *aPlayer)
{
	if (mMembers.length() <= 1) return NULL;

	static CString SelectMember;
	SelectMember.clear();

	SelectMember = "<SELECT NAME = \"PLAYER_ID\">\n";

	for(int i=0; i<mMembers.length(); i++)
	{
		CPlayer *
		Player = (CPlayer *)mMembers.get(i);
		if (Player->get_game_id() == aPlayer->get_game_id()) continue;
		// start telecard 2001/01/21
		if ( Player->is_dead() == TRUE )
		{
			continue;
		}
		// end telecard
		SelectMember.format("<OPTION VALUE = \"%d\">%s</OPTION>\n",
			Player->get_game_id(), Player->get_nick());
	}

	SelectMember += "</SELECT>\n";

	return (char *)SelectMember;
}

const char*
	CCouncil::player_relation_table_html(CPlayer *aPlayer)
{
	static CString HTML;

	if (mMembers.length() <= 1) return NULL;

	HTML.clear();

	HTML =
		"<TABLE WIDTH=550 BORDER=1 CELLSPACING=0 CELLPADDING=0 "
		"BORDERCOLOR=\"#2A2A2A\">\n";

	HTML +=
		"<TR>\n"
		"<TH WIDTH=106 CLASS=\"tabletxt\" BGCOLOR=\"#171717\">"
		"<FONT COLOR=\"666666\">";
	HTML += GETTEXT("Player");
	HTML +=
		"</FONT>"
		"</TH>\n"
		"<TH WIDTH=83 CLASS=\"tabletxt\" BGCOLOR=\"#171717\">"
		"<FONT COLOR=\"666666\">";
	HTML += GETTEXT("Relation");
	HTML +=
		"</FONT>"
		"</TH>\n"
		"<TH CLASS=\"tabletxt\" WIDTH=100 BGCOLOR=\"#171717\">"
		"<FONT COLOR=\"666666\">";
	HTML += GETTEXT("Supporting");
	HTML +=
		"</FONT>"
		"</TH>\n"
		"<TH CLASS=\"tabletxt\" WIDTH=82 BGCOLOR=\"#171717\">"
		"<FONT COLOR=\"666666\">";
	HTML += GETTEXT("Power");
	HTML +=
		"</FONT>"
		"</TH>\n"
		"<TH CLASS=\"tabletxt\" WIDTH=56 BGCOLOR=\"#171717\">"
		"<FONT COLOR=\"666666\">";
	HTML += GETTEXT("Voting Power");
	HTML +=
		"</FONT>"
		"</TH>\n"
		"<TH CLASS=\"tabletxt\" WIDTH=109 BGCOLOR=\"#171717\">"
		"<FONT COLOR=\"666666\">";
	HTML += GETTEXT("Honor");
	HTML +=
		"</FONT>"
		"</TH>\n"
		"</TR>\n";

	for(int i=0; i<mMembers.length(); i++)
	{
		CPlayer *
		Player = (CPlayer *)mMembers.get(i);
		if (Player->get_game_id() == aPlayer->get_game_id()) continue;
		if (Player->is_dead()) continue;

		CString
		Support;

		if (Player->get_council_vote())
		{
			CPlayer* SupportPlayer =
				get_member_by_game_id(Player->get_council_vote());
//			assert(SupportPlayer);
			if (SupportPlayer) Support = SupportPlayer->get_nick();
			else Support = GETTEXT("None");
		}
		else
		{
			Support = GETTEXT("None");
		}

		CPlayerRelation *
		Relation = aPlayer->get_relation(Player);

		HTML.format(
			"<TR>\n"
			"<TD WIDTH=106 CLASS=\"tabletxt\">"
			"&nbsp;%s"
			"</TD>\n"
			"<TD WIDTH=83 CLASS=\"tabletxt\">"
			"&nbsp;%s"
			"</TD>\n"
			"<TD CLASS=\"tabletxt\" WIDTH=100>"
			"&nbsp;%s"
			"</TD>\n"
			"<TD CLASS=\"tabletxt\" WIDTH=82 ALIGN=CENTER "
			"VALIGN=MIDDLE>"
			"%s"
			"</TD>\n"
			"<TD CLASS=\"tabletxt\" WIDTH=56 ALIGN=CENTER>"
			"%d"
			"</TD>"
			"<TD CLASS=\"tabletxt\" WIDTH=109 ALIGN=CENTER>"
			"%s"
			"</TD>\n"
			"</TR>",
			Player->get_nick(),
			(Relation) ? CRelation::get_relation_description(Relation->get_relation()) : CRelation::get_relation_description(CRelation::RELATION_NONE),
			(char*)Support,
			dec2unit(Player->get_power()),
			Player->get_council_vote_power(),
			Player->get_honor_description());
	}

	HTML += "</TABLE>\n";

	return (char*)HTML;
}

void
	CCouncil::add_action(CCouncilAction *aAction)
{
	mActionList.add_action(aAction);
}

void
	CCouncil::remove_action(int aActionID)
{
	mActionList.remove_action(aActionID);
}

const char*
	CCouncil::council_relation_table_html()
{
	static CString HTML;
	HTML.clear();

	CString
		TotalWarList,
		WarList,
		TruceList,
		PeaceList,
		AllyList,
		SupremacyList,
		SubordinaryList;

	for(int i=0; i<mRelationList.length(); i++)
	{
		CCouncilRelation *Relation =
			(CCouncilRelation*)mRelationList.get(i);
		assert(Relation);

		switch(Relation->get_relation())
		{
			case CRelation::RELATION_NONE:
				SLOG("WARNING RELATION_NONE");
				break;
			case CRelation::RELATION_SUBORDINARY:
				if (Relation->get_council2() == this)
				{
					if (SupremacyList.length()) SupremacyList += ", ";
					SupremacyList += Relation->get_council1()->get_nick();
				} else {
					if (SubordinaryList.length()) SubordinaryList += ", ";
					SubordinaryList += Relation->get_council1()->get_nick();
				}
				break;
			case CRelation::RELATION_ALLY:
				if (AllyList.length()) AllyList += ", ";
				AllyList += (Relation->get_council1() == this) ?
					Relation->get_council2()->get_nick():
					Relation->get_council1()->get_nick();
				break;
			case CRelation::RELATION_PEACE:
				if (PeaceList.length()) PeaceList += ", ";
				PeaceList += (Relation->get_council1() == this) ?
					Relation->get_council2()->get_nick():
					Relation->get_council1()->get_nick();
				break;
			case CRelation::RELATION_TRUCE:
				if (TruceList.length()) TruceList += ", ";
				TruceList += (Relation->get_council1() == this) ?
					Relation->get_council2()->get_nick():
					Relation->get_council1()->get_nick();
				break;
			case CRelation::RELATION_WAR:
				if (WarList.length()) WarList += ", ";
				WarList += (Relation->get_council1() == this) ?
					Relation->get_council2()->get_nick():
					Relation->get_council1()->get_nick();
				break;
			case CRelation::RELATION_TOTAL_WAR:
				if (TotalWarList.length()) TotalWarList += ", ";
				TotalWarList += (Relation->get_council1() == this) ?
					Relation->get_council2()->get_nick():
					Relation->get_council1()->get_nick();
				break;
		}
	}


	HTML += "<TR>\n"
		"<TD CLASS=tabletxt WIDTH=96 ALIGN=CENTER>&nbsp;";
	HTML += GETTEXT("Total War");
	HTML += "</TD>\n"
		"<TD COLSPAN=3 CLASS=tabletxt WIDTH=418>&nbsp;&nbsp;";
	HTML += (TotalWarList.length()) ? (char*)TotalWarList:GETTEXT("None");
	HTML += "</TD>\n"
		"</TR>\n"
		"<TR>\n"
		"<TD CLASS=tabletxt WIDTH=96 ALIGN=CENTER>&nbsp;";
	HTML += GETTEXT("War");
	HTML += "</TD>\n"
		"<TD COLSPAN=3 CLASS=tabletxt WIDTH=418>&nbsp;&nbsp;";
	HTML += (WarList.length()) ? (char*)WarList:GETTEXT("None");
	HTML += "</TD>\n"
		"</TR>\n"
		"<TR>\n"
		"<TD CLASS=tabletxt WIDTH=96 ALIGN=CENTER>&nbsp;";
	HTML += GETTEXT("Truce");
	HTML += "</TD>\n"
		"<TD COLSPAN=3 CLASS=tabletxt WIDTH=418>&nbsp;&nbsp;";
	HTML += (TruceList.length()) ? (char*)TruceList:GETTEXT("None");
	HTML += "</TD>\n"
		"</TR>\n"
		"<TR>\n"
		"<TD CLASS=tabletxt WIDTH=96 ALIGN=CENTER>&nbsp;";
	HTML += GETTEXT("Peace");
	HTML += "</TD>\n"
		"<TD COLSPAN=3 CLASS=tabletxt WIDTH=418>&nbsp;&nbsp;";
	HTML += (PeaceList.length()) ? (char*)PeaceList:GETTEXT("None");
	HTML += "</TD>\n"
		"</TR>\n"
		"<TR>\n"
		"<TD CLASS=tabletxt WIDTH=96 ALIGN=CENTER>&nbsp;";
	HTML += GETTEXT("Alliance");
	HTML += "</TD>\n"
		"<TD COLSPAN=3 CLASS=tabletxt WIDTH=418>&nbsp;&nbsp;";
	HTML += (AllyList.length()) ? (char*)AllyList:GETTEXT("None");
	HTML += "</TD>\n"
		"</TR>\n"
		"<TR>\n"
		"<TD CLASS=tabletxt WIDTH=96 ALIGN=CENTER>&nbsp;";
	HTML += GETTEXT("Supremacy");
	HTML += "</TD>\n"
		"<TD COLSPAN=3 CLASS=tabletxt WIDTH=418>&nbsp;&nbsp;";
	HTML += (SupremacyList.length()) ? (char*)SupremacyList:GETTEXT("None");
	HTML += "</TD>\n"
		"</TR>\n"
		"<TR>\n"
		"<TD CLASS=tabletxt WIDTH=96 ALIGN=CENTER>&nbsp;";
	HTML += GETTEXT("Subordinary");
	HTML += "</TD>\n"
		"<TD COLSPAN=3 CLASS=tabletxt WIDTH=418>&nbsp;&nbsp;";
	HTML += (SubordinaryList.length()) ?
		(char*)SubordinaryList:GETTEXT("None");
	HTML += "</TD>\n"
		"</TR>\n";

	return (char*)HTML;
}

const char *
	CCouncil::select_council_html()
{
	CCouncilList CouncilList;

	for (int i=0 ; i<mClusterList.length() ; i++)
	{
		CCluster *
		Cluster = (CCluster *)mClusterList.get(i);
		CCouncilList *
		CL = Cluster->get_council_list();
		if (CL == NULL)
		{
			SLOG("ERROR : CL is NULL in CCouncil::select_council_html(), Cluster->get_id() = %d", Cluster->get_id());
			continue;
		}

		for (int j=0 ; j<CL->length() ; j++)
		{
			CCouncil *
			Council = (CCouncil *)CL->get(j);
			CouncilList.add_council(Council);
		}
	}

	if (CouncilList.length() == 0) return NULL;

	static CString
		HTML;

	HTML = "<SELECT NAME=COUNCIL_ID>\n";
	for (int i=0 ; i<CouncilList.length() ; i++)
	{
		CCouncil *
		Council = (CCouncil *)CouncilList.get(i);
		if (Council == this) continue;

		HTML.format("<OPTION VALUE=%d>%s</OPTION>\n",
			Council->get_id(), Council->get_nick());
	}
	HTML += "</SELECT>\n";

	return (char*)HTML;
}

CCouncil*
CCouncil::relation_council(int aCouncilID)
{
	if (aCouncilID == mID) return NULL;

	CCouncilList *CL;
	CCouncil *Council;
	for(int i=0; i<mClusterList.length(); i++)
	{
		CCluster *Cluster = (CCluster*)mClusterList.get(i);
		assert(Cluster);

		CL = Cluster->get_council_list();
		assert(CL);

		Council = CL->get_by_id(aCouncilID);
		if (Council) return Council;
	}

	return NULL;
}

CCouncilRelation*
CCouncil::get_relation(CCouncil* aCouncil)
{
	CCouncilRelation* Relation
		= mRelationList.find_by_target_id(aCouncil->get_id());

	return Relation;
}

const char*
	CCouncil::top_three_list_html()
{
	CMemberRankList RankList;

	static CString HTML;

	HTML = " ";

	for(int i=0; i<mMembers.length(); i++)
		RankList.add_player((CPlayer*)mMembers.get(i));

	for(int i=0; i<3; i++)
	{
		CPlayer *Player = (CPlayer*)RankList.get(i);
		if (!Player) continue;
		HTML += "<TR ALIGN=\"CENTER\">"
			"<TD CLASS=\"tabletxt\" WIDTH=\"151\">";
		HTML += Player->get_nick();
		HTML +=	"</TD>"
			"<TD CLASS=\"tabletxt\" WIDTH=\"82\">&nbsp;&nbsp;";
		HTML += Player->get_race_name();
		HTML +=	"&nbsp;</TD>"
			"<TD CLASS=\"tabletxt\" WIDTH=\"80\">";
		HTML += dec2unit(Player->get_power());
		HTML += "</TD>"
			"<TD CLASS=\"tabletxt\" WIDTH=\"165\">";
		HTML.format("%d", i+1);
		HTML +=	"</TD>"
			"</TR>";
	}

	return (char*)HTML;
}

void
	CCouncil::add_council_message(CCouncilMessage* aMessage)
{
	mMessageBox.add_council_message(aMessage);
}

CCouncilMessage *
CCouncil::get_council_message(int aMessageID)
{
	return mMessageBox.get_by_id(aMessageID);
}

void
	CCouncil::remove_council_message(int aMessageID)
{
	mMessageBox.remove_council_message(aMessageID);
}

const char*
	CCouncil::council_message_select_html(CCouncil *aCouncil)
{
	CCouncilRelation *Relation = get_relation(aCouncil);

	const char *HTML;

	HTML = CCouncilMessage::message_select_html(Relation, this);

	if (!HTML) return " ";

	return HTML;
}

char *
	CCouncil::check_council_message(CCouncilMessage::EType aType, CCouncil* aCouncil, CCouncil *aActiveCouncil)
{
	if (aType == CCouncilMessage::TYPE_NORMAL) return NULL;

	CCouncilRelation *
	Relation = get_relation(aCouncil);

	CRelation::ERelation Relationship;

	if (!Relation)
		Relationship = CRelation::RELATION_TRUCE;
	else
		Relationship = Relation->get_relation();

	switch (aType)
	{
		case CCouncilMessage::TYPE_SUGGEST_TRUCE :
			{
				if (Relationship == CRelation::RELATION_WAR ||
					Relationship == CRelation::RELATION_TOTAL_WAR) return NULL;
			}
			break;

		case CCouncilMessage::TYPE_DEMAND_TRUCE :
			{
				if (Relationship == CRelation::RELATION_WAR ||
					Relationship == CRelation::RELATION_TOTAL_WAR) return NULL;
			}
			break;

		case CCouncilMessage::TYPE_DEMAND_SUBMISSION :
			{
				if (Relationship == CRelation::RELATION_WAR ||
					Relationship == CRelation::RELATION_TRUCE ||
					Relationship == CRelation::RELATION_PEACE ||
					Relationship == CRelation::RELATION_TOTAL_WAR) return NULL;
			}
			break;

		case CCouncilMessage::TYPE_DECLARE_TOTAL_WAR :
			{
				if (Relationship == CRelation::RELATION_WAR) return NULL;
			}
			break;

		case CCouncilMessage::TYPE_DECLARE_WAR :
			{
				if (Relationship == CRelation::RELATION_TRUCE) return NULL;
			}
			break;

		case CCouncilMessage::TYPE_SUGGEST_PACT :
			{
				if (Relationship == CRelation::RELATION_TRUCE) return NULL;
			}
			break;

		case CCouncilMessage::TYPE_BREAK_PACT :
			{
				if (Relationship == CRelation::RELATION_PEACE) return NULL;
			}
			break;

		case CCouncilMessage::TYPE_SUGGEST_ALLY :
			{
				if (Relationship == CRelation::RELATION_PEACE) return NULL;
			}
			break;

		case CCouncilMessage::TYPE_BREAK_ALLY :
			{
				if (Relationship == CRelation::RELATION_ALLY) return NULL;
			}
			break;

		case CCouncilMessage::TYPE_COUNCIL_FUSION_OFFER :
			{
				if (mPower >= aCouncil->get_power())
				{
					return can_merge_with(aCouncil, aActiveCouncil);
				}
				else
				{
					return aCouncil->can_merge_with(this, aActiveCouncil);
				}
			}
			break;

		case CCouncilMessage::TYPE_SET_FREE_SUBORDINARY :
			{
				if (Relationship == CRelation::RELATION_SUBORDINARY &&
					Relation->get_council1() == this) return NULL;
			}
			break;

		case CCouncilMessage::TYPE_DECLARE_INDEPENDENCE :
			{
				if (Relationship == CRelation::RELATION_SUBORDINARY &&
					Relation->get_council2() == this) return NULL;
			}
			break;

		default :
			break;
	}

	return (char *)format(GETTEXT("The message type(%1$s) is not available."),
		CCouncilMessage::get_type_description(aType));
}

bool
	CCouncil::check_duplication_diplomatic_message(CCouncilMessage::EType aType, CCouncil *aCouncil)
{
	if (CCouncilMessage::check_need_required(aType) == false) return false;

	for (int i=0 ; i<mMessageBox.length() ; i++)
	{
		CCouncilMessage *
		Message = (CCouncilMessage *)mMessageBox.get(i);
		if (Message->get_type() == aType && aCouncil == Message->get_sender())
		{
			if (Message->check_need_required() == true)
			{
				return true;
			}
		}
	}
	return false;
}

CCouncilAction*
CCouncil::check_declare_total_war_effect_timer(CCouncil *aCouncil)
{
	return mActionList.get_declare_total_war_action(aCouncil->get_id());
}

void
	CCouncil::set_declare_total_war_effect_timer(CCouncil *aCouncil)
{
	CCouncilActionDeclareTotalWar *Action
		= new CCouncilActionDeclareTotalWar();

	Action->set_owner(this);
	Action->set_start_time(CGame::get_game_time());
	Action->set_argument(aCouncil->get_id());
	Action->type(QUERY_INSERT);
	STORE_CENTER->store(*Action);

	mActionList.add_action(Action);
	COUNCIL_ACTION_TABLE->add_action(Action);
}

CCouncilAction*
CCouncil::check_declare_war_effect_timer(CCouncil *aCouncil)
{
	return mActionList.get_declare_war_action(aCouncil->get_id());
}

void
	CCouncil::set_declare_war_effect_timer(CCouncil *aCouncil)
{
	CCouncilActionDeclareWar *Action
		= new CCouncilActionDeclareWar();

	Action->set_owner(this);
	Action->set_start_time(CGame::get_game_time());
	Action->set_argument(aCouncil->get_id());
	Action->type(QUERY_INSERT);
	STORE_CENTER->store(*Action);

	mActionList.add_action(Action);
	COUNCIL_ACTION_TABLE->add_action(Action);
}


CCouncilAction*
CCouncil::check_break_ally_effect_timer(CCouncil *aCouncil)
{
	return mActionList.get_break_ally_action(aCouncil->get_id());
}

void
	CCouncil::set_break_ally_effect_timer(CCouncil *aCouncil)
{
	CCouncilActionBreakAlly *Action = new CCouncilActionBreakAlly();

	Action->set_owner(this);
	Action->set_start_time(CGame::get_game_time());
	Action->set_argument(aCouncil->get_id());
	Action->type(QUERY_INSERT);
	STORE_CENTER->store(*Action);

	mActionList.add_action(Action);
	COUNCIL_ACTION_TABLE->add_action(Action);
}

CCouncilAction*
CCouncil::check_break_pact_effect_timer(CCouncil *aCouncil)
{
	return mActionList.get_break_pact_action(aCouncil->get_id());
}

void
	CCouncil::set_break_pact_effect_timer(CCouncil *aCouncil)
{
	CCouncilActionBreakPact *Action = new CCouncilActionBreakPact();

	Action->set_owner(this);
	Action->set_start_time(CGame::get_game_time());
	Action->set_argument(aCouncil->get_id());
	Action->type(QUERY_INSERT);
	STORE_CENTER->store(*Action);

	mActionList.add_action(Action);
	COUNCIL_ACTION_TABLE->add_action(Action);
}

CCouncilAction*
CCouncil::check_improve_relation_effect_timer(CCouncil *aCouncil)
{
	return mActionList.get_break_pact_action(aCouncil->get_id());
}

void
	CCouncil::clear_improve_relation_effect_timer(CCouncil *aCouncil)
{
	CCouncilAction *Action =
		check_improve_relation_effect_timer(aCouncil);
	if (!Action) return;

	Action->type(QUERY_DELETE);
	STORE_CENTER->store(*Action);
	remove_action(Action->get_id());
	COUNCIL_ACTION_TABLE->remove_action(Action->get_id());
}

void
	CCouncil::set_improve_relation_effect_timer(CCouncil *aCouncil)
{
	if (check_improve_relation_effect_timer(aCouncil)) return;

	CCouncilActionImproveRelation *Action = new CCouncilActionImproveRelation();

	Action->set_owner(this);
	Action->set_start_time(CGame::get_game_time());
	Action->set_argument(aCouncil->get_id());
	Action->type(QUERY_INSERT);
	STORE_CENTER->store(*Action);

	mActionList.add_action(Action);
	COUNCIL_ACTION_TABLE->add_action(Action);
}



void
	CCouncil::change_honor_with_news(int aChange)
{
	CString
		Honor = get_honor_description();

//	int
//		OldHonor = mHonor; //no longer used

	change_honor(aChange);

	if (!Honor.compare(get_honor_description()))
	{
		CString News;
		News.format(GETTEXT("Your council's honor has become %1$s by your breach of faith."),
			get_honor_description());
		//News.format("SUPPOSED HONOR LOSS: %d, REAL HONOR LOSS: %d", aChange, OldHonor-mHonor); // debug
		send_time_news_to_all_member((char*)News);
	}
}

void
	CCouncil::declare_war(CCouncil* aCouncil)
{
	CCouncilRelation* Relation = get_relation(aCouncil);

	//SLOG("Declare war");

	if (Relation)
	{
		Relation->set_relation(CRelation::RELATION_WAR);
		Relation->set_time(CGame::get_game_time());
		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);
		//SLOG("Update Relation-Declare war");
	} else {
		Relation = new CCouncilRelation(this, aCouncil,
				CRelation::RELATION_WAR);

		aCouncil->add_relation(Relation);
		add_relation(Relation);
		COUNCIL_RELATION_TABLE->add_relation(Relation);
		Relation->type(QUERY_INSERT);
		STORE_CENTER->store(*Relation);
		//SLOG("Insert Relation-Declare war");
	}

	// new honor loss punishment system
	int	HonorLoss = 0;
	int	AttackRatio = (long long int)aCouncil->get_power()*100/(long long int)get_power();

	if (AttackRatio < 60) HonorLoss += 8;
	else if (AttackRatio < 70) HonorLoss += 5;
	else if (AttackRatio < 80) HonorLoss += 3;
	else if (AttackRatio < 90) HonorLoss += 2;
	else if (AttackRatio < 100) HonorLoss += 1;

	// modify the loss by the difference of honor divided by 6
	HonorLoss += (int)((aCouncil->get_honor()-get_honor())/6);

	set_declare_war_effect_timer(aCouncil);

	CCouncilAction *Action = check_break_pact_effect_timer(aCouncil);
	if (Action)
	{
		if (Action->get_start_time() <= PERIOD_24HOURS*CGame::mSecondPerTurn)
			HonorLoss += 6;
		else if (Action->get_start_time() <= PERIOD_48HOURS*CGame::mSecondPerTurn)
			HonorLoss += 3;
		else // over 48 hours
			HonorLoss += 1;

		Action->type(QUERY_DELETE);
		STORE_CENTER->store(*Action);
		remove_action(Action->get_id());
		COUNCIL_ACTION_TABLE->remove_action(Action->get_id());
	}

	if (HonorLoss > 0) change_honor_with_news(-HonorLoss);
	if (get_honor() <= 5 && get_speaker()->get_honor() > 50) {
        get_speaker()->change_honor(-HonorLoss/2);
        if (get_speaker()->get_honor() < 50) {
            get_speaker()->set_honor(50);
        }
    }

	send_time_news_to_all_member(
		(char*)format(GETTEXT("Your council has declared war against %1$s."),
			aCouncil->get_nick()));
	aCouncil->send_time_news_to_all_member(
		(char*)format(GETTEXT(
				"%1$s council has declared war with your council."),
			get_nick()));

	clear_improve_relation_effect_timer(aCouncil);
}

void
	CCouncil::declare_total_war(CCouncil* aCouncil)
{
	CCouncilRelation* Relation = get_relation(aCouncil);

	if (Relation)
	{
		Relation->set_relation(CRelation::RELATION_TOTAL_WAR);
		Relation->set_time(CGame::get_game_time());
		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);
	} else {
		Relation = new CCouncilRelation(this, aCouncil,
				CRelation::RELATION_TOTAL_WAR);

		aCouncil->add_relation(Relation);
		add_relation(Relation);
		COUNCIL_RELATION_TABLE->add_relation(Relation);
		Relation->type(QUERY_INSERT);
		STORE_CENTER->store(*Relation);
	}

	int
		HonorLoss = 0;

	// modify the loss by the difference of honor divided by 6
	HonorLoss += (int)((aCouncil->get_honor()-get_honor())/6);

	set_declare_total_war_effect_timer(aCouncil);

	CCouncilAction *Action = check_declare_war_effect_timer(aCouncil);
	if (Action)
	{
		if (Action->get_start_time() <= PERIOD_24HOURS*CGame::mSecondPerTurn)
			HonorLoss += 4;
		else if (Action->get_start_time() <= PERIOD_48HOURS*CGame::mSecondPerTurn)
			HonorLoss += 2;
		else // over 48 hours
			HonorLoss += 1;

		Action->type(QUERY_DELETE);
		STORE_CENTER->store(*Action);
		remove_action(Action->get_id());
		COUNCIL_ACTION_TABLE->remove_action(Action->get_id());
	}

	if (HonorLoss > 0) change_honor_with_news(-HonorLoss);

	send_time_news_to_all_member(
		(char*)format(GETTEXT("Your council has declared total war against %1$s."),
			aCouncil->get_nick()));
	aCouncil->send_time_news_to_all_member(
		(char*)format(GETTEXT(
				"%1$s council has declared total war to your council."),
			get_nick()));

	clear_improve_relation_effect_timer(aCouncil);
}

void
	CCouncil::break_pact(CCouncil* aCouncil)
{
	CCouncilRelation* Relation = get_relation(aCouncil);

	if (Relation)
	{
		if (Relation->get_relation() != CRelation::RELATION_PEACE)
		{
			SLOG("WARNING call break pact in NON PEACE");
			return;
		}
		Relation->set_relation(CRelation::RELATION_TRUCE);
		Relation->set_time(CGame::get_game_time());
		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);
	} else {
		SLOG("WARNING call break pact in NON_RELATION");
		return;
	}

	int
		HonorLoss = 0;

	// modify the loss by the difference of honor divided by 6
	HonorLoss += (int)((aCouncil->get_honor()-get_honor())/6);

	CCouncilAction *Action = check_break_ally_effect_timer(aCouncil);
	if (Action)
	{
		if (Action->get_start_time() <= PERIOD_24HOURS*CGame::mSecondPerTurn)
			HonorLoss += 6;
		else if (Action->get_start_time() <= PERIOD_48HOURS*CGame::mSecondPerTurn)
			HonorLoss += 4;
		else // over 48 hours
			HonorLoss += 2;

		Action->type(QUERY_DELETE);
		STORE_CENTER->store(*Action);
		remove_action(Action->get_id());
		COUNCIL_ACTION_TABLE->remove_action(Action->get_id());
	}

	if (HonorLoss > 0) change_honor_with_news(-HonorLoss);

	set_break_pact_effect_timer(aCouncil);

	send_time_news_to_all_member((char*)format(
			GETTEXT("Your council has broken the pact with %1$s council."),
			aCouncil->get_nick()));
	aCouncil->send_time_news_to_all_member((char*)format(
			GETTEXT("%1$s council has broken the pact."), get_nick()));

	clear_commerce_with(aCouncil);
	clear_improve_relation_effect_timer(aCouncil);
}

void
	CCouncil::break_ally(CCouncil* aCouncil)
{
	CCouncilRelation* Relation = get_relation(aCouncil);

	if (Relation)
	{
		if (Relation->get_relation() != CRelation::RELATION_ALLY)
		{
			SLOG("WARNING call break ally in NON ALLY");
			return;
		}
		Relation->set_relation(CRelation::RELATION_PEACE);
		Relation->set_time(CGame::get_game_time());
		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);
	} else {
		SLOG("WARNING call break pact in NON_RELATION");
		return;
	}

	int
		HonorLoss = 0;

	// modify the loss by the difference of honor divided by 6
	HonorLoss += (int)((aCouncil->get_honor()-get_honor())/6);

	if (HonorLoss > 0) change_honor_with_news(-HonorLoss);

	set_break_ally_effect_timer(aCouncil);

	send_time_news_to_all_member((char*)format(
			GETTEXT("Your council has broken the alliance with %1$s council."),
			aCouncil->get_nick()));
	aCouncil->send_time_news_to_all_member(
		(char *)format(GETTEXT("%1$s council has broken the alliance."),
			get_nick()));

	clear_improve_relation_effect_timer(aCouncil);
}

void
	CCouncil::set_free_subordinary(CCouncil* aCouncil)
{
	CCouncilRelation* Relation = get_relation(aCouncil);

	if (Relation)
	{
		if (Relation->get_relation() != CRelation::RELATION_SUBORDINARY)
		{
			SLOG("WARNING call break ally in NON SUBORDINARY");
			return;
		}
		if (Relation->get_council2() != aCouncil)
		{
			SLOG("WARNING NOT SUPREMACY COUNCIL");
			return;
		}
		Relation->set_relation(CRelation::RELATION_TRUCE);
		Relation->set_time(CGame::get_game_time());
		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);
	} else {
		SLOG("WARNING call break pact in NON_RELATION");
		return;
	}

	set_improve_relation_effect_timer(aCouncil);

	send_time_news_to_all_member((char*)format(
			GETTEXT("Your council has released %1$s council."),
			aCouncil->get_nick()));
	aCouncil->send_time_news_to_all_member((char*)format(
			GETTEXT("Your council is released under rule of %1$s council"), get_nick()));
}

const char*
	CCouncil::council_unread_message_table_html()
{
	if (!mMessageBox.length()) return NULL;

	static CString Table;

	Table.clear();

	for(int i=0; i<mMessageBox.length(); i++)
	{
		CCouncilMessage *Message =
			(CCouncilMessage*)mMessageBox.get(i);

		assert(Message);

		if (Message->get_status() == CCouncilMessage::STATUS_UNREAD)
		{
			Table += Message->record_html();
		}
	}

	if (!Table.length()) return NULL;

	return (char*)Table;
}


const char *
	CCouncil::get_message_list_html(int aFirst, bool aIsForAdmin)
{
	(void)aFirst;
	static CString HTML;

	if (mMessageBox.length() == 0) return NULL;

	CString
	Records;
	int
		Index = 0;

	while (Index < mMessageBox.length())
	{
		CCouncilMessage *
		Message =
			(CCouncilMessage*)mMessageBox.get(Index);
		assert(Message);
		if (Message->get_status() != CCouncilMessage::STATUS_WELL)
		{
			if ((time(0) - Message->get_time()) > ONE_WEEK)
			{
				Message->type(QUERY_DELETE);
				STORE_CENTER->store(*Message);

				mMessageBox.remove_council_message(Message->get_id());
				continue;
			}
		}
		Records += Message->record_html(aIsForAdmin);
		Index++;
	}

	if (Records.length() == 0) return NULL;

	HTML = "<TR>\n";
	HTML += "<TD ALIGN=center CLASS=maintext COLSPAN=3>\n";

	HTML += "<TABLE WIDTH=529 BORDER=0 CELLSPACING=0 CELLPADDING=0>\n";
	HTML += "<TR VALIGN=TOP>\n";
	HTML += "<TD CLASS=maintext HEIGHT=24 WIDTH=244>&nbsp;</TD>\n";
	HTML += "<TD CLASS=maintext HEIGHT=24 WIDTH=28 ALIGN=RIGHT>&nbsp;</TD>\n";
	HTML += "<TD CLASS=maintext HEIGHT=24 WIDTH=257 ALIGN=RIGHT>\n";
	HTML += GETTEXT("Reply Required");
	HTML.format("<IMG SRC=%s/image/as_game/council/required1_icon.gif WIDTH=19 HEIGHT=16 ALIGN=ABSBOTTOM>\n&nbsp;&nbsp;&nbsp;&nbsp;",
		(char *)CGame::mImageServerURL);
	HTML += GETTEXT("Not Required");
	HTML.format("<IMG SRC=%s/image/as_game/council/notrequired1_icon.gif WIDTH=19 HEIGHT=16 ALIGN=ABSBOTTOM>",
		(char *)CGame::mImageServerURL);
	HTML += "</TD>\n";
	HTML += "</TR>\n";

	HTML += "</TABLE>\n";

	HTML += "</TD>\n";
	HTML += "</TR>\n";

	HTML += "<TR>\n";
	HTML += "<TD ALIGN=center COLSPAN=3>\n";

	HTML += "<TABLE WIDTH=550 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	HTML += "<TR>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=32 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>&nbsp;%s</FONT>\n", GETTEXT("ID"));
	HTML += "</TH>\n";

	HTML += "<TH WIDTH=149 CLASS=tabletxt BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("CouncilName"));
	HTML += "</TH>\n";

	HTML += "<TH WIDTH=119 CLASS=tabletxt BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Date"));
	HTML += "</TH>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=119 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Type"));
	HTML += "</TH>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=47 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Status"));
	HTML += "</TH>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=70 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Read"));
	HTML += "</TH>\n";

	HTML += "</TR>\n";

	HTML += Records;

	HTML += "</TABLE>\n";

	HTML += "</TD>\n";
	HTML += "</TR>\n";

	HTML += "<TR>\n";
	HTML += "<TD CLASS=maintext ALIGN=center COLSPAN=3>&nbsp;</TD>\n";
	HTML += "</TR>\n";

	return (char *)HTML;
}

char *
	CCouncil::get_message_list_by_sender_html(CCouncil *aSender, bool aIsForAdmin)
{
	static CString
		HTML;
	HTML.clear();

	CString
		Records;
	for (int i=0 ; i<COUNCIL_TABLE->length() ; i++)
	{
		CCouncil *
		TempCouncil = (CCouncil *)COUNCIL_TABLE->get(i);
		CCouncilMessageBox *
		MessageBox = TempCouncil->get_council_message_box();
		for (int j=0 ; j<MessageBox->length() ; j++)
		{
			CCouncilMessage *
			Message = (CCouncilMessage *)MessageBox->get(j);
			if (Message->get_sender()->get_id() == aSender->get_id())
			{
				Records += Message->record_html(aIsForAdmin);
			}
		}
	}

	if (Records.length() == 0) return NULL;

	HTML = "<TR>\n";
	HTML += "<TD ALIGN=center CLASS=maintext COLSPAN=3>\n";

	HTML += "<TABLE WIDTH=529 BORDER=0 CELLSPACING=0 CELLPADDING=0>\n";
	HTML += "<TR VALIGN=TOP>\n";
	HTML += "<TD CLASS=maintext HEIGHT=24 WIDTH=244>&nbsp;</TD>\n";
	HTML += "<TD CLASS=maintext HEIGHT=24 WIDTH=28 ALIGN=RIGHT>&nbsp;</TD>\n";
	HTML += "<TD CLASS=maintext HEIGHT=24 WIDTH=257 ALIGN=RIGHT>\n";
	HTML += GETTEXT("Reply Required");
	HTML.format("<IMG SRC=%s/image/as_game/council/required1_icon.gif WIDTH=19 HEIGHT=16 ALIGN=ABSBOTTOM>\n&nbsp;&nbsp;&nbsp;&nbsp;",
		(char *)CGame::mImageServerURL);
	HTML += GETTEXT("Not Required");
	HTML.format("<IMG SRC=%s/image/as_game/council/notrequired1_icon.gif WIDTH=19 HEIGHT=16 ALIGN=ABSBOTTOM>",
		(char *)CGame::mImageServerURL);
	HTML += "</TD>\n";
	HTML += "</TR>\n";

	HTML += "</TABLE>\n";

	HTML += "</TD>\n";
	HTML += "</TR>\n";

	HTML += "<TR>\n";
	HTML += "<TD ALIGN=center COLSPAN=3>\n";

	HTML += "<TABLE WIDTH=550 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	HTML += "<TR>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=32 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>&nbsp;%s</FONT>\n", GETTEXT("ID"));
	HTML += "</TH>\n";

	HTML += "<TH WIDTH=149 CLASS=tabletxt BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("CouncilName"));
	HTML += "</TH>\n";

	HTML += "<TH WIDTH=119 CLASS=tabletxt BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Date"));
	HTML += "</TH>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=119 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Type"));
	HTML += "</TH>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=47 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Status"));
	HTML += "</TH>\n";

	HTML += "<TH CLASS=tabletxt WIDTH=70 BGCOLOR=#171717>\n";
	HTML.format("<FONT COLOR=666666>%s</FONT>\n", GETTEXT("Read"));
	HTML += "</TH>\n";

	HTML += "</TR>\n";

	HTML += Records;

	HTML += "</TABLE>\n";

	HTML += "</TD>\n";
	HTML += "</TR>\n";

	HTML += "<TR>\n";
	HTML += "<TD CLASS=maintext ALIGN=center COLSPAN=3>&nbsp;</TD>\n";
	HTML += "</TR>\n";

	return (char *)HTML;
}

const char *
	CCouncil::improve_relationship(CCouncilMessage::EType aType, CCouncil *aCouncil)
{
	static CString
		Ret;
	Ret.clear();

	CCouncilRelation *
	Relation = get_relation(aCouncil);
	if (Relation == NULL && aType != CCouncilMessage::TYPE_SUGGEST_PACT)
	{
		Ret.format(GETTEXT("You don't have any relationship with %1$s council."),
			aCouncil->get_nick());
		return (char *)Ret;
	}

	if (Relation == NULL && aType == CCouncilMessage::TYPE_SUGGEST_PACT)
	{
		Relation = new CCouncilRelation(this, aCouncil, CRelation::RELATION_TRUCE);

		Relation->type(QUERY_INSERT);
		STORE_CENTER->store(*Relation);

		add_relation(Relation);
		aCouncil->add_relation(Relation);
		COUNCIL_RELATION_TABLE->add_relation(Relation);
	}

	CCouncil
	*Council1 = Relation->get_council1(),
	*Council2 = Relation->get_council2();

	if (Relation->get_relation() == CRelation::RELATION_PEACE &&
		aType == CCouncilMessage::TYPE_SUGGEST_ALLY)
	{
		Relation->set_relation(CRelation::RELATION_ALLY);
		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);

		Council1->send_time_news_to_all_member(
			(char *)format(GETTEXT("Your council relationship with %1$s council has become \"%2$s\"."),
				Relation->get_council2()->get_nick(),
				Relation->get_relation_description()));
		Council2->send_time_news_to_all_member(
			(char *)format(GETTEXT("Your council relationship with %1$s council has become \"%2$s\"."),
				Relation->get_council1()->get_nick(),
				Relation->get_relation_description()));
		set_improve_relation_effect_timer(aCouncil);

		ADMIN_TOOL->add_council_relation_log(
			(char *)format("The relationship between the council %s and the council %s has changed into ally from peace.",
				Council1->get_nick(),
				Council2->get_nick()));

		return NULL;
	}

	if (Relation->get_relation() == CRelation::RELATION_TRUCE &&
		aType == CCouncilMessage::TYPE_SUGGEST_PACT)
	{
		Relation->set_relation(CRelation::RELATION_PEACE);
		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);

		set_commerce_with(aCouncil);

		Relation->get_council1()->send_time_news_to_all_member(
			(char *)format(GETTEXT("Your council relationship with %1$s council has become \"%2$s\"."),
				Relation->get_council2()->get_nick(),
				Relation->get_relation_description()));
		Relation->get_council2()->send_time_news_to_all_member(
			(char *)format(GETTEXT("Your council relationship with %1$s council has become \"%2$s\"."),
				Relation->get_council1()->get_nick(),
				Relation->get_relation_description()));
		set_improve_relation_effect_timer(aCouncil);

		ADMIN_TOOL->add_council_relation_log(
			(char *)format("The relationship between the council %s and the council %s has changed into peace from truce.",
				Council1->get_nick(),
				Council2->get_nick()));

		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);

		return NULL;
	}

	if ((Relation->get_relation() == CRelation::RELATION_WAR ||
			Relation->get_relation() == CRelation::RELATION_TOTAL_WAR) &&
		(aType == CCouncilMessage::TYPE_SUGGEST_TRUCE ||
			aType == CCouncilMessage::TYPE_DEMAND_TRUCE))
	{
		if (aType == CCouncilMessage::TYPE_DEMAND_TRUCE)
		{
			// indemnities
			int
				Indemnities;
			if (Relation->get_relation() == CRelation::RELATION_WAR)
			{
				if (get_member_total_turn_production() > MAX_RESOURCE/5)
				{
					Indemnities = MAX_RESOURCE;
				}
				else
				{
					Indemnities = get_member_total_turn_production() * 5;
				}
			}
			else
			{
				if (get_member_total_turn_production() > MAX_RESOURCE/10)
				{
					Indemnities = MAX_RESOURCE;
				}
				else
				{
					Indemnities = get_member_total_turn_production() * 10;
				}
			}

			int
				Compensation = (Indemnities > get_production()) ?
				get_production():Indemnities;
			change_production(-Compensation);
			aCouncil->change_production(Compensation);

			if (Relation->get_relation() == CRelation::RELATION_WAR)
			{
				aCouncil->send_time_news_to_all_member(
					(char *)format(GETTEXT("Your council received the compensation(%1$s PP) for the war with %2$s council."),
						dec2unit(Compensation), get_nick()));

				send_time_news_to_all_member(
					(char *)format(GETTEXT("Your council give the compensation for war(%1$sPP) to %2$s council."),
						dec2unit(Compensation),
						aCouncil->get_nick()));
			}
			else
			{
				aCouncil->send_time_news_to_all_member(
					(char *)format(GETTEXT("Your council received the compensation(%1$s PP) for the total war with %2$s council."),
						dec2unit(Compensation), get_nick()));

				send_time_news_to_all_member(
					(char *)format(GETTEXT("Your council give the compensation for total war(%1$sPP) to %2$s council."),
						dec2unit(Compensation),
						aCouncil->get_nick()));
			}
		}

		if (Relation->get_relation() == CRelation::RELATION_TOTAL_WAR)
		{
			CCouncilAction *
			Action = check_declare_total_war_effect_timer(aCouncil);

			if (Action != NULL)
			{
				Action->type(QUERY_DELETE);
				STORE_CENTER->store(*Action);

				remove_action(Action->get_id());
				COUNCIL_ACTION_TABLE->remove_action(Action->get_id());
			}
		}
		Relation->set_relation(CRelation::RELATION_TRUCE);

		Relation->type(QUERY_UPDATE);
		STORE_CENTER->store(*Relation);

		Relation->get_council1()->send_time_news_to_all_member(
			(char *)format(GETTEXT("Your council relationship with %1$s council has become \"%2$s\"."),
				Relation->get_council2()->get_nick(),
				Relation->get_relation_description()));
		Relation->get_council2()->send_time_news_to_all_member(
			(char *)format(GETTEXT("Your council relationship with %1$s council has become \"%2$s\"."),
				Relation->get_council1()->get_nick(),
				Relation->get_relation_description()));
		set_improve_relation_effect_timer(aCouncil);

		ADMIN_TOOL->add_council_relation_log(
			(char *)format("The relationship between the council %s and the council %s has changed into truce from war/total war.",
				Council1->get_nick(),
				Council2->get_nick()));

		return NULL;
	}

	if ((Relation->get_relation() == CRelation::RELATION_TOTAL_WAR ||
			Relation->get_relation() == CRelation::RELATION_WAR ||
			Relation->get_relation() == CRelation::RELATION_TRUCE ||
			Relation->get_relation() == CRelation::RELATION_PEACE) &&
		(aType == CCouncilMessage::TYPE_DEMAND_SUBMISSION))
	{
		if (mSupremacyCouncil == NULL)
		{
			Relation->set_relation(CRelation::RELATION_SUBORDINARY);
			if (Relation->get_council2() == aCouncil)
			{
				Relation->set_council1(aCouncil);
				Relation->set_council2(this);
			}

			Relation->type(QUERY_UPDATE);
			STORE_CENTER->store(*Relation);

			Relation->get_council1()->send_time_news_to_all_member(
				(char*)format(GETTEXT("Your council conquered %1$s council."),
					Relation->get_council2()->get_nick()));
			Relation->get_council2()->send_time_news_to_all_member(
				(char*)format(GETTEXT("Your council has been conquered by %1$s council."),
					Relation->get_council1()->get_nick()));

			mSupremacyCouncil = aCouncil;

			ADMIN_TOOL->add_council_relation_log(
				(char *)format("The council %s has conquered the council %s.",
					Council1->get_nick(),
					Council2->get_nick()));

			return NULL;
		}
		else
		{
			mSupremacyCouncil->send_time_news_to_all_member(
				(char *)format(GETTEXT("%1$s has bulldozed your ordinary council %2$s."),
					aCouncil->get_nick(), (char *)mNick));

			return GETTEXT("Your council has a supremacy council already.");
		}
	}

	if ((Relation->get_relation() == CRelation::RELATION_ALLY &&
			aType == CCouncilMessage::TYPE_COUNCIL_FUSION_OFFER) ||
		(Relation->get_relation() == CRelation::RELATION_SUBORDINARY &&
			aType == CCouncilMessage::TYPE_COUNCIL_FUSION_OFFER &&
			Relation->get_council2() == aCouncil))
	{
		return GETTEXT("Council merging is not available yet."); //try_council_fusion(aCouncil);
	}

	return GETTEXT("This message has unmatching message type");
}

void
	CCouncil::set_commerce_with(CCouncil* aCouncil)
{
	CPlayerList *
	TargetCouncilMemberList = aCouncil->get_members();

	for(int i=0; i<mMembers.length(); i++)
	{
		CPlayer *
		Member1 = (CPlayer *)mMembers.get(i);
		for(int j=0; j<TargetCouncilMemberList->length(); j++)
		{
			CPlayer *
			Member2 = (CPlayer*)TargetCouncilMemberList->get(j);
			Member1->set_commerce_with(Member2);
		}
	}
}

void
	CCouncil::clear_commerce_with(CCouncil* aCouncil)
{
	CPlayerList *
	TargetCouncilMemberList = aCouncil->get_members();
	for(int i=0; i<mMembers.length(); i++)
	{
		CPlayer *
		Member1 = (CPlayer*)mMembers.get(i);
		for(int j=0; j<TargetCouncilMemberList->length(); j++)
		{
			CPlayer *
			Member2 = (CPlayer*)TargetCouncilMemberList->get(j);
			Member1->clear_commerce_with(Member2);
		}
	}
}

CCouncil*
CCouncil::get_supremacy_council()
{
	return mSupremacyCouncil;
}

bool
	CCouncil::check_total_war()
{
	for(int i=0; i<mRelationList.length(); i++)
	{
		CCouncilRelation* Relation
			= (CCouncilRelation*)mRelationList.get(i);
		if (Relation->get_relation() == CRelation::RELATION_TOTAL_WAR)
			return true;
	}

	return false;
}

bool
	CCouncil::check_war()
{
	for(int i=0; i<mRelationList.length(); i++)
	{
		CCouncilRelation* Relation
			= (CCouncilRelation*)mRelationList.get(i);
		if (Relation->get_relation() == CRelation::RELATION_WAR)
			return true;
	}

	return false;
}

int
	CCouncil::get_member_total_turn_production()
{
	int
		Total = 0;

	for (int i=0 ; i<mMembers.length() ; i++)
	{
		CPlayer *
		Player = (CPlayer *)mMembers.get(i);
		if (MAX_RESOURCE - Total < Player->get_last_turn_production())
		{
			return MAX_RESOURCE;
		}
		Total += Player->get_last_turn_production();
	}

	return Total;
}

bool
	CCouncil::process_merge_with(CCouncil *aCouncil, CCouncil *aActiveCouncil)
{
	if (can_merge_with(aCouncil, aActiveCouncil) != NULL) return false;

	CCouncilActionMergingOffer
	*MyMergingOffer = (CCouncilActionMergingOffer *)COUNCIL_ACTION_TABLE->get_by_owner_type(this, CAction::ACTION_COUNCIL_MERGING_OFFER),
	*TargetMergingOffer = (CCouncilActionMergingOffer *)COUNCIL_ACTION_TABLE->get_by_owner_type(aCouncil, CAction::ACTION_COUNCIL_MERGING_OFFER);

	MyMergingOffer->type(QUERY_DELETE);
	STORE_CENTER->store(*MyMergingOffer);

	TargetMergingOffer->type(QUERY_DELETE);
	STORE_CENTER->store(*TargetMergingOffer);

	COUNCIL_ACTION_TABLE->remove_action(MyMergingOffer->get_id());
	remove_action(MyMergingOffer->get_id());
	COUNCIL_ACTION_TABLE->remove_action(TargetMergingOffer->get_id());
	aCouncil->remove_action(TargetMergingOffer->get_id());

	CString
		TargetCouncilNick;
	TargetCouncilNick = aCouncil->get_nick();
	int
		TargetCouncilProduction = aCouncil->get_production();
	int
		TargetCouncilSpeakerID = aCouncil->get_speaker_id();
	int
		NumberOfTargetCouncilMember = aCouncil->get_number_of_members();

	CPlayerList *
	TargetCouncilMemberList = aCouncil->get_members();
	for (int i=TargetCouncilMemberList->length()-1 ; i>=0 ; i--)
	{
		CPlayer *
		TargetCouncilMember = (CPlayer *)TargetCouncilMemberList->get(i);
		CAdmission *
		TargetCouncilMemberAdmission = TargetCouncilMember->get_admission();
		if (TargetCouncilMemberAdmission != NULL)
		{
			if (TargetCouncilMemberAdmission->get_council()->get_id() == mID)
			{
				TargetCouncilMemberAdmission->type(QUERY_DELETE);
				STORE_CENTER->store(*TargetCouncilMemberAdmission);

				TargetCouncilMember->set_admission(NULL);
			}
		}

		aCouncil->remove_member(TargetCouncilMember->get_game_id());
		add_member(TargetCouncilMember);
		TargetCouncilMember->set_council(this);

		if (TargetCouncilSpeakerID > 0)
		{
			TargetCouncilMember->set_council_vote(TargetCouncilSpeakerID);
		}
		else
		{
			TargetCouncilMember->set_council_vote(NONE);
		}


		TargetCouncilMember->type(QUERY_UPDATE);
		STORE_CENTER->store(*TargetCouncilMember);
	}

	if (aCouncil->get_honor() < get_honor()) set_honor(aCouncil->get_honor());

	COUNCIL_TABLE->remove_council(aCouncil->get_id());

	change_production(TargetCouncilProduction);
	send_time_news_to_all_member(
		(char *)format(GETTEXT("%1$s PP has been moved into your council's safe from the council %2$s while the councils were being merged."),
			dec2unit(TargetCouncilProduction),
			(char *)TargetCouncilNick));

	type(QUERY_UPDATE);
	STORE_CENTER->store(*this);

	if (NumberOfTargetCouncilMember > 1)
	{
		for (int i=0 ; i<COUNCIL_TABLE->length() ; i++)
		{
			CCouncil *
			Council = (CCouncil *)COUNCIL_TABLE->get(i);
			CPlayer *
			Speaker = Council->get_speaker();
			if (Speaker == NULL) continue;

			Speaker->time_news(
				(char *)format(GETTEXT("The council %1$s has joined the council %2$s, so the council %3$s became stronger."),
					(char *)TargetCouncilNick,
					(char *)mNick,
					(char *)mNick));
		}
	}

	return true;
}
bool CCouncil::can_merge()
{
//SLOG("CAN MERGE: ID -- %d mMergePenaltyTime=%d", mID, mMergePenaltyTime);
//SLOG("CAN MERGE: ID -- %d time(0)=%d", mID, time(0));
	if (mMergePenaltyTime == -1) // npos
		return true;

	if (mMergePenaltyTime <= time(0))
	{
		enable_merge();
		return true;
	}
	return false;
}

void CCouncil::disable_merge()
{
	mMergePenaltyTime = 0;
	mStoreFlag += STORE_MERGE_PENALTY_TIME;
	type(QUERY_UPDATE);
	STORE_CENTER->store(*this);
}

void CCouncil::disable_merge(time_t aTime)
{
	mMergePenaltyTime = time(0) + aTime;
	mStoreFlag += STORE_MERGE_PENALTY_TIME;
	type(QUERY_UPDATE);
	STORE_CENTER->store(*this);
}

void CCouncil::enable_merge()
{
	mMergePenaltyTime = -1; // npos
	mStoreFlag += STORE_MERGE_PENALTY_TIME;
	type(QUERY_UPDATE);
	STORE_CENTER->store(*this);
}

const int CCouncil::get_merge_penalty_time()
{
	return mMergePenaltyTime;
}

char *
	CCouncil::can_merge_with(CCouncil *aCouncil, CCouncil *aActiveCouncil)
{
	CPlayer *
	Speaker = get_speaker();
	if (Speaker == NULL)
	{
		return (char *)format(GETTEXT("The council %1$s has no speaker."),
			(char *)mNick);
	}

#define GALATIC_ECONOMICS	1112
	CTech *
	Tech = TECH_TABLE->get_by_id(GALATIC_ECONOMICS);
	if (Speaker->has_tech(Tech->get_id()) == false)
	{
		return (char *)format(GETTEXT("The speaker of the council %1$s doesn't have the tech %2$s."),
			(char *)mNick, Tech->get_name_with_level());
	}
#undef GALATIC_ECONOMICS

	CCouncilRelation *
	Relationship = mRelationList.find_by_target_id(aCouncil->get_id());
	if (Relationship != NULL)
	{
		if (Relationship->get_relation() == CRelation::RELATION_WAR ||
			Relationship->get_relation() == CRelation::RELATION_TOTAL_WAR)
		{
			return (char *)format(GETTEXT("The council %1$s is at war with the council %2$s."),
				(char *)mNick, aCouncil->get_nick());
		}
	}

	if (!can_merge())
	{
//	    SLOG("mMergePenaltyTime = %d", mMergePenaltyTime);
		if (mMergePenaltyTime <= 0)
		{
			return (char *)format(GETTEXT("The council %1$s currently cannot merge."),
				(char *)mNick);
		}
		else
		{
			time_t Current = mMergePenaltyTime-time(0);
//      	        SLOG("HOURS: %d MINUTES: %d", HoursLeft, MinutesLeft);
			return (char *)format(GETTEXT("The council %1$s currently cannot merge for another %2$s."),
				(char *)mNick, timetostring(Current));
		}
	}

	if (!aCouncil->can_merge())
	{
//	    SLOG("mMergePenaltyTime = %d", aCouncil->get_merge_penalty_time());
		if (aCouncil->get_merge_penalty_time() <= 0)
		{
			return (char *)format(GETTEXT("The council %1$s currently cannot merge."),
				aCouncil->get_nick());
		}
		else
		{
			time_t Current = aCouncil->get_merge_penalty_time()-time(0);
//      	        SLOG("HOURS: %d MINUTES: %d", HoursLeft, MinutesLeft);
			return (char *)format(GETTEXT("The council %1$s currently cannot merge for another %2$s."),
				aCouncil->get_nick(), timetostring(Current));
		}
	}

	if (mMembers.length() + aCouncil->get_number_of_members() > max_member())
	{
		return (char *)format(GETTEXT("The council %1$s doesn't have enough space to admit the members in the council %2$s into."),
			aCouncil->get_nick(), (char *)mNick);
	}

	CClusterList *
	TargetCouncilClusterList = aCouncil->get_cluster_list();
	bool
		SharedCluster = false;
	for (int i=0 ; i<mClusterList.length() ; i++)
	{
		CCluster *
		Cluster = (CCluster *)mClusterList.get(i);

		for (int j=0 ; j<TargetCouncilClusterList->length() ; j++)
		{
			CCluster *
			TargetCluster = (CCluster *)TargetCouncilClusterList->get(j);

			if (Cluster->get_id() == TargetCluster->get_id())
			{
				SharedCluster = true;
				break;
			}
		}
	}

	if (SharedCluster == false)
	{
		return (char *)format(GETTEXT("Your council shares no clusters with the council %1$s."),
			aCouncil->get_nick());
	}

	CCouncilActionMergingOffer
	*BigCouncilMergingOffer = (CCouncilActionMergingOffer *)COUNCIL_ACTION_TABLE->get_by_owner_type(this, CAction::ACTION_COUNCIL_MERGING_OFFER),
	*SmallCouncilMergingOffer = (CCouncilActionMergingOffer *)COUNCIL_ACTION_TABLE->get_by_owner_type(aCouncil, CAction::ACTION_COUNCIL_MERGING_OFFER);
	if (BigCouncilMergingOffer != NULL)
	{
		int
			TargetCouncilID = BigCouncilMergingOffer->get_argument();
		CCouncil *
		TargetCouncil = COUNCIL_TABLE->get_by_id(TargetCouncilID);

		if (TargetCouncil == NULL)
		{
			if (aActiveCouncil->get_id() == mID)
			{
				return (char *)format(GETTEXT("Your council sent merge offer to a certain council or has merge offer from it, but it doesn't exist anymore."));
			}
			else
			{
				return (char *)format(GETTEXT("The council %1$s sent merge offer to a certain council or has merge offer from it, but it doesn't exist anymore."), (char *)mNick);
			}
		}
		else
		{
			if (TargetCouncilID != aCouncil->get_id())
			{
				if (aActiveCouncil->get_id() == mID)
				{
					return (char *)format(GETTEXT("Your council suggested the council %1$s to merge the 2 councils or has merge offer from it already."), TargetCouncil->get_nick());
				}
				else
				{
					return (char *)format(GETTEXT("The council %1$s suggested the council %2$s to merge the 2 councils or has merge offer from it already."), (char *)mNick, TargetCouncil->get_nick());
				}
			}
		}
	}
	else if (SmallCouncilMergingOffer != NULL)
	{
		int
			TargetCouncilID = SmallCouncilMergingOffer->get_argument();
		CCouncil *
		TargetCouncil = COUNCIL_TABLE->get_by_id(TargetCouncilID);

		if (TargetCouncil == NULL)
		{
			if (aActiveCouncil->get_id() == mID)
			{
				return (char *)format(GETTEXT("The council %1$s sent merge offer to a certain council or has merge offer from it, but it doesn't exist anymore."), aCouncil->get_nick());
			}
			else
			{
				return (char *)format(GETTEXT("Your council sent merge offer to a certain council or has merge offer from it, but it doesn't exist anymore."));
			}
		}
		else
		{
			if (TargetCouncilID != mID)
			{
				if (aActiveCouncil->get_id() == aCouncil->get_id())
				{
					return (char *)format(GETTEXT("Your council suggested the council %1$s to merge the 2 councils or has merge offer from it already."), TargetCouncil->get_nick());
				}
				else
				{
					return (char *)format(GETTEXT("The council %1$s suggested the council %2$s to merge the 2 councils or has merge offer from it already."), aCouncil->get_nick(), TargetCouncil->get_nick());
				}
			}
		}
	}

	return NULL;
}

void
	CCouncil::set_supremacy_council(CCouncil* aCouncil)
{
	mSupremacyCouncil = aCouncil;
}

char *
	CCouncil::info_members_information_html()
{
	static CString
		Info;
	Info.clear();

	Info = "<TABLE WIDTH=\"480\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n";
	Info += "<TR>\n";

	Info.format("<TH CLASS=\"tabletxt\" WIDTH=\"151\">%s</TH>\n", GETTEXT("Name"));

	Info.format("<TH CLASS=\"tabletxt\" WIDTH=\"82\">%s</TH>\n", GETTEXT("Race"));

	Info.format("<TH CLASS=\"tabletxt\" WIDTH=\"80\">%s</TH>\n", GETTEXT("Power"));

	Info.format("<TH CLASS=\"tabletxt\" WIDTH=\"165\">%s</TH>\n",
		GETTEXT("Ranking"));

	Info += "</TR>\n";

	for (int i=0 ; i<mMembers.length() ; i++)
	{
		CPlayer *
		Member = (CPlayer *)mMembers.get(i);

		if (Member->is_dead()) continue;

		Info += "<TR ALIGN=\"CENTER\">\n";

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"151\"><A CLASS=\"rank\" HREF=\"player_search_result.as?PLAYER_ID=%d\">%s</A></TD>\n",
			Member->get_game_id(), Member->get_nick());
		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"82\">%s</TD>\n",
			Member->get_race_name());
		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"80\">%s</TD>\n",
			dec2unit(Member->get_power()));

		CRankTable *
		RankingTable = PLAYER_TABLE->get_overall_rank_table();
		int
			Ranking = RankingTable->get_rank_by_id(Member->get_game_id());
		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"165\">%s</TD>\n",
			dec2unit(Ranking));

		Info += "</TR>\n";
	}

	Info += "</TABLE>\n";

	return (char *)Info;
}

char *
	CCouncil::council_war_status_information_html()
{
	static CString
		Info;
	Info.clear();

	bool
		AnyWarStatus = false;

	for (int i=0 ; i<mRelationList.length() ; i++)
	{
		CCouncilRelation *
		Relation = (CCouncilRelation *)mRelationList.get(i);

		if (Relation->get_relation() != CRelation::RELATION_WAR &&
			Relation->get_relation() != CRelation::RELATION_TOTAL_WAR) continue;

		AnyWarStatus = true;
	}

	if (!AnyWarStatus)
	{
		Info = "<TABLE WIDTH=\"550\" BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
		Info += "<TR>\n";
		Info.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n", GETTEXT("There are no councils at war with you."));
		Info += "</TR>\n";
		Info += "</TABLE>\n";

		return (char *)Info;
	}

	Info = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	Info += "<TR BGCOLOR=\"#171717\">\n";

	Info += "<TH CLASS=\"tabletxt\" WIDTH=\"136\" COLOR=\"#666666\">";
	Info += GETTEXT("Council");
	Info += "</TH>\n";

	Info += "<TH WIDTH=\"59\" CLASS=\"tabletxt\" COLOR=\"#666666\">";
	Info += GETTEXT("Relation");
	Info += "</TH>\n";

	Info += "<TH WIDTH=\"121\" CLASS=\"tabletxt\" COLOR=\"#666666\">";
	Info += GETTEXT("Speaker");
	Info += "</TH>\n";

	Info += "<TH CLASS=\"tabletxt\" WIDTH=\"69\" COLOR=\"#666666\">";
	Info += GETTEXT("Members");
	Info += "</TH>\n";

	Info += "<TH CLASS=\"tabletxt\" WIDTH=\"53\" COLOR=\"#666666\">";
	Info += GETTEXT("Power");
	Info += "</TH>\n";

	Info += "<TH CLASS=\"tabletxt\" WIDTH=\"53\" COLOR=\"#666666\">";
	Info += GETTEXT("Honor");
	Info += "</TH>\n";

	Info += "<TH CLASS=\"tabletxt\" WIDTH=\"53\" COLOR=\"#666666\">";
	Info += GETTEXT("Empire Relation");
	Info += "</TH>\n";

	Info += "</TR>\n";

	for (int i=0 ; i<mRelationList.length() ; i++)
	{
		CCouncilRelation *
		Relation = (CCouncilRelation *)mRelationList.get(i);

		if (Relation->get_relation() != CRelation::RELATION_WAR &&
			Relation->get_relation() != CRelation::RELATION_TOTAL_WAR) continue;

		CCouncil *
		Council;
		CCouncil *
		Council1 = Relation->get_council1();
		CCouncil *
		Council2 = Relation->get_council2();

		if (Council1->get_id() == mID)
		{
			Council = Council2;
		} else if (Council2->get_id() == mID)
		{
			Council = Council1;
		} else continue;

		CPlayer *
		Speaker = Council->get_speaker();

		Info += "<TR>\n";

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"136\"><A CLASS=\"rank\" HREF=\"../info/council_search_result.as?COUNCIL_ID=%d\">&nbsp;%s</TD>",
			Council->get_id(), Council->get_nick());

		Info.format("<TD WIDTH=\"59\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
			Relation->get_relation_description());

		if (Speaker != NULL)
		{
			Info.format("<TD WIDTH=\"121\" CLASS=\"tabletxt\" ALIGN=\"CENTER\"><A CLASS=\"rank\" HREF=\"../info/player_search_result.as?PLAYER_ID=%d\">%s</A></TD>\n",
				Speaker->get_game_id(), Speaker->get_nick());
		}
		else
		{
			Info.format("<TD WIDTH=\"121\" CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
				GETTEXT("None"));
		}

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"69\" ALIGN=\"CENTER\">%d</TD>\n",
			Council->get_number_of_members());

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"96\" ALIGN=\"CENTER\">%s</TD>\n",
			dec2unit(Council->get_power()));

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"53\" ALIGN=\"CENTER\">%d</TD>\n",
			Council->get_honor());

		Info.format("<TD CLASS=\"tabletxt\" WIDTH=\"43\" ALIGN=\"CENTER\">%d</TD>\n",
			Council->get_empire_relation());

		Info += "</TR>\n";
	}

	Info += "</TABLE>\n";

	return (char *)Info;
}

//added by zbyte 2004-03-10

char *
CCouncil::get_dismissable_html() {
	static CString
		HTML;
	HTML.clear();
#ifdef COUNCIL_PLAYER_DISMISSAL
	for (int i=0 ; i<mMembers.length() ; i++)
	{
		CPlayer *
		Member = (CPlayer *)mMembers.get(i);

		if (Member->is_dead()) continue;
		if (!Member->is_dismissable()) continue;

		HTML.format("<OPTION VALUE=\"%d\">",
			Member->get_game_id());
		HTML.format("%s</OPTION>",
			Member->get_nick());
	}
#endif
	return (char *)HTML;
}
