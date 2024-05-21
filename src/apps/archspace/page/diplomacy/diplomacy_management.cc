#include <libintl.h>
#include "../../archspace.h"
#include "../../pages.h"

bool
CPagePlayerDiplomacyManagement::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

int typelen = 5;
CDiplomaticMessage::EType AllowedTypes[] = {
						CDiplomaticMessage::TYPE_SUGGEST_PACT,
						CDiplomaticMessage::TYPE_SUGGEST_ALLY,
						CDiplomaticMessage::TYPE_SUGGEST_TRUCE,
						CDiplomaticMessage::TYPE_BREAK_PACT,
						CDiplomaticMessage::TYPE_BREAK_ALLY
						};
int preflen = 3;
CPreference::EPreference AllowedPrefs[] = {
						CPreference::PR_ACCEPT_ALLY,
						CPreference::PR_ACCEPT_TRUCE,
						CPreference::PR_ACCEPT_PACT,
						};

	CCouncil *Council = aPlayer->get_council();
	CPlayerList *Members = Council->get_members();	
	if (Members->length() <= 1)
	{
ITEM("ERROR_MESSAGE", GETTEXT("There are no players who you know."));
return output("diplomacy/diplomacy_management_error.html");
}    

	static CString HTML;
	

	// if player doesn't have a preference object yet, make it
	if (aPlayer->get_preference() == NULL)
	aPlayer->set_preference(new CPreference(aPlayer->get_game_id()));

CPreference *aPreference = aPlayer->get_preference();

	QUERY("MANAGE", ManageString);
	if (ManageString != NULL)
	{
	if (!strcmp(ManageString, "PLAYER"))
	{
	QUERY("ACTION", ActionString); 
	if (!ActionString)
	{
	ITEM("ERROR_MESSAGE", GETTEXT("Invalid Selection"));
	return output("diplomacy/diplomacy_management_error.html");
	}    
	CDiplomaticMessage::EType MessageType = (CDiplomaticMessage::EType)as_atoi(ActionString);
	bool found = false;
	for (int i=0; i < typelen; i++)
	if (AllowedTypes[i] == MessageType)
		found = true;
	if (!found)
	{
	ITEM("ERROR_MESSAGE", GETTEXT("Invalid Selection"));
	return output("diplomacy/diplomacy_management_error.html");
	}    
	
	CString ActionResult;
	ActionResult.clear();
	bool MessageError = false;
		for(int i=0; i < Members->length(); i++)
		{
	CPlayer *
		Player = (CPlayer *)Members->get(i);
	if (Player->is_dead()) continue;
	if (Player->get_game_id() == aPlayer->get_game_id()) continue;
//	CPlayerRelation *
//			Relation = aPlayer->get_relation(Player); //no longer used
		CString QueryVar;
		QueryVar.clear();
		QueryVar.format("PLAYER_%d", Player->get_game_id());
		QUERY((char *)QueryVar, SelectedPlayerString);	 	  
		if (SelectedPlayerString)
	{    
	if (!Player->check_duplication_diplomatic_message(MessageType, aPlayer))
	{
		if (MessageType == CDiplomaticMessage::TYPE_SUGGEST_ALLY ||
		    MessageType == CDiplomaticMessage::TYPE_SUGGEST_PACT ||
	        MessageType == CDiplomaticMessage::TYPE_SUGGEST_TRUCE) {
            if (aPlayer->check_diplomatic_message(MessageType, Player))
		    {
			    CDiplomaticMessage *Message = new CDiplomaticMessage();
		    	Message->initialize(MessageType, aPlayer, Player, "Diplomacy Management Message");
	    		Player->add_diplomatic_message(Message);
	    		Message->CStore::type(QUERY_INSERT);
	    		STORE_CENTER->store(*Message);
	    		ActionResult.format("Message of type '%s' <b>sent</b> to %s<BR>",
		    		CDiplomaticMessage::get_type_description(MessageType),
		    		Player->get_nick());
	    	}
	    	else
	    	{
	    	ActionResult.format("Message of type '%s' <b>cannot be sent</b> to %s<BR>",
		    		CDiplomaticMessage::get_type_description(MessageType),
		    		Player->get_nick());
	    	MessageError = true;
	    	}
  	   } else if (MessageType == CDiplomaticMessage::TYPE_BREAK_PACT ||
  	              MessageType == CDiplomaticMessage::TYPE_BREAK_ALLY) {
            if (aPlayer->check_diplomatic_message(MessageType, Player)) {
			    CDiplomaticMessage *Message = new CDiplomaticMessage();
		    	Message->initialize(MessageType, aPlayer, Player, "Diplomacy Management Message");
	    		Player->add_diplomatic_message(Message);
	    		Message->CStore::type(QUERY_INSERT);
	    		STORE_CENTER->store(*Message);
	    		ActionResult.format("Message of type '%s' <b>sent</b> to %s<BR>",
		    		CDiplomaticMessage::get_type_description(MessageType),
		    		Player->get_nick());
	    		if (MessageType == CDiplomaticMessage::TYPE_BREAK_PACT) {
                    Player->break_pact(aPlayer);
                }
                if (MessageType == CDiplomaticMessage::TYPE_BREAK_ALLY) {
                    Player->break_ally(aPlayer);
                }
	    	}
	    	else
	    	{
	    	ActionResult.format("Message of type '%s' <b>cannot be sent</b> to %s<BR>",
		    		CDiplomaticMessage::get_type_description(MessageType),
		    		Player->get_nick());
	    	MessageError = true;
	    	}
	    }
	}
	else
	{
		ActionResult.format("Message of type '%s' <b>already sent</b> to %s<BR>", 
				CDiplomaticMessage::get_type_description(MessageType),
				Player->get_nick());
		MessageError = true;                                     
	}    
	}    
		}		
		if (!ActionResult)
		{
		ActionResult = GETTEXT("No Player(s) Selected");
		MessageError = true;
	}    
if (ActionResult && MessageError)
{
	ITEM("ERROR_MESSAGE", (char *)ActionResult);
	return output("diplomacy/diplomacy_management_error.html");
}    
}
else if (!strcmp(ManageString, "PREFERENCE"))
{
	for (int i=0; i < preflen ; i++) 
	{
	CString QueryString;
	QueryString.clear();
	QueryString.format("PR_%d", (int)AllowedPrefs[i]);
	QUERY((char*)QueryString, PrefString);
	if ((!PrefString && aPreference->has(AllowedPrefs[i])) || 
		(PrefString && !aPreference->has(AllowedPrefs[i])))
	aPreference->handle(AllowedPrefs[i]);
}    
}
else
{
	ITEM("ERROR_MESSAGE", GETTEXT("Invalid Selection"));
	return output("diplomacy/diplomacy_management_error.html");
}    
}    

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
		"<TH WIDTH=93 CLASS=\"tabletxt\" BGCOLOR=\"#171717\">"
			"<FONT COLOR=\"666666\">";
	HTML += GETTEXT("Relation");
	HTML += 
			"</FONT>"
		"</TH>\n"
		"<TH CLASS=\"tabletxt\" WIDTH=219 BGCOLOR=\"#171717\">"
			"<FONT COLOR=\"666666\">";
	HTML += GETTEXT("Waiting for Reply");
	HTML += 
			"</FONT>"
		"</TH>\n"
		"<TH CLASS=\"tabletxt\" WIDTH=100 BGCOLOR=\"#171717\">"
			"<FONT COLOR=\"666666\">";
	HTML += GETTEXT("Power");
	HTML += 
			"</FONT>"
		"</TH>\n"
		"<TH CLASS=\"tabletxt\" WIDTH=15 BGCOLOR=\"#171717\">"
			"<FONT COLOR=\"666666\">";
	HTML += GETTEXT("Select");
	HTML += 
			"</FONT>"
		"</TH>\n"
	"</TR>\n";

	for(int i=0; i < Members->length(); i++)
	{
		CPlayer *
			Player = (CPlayer *)Members->get(i);
		if (Player->is_dead()) continue;

		CPlayerRelation *
			Relation = aPlayer->get_relation(Player);
		CString WaitingText;
		WaitingText.clear();
		bool HitFirst = false;
		for (int i=0; i < typelen ; i++)
		{
		if (Player->check_duplication_diplomatic_message(AllowedTypes[i], aPlayer))
		{
			if (!HitFirst)
			{
			HitFirst = true;
			WaitingText = CDiplomaticMessage::get_type_description(AllowedTypes[i]);
			}
			else
			{
			WaitingText += ", ";
		WaitingText += CDiplomaticMessage::get_type_description(AllowedTypes[i]); 
			}        
	}        
		}
		if (!WaitingText)
		WaitingText = GETTEXT("Waiting on None");
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
					"&nbsp;%s"
				"</TD>\n",
			Player->get_nick(),
				(Relation) ? CRelation::get_relation_description(Relation->get_relation()) : CRelation::get_relation_description(CRelation::RELATION_NONE),
				(Player->get_game_id() != aPlayer->get_game_id()) ? (char *)WaitingText : "N/A",
				dec2unit(Player->get_power()));

		if (Player->get_game_id() == aPlayer->get_game_id())
		{
	HTML.format(
				"<TD CLASS=\"tabletxt\" WIDTH=109 ALIGN=CENTER>"
					"&nbsp;- "
				"</TD>\n"
			"</TR>",
			Player->get_game_id());
		}
		else
		{
	HTML.format(
				"<TD CLASS=\"tabletxt\" WIDTH=109 ALIGN=CENTER>"
					"<INPUT TYPE=CHECKBOX NAME=PLAYER_%d>"
				"</TD>\n"
			"</TR>",
			Player->get_game_id());
		}		
	}			
	HTML += "</TABLE>\n";
	
	static CString ACTION_LIST_HTML;
	ACTION_LIST_HTML.clear();
	ACTION_LIST_HTML.format("%s<SELECT NAME=ACTION>\n", GETTEXT("Action: "));

	for (int i=0; i < typelen ; i++) 
	ACTION_LIST_HTML.format("<OPTION VALUE=%d>%s</OPTION>\n", 
				(int)AllowedTypes[i], 
				CDiplomaticMessage::get_type_description(AllowedTypes[i]));
ACTION_LIST_HTML += "</SELECT>\n";
ITEM("ACTION_LIST",  (char*)ACTION_LIST_HTML);
	ITEM("STATUS_TABLE", (char *)HTML);


	// Preference stuff
static CString PrefHTML;
PrefHTML.clear();
PrefHTML += "<TABLE CELLPADDING=0 CELLSPACING=0 ALIGN=CENTER BORDER=1 BORDERCOLOR=\"#171717\">";
PrefHTML.format("<TR><TH class=\"maintext\" VALIGN=\"TOP\" COLSPAN=2 ALIGN=CENTER BGCOLOR=\"#171717\">%s</TH></TR>", GETTEXT("Your current preferences:"));
PrefHTML.format("<TR><TD class=\"maintext\" VALIGN=\"TOP\" BGCOLOR=\"#171717\">&nbsp;%s</TD><TD class=\"maintext\" WIDTH=15 BGCOLOR=\"#171717\">&nbsp;</TD></TR>", GETTEXT("Preference"));
for (int i=0; i < preflen; i++)
{
PrefHTML.format("<TR><TD class=\"maintext\" VALIGN=\"TOP\">&nbsp;%s</TD>", 
		aPreference->EPreferencetoString(AllowedPrefs[i]));
PrefHTML += "<TD class=\"maintext\" VALIGN=\"TOP\">&nbsp;";
if (aPreference->has(AllowedPrefs[i]))
{
	PrefHTML.format("<INPUT TYPE=CHECKBOX NAME=\"PR_%d\" CHECKED></TD></TR>", (int)AllowedPrefs[i]);
}
else
{
	PrefHTML.format("<INPUT TYPE=CHECKBOX NAME=\"PR_%d\"></TD></TR>", (int)AllowedPrefs[i]);
}
} 
PrefHTML += "</TABLE>";
ITEM("PREF_TABLE", (char *)PrefHTML); 	
//	system_log("end page handler %s", get_name());

	return output( "diplomacy/diplomacy_management.html" );
}

