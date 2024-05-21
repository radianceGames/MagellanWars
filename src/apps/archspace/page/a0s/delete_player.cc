#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool 
CBotPageDeletePlayer::handle(CConnection &aConnection)
{
//	system_log("start handler %s", get_name());
	if (!CBotPage::handle(aConnection)) return false;
	QUERY("ID", IDString);
	if (!IDString)
	{
	    output_text("error");
	    return true;
    }
        
	int ID = as_atoi(IDString);

	CPlayer *
		Player = PLAYER_TABLE->get_by_portal_id(ID);
	if (!Player)
	{
	    output_text("Player is NULL");
	    return true;
	}    
	Player->remove_from_database();
	Player->remove_from_memory();
	Player->remove_news_files();
	PLAYER_TABLE->remove_player(Player->get_game_id());
	Player->set_dead(
			(char *)GETTEXT("Deleted by GM's punishment."));

   	output_text("delete successfull");
//	system_log("end handler %s", get_name());
	return true;
}
