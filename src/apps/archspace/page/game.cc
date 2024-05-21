#include <libintl.h>
#include "../pages.h"
#include "../archspace.h"
#include "../game.h"
#include "../define.h"
int
CPageGame::get_game_id()
{
	if (!mConnection) return -1;

	int
		ArchspaceID = CONNECTION->get_game_id();

	if (ArchspaceID < 0)
	{
		return -1;
	}
	else
	{
		return ArchspaceID;
	}
}


CPlayer *
CPageGame::get_player()
{
	if (!mConnection) return NULL;

	int 
		ArchspaceID = get_game_id();
	if (ArchspaceID < 0) return NULL;

	CPlayer *
		Player = GAME->get_player_by_game_id(ArchspaceID);
	if (Player == NULL) return NULL;

	return Player;
}

bool
CPageGame::handle(CConnection &aConnection)
{
	mConnection = &aConnection;

	mConversion.remove_all();
	get_conversion();

	if (!check_referrer())
	    return false;
	    
	CPlayer *
		Player = get_player(); 
	if (Player == NULL)
	{
		ITEM("STRING_CHARACTER_NAME", GETTEXT("Character Name"));

		return output("create_character.html");
	}       

	if (CONNECTION->get_game_id() <= 0)
	{
		message_page(GETTEXT("Your character was not found."));
		return false; 
	}

	if (strstr(allow_method(), aConnection.get_method()) == NULL)
	{
		message_page(GETTEXT("This access is forbidden."));
		return true;
	}

	mQuery.remove_all();
	mQuery.set(QUERY_STRING, '&');

	if (Player->is_dead())
	{
		return output("death.html");
	}

	CCouncil *
		Council = Player->get_council();
	CONNECTION->as_string().set_value("COUNCIL_ID",
										(char *)format("%d", Council->get_id()));
	CONNECTION->as_string().set_value("COUNCIL_NAME",
										(char *)format("%s", Council->get_name()));

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

	return handler(Player);
}
