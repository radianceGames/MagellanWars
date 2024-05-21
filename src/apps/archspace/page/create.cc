#include <libintl.h>
#include "../pages.h"
#include "../archspace.h"
#include "../game.h"
#include "../race.h"

const char* 
CPageCreate::race_html()
{
	static CString
		Buffer;

	Buffer.clear();

	Buffer = "<TABLE border=\"\0\" cellspacing=\"0\" cellpadding=\"0\">\n";
	Buffer += "<TH class=\"maintext\">Name</TH>"
				"<TH class=\"maintext\">Description</TH>";
	
	for(int i=0; i<RACE_TABLE->length(); i++)
	{
		CRace *
			Race = (CRace *)RACE_TABLE->get(i);

		Buffer += "<TR bgcolor=\"#0A0013\">\n";

		if (!i) 
			Buffer.format("<TD class=\"maintext\">"
						"<INPUT TYPE=\"radio\""
						" NAME=\"RACE\" VALUE=\"%d\" CHECKED>%s"
						"</TD>\n", 
						Race->get_id(), Race->get_name());
		else    
			Buffer.format("<TD class=\"maintext\">"
						"<INPUT TYPE=\"radio\""
						" NAME=\"RACE\" VALUE=\"%d\">%s"
						"</TD>\n", 
						Race->get_id(), Race->get_name());

		Buffer.format("<TD class=\"maintext\">%s</TD>\n",
		Race->get_description());
		Buffer += "</TR>\n";
	}
	Buffer += "</TABLE>\n";

	return (char*)Buffer;
}

bool 
CPageCreate::handle(CConnection &aConnection)
{
//	system_log("start handler %s", get_name());
	if (!CPageCommon::handle(aConnection)) return false;

	int
		PortalID = CONNECTION->get_portal_id();
	CPlayer *
		Player = PLAYER_TABLE->get_by_portal_id(PortalID);
	if (Player)
	{
		if (EMPIRE->is_dead() == true)
		{
			CGlobalEnding *
				GlobalEndingData = GAME->get_global_ending_data();

			if (GlobalEndingData->is_final_score() == false)
			{
				ITEM("ENDING_SCORE_MESSAGE",
						GETTEXT("The players' score and councils' score are now being calculated.<BR>\n"
								"Please wait for a while....."));

				ITEM("ENDING_SCORE", " ");
			}
			else
			{
				ITEM("ENDING_SCORE_MESSAGE",
						GETTEXT("The Imperial Capital of Caput Mundi has fallen!<BR>\n"
							"The old regime has collapsed, and the new era has begun."
							" It still remains to be seen whether the new era will be any better, however...<BR>\n"
							"In this time of galactic turbulence, the achievement by you and your colleagues is evaluated as in the following :"));

				ITEM("ENDING_SCORE", GlobalEndingData->get_score_html(Player));
			}

			return output("ending_score.html");
		}
		else
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("You already have your character : %1$s"),
						Player->get_nick()));
			return output("create_character_error.html");
		}
	}

	if (EMPIRE->is_dead() == true)
	{
		CGlobalEnding *
			GlobalEndingData = GAME->get_global_ending_data();

		if (GlobalEndingData->is_final_score() == false)
		{
			ITEM("ENDING_SCORE_MESSAGE",
					GETTEXT("The players' score and councils' score are now being calculated.<BR>\n"
							"Please wait for a while....."));

			ITEM("ENDING_SCORE", " ");
		}
		else
		{
			ITEM("ENDING_SCORE_MESSAGE",
					GETTEXT("The Imperial Capital of Caput Mundi has fallen!<BR>\n"
						"The old regime has collapsed, and the new era has begun."
						" It still remains to be seen whether the new era will be any better, however...<BR>\n"
						"In this time of galactic turbulence, the achievement by you and your colleagues is evaluated as in the following :"));

			ITEM("ENDING_SCORE", GlobalEndingData->get_score_html(NULL));
		}

		return output("ending_score.html");
	}

	if (PLAYER_TABLE->length() > CGame::mMaxUser)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(
						GETTEXT("The game server has maximum number of players(%1$s)."),
						dec2unit(CGame::mMaxUser)));
		return output("create_character_error.html");
	}

	ITEM("STRING_CHARACTER_NAME", GETTEXT("Character Name"));

//	system_log("end handler %s", get_name());
	
	return output("create_character.html");
}
