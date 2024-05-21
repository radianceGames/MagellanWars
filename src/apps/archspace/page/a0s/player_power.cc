#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool 
CBotPagePlayerPower::handle(CConnection &aConnection)
{
//	system_log("start handler %s", get_name());
	if (!CBotPage::handle(aConnection)) return false;
	static CString out;
	out.clear();
	QUERY("ID", IDString);
	QUERY("TYPE", TypeString);
	if (!IDString || !TypeString)
	{
	    output_text("error");
	    return true;
    }
        
	int ID = as_atoi(IDString);
	
	if (!strcmp(TypeString, "COUNCIL"))
	{
	    CCouncil *Council = COUNCIL_TABLE->get_by_id(ID);
	    if (!Council)
            out = "|N/A|N/A|N/A";
        else
        {
           int Planets = 0;
           int Fleets = 0;
           int Power = Council->get_power();
           CRankTable *RankTable = COUNCIL_TABLE->get_planet_rank_table();
           CRank *Rank = RankTable->get_by_id(ID);
           if (Rank)
              Planets = Rank->get_number();
           RankTable = COUNCIL_TABLE->get_fleet_rank_table();
           Rank = RankTable->get_by_id(ID);
           if (Rank)
              Fleets = Rank->get_number();
           out.format("|%d|%d|%d", Power, Planets, Fleets);
        }    
	}
    else if (!strcmp(TypeString, "PLAYER"))
    {	    
	    CPlayer *Player = PLAYER_TABLE->get_by_game_id(ID);
	    if (!Player || Player->get_game_id() == EMPIRE_GAME_ID || Player->is_dead())
	    {
	         out = "|N/A|N/A";
        } 	
        else
        {
             int CouncilPower = 0;	    
             CCouncil *Council = Player->get_council();
             if (Council)
             CouncilPower = Council->get_power();
             out.format("|%d|%d", Player->get_power(),CouncilPower);
        }
    }
    else
    {
        out = "invalid choice";    
    }    	
  	output_text((char *)out);
//	system_log("end handler %s", get_name());
	return true;
}
