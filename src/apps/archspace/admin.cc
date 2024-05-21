#include <libintl.h>
#include "admin.h"
#include <cstdlib>
#include "archspace.h"
#include "game.h"
#include "portal.h"
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

char *CAdminTool::mSEDPath = "/usr/bin/sed";

TZone gDetachmentPlayerPlayerZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CDetachmentPlayerPlayer),
	0,
	0,
	NULL,
	"Zone CDetachmentPlayerPlayer"
};

TZone gDetachmentPlayerCouncilZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CDetachmentPlayerCouncil),
	0,
	0,
	NULL,
	"Zone CDetachmentPlayerCouncil"
};

TZone gDetachmentCouncilCouncilZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CDetachmentCouncilCouncil),
	0,
	0,
	NULL,
	"Zone CDetachmentCouncilCouncil"
};


/**
 * CAdminList constructor
 */
CAdminList::CAdminList()
{
}

/**
 * CAdminList destructor
 */
CAdminList::~CAdminList()
{
    remove_all();
}

/**
 * CAdminList item destructor
 * Doesn't need to do anything -- CPortalUserTable handles the memory
 */
bool
CAdminList::free_item(TSomething aItem)
{
	(void)aItem;
	return true;
}

/**
 * CAdminList comparator
 * Compares two CPortalUser values by portal ID
 */
int
CAdminList::compare(TSomething aItem1, TSomething aItem2) const
{
	CPortalUser
		*PortalUser1 = (CPortalUser *)aItem1,
		*PortalUser2 = (CPortalUser *)aItem2;

	if (PortalUser1->get_portal_id() > PortalUser2->get_portal_id()) return 1;
	if (PortalUser1->get_portal_id() < PortalUser2->get_portal_id()) return -1;
	return 0;
}

/**
 * CAdminList comparator
 * Compares a CPortalUser portal ID value to a key
 */
int
CAdminList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CPortalUser *
		PortalUser = (CPortalUser *)aItem;
 
	if (PortalUser->get_portal_id() > (int)aKey) return 1;
	if (PortalUser->get_portal_id() < (int)aKey) return -1;
	return 0;
}

/**
 * Adds a CPortalUser to CAdminList and sets to min admin level if not already
 * @aPortalUser the CPortalUser to add to the CAdminList
 */
bool
CAdminList::add_admin(CPortalUser *aPortalUser)
{
	if (aPortalUser == NULL) return false;
    if (aPortalUser->get_user_level() < MIN_ADMIN_LEVEL)
    {
       aPortalUser->set_user_level(MIN_ADMIN_LEVEL);
       aPortalUser->type(QUERY_UPDATE);
   	   STORE_CENTER->store(*aPortalUser);
    }
	int
		Index = find_sorted_key((void *)aPortalUser->get_portal_id());
	if (Index >= 0) return false;

	insert_sorted(aPortalUser);
	return true;
}


/**
 * Removes a CPortalUser from CAdminList and resets the Users level if necessary
 * @aPortalUser the CPortalUser to remove from the CAdminList
 */
bool
CAdminList::remove_admin(CPortalUser *aPortalUser)
{
	int
		Index = find_sorted_key((void *)aPortalUser->get_portal_id());
	if (Index < 0) return false;
    
    if (aPortalUser->get_user_level() >= MIN_ADMIN_LEVEL)
    {
       aPortalUser->set_user_level(DEFAULT_USER_LEVEL);
       aPortalUser->type(QUERY_UPDATE);
    }
	return CSortedList::remove(Index);
}

/**
 * Removes a CPortalUser from CAdminList and resets the Users level if necessary
 * @aPortalID the CPortalUser portal ID to remove from the CAdminList
 */
bool
CAdminList::remove_admin(int aPortalID)
{
	int
		Index = find_sorted_key((void *)aPortalID);
	if (Index < 0) return false;
    CPortalUser *aPortalUser = PORTAL_TABLE->get_by_portal_id(aPortalID);
    if (aPortalUser != NULL && aPortalUser->get_user_level() >= MIN_ADMIN_LEVEL)
    {
       aPortalUser->set_user_level(DEFAULT_USER_LEVEL);
       aPortalUser->type(QUERY_UPDATE);
    }

	return CSortedList::remove(Index);
}

/**
 * Loads all admins defined by MIN_ADMIN_LEVEL from the player table
 * into the CAdminList
 */
bool 
CAdminList::load()
{
	for (int i=0; i < PORTAL_TABLE->length(); i++)
	{
		CPortalUser *aPortalUser = (CPortalUser *)PORTAL_TABLE->get(i);  
        if (aPortalUser == NULL)
              continue;
        if (aPortalUser->get_user_level() < MIN_ADMIN_LEVEL)
              continue;
       	add_admin(aPortalUser);
	}

	return true;
}

/**
 * CAdminTool constructor
 */
CAdminTool::CAdminTool()
{
}

/**
 * CAdminTool destructor
 */
CAdminTool::~CAdminTool()
{
}

/**
 * CAdminTool initialiazer
 * @param aMySQL Archspace mysql database connection to load data from
 */
bool
CAdminTool::initialize(CMySQL *aMySQL)
{
	mDetachmentPlayerPlayerTable.load(aMySQL);
	mDetachmentPlayerCouncilTable.load(aMySQL);
	mDetachmentCouncilCouncilTable.load(aMySQL);

	mAdminList.load();

	load_banned_ip_list();

	return true;
}

// TODO: Continue documenting
bool CAdminTool::has_access(CPlayer *aPlayer)
{
 if (aPlayer == NULL)
     return false;
 CPortalUser 
 *aPortalUser = PORTAL_TABLE->get_by_portal_id(aPlayer->get_portal_id());
 if (aPortalUser == NULL || aPortalUser->get_user_level() < MIN_ADMIN_LEVEL)
     return false;
 return true;
}

bool CAdminTool::has_access(CPlayer *aPlayer, EUserLevel aUserLevel)
{
 if (aPlayer == NULL)
     return false;
 CPortalUser 
 *aPortalUser = PORTAL_TABLE->get_by_portal_id(aPlayer->get_portal_id());
 if (aPortalUser == NULL || aPortalUser->get_user_level() < aUserLevel)
     return false;
 return true;
}

bool CAdminTool::has_access(CPlayer *aPlayer, EDevLevel aDevLevel)
{
 if (aPlayer == NULL)
     return false;
 CPortalUser 
 *aPortalUser = PORTAL_TABLE->get_by_portal_id(aPlayer->get_portal_id());
 if (aPortalUser == NULL || aPortalUser->get_dev_level() < aDevLevel)
     return false;
 return true;
}


bool
CAdminTool::set_data_file_directory(char *aDirectory)
{
	if (aDirectory == NULL) return false;

	mDataFileDirectory = aDirectory;
	return true;
}

bool
CAdminTool::set_CS_mail_address(char *aAddress)
{
	if (aAddress == NULL) return false;

	mCSMailAddress = aAddress;
	return true;
}

bool
CAdminTool::set_accuse_mail_name(char *aName)
{
	if (aName == NULL) return false;

	mAccuseMailName = aName;
	return true;
}

bool
CAdminTool::add_new_player_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_player", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_new_player_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: NEW_PLAYER - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_dead_player_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_player", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_dead_player_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: DEAD_PLAYER - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_restarting_player_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_player", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_restarting_player_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: RESTARTING_PLAYER - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_login_player_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_player", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_login_player_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: PLAYER_LOGIN - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_bankrupt_report_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_report", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_bankrupt_report_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: BANKRUPT - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_player_relation_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_diplomacy", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_player_relation_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: PLAYER_RELATION - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_spy_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_diplomacy", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_spy_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: SPY - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_speaker_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_council", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_speaker_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: COUNCIL_SPEAKER - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_financial_aid_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_council", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_financial_aid_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: FINANCIAL_AID - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_donation_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_council", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_donation_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: DONATION - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_emigration_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_council", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_emigration_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: EMIGRATION - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_independence_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_council", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_independence_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: INDEPENDENCE - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_council_relation_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_council", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_council_relation_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: COUNCIL_RELATION - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_new_bid_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_blackmarket", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_new_bid_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: NEW_BID - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_winning_bid_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_blackmarket", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_winning_bid_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: WINNING_BID - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_siege_planet_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_war", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_siege_planet_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: SIEGE_PLANET - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_blockade_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_war", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_blockade_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: BLOCKADE - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_raid_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_war", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_raid_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: RAID - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_privateer_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_war", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_privateer_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: PRIVATEER - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_invade_magistrate_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_invade_empire", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_invade_magistrate_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: INVADE_MAGISTRATE - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_admin_action_log(CPortalUser *aAdmin, char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_admin_action", (char *)mDataFileDirectory);
	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_admin_action_log()");
		return false;
	}
	
    if (aAdmin == NULL)
    {
   	   SLOG("ERROR : aAdmin is NULL in CAdminTool::add_admin_action_log()");
       return false;
    }
    CString aAdminString;
    aAdminString.format("Admin: %s PortalID: %d Action Message: ",aAdmin->get_name(), aAdmin->get_portal_id());
    
	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';
	fprintf(File, "%s :: ADMIN_ACTION - %s%s\n", TimeString, (char *)aAdminString, aLogString);

	fclose(File);

	return true;
}


bool
CAdminTool::add_invade_empire_planet_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_invade_empire", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_invade_empire_planet_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: INVADE_EMPIRE_PLANET - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_invade_fortress_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_invade_empire", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_invade_fortress_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: INVADE_FORTRESS - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_invade_empire_capital_planet_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_invade_empire", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_invade_empire_capital_planet_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: INVADE_EMPIRE_CAPITAL_PLANET - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::add_punishment_log(char *aLogString)
{
	FILE *
		File;
	CString
		FileName;
	FileName.format("%s/log_admin_tool", (char *)mDataFileDirectory);

	File = fopen((char *)FileName, "a");
	if (File == NULL)
	{
		SLOG("ERROR : Couldn't open the log file in CAdminTool::add_punishment_log()");
		return false;
	}

	time_t
		Time = time(0);
	char *
		TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) - 1) = '\0';

	fprintf(File, "%s :: PUNISHMENT - %s\n", TimeString, aLogString);

	fclose(File);

	return true;
}

bool
CAdminTool::accuse_message(CDiplomaticMessage *aMessage)
{
	CString
		Mail;
	if (mAccuseMailName.length() == 0)
	{
		SLOG("WARNING : mAccuseMailName.length() is 0 in CAdminTool::accuse_message()");
		Mail.format("MAIL FROM: %s\n", GETTEXT("Unknown"));
	}
	else
	{
		Mail.format("MAIL FROM: %s\n", (char *)mAccuseMailName);
	}
	if (mCSMailAddress.length() == 0)
	{
		SLOG("WARNING : mCSMailAddress.length() is 0 in CAdminTool::accuse_message()");
		Mail.format("RCPT TO: %s\n", GETTEXT("Unknown"));
	}
	else
	{
		Mail.format("RCPT TO: %s\n", (char *)mCSMailAddress);
	}
	Mail += "DATA\n";
	Mail.format("Subject: %s\n", GETTEXT("Accused Diplomatic Message"));
	if (mAccuseMailName.length() == 0)
	{
		SLOG("WARNING : mAccuseMailName.length() is 0 in CAdminTool::accuse_message()");
		Mail.format("From: %s\n", GETTEXT("Unknown"));
	}
	else
	{
		Mail.format("From: %s\n", (char *)mAccuseMailName);
	}
	if (mCSMailAddress.length() == 0)
	{
		SLOG("WARNING : mCSMailAddress.length() is 0 in CAdminTool::accuse_message()");
		Mail.format("To: %s\n", GETTEXT("Unknown"));
	}
	else
	{
		Mail.format("To: %s\n", (char *)mCSMailAddress);
	}

	CString
		DottedContent;
	add_dot_for_sendmail(aMessage->get_content(), &DottedContent);

	if (DottedContent.length() == 0)
	{
		Mail += "\n";
	}
	else
	{
		Mail.format("%s\n", (char *)DottedContent);
	}
	Mail += ".\n";

	CString
		Command;
	Command.format("echo \"%s\" | /usr/sbin/sendmail -bs", (char *)Mail);

	//system((char *)Command);

	return true;
}

bool
CAdminTool::accuse_message(CCouncilMessage *aMessage)
{
	CString
		Mail;
	if (mAccuseMailName.length() == 0)
	{
		SLOG("WARNING : mAccuseMailName.length() is 0 in CAdminTool::accuse_message()");
		Mail.format("MAIL FROM: %s\n", GETTEXT("Unknown"));
	}
	else
	{
		Mail.format("MAIL FROM: %s\n", (char *)mAccuseMailName);
	}
	if (mCSMailAddress.length() == 0)
	{
		SLOG("WARNING : mCSMailAddress.length() is 0 in CAdminTool::accuse_message()");
		Mail.format("RCPT TO: %s\n", GETTEXT("Unknown"));
	}
	else
	{
		Mail.format("RCPT TO: %s\n", (char *)mCSMailAddress);
	}
	Mail += "DATA\n";
	Mail.format("Subject: %s\n", GETTEXT("Accused Diplomatic Message"));
	if (mAccuseMailName.length() == 0)
	{
		SLOG("WARNING : mAccuseMailName.length() is 0 in CAdminTool::accuse_message()");
		Mail.format("From: %s\n", GETTEXT("Unknown"));
	}
	else
	{
		Mail.format("From: %s\n", (char *)mAccuseMailName);
	}
	if (mCSMailAddress.length() == 0)
	{
		SLOG("WARNING : mCSMailAddress.length() is 0 in CAdminTool::accuse_message()");
		Mail.format("To: %s\n", GETTEXT("Unknown"));
	}
	else
	{
		Mail.format("To: %s\n", (char *)mCSMailAddress);
	}


	CString
		DottedContent;
	add_dot_for_sendmail(aMessage->get_content(), &DottedContent);

	if (DottedContent.length() == 0)
	{
		Mail += "\n";
	}
	else
	{
		Mail.format("%s\n", (char *)DottedContent);
	}
	Mail += ".\n";

	CString
		Command;
	Command.format("echo \"%s\" | /usr/sbin/sendmail -bs", (char *)Mail);

	//system((char *)Command);

	return true;
}

bool
CAdminTool::accuse_message(CAdmission *aAdmission)
{
	CString
		Mail;
	Mail.format("MAIL FROM: %s\n", (char *)mAccuseMailName);
	Mail.format("RCPT TO: %s\n", (char *)mCSMailAddress);
	Mail += "DATA\n";
	Mail.format("Subject: %s\n", GETTEXT("Accused Admission Request"));
	Mail.format("From: %s\n", (char *)mAccuseMailName);
	Mail.format("To: %s\n", (char *)mCSMailAddress);

	CString
		DottedContent;
	add_dot_for_sendmail(aAdmission->get_content(), &DottedContent);

	Mail.format("%s\n", (char *)DottedContent);
	Mail += ".\n";

	CString
		Command;
	Command.format("echo \"%s\" | /usr/sbin/sendmail -bs", (char *)Mail);

	system((char *)Command);
//
	return true;
}

bool
CAdminTool::are_they_restricted(CPlayer *aPlayer1, CPlayer *aPlayer2)
{
	CDetachmentPlayerPlayer *
		DetachmentPlayerPlayer = mDetachmentPlayerPlayerTable.get_by_player_player(aPlayer1, aPlayer2);

	if (DetachmentPlayerPlayer == NULL)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool
CAdminTool::are_they_restricted(CPlayer *aPlayer, CCouncil *aCouncil)
{
	CDetachmentPlayerCouncil *
		DetachmentPlayerCouncil = mDetachmentPlayerCouncilTable.get_by_player_council(aPlayer, aCouncil);

	if (DetachmentPlayerCouncil == NULL)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool
CAdminTool::are_they_restricted(CCouncilMessage::EType aType,
								CCouncil *aCouncil1,
								CCouncil *aCouncil2)
{
	CDetachmentCouncilCouncil *
		DetachmentCouncilCouncil = mDetachmentCouncilCouncilTable.get_by_council_council(aCouncil1, aCouncil2);

	if (DetachmentCouncilCouncil == NULL)
	{
		return false;
	}

	switch (DetachmentCouncilCouncil->get_type())
	{
		case CDetachmentCouncilCouncil::TYPE_SUBMISSION :
		{
			if (aType == CCouncilMessage::TYPE_DEMAND_SUBMISSION)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		case CDetachmentCouncilCouncil::TYPE_MERGING :
		{
			if (aType == CCouncilMessage::TYPE_COUNCIL_FUSION_OFFER)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		case CDetachmentCouncilCouncil::TYPE_ALLIANCE :
		{
			if (aType == CCouncilMessage::TYPE_SUGGEST_ALLY)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		case CDetachmentCouncilCouncil::TYPE_WAR :
		{
			if (aType == CCouncilMessage::TYPE_DECLARE_WAR ||
				aType == CCouncilMessage::TYPE_DECLARE_TOTAL_WAR)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		default :
			return false;
	}
}

bool
CAdminTool::add_admin(CPortalUser *aPortalUser)
{
	return mAdminList.add_admin(aPortalUser);
}

bool
CAdminTool::load_banned_ip_list()
{
	const char *
		BannedIPListFile = CONFIG->get_string("Game", "IPBanFile", NULL);
	if (BannedIPListFile == NULL)
	{
		SLOG("ERROR : Could not get the banned IP list file in the config.");
		return false;
	}

	if (mBannedIPList.load(BannedIPListFile) == false)
	{
		SLOG("ERROR : Could not load BannedIPListFile!");
	}

	return true;
}

bool
CAdminTool::save_banned_ip_list()
{
	return mBannedIPList.save();
}

bool
CAdminTool::add_banned_ip(char *aBannedIP)
{
	return mBannedIPList.add_ip(aBannedIP);
}

char *
CAdminTool::admin_list_html()
{
	static CString
		List;
	List.clear();

	for (int i=0 ; i< mAdminList.length() ; i++)
	{
		CPortalUser *PortalUser = (CPortalUser *)mAdminList.get(i);
		int	PortalID = PortalUser->get_portal_id();
		CString PortalName = PortalUser->get_name();

		if (PortalName == NULL)
		{
			SLOG("ERROR : PortalName is NULL in CAdminTool::admin_list_html(), PortalID = %d", PortalID);
			continue;
		}
  
		List += "<TR>\n";
		List.format("<TD CLASS=\"tabletxt\"><b>%d</b></TD>\n", PortalID);
		List.format("<TD CLASS=\"tabletxt\">%s</TD>\n", (char *)PortalName);
		List += "<TD CLASS=\"tabletxt\">\n";
        List.format("<SELECT NAME=USER_LEVEL%d>", i);
        List.format("<OPTION SELECTED VALUE=%d>%s</OPTION>", PortalUser->get_user_level(),
                user_level_to_string(PortalUser->get_user_level()));
        for (int UserLevel=MIN_USER_LEVEL; UserLevel <= MAX_USER_LEVEL; UserLevel++)
            {
              if (PortalUser->get_user_level() != UserLevel)
                 List.format("<OPTION VALUE=%d>%s</OPTION>", UserLevel,
              user_level_to_string(EUserLevel(UserLevel)));
            }
		List += "</SELECT>";

		List += "</TD>\n";
		List += "<TD CLASS=\"tabletxt\">\n";
        if (PortalUser->get_user_level() != UL_DEVELOPER)
      		List.format("<SELECT NAME=DEV_LEVEL%d DISABLED>", i);        
		else
            List.format("<SELECT NAME=DEV_LEVEL%d>", i);
        List.format("<OPTION SELECTED VALUE=%d>%s</OPTION>", PortalUser->get_dev_level(),
                dev_level_to_string(PortalUser->get_dev_level()));
        for (int DevLevel=MIN_DEV_LEVEL; DevLevel <= MAX_DEV_LEVEL; DevLevel++)
            {
              if (PortalUser->get_dev_level() != DevLevel)
                 List.format("<OPTION VALUE=%d>%s</OPTION>", DevLevel,
              dev_level_to_string(EDevLevel(DevLevel)));
            }
		List += "</SELECT>";
		List += "</TD>\n";

		List += "</TR>\n";
	}

	return (char *)List;
}

char *
CAdminTool::banned_ip_list_html()
{
	static CString
		List;
	List.clear();

	for (int i=0 ; i<mBannedIPList.length() ; i++)
	{
		CIP *
			IP = (CIP *)mBannedIPList.get(i);

		List += "<TR>\n";
		List.format("<TD CLASS=\"tabletxt\">%s</TD>\n", IP->get_name());
		List += "<TD CLASS=\"tabletxt\">\n";
		List.format("<INPUT TYPE=checkbox NAME=BANNED_IP%d>", i);
		List += "</TD>\n";
		List += "</TR>\n";
	}

	return (char *)List;

}

CDetachmentPlayerPlayer::CDetachmentPlayerPlayer()
{
	mID = 0;
	mPlayer1 = NULL;
	mPlayer2 = NULL;
}

bool
CDetachmentPlayerPlayer::initialize(MYSQL_ROW aRow)
{
	enum 
	{
		FIELD_ID = 0,
		FIELD_PLAYER1,
		FIELD_PLAYER2
	};

	mID = atoi(aRow[FIELD_ID]);
	mPlayer1 = PLAYER_TABLE->get_by_portal_id(atoi(aRow[FIELD_PLAYER1]));
	mPlayer2 = PLAYER_TABLE->get_by_portal_id(atoi(aRow[FIELD_PLAYER2]));

	if (mID == 0 || !mPlayer1 || !mPlayer2) return false;

	return true;
}

CString &
CDetachmentPlayerPlayer::query()
{
	static CString
		Query;
	Query.clear();

	switch (mQueryType)
	{
		case QUERY_INSERT :
			Query.format("INSERT INTO detachment_player_player"
							" (id, player1, player2) VALUES (%d, %d, %d)",
							mID, mPlayer1->get_portal_id(), mPlayer2->get_portal_id());
			break;

		case QUERY_UPDATE :
			Query.format("UPDATE detachment_player_player SET"
							" player1 = %d, player2 = %d WHERE id = %d",
							mPlayer1->get_portal_id(), mPlayer2->get_portal_id(), mID);
			break;

		case QUERY_DELETE :
			Query.format("DELETE FROM detachment_player_player WHERE id = %d", mID);
			break;

		default :
			break;
	}

	mStoreFlag.clear();

	return Query;
}

CDetachmentPlayerPlayerTable::~CDetachmentPlayerPlayerTable()
{
	remove_all();
}

bool
CDetachmentPlayerPlayerTable::free_item(TSomething aItem)
{
	CDetachmentPlayerPlayer *
		Detachment = (CDetachmentPlayerPlayer *)aItem;
	if (!Detachment) return false;

	delete Detachment;

	return true;
}

int
CDetachmentPlayerPlayerTable::compare(TSomething aItem1, TSomething aItem2) const
{
	CDetachmentPlayerPlayer
		*Detachment1 = (CDetachmentPlayerPlayer *)aItem1,
		*Detachment2 = (CDetachmentPlayerPlayer *)aItem2;

	if (Detachment1->get_id() < Detachment2->get_id()) return -1;
	if (Detachment1->get_id() > Detachment2->get_id()) return 1;

	return 0;
}

int 
CDetachmentPlayerPlayerTable::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CDetachmentPlayerPlayer *
		Detachment = (CDetachmentPlayerPlayer *)aItem;

	if (Detachment->get_id() < (int)aKey) return -1;
	if (Detachment->get_id() > (int)aKey) return 1;

	return 0;
}

bool
CDetachmentPlayerPlayerTable::add_detachment(CDetachmentPlayerPlayer *aDetachment)
{
	if (!aDetachment) return false;

	if (find_sorted_key((void *)aDetachment->get_id()) >= 0) return false;

	insert_sorted(aDetachment);

	return true;
}

bool
CDetachmentPlayerPlayerTable::remove_detachment(int aID)
{
	int
		Index = find_sorted_key((void *)aID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

CDetachmentPlayerPlayer *
CDetachmentPlayerPlayerTable::get_by_player_player(CPlayer *aPlayer1, CPlayer *aPlayer2)
{
	for (int i=0 ; i<length() ; i++)
	{
		CDetachmentPlayerPlayer *
			DetachmentPlayerPlayer = (CDetachmentPlayerPlayer *)get(i);
		CPlayer
			*Player1 = DetachmentPlayerPlayer->get_player1(),
			*Player2 = DetachmentPlayerPlayer->get_player2();

		if (Player1->get_portal_id() == aPlayer1->get_portal_id())
		{
			if (Player2->get_portal_id() == aPlayer2->get_portal_id())
			{
				return DetachmentPlayerPlayer;
			}
		}
		else if (Player1->get_portal_id() == aPlayer2->get_portal_id())
		{
			if (Player2->get_portal_id() == aPlayer1->get_portal_id())
			{
				return DetachmentPlayerPlayer;
			}
		}
	}

	return NULL;
}

int
CDetachmentPlayerPlayerTable::get_new_id()
{
	int
		NewID = 1;
	while (find_sorted_key((void *)NewID) >= 0) NewID++;

	return NewID;
}

bool
CDetachmentPlayerPlayerTable::load(CMySQL *aMySQL)
{
	SLOG ("Detachment Player-Player Loading");

	//aMySQL->query("LOCK TABLE detachment_player_player READ");

	aMySQL->query("SELECT id FROM detachment_player_player ORDER BY id DESC LIMIT 1");
	aMySQL->use_result();

	if (aMySQL->fetch_row())
	{
		int
			MaxID = atoi(aMySQL->row(0));
		aMySQL->free_result();

		for (int i=1 ; i<=MaxID ; i += 1000)
		{
			CString
				Query;

			Query.format("SELECT id, player1, player2 FROM detachment_player_player"
							" WHERE id >= %d AND id < %d + 1000", i);
			aMySQL->query((char *)Query);

			aMySQL->use_result();

			while (aMySQL->fetch_row())
			{
				CDetachmentPlayerPlayer *
					Detachment = new CDetachmentPlayerPlayer();
				if (Detachment->initialize(aMySQL->row()) == false)
				{
					SLOG("ERROR : Error found in DetachmentPlayerPlayer while loading!(#%d)",
							Detachment->get_id());
					delete Detachment;
					continue;
				}

				add_detachment(Detachment);
			}
		}
	}

	aMySQL->free_result();

	//aMySQL->query("UNLOCK TABLES");

	SLOG("%d Detachment Player-Player Records are loaded", length());

	return true;
}

char *
CDetachmentPlayerPlayerTable::get_restricted_player_list_html(CPlayer *aPlayer)
{
	static CString
		HTML;
	HTML.clear();

	HTML = "<TR>\n";
	HTML += "<TD>\n";

	HTML += "<TABLE WIDTH=570 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	HTML += "<TR BGCOLOR=#171717>\n";

	HTML += "<TH CLASS=tabletxt><FONT COLOR=666666>";
	HTML += GETTEXT("Name");
	HTML += "</FONT></TH>\n";

	HTML += "<TH CLASS=tabletxt><FONT COLOR=666666>";
	HTML += GETTEXT("Council");
	HTML += "</FONT></TH>\n";

	HTML += "</TR>\n";

	bool
		AnyRestriction = false;
	for (int i=0 ; i<length() ; i++)
	{
		CDetachmentPlayerPlayer *
			DetachmentPlayerPlayer = (CDetachmentPlayerPlayer *)get(i);
		CPlayer
			*Player1 = DetachmentPlayerPlayer->get_player1(),
			*Player2 = DetachmentPlayerPlayer->get_player2();
		CPlayer *
			TargetPlayer = NULL;

		if (Player1->get_portal_id() == aPlayer->get_portal_id())
		{
			TargetPlayer = Player2;
			AnyRestriction = true;
		}
		else if (Player2->get_portal_id() == aPlayer->get_portal_id())
		{
			TargetPlayer = Player1;
			AnyRestriction = true;
		}
		else
		{
			continue;
		}

		CCouncil *
			Council = TargetPlayer->get_council();

		HTML += "<TR>\n";

		HTML.format("<TD CLASS=tabletxt ALIGN=\"center\">%1$s</TD>\n",
					TargetPlayer->get_nick());
		HTML.format("<TD CLASS=tabletxt ALIGN=\"center\">%1$s</TD>\n",
					Council->get_nick());

		HTML += "</TR>\n";
	}

	HTML += "</TABLE>\n";

	HTML += "</TD>\n";
	HTML += "</TR>\n";

	if (AnyRestriction == true)
	{
		return (char *)HTML;
	}
	else
	{
		HTML.clear();

		HTML = "<TR>\n";
		HTML.format("<TD CLASS=maintext ALIGN=\"center\">%s</TD>\n",
					GETTEXT("That player has no restriction from a player."));
		HTML += "</TR>\n";

		return (char *)HTML;
	}
}

CDetachmentPlayerCouncil::CDetachmentPlayerCouncil()
{
	mID = 0;
	mPlayer = NULL;
	mCouncil = NULL;
}

bool
CDetachmentPlayerCouncil::initialize(MYSQL_ROW aRow)
{
	enum 
	{
		FIELD_ID = 0,
		FIELD_PLAYER,
		FIELD_COUNCIL
	};

	mID = atoi(aRow[FIELD_ID]);
	mPlayer = PLAYER_TABLE->get_by_portal_id(atoi(aRow[FIELD_PLAYER]));
	mCouncil = COUNCIL_TABLE->get_by_id(atoi(aRow[FIELD_COUNCIL]));

	if (mID == 0 || !mPlayer || !mCouncil) return false;

	return true;
}

CString &
CDetachmentPlayerCouncil::query()
{
	static CString
		Query;
	Query.clear();

	switch (mQueryType)
	{
		case QUERY_INSERT :
			Query.format("INSERT INTO detachment_player_council"
							" (id, player, council) VALUES (%d, %d, %d)",
							mID, mPlayer->get_portal_id(), mCouncil->get_id());
			break;

		case QUERY_UPDATE :
			Query.format("UPDATE detachment_player_council SET"
							" player = %d, council = %d WHERE id = %d",
							mPlayer->get_portal_id(), mCouncil->get_id(), mID);
			break;

		case QUERY_DELETE :
			Query.format("DELETE FROM detachment_player_council WHERE id = %d", mID);
			break;

		default :
			break;
	}

	mStoreFlag.clear();

	return Query;
}

CDetachmentPlayerCouncilTable::~CDetachmentPlayerCouncilTable()
{
	remove_all();
}

bool
CDetachmentPlayerCouncilTable::free_item(TSomething aItem)
{
	CDetachmentPlayerCouncil *
		Detachment = (CDetachmentPlayerCouncil *)aItem;
	if (!Detachment) return false;

	delete Detachment;

	return true;
}

int
CDetachmentPlayerCouncilTable::compare(TSomething aItem1, TSomething aItem2) const
{
	CDetachmentPlayerCouncil
		*Detachment1 = (CDetachmentPlayerCouncil *)aItem1,
		*Detachment2 = (CDetachmentPlayerCouncil *)aItem2;

	if (Detachment1->get_id() < Detachment2->get_id()) return -1;
	if (Detachment1->get_id() > Detachment2->get_id()) return 1;

	return 0;
}

int 
CDetachmentPlayerCouncilTable::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CDetachmentPlayerCouncil *
		Detachment = (CDetachmentPlayerCouncil *)aItem;

	if (Detachment->get_id() < (int)aKey) return -1;
	if (Detachment->get_id() > (int)aKey) return 1;

	return 0;
}

bool
CDetachmentPlayerCouncilTable::add_detachment(CDetachmentPlayerCouncil *aDetachment)
{
	if (!aDetachment) return false;

	if (find_sorted_key((void *)aDetachment->get_id()) >= 0) return false;

	insert_sorted(aDetachment);

	return true;
}

bool
CDetachmentPlayerCouncilTable::remove_detachment(int aID)
{
	int
		Index = find_sorted_key((void *)aID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

CDetachmentPlayerCouncil *
CDetachmentPlayerCouncilTable::get_by_player_council(CPlayer *aPlayer, CCouncil *aCouncil)
{
	for (int i=0 ; i<length() ; i++)
	{
		CDetachmentPlayerCouncil *
			DetachmentPlayerCouncil = (CDetachmentPlayerCouncil *)get(i);
		CPlayer *
			Player = DetachmentPlayerCouncil->get_player();
		CCouncil *
			Council = DetachmentPlayerCouncil->get_council();

		if (Player->get_portal_id() == aPlayer->get_portal_id())
		{
			if (Council->get_id() == aCouncil->get_id())
			{
				return DetachmentPlayerCouncil;
			}
		}
	}

	return NULL;
}

int
CDetachmentPlayerCouncilTable::get_new_id()
{
	int
		NewID = 1;
	while (find_sorted_key((void *)NewID) >= 0) NewID++;

	return NewID;
}

bool
CDetachmentPlayerCouncilTable::load(CMySQL *aMySQL)
{
	SLOG ("Detachment Player-Council Loading");

	//aMySQL->query("LOCK TABLE detachment_player_council READ");

	aMySQL->query("SELECT id FROM detachment_player_council ORDER BY id DESC LIMIT 1");
	aMySQL->use_result();

	if (aMySQL->fetch_row())
	{
		int
			MaxID = atoi(aMySQL->row(0));
		aMySQL->free_result();

		for (int i=1 ; i<=MaxID ; i += 1000)
		{
			CString
				Query;

			Query.format("SELECT id, player, council FROM detachment_player_council"
							" WHERE id >= %d AND id < %d + 1000", i);
			aMySQL->query((char *)Query);

			aMySQL->use_result();

			while (aMySQL->fetch_row())
			{
				CDetachmentPlayerCouncil *
					Detachment = new CDetachmentPlayerCouncil();
				if (Detachment->initialize(aMySQL->row()) == false)
				{
					SLOG("ERROR : Error found in DetachmentPlayerCouncil while loading!(#%d)",
							Detachment->get_id());
					delete Detachment;
					continue;
				}

				add_detachment(Detachment);
			}
		}
	}

	aMySQL->free_result();

	//aMySQL->query("UNLOCK TABLES");

	SLOG("%d Detachment Player-Council Records are loaded", length());

	return true;
}

bool
CDetachmentPlayerCouncilTable::remove_from_db_memory(CCouncil *aCouncil)
{
	for (int i=length()-1 ; i>=0 ; i--)
	{
		CDetachmentPlayerCouncil *
			Detachment = (CDetachmentPlayerCouncil *)get(i);
		CCouncil *
			Council = Detachment->get_council();
		if (Council->get_id() == aCouncil->get_id())
		{
			Detachment->type(QUERY_DELETE);
			STORE_CENTER->store(*Detachment);

			remove_detachment(Detachment->get_id());
		}
	}

	return true;
}

char *
CDetachmentPlayerCouncilTable::get_restricted_council_list_html(CPlayer *aPlayer)
{
	static CString
		HTML;
	HTML.clear();

	HTML = "<TR>\n";
	HTML += "<TD>\n";

	HTML += "<TABLE WIDTH=570 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	HTML += "<TR BGCOLOR=#171717>\n";

	HTML += "<TH CLASS=tabletxt><FONT COLOR=666666>";
	HTML += GETTEXT("Name");
	HTML += "</FONT></TH>\n";

	HTML += "<TH CLASS=tabletxt><FONT COLOR=666666>";
	HTML += GETTEXT("Speaker");
	HTML += "</FONT></TH>\n";

	HTML += "</TR>\n";

	bool
		AnyRestriction = false;
	for (int i=0 ; i<length() ; i++)
	{
		CDetachmentPlayerCouncil *
			DetachmentPlayerCouncil = (CDetachmentPlayerCouncil *)get(i);
		CPlayer *
			Player = DetachmentPlayerCouncil->get_player();
		CCouncil *
			Council = DetachmentPlayerCouncil->get_council();

		if (Player->get_portal_id() == aPlayer->get_portal_id())
		{
			CPlayer *
				Speaker = Council->get_speaker();

			HTML += "<TR>\n";

			HTML.format("<TD CLASS=tabletxt ALIGN=\"center\">%1$s</TD>\n",
						Council->get_nick());
			if (Speaker != NULL)
			{
				HTML.format("<TD CLASS=tabletxt ALIGN=\"center\">%1$s</TD>\n",
							Speaker->get_nick());
			}
			else
			{
				HTML.format("<TD CLASS=tabletxt ALIGN=\"center\">%1$s</TD>\n",
							GETTEXT("None"));
			}

			HTML += "</TR>\n";

			AnyRestriction = true;
		}
	}

	HTML += "</TABLE>\n";

	HTML += "</TD>\n";
	HTML += "</TR>\n";

	if (AnyRestriction == true)
	{
		return (char *)HTML;
	}
	else
	{
		HTML.clear();

		HTML = "<TR>\n";
		HTML.format("<TD CLASS=maintext ALIGN=\"center\">%s</TD>\n",
					GETTEXT("That player has no restriction from a council."));
		HTML += "</TR>\n";

		return (char *)HTML;
	}
}

char *
CDetachmentPlayerCouncilTable::get_restricted_player_list_html(CCouncil *aCouncil)
{
	static CString
		HTML;
	HTML.clear();

	HTML = "<TR>\n";
	HTML += "<TD>\n";

	HTML += "<TABLE WIDTH=570 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	HTML += "<TR BGCOLOR=#171717>\n";

	HTML += "<TH CLASS=tabletxt><FONT COLOR=666666>";
	HTML += GETTEXT("Name");
	HTML += "</FONT></TH>\n";

	HTML += "<TH CLASS=tabletxt><FONT COLOR=666666>";
	HTML += GETTEXT("Council");
	HTML += "</FONT></TH>\n";

	HTML += "</TR>\n";

	bool
		AnyRestriction = false;
	for (int i=0 ; i<length() ; i++)
	{
		CDetachmentPlayerCouncil *
			DetachmentPlayerCouncil = (CDetachmentPlayerCouncil *)get(i);
		CPlayer *
			Player = DetachmentPlayerCouncil->get_player();
		CCouncil *
			Council = DetachmentPlayerCouncil->get_council();

		if (Council->get_id() == aCouncil->get_id())
		{
			CCouncil *
				Council = Player->get_council();

			HTML += "<TR>\n";

			HTML.format("<TD CLASS=tabletxt ALIGN=\"center\">%1$s</TD>\n",
						Player->get_nick());
			HTML.format("<TD CLASS=tabletxt ALIGN=\"center\">%1$s</TD>\n",
						Council->get_nick());

			HTML += "</TR>\n";

			AnyRestriction = true;
		}
	}

	HTML += "</TABLE>\n";

	HTML += "</TD>\n";
	HTML += "</TR>\n";

	if (AnyRestriction == true)
	{
		return (char *)HTML;
	}
	else
	{
		HTML.clear();

		HTML = "<TR>\n";
		HTML.format("<TD CLASS=maintext ALIGN=\"center\">%s</TD>\n",
					GETTEXT("That council has no restriction from a player."));
		HTML += "</TR>\n";

		return (char *)HTML;
	}
}

CDetachmentCouncilCouncil::CDetachmentCouncilCouncil()
{
	mID = 0;
	mType = TYPE_NONE;
	mCouncil1 = NULL;
	mCouncil2 = NULL;
}

bool
CDetachmentCouncilCouncil::set_type(int aType)
{
	if (aType < TYPE_NONE || aType > TYPE_WAR) return false;

	mType = aType;
	return true;
}

bool
CDetachmentCouncilCouncil::initialize(MYSQL_ROW aRow)
{
	enum 
	{
		FIELD_ID = 0,
		FIELD_COUNCIL1,
		FIELD_COUNCIL2
	};

	mID = atoi(aRow[FIELD_ID]);
	mCouncil1 = COUNCIL_TABLE->get_by_id(atoi(aRow[FIELD_COUNCIL1]));
	mCouncil2 = COUNCIL_TABLE->get_by_id(atoi(aRow[FIELD_COUNCIL2]));

	if (mID == 0 || !mCouncil1 || !mCouncil2) return false;

	return true;
}

CString &
CDetachmentCouncilCouncil::query()
{
	static CString
		Query;
	Query.clear();

	switch (mQueryType)
	{
		case QUERY_INSERT :
			Query.format("INSERT INTO detachment_council_council"
							" (id, type, council1, council2) VALUES (%d, %d, %d, %d)",
							mID, mType, mCouncil1->get_id(), mCouncil2->get_id());
			break;

		case QUERY_UPDATE :
			Query.format("UPDATE detachment_council_council SET"
							" type = %d, council1 = %d, council2 = %d WHERE id = %d",
							mType, mCouncil1->get_id(), mCouncil2->get_id(), mID);
			break;

		case QUERY_DELETE :
			Query.format("DELETE FROM detachment_council_council WHERE id = %d", mID);
			break;

		default :
			break;
	}

	mStoreFlag.clear();

	return Query;
}

CDetachmentCouncilCouncilTable::~CDetachmentCouncilCouncilTable()
{
	remove_all();
}

bool
CDetachmentCouncilCouncilTable::free_item(TSomething aItem)
{
	CDetachmentCouncilCouncil *
		Detachment = (CDetachmentCouncilCouncil *)aItem;
	if (!Detachment) return false;

	delete Detachment;

	return true;
}

int
CDetachmentCouncilCouncilTable::compare(TSomething aItem1, TSomething aItem2) const
{
	CDetachmentCouncilCouncil
		*Detachment1 = (CDetachmentCouncilCouncil *)aItem1,
		*Detachment2 = (CDetachmentCouncilCouncil *)aItem2;

	if (Detachment1->get_id() < Detachment2->get_id()) return -1;
	if (Detachment1->get_id() > Detachment2->get_id()) return 1;

	return 0;
}

int 
CDetachmentCouncilCouncilTable::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CDetachmentCouncilCouncil *
		Detachment = (CDetachmentCouncilCouncil *)aItem;

	if (Detachment->get_id() < (int)aKey) return -1;
	if (Detachment->get_id() > (int)aKey) return 1;

	return 0;
}

bool
CDetachmentCouncilCouncilTable::add_detachment(CDetachmentCouncilCouncil *aDetachment)
{
	if (!aDetachment) return false;

	if (find_sorted_key((void *)aDetachment->get_id()) >= 0) return false;

	insert_sorted(aDetachment);

	return true;
}

bool
CDetachmentCouncilCouncilTable::remove_detachment(int aID)
{
	int
		Index = find_sorted_key((void *)aID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

CDetachmentCouncilCouncil *
CDetachmentCouncilCouncilTable::get_by_council_council(CCouncil *aCouncil1, CCouncil *aCouncil2)
{
	for (int i=0 ; i<length() ; i++)
	{
		CDetachmentCouncilCouncil *
			DetachmentCouncilCouncil = (CDetachmentCouncilCouncil *)get(i);
		CCouncil
			*Council1 = DetachmentCouncilCouncil->get_council1(),
			*Council2 = DetachmentCouncilCouncil->get_council2();

		if (Council1->get_id() == aCouncil1->get_id())
		{
			if (Council2->get_id() == aCouncil2->get_id())
			{
				return DetachmentCouncilCouncil;
			}
		}
		else if (Council1->get_id() == aCouncil2->get_id())
		{
			if (Council2->get_id() == aCouncil1->get_id())
			{
				return DetachmentCouncilCouncil;
			}
		}
	}

	return NULL;
}

int
CDetachmentCouncilCouncilTable::get_new_id()
{
	int
		NewID = 1;
	while (find_sorted_key((void *)NewID) >= 0) NewID++;

	return NewID;
}

bool
CDetachmentCouncilCouncilTable::load(CMySQL *aMySQL)
{
	SLOG ("Detachment Council-Council Loading");

	//aMySQL->query("LOCK TABLE detachment_council_council READ");

	aMySQL->query("SELECT id FROM detachment_council_council ORDER BY id DESC LIMIT 1");
	aMySQL->use_result();

	if (aMySQL->fetch_row())
	{
		int
			MaxID = atoi(aMySQL->row(0));
		aMySQL->free_result();

		for (int i=1 ; i<=MaxID ; i += 1000)
		{
			CString
				Query;

			Query.format("SELECT id, type, council1, council2 FROM detachment_council_council"
							" WHERE id >= %d AND id < %d + 1000", i);
			aMySQL->query((char *)Query);

			aMySQL->use_result();

			while (aMySQL->fetch_row())
			{
				CDetachmentCouncilCouncil *
					Detachment = new CDetachmentCouncilCouncil();
				if (Detachment->initialize(aMySQL->row()) == false)
				{
					SLOG("ERROR : Error found in DetachmentCouncilCouncil while loading!(#%d)",
							aMySQL->row(0));
					delete Detachment;
					continue;
				}

				add_detachment(Detachment);
			}
		}
	}

	aMySQL->free_result();

	//aMySQL->query("UNLOCK TABLES");

	SLOG("%d Detachment Council-Council Records are loaded", length());

	return true;
}

bool
CDetachmentCouncilCouncilTable::remove_from_db_memory(CCouncil *aCouncil)
{
	for (int i=length()-1 ; i>=0 ; i--)
	{
		CDetachmentCouncilCouncil *
			Detachment = (CDetachmentCouncilCouncil *)get(i);
		CCouncil
			*Council1 = Detachment->get_council1(),
			*Council2 = Detachment->get_council2();
		if (Council1->get_id() == aCouncil->get_id() ||
			Council2->get_id() == aCouncil->get_id())
		{
			Detachment->type(QUERY_DELETE);
			STORE_CENTER->store(*Detachment);

			remove_detachment(Detachment->get_id());
		}
	}

	return true;
}

char *
CDetachmentCouncilCouncilTable::get_restricted_council_list_html(CCouncil *aCouncil)
{
	static CString
		HTML;
	HTML.clear();

	HTML = "<TR>\n";
	HTML += "<TD>\n";

	HTML += "<TABLE WIDTH=570 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
	HTML += "<TR BGCOLOR=#171717>\n";

	HTML += "<TH CLASS=tabletxt><FONT COLOR=666666>";
	HTML += GETTEXT("Name");
	HTML += "</FONT></TH>\n";

	HTML += "<TH CLASS=tabletxt><FONT COLOR=666666>";
	HTML += GETTEXT("Speaker");
	HTML += "</FONT></TH>\n";

	HTML += "</TR>\n";

	bool
		AnyRestriction = false;
	for (int i=0 ; i<length() ; i++)
	{
		CDetachmentCouncilCouncil *
			DetachmentCouncilCouncil = (CDetachmentCouncilCouncil *)get(i);
		CCouncil
			*Council1 = DetachmentCouncilCouncil->get_council1(),
			*Council2 = DetachmentCouncilCouncil->get_council2();
		CCouncil *
			TargetCouncil = NULL;

		if (Council1->get_id() == aCouncil->get_id())
		{
			TargetCouncil = Council2;
			AnyRestriction = true;
		}
		else if (Council2->get_id() == aCouncil->get_id())
		{
			TargetCouncil = Council1;
			AnyRestriction = true;
		}
		else
		{
			continue;
		}

		CPlayer *
			Speaker = TargetCouncil->get_speaker();

		HTML += "<TR>\n";

		HTML.format("<TD CLASS=tabletxt ALIGN=\"center\">%1$s</TD>\n",
					TargetCouncil->get_nick());
		if (Speaker != NULL)
		{
			HTML.format("<TD CLASS=tabletxt ALIGN=\"center\">%1$s</TD>\n",
						Speaker->get_nick());
		}
		else
		{
			HTML.format("<TD CLASS=tabletxt ALIGN=\"center\">%1$s</TD>\n",
						GETTEXT("None"));
		}

		HTML += "</TR>\n";
	}

	HTML += "</TABLE>\n";

	HTML += "</TD>\n";
	HTML += "</TR>\n";

	if (AnyRestriction == true)
	{
		return (char *)HTML;
	}
	else
	{
		HTML.clear();

		HTML = "<TR>\n";
		HTML.format("<TD CLASS=maintext ALIGN=\"center\">%s</TD>\n",
					GETTEXT("That council has no restriction from a council."));
		HTML += "</TR>\n";

		return (char *)HTML;
	}
}


