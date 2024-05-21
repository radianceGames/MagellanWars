#include <libintl.h>
#include "../pages.h"
#include "../archspace.h"
#include "../game.h"

bool 
CPageLogin::handle(CConnection &aConnection)
{
//	system_log("start handler %s", get_name());
	if (!CPageCommon::handle(aConnection)) return false;

	int PortalID = CONNECTION->get_portal_id();
	system_log("%d", PortalID);
	CPlayer*
		Player = PLAYER_TABLE->get_by_portal_id(PortalID);

	if (!Player)
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

				ITEM("ENDING_SCORE", GlobalEndingData->get_score_html(NULL));
			}

			return output("ending_score.html");
		}
		else
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("You should create your own character first."
							" Please use the create character menu."));
			return output("login_error.html");
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

			ITEM("ENDING_SCORE", GlobalEndingData->get_score_html(Player));
		}

		return output("ending_score.html");
	}

	if (Player->is_dead())
	{
		CONNECTION->as_string().set_value("GAME_ID", 
					(char*)format("%d", Player->get_game_id()));
		CONNECTION->as_string().set_value("GAME_NAME",
					Player->get_name());
		
		return output("death.html");
		
	}

	static time_t t = time(0);
	static int count = 0;

	if( t <= time(0)-60 ){
		system_log( "login users %d during %d seconds", count, time(0)-t );
		count = 0;
		t = time(0);
	}
	count++;

	CONNECTION->as_string().set_value("GAME_ID", 
				(char*)format("%d", Player->get_game_id()));
	CONNECTION->as_string().set_value("GAME_NAME", Player->get_name());
	CONNECTION->as_string().set_value("COUNCIL_ID", 
				(char*)format("%d", Player->get_council()->get_id()));
	CONNECTION->as_string().set_value("COUNCIL_NAME", 
				(char*)format("%s", Player->get_council()->get_name()));

	CCouncil *
		Council = Player->get_council();
	if (Council->get_speaker_id() == Player->get_game_id())
	{
		CONNECTION->as_string().set_value("IS_SPEAKER", "YES");
	}
	else
	{
		CONNECTION->as_string().set_value("IS_SPEAKER", "NO");
	}

	if (Council->get_speaker() == NULL)
	{
		CONNECTION->as_string().set_value("HAS_SPEAKER", "NO");
	}
	else
	{
		CONNECTION->as_string().set_value("HAS_SPEAKER", "YES");
	}

//	CString ASString = CONNECTION->as_string().get('&');
//	SLOG("as_string:%s", (char*)ASString);

	Player->set_last_login(time(0));
	Player->set_last_login_ip((char *)aConnection.get_connection());
	Player->type(QUERY_UPDATE);
	STORE_CENTER->store(*Player);

	ITEM("LOADING_MESSAGE",
			GETTEXT("Entering Archspace. Please wait..."));

	ADMIN_TOOL->add_login_player_log(
			(char *)format("Player %s has logged in. [%s]",
							Player->get_nick(), Player->get_last_login_ip()));

	SLOG_FOR_PSTAT(
			(char *)format("Enter game %s [%s]",
							Player->get_nick(), Player->get_last_login_ip()));

//	system_log("end handler %s", get_name());
	
	return output("login.html");
}
