#include <libintl.h>
#include "portal.h"
#include "archspace.h"
#include "pages.h"
#include "game.h"
#include "player.h"
#include "preference.h"

TZone gPreferenceZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CPreference),
	0,
	0,
	NULL,
	"Zone CPreference"
};

CPreference::CPreference(int aGameID)
{
	mPlayerID = aGameID;
	mCommanderView = 0x0000000F;
	enable_command(PR_JAVASCRIPT);
	type(QUERY_INSERT);
	STORE_CENTER->store(*this);
}

CPreference::CPreference(MYSQL_ROW aRow)
{
	mPlayerID = atoi(aRow[STORE_PLAYER_ID]);
	mCommanderView = atoi(aRow[STORE_COMMANDER_VIEW]);
	handle(atoi(aRow[STORE_ACCEPT_ALLY]));
	handle(atoi(aRow[STORE_ACCEPT_PACT]));    
	handle(atoi(aRow[STORE_ACCEPT_TRUCE]));        
	handle(atoi(aRow[STORE_JAVA_CHOICE]));
	type(QUERY_NONE);
}

CPreference::~CPreference()
{

}

CString&
CPreference::query()
{
	static CString 
		Query;

	Query.clear();

	switch(CStore::type())
	{
		case QUERY_INSERT :
			Query.format("INSERT INTO player_pref"
				" (player_id, java_choice)"
				" VALUES (%d, %d)",
				mPlayerID,
				has(PR_JAVA) ? (int)PR_JAVA : (int)PR_JAVASCRIPT);
			break;

		case QUERY_UPDATE :
			Query.format("UPDATE player_pref SET");
			Query.format(" java_choice = %d, ", has(PR_JAVA) ? (int)PR_JAVA : (int)PR_JAVASCRIPT);
			Query.format(" accept_truce = %d, ", has(PR_ACCEPT_TRUCE) ? (int)PR_ACCEPT_TRUCE : -1);
			Query.format(" accept_ally = %d, ", has(PR_ACCEPT_ALLY) ? (int)PR_ACCEPT_ALLY : -1 );
			Query.format(" accept_pact = %d,", has(PR_ACCEPT_PACT) ? (int)PR_ACCEPT_PACT : -1);
			Query.format(" commander_view = %d", mCommanderView);
			Query.format(" WHERE player_id = %d", mPlayerID);
			break;

		case QUERY_DELETE :
			Query.format("DELETE FROM player_pref WHERE player_id = %d", 
				mPlayerID );
			break;
	}
	return Query;
}

char *CPreference::handle(int aPreference)
{
	static CString output;
	bool isValid = true;
	output.clear();
	switch(aPreference)
	{
		case PR_JAVA:  if (has(PR_JAVASCRIPT))
				disable_command(PR_JAVASCRIPT);
			enable_command(PR_JAVA);
			break;
		case PR_JAVASCRIPT:  if (has(PR_JAVA))
				disable_command(PR_JAVA);
			enable_command(PR_JAVASCRIPT);
			break;
		case PR_ACCEPT_TRUCE:
			if (has(PR_ACCEPT_TRUCE))
				disable_command(PR_ACCEPT_TRUCE);
			else   
				enable_command(PR_ACCEPT_TRUCE);
			break;
		case PR_ACCEPT_ALLY:
			if (has(PR_ACCEPT_ALLY))
				disable_command(PR_ACCEPT_ALLY);
			else   
				enable_command(PR_ACCEPT_ALLY);
			break;
		case PR_ACCEPT_PACT:
			if (has(PR_ACCEPT_PACT))
				disable_command(PR_ACCEPT_PACT);
			else   
				enable_command(PR_ACCEPT_PACT);
			break;
		default:       output.format(GETTEXT("Invalid Preference"));     
			isValid = false;
			break;
	}
	if (isValid)
	{
		output.format("%s %s", EPreferencetoString((EPreference)aPreference),GETTEXT("Preference Enabled"));         
		type(QUERY_UPDATE);
		STORE_CENTER->store(*this);
	}
	return (char *)output;
}

char *CPreference::handle(EPreference aPreference)
{
	static CString output;
	bool isValid = true;
	output.clear();
	switch(aPreference)
	{
		case PR_JAVA:  if (has(PR_JAVASCRIPT))
				disable_command(PR_JAVASCRIPT);
			enable_command(PR_JAVA);
			break;
		case PR_JAVASCRIPT:  if (has(PR_JAVA))
				disable_command(PR_JAVA);
			enable_command(PR_JAVASCRIPT);
			break;
		case PR_ACCEPT_TRUCE:
			if (has(PR_ACCEPT_TRUCE))
				disable_command(PR_ACCEPT_TRUCE);
			else   
				enable_command(PR_ACCEPT_TRUCE);
			break;
		case PR_ACCEPT_ALLY:
			if (has(PR_ACCEPT_ALLY))
				disable_command(PR_ACCEPT_ALLY);
			else   
				enable_command(PR_ACCEPT_ALLY);
			break;
		case PR_ACCEPT_PACT:
			if (has(PR_ACCEPT_PACT))
				disable_command(PR_ACCEPT_PACT);
			else   
				enable_command(PR_ACCEPT_PACT);
			break;
		default:       output.format(GETTEXT("Invalid Preference"));     
			isValid = false;
			break;
	}
	if (isValid)
	{
		output.format("%s %s", EPreferencetoString(aPreference),GETTEXT("Preference Enabled"));         
		type(QUERY_UPDATE);
		STORE_CENTER->store(*this);
	}
	return (char *)output;
}


const char *CPreference::HTMLTable()
{
	static CString output;
	output.clear();
	output += "<TABLE CELLPADDING=0 CELLSPACING=0 ALIGN=CENTER BORDER=1 BORDERCOLOR=\"#171717\">";
	output.format("<TR><TH class=\"maintext\" VALIGN=\"TOP\" COLSPAN=2 ALIGN=CENTER BGCOLOR=\"#171717\">%s</TH></TR>", GETTEXT("Your current preferences:"));
	output.format("<TR><TD class=\"maintext\" VALIGN=\"TOP\" BGCOLOR=\"#171717\">&nbsp;%s</TD><TD class=\"maintext\" BGCOLOR=\"#171717\">&nbsp;%s</TD></TR>", GETTEXT("Option"), GETTEXT("Setting"));
	for (int i=0; i < PR_END; i++)
	{
		output.format("<TR><TD class=\"maintext\" VALIGN=\"TOP\">&nbsp;%s</TD>", 
			EPreferencetoString((EPreference)i));
		if (has(i))
		{
			output.format("<TD class=\"maintext\" VALIGN=\"TOP\">&nbsp;%s</TD></TR>", GETTEXT("SELECTED"));
		}
		else
		{
			output.format("<TD class=\"maintext\" VALIGN=\"TOP\">&nbsp;%s</TD></TR>", GETTEXT("NOT SELECTED")); 
		}
	} 
	output += "</TABLE>";
	return (char *)output;
}

const char *CPreference::HTMLSelectOptions()
{
	static CString output;
	output.clear();
	for (int i=0; i < PR_END; i++)
	{
		if (has(i)) 
			output.format("<OPTION VALUE=%d SELECTED>%s</OPTION>\n",i,EPreferencetoString((EPreference)i)); 
		else
			output.format("<OPTION VALUE=%d>%s</OPTION>\n",i,EPreferencetoString((EPreference)i));
	} 
	return (char *)output;
}

char *CPreference::EPreferencetoString(EPreference aPreference)
{
	switch(aPreference)
	{
		case PR_JAVA: return "Java applet (new)";
		case PR_JAVASCRIPT: return "JavaScript (classic)";
		case PR_ACCEPT_ALLY: return "Auto-Accept Alliances";
		case PR_ACCEPT_PACT: return "Auto-Accept Pacts";
		case PR_ACCEPT_TRUCE: return "Auto-Accept Truces";  
		default: return GETTEXT("NONE"); 
	};
}

bool CPreference::hasCommanderStat(unsigned int aCommanderStat)
{
//	SLOG("my player's id:%d",mPlayerID);
	unsigned int rawStat = mCommanderView >> aCommanderStat;
	return (rawStat & 0x00000001);
}

void CPreference::setCommanderView(unsigned int aCommanderView)
{
	mCommanderView = aCommanderView;
	type(QUERY_UPDATE);
	STORE_CENTER->store(*this);
}

const char *CPreference::commander_view_html()
{
	static CString
		Info;

	Info.clear();
	Info = "<TABLE WIDTH=\"550\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
	Info += "<TR BGCOLOR=\"#171717\">\n";

#define COMMANDER_STAT_OPTION(enum, title, width) \
	Info.format("<TH WIDTH=\"%d\" ALIGN=\"CENTER\" CLASS=\"tabletxt\"><FONT COLOR=\"666666\">%s<BR>", width, title); \
	if (hasCommanderStat(CPreference::enum)) \
	{ \
		Info.format("<INPUT TYPE=checkbox NAME=STAT_OPTION%d CHECKED VALUE=\"ON\">", CPreference::enum); \
	} \
	else \
	{ \
		Info.format("<INPUT TYPE=checkbox NAME=STAT_OPTION%d UNCHECKED VALUE=\"ON\">", CPreference::enum); \
	} \
	Info += "</FONT></TH>\n";

	// might wanna GETTEXT these in the future
	COMMANDER_STAT_OPTION(LEVEL, "Level", 51);
	COMMANDER_STAT_OPTION(EXP, "Exp", 44);
	COMMANDER_STAT_OPTION(FLEET_COMMANDING, "Fleet Commanding", 102);
	COMMANDER_STAT_OPTION(EFFICIENCY, "Efficiency", 42);
	/*COMMANDER_STAT_OPTION(SIEGE_PLANET, "Siege Planet", 44);
	COMMANDER_STAT_OPTION(BLOCKADE, "Blockade", 40);
	COMMANDER_STAT_OPTION(RAID, "Raid", 40);
	COMMANDER_STAT_OPTION(PRIVATEER, "Privateer", 40);*/
	COMMANDER_STAT_OPTION(OFFENSE, "Offense", 40);
	COMMANDER_STAT_OPTION(DEFENSE, "Defense", 40);
	Info += "</TR><TR BGCOLOR=\"#171717\">\n";
	/*COMMANDER_STAT_OPTION(SIEGE_REPELLING, "Siege Repelling", 44);
	COMMANDER_STAT_OPTION(BREAK_BLOCKADE, "Break Blockade", 44);
	COMMANDER_STAT_OPTION(PREVENT_RAID, "Prevent Raid", 44);*/
	COMMANDER_STAT_OPTION(MANEUVER, "Maneuver", 40);
	COMMANDER_STAT_OPTION(DETECTION, "Detection", 40);
	//COMMANDER_STAT_OPTION(INTERPRETATION, "Interpretation", 44);
	COMMANDER_STAT_OPTION(ARMADA_CLASS, "Armada Class", 44);
	COMMANDER_STAT_OPTION(ABILITY, "Ability", 44);

#undef COMMANDER_STAT_OPTION

	Info += "</TR></TABLE>\n";

	return (char *)Info;
}
