#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool 
CBotPageEndScore::handle(CConnection &aConnection)
{
//	system_log("start handler %s", get_name());
	if (!CBotPage::handle(aConnection)) return false;
	static CString out;
	out.clear();
	CGlobalEnding *GlobalEndingData = GAME->get_global_ending_data();
	if (GlobalEndingData->is_final_score() == false)
	{
		output_text("no ending...");
	}
	CRank *aRank = NULL;
	CRankTable *aRankTable = GlobalEndingData->get_player_score_list();
	for (int i = 0; i < aRankTable->length(); i++)
	{
		aRank = (CRank*)aRankTable->get(i);
		if (aRank == NULL)
			continue;
		CPlayer *aPlayer = PLAYER_TABLE->get_by_game_id(aRank->get_id());
		if (aPlayer == NULL)
			continue;
		CCouncil *aCouncil = aPlayer->get_council();
		if (aCouncil == NULL)
			continue;
		int numPlanets = -1;
		CPlanetList *aPlanetList = aPlayer->get_planet_list();
		if (aPlanetList == NULL)
			continue;
		numPlanets = aPlanetList->length();
		if (numPlanets <= 0)
			continue;
		out.format("Rank: %s: Player %s: Score: %d Planets: %d Council: %s<BR>", dec2unit(i+1), aPlayer->get_nick(), aRank->get_power(), numPlanets, aCouncil->get_nick());
	}

	out.format("<BR><BR><BR>******************<BR><BR><BR>");

	aRankTable = GlobalEndingData->get_council_score_list();
	for (int i = 0; i < aRankTable->length(); i++)
	{
		aRank = (CRank*)aRankTable->get(i);
		if (aRank == NULL)
			continue;
		CCouncil *aCouncil = COUNCIL_TABLE->get_by_id(aRank->get_id());
		if (aCouncil == NULL)
			continue;
		CPlayerList *MemberList = aCouncil->get_members();
		if (MemberList == NULL)
			continue;
		int numPlanets = 0;
		for (int j=0; j < MemberList->length(); j++)
		{
			CPlayer *Member = (CPlayer *)MemberList->get(j);
			if (Member == NULL)
				continue;
			CPlanetList *PlanetList = Member->get_planet_list();
			if (PlanetList == NULL)
				continue;
			numPlanets += PlanetList->length();
		} 
		if (numPlanets <= 0)
			continue;
		out.format("Rank: %s; Council: %s; Score: %d; Planets: %d; Members: %d<BR>", dec2unit(i+1), aCouncil->get_nick(), aRank->get_power(), numPlanets, MemberList->length());
	}

  	output_text((char *)out);
//	system_log("end handler %s", get_name());
	return true;
}
