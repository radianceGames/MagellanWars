#include "ending.h"
#include "define.h"
#include <libintl.h>
#include "archspace.h"
#include "game.h"
#include "council.h"

int CGlobalEnding::mScorePerPlanet = 100;
int CGlobalEnding::mPopulationPerScore = 1000;
int CGlobalEnding::mScorePerTechLevel = 10;
int CGlobalEnding::mAdmiralExpPerScore = 100;
int CGlobalEnding::mProjectPricePerScore = 1000000;
int CGlobalEnding::mScorePerSecretProject = 1000;
int CGlobalEnding::mScorePerUsedTurn = -1;

int CGlobalEnding::mMultiplierForPersonalEnding = 10;
int CGlobalEnding::mMultiplierForAllKnownTechs = 10;
int CGlobalEnding::mMultiplierForTitle = 5;
int CGlobalEnding::mMultiplierForSpeaker = 5;
int CGlobalEnding::mMultiplierForHonor1 = 50;
int CGlobalEnding::mMultiplierForHonor2 = 5;

int CGlobalEnding::mScorePerFortressForPlayer = 1000;
int CGlobalEnding::mScorePerFortressForCouncilLayer1 = 9000;
int CGlobalEnding::mScorePerFortressForCouncilLayer2 = 12000;
int CGlobalEnding::mScorePerFortressForCouncilLayer3 = 15000;
int CGlobalEnding::mScorePerFortressForCouncilLayer4 = 18000;
int CGlobalEnding::mScorePerEmpireCapitalPlanet = 21000;

CGlobalEnding::CGlobalEnding()
{
	mIsFinalScore = false;
}

CGlobalEnding::~CGlobalEnding()
{
}

bool
CGlobalEnding::add_player_score(CRank *aScore)
{
	if (aScore == NULL) return false;

	mPlayerScoreList.add_rank(aScore);
	return true;
}

bool
CGlobalEnding::add_council_score(CRank *aScore)
{
	if (aScore == NULL) return false;

	mCouncilScoreList.add_rank(aScore);
	return true;
}

void
CGlobalEnding::set_final_score()
{
	mIsFinalScore = true;
}

char *
CGlobalEnding::get_score_html(CPlayer *aPlayer)
{
	static CString
		HTML;
	HTML.clear();

	/* The Council That Defeated Caput Mundi */
	/*HTML = "<TR>\n";
	HTML.format("<TH CLASS=maintext ALIGN=\"CENTER\">%s</TH>",
				GETTEXT("The council that defeated Caput Mundi"));
	HTML += "</TR>\n";

	HTML += "<TR>\n";

	HTML += "<TD ALIGN=\"CENTER\">\n";
	HTML += "<TABLE WIDTH=570 BORDER=1 BORDERCOLOR=#2A2A2A CELLSPACING=0 CELLPADDING=0>\n";

	HTML += "<TR>\n";
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("Rank"));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("Name"));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("# of Members"));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("# of Planets of Members"));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("Total Score of Members"));
	HTML += "</TR>\n";

	CPlayer *
		Player = PLAYER_TABLE->get_by_game_id(EMPIRE_CAPITAL_PLANET->get_owner_id());
	if (Player == NULL)
	{
		SLOG("ERROR : Player is NULL in CGlobalEnding::get_score_html(), EMPIRE_CAPITAL_PLANET->get_owner() = %d", EMPIRE_CAPITAL_PLANET->get_owner_id());
		return " ";
	}

	CCouncil *
		Council = Player->get_council();
	CPlayerList *
		MemberList = Council->get_members();

	int
		NumberOfPlanets = 0;
	for (int i=0 ; i<MemberList->length() ; i++)
	{
		CPlayer *
			Member = (CPlayer *)MemberList->get(i);
		CPlanetList *
			PlanetList = Member->get_planet_list();
		NumberOfPlanets += PlanetList->length();
	}

	CRank *
		CouncilScore = mCouncilScoreList.get_by_id(Council->get_id());
	int
		CouncilRank = mCouncilScoreList.get_rank_by_id(Council->get_id());

	HTML += "<TR>\n";
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
				dec2unit(CouncilRank));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
				Council->get_nick());
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
				dec2unit(MemberList->length()));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
				dec2unit(NumberOfPlanets));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
				dec2unit(CouncilScore->get_power()));
	HTML += "</TR>\n";

	HTML += "</TABLE>\n";
	HTML += "</TD>\n";

	HTML += "</TR>\n";*/

	/* Blank Line */
	HTML += "<TR>\n";
	HTML += "<TD>&nbsp;</TD>";
	HTML += "</TR>\n";

	/* The Universe Information */
	HTML += "<TR>\n";
	HTML.format("<TH CLASS=maintext ALIGN=\"CENTER\">%s</TH>",
				GETTEXT("The Universe Information"));
	HTML += "</TR>\n";

	HTML += "<TR>\n";

	HTML += "<TD ALIGN=\"CENTER\">\n";
	HTML += "<TABLE WIDTH=570 BORDER=1 BORDERCOLOR=#2A2A2A CELLSPACING=0 CELLPADDING=0>\n";

	HTML += "<TR>\n";
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
			GETTEXT("Total # of Councils"));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
			GETTEXT("Total # of Players"));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
			GETTEXT("Total # of Planets"));
	HTML += "</TR>\n";

	HTML += "<TR>\n";
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
			dec2unit(COUNCIL_TABLE->length()));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
			dec2unit(PLAYER_TABLE->length()));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
			dec2unit(PLANET_TABLE->length()));
	HTML += "</TR>\n";

	HTML += "</TABLE>\n";
	HTML += "</TD>\n";

	HTML += "</TR>\n";

	/* Blank Line */
	HTML += "<TR>\n";
	HTML += "<TD>&nbsp;</TD>";
	HTML += "</TR>\n";

	/* Top 10 Councils */
	HTML += "<TR>\n";
	HTML.format("<TH CLASS=maintext ALIGN=\"CENTER\">%s</TH>",
				GETTEXT("Top 10 Councils"));
	HTML += "</TR>\n";

	HTML += "<TR>\n";

	HTML += "<TD ALIGN=\"CENTER\">\n";
	HTML += "<TABLE WIDTH=570 BORDER=1 BORDERCOLOR=#2A2A2A CELLSPACING=0 CELLPADDING=0>\n";

	HTML += "<TR>\n";
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("Rank"));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("Name"));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("# of Members"));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("# of Planets of Members"));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("Total Score of Members"));
	HTML += "</TR>\n";

	for (int i=0 ; i<10 & i<mCouncilScoreList.length() ; i++)
	{
		CRank *
			CouncilScore = (CRank *)mCouncilScoreList.get(i);
		CCouncil *
			Council = COUNCIL_TABLE->get_by_id(CouncilScore->get_id());
		CPlayerList *
			MemberList = Council->get_members();

		int
			NumberOfPlanets = 0;
		for (int j=0 ; j<MemberList->length() ; j++)
		{
			CPlayer *
				Member = (CPlayer *)MemberList->get(j);
			CPlanetList *
				PlanetList = Member->get_planet_list();
			NumberOfPlanets += PlanetList->length();
		}

		HTML += "<TR>\n";
		HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
					dec2unit(i+1));
		HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
					Council->get_nick());
		HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
					dec2unit(MemberList->length()));
		HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
					dec2unit(NumberOfPlanets));
		HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
					dec2unit(CouncilScore->get_power()));
		HTML += "</TR>\n";
	}

	HTML += "</TABLE>\n";
	HTML += "</TD>\n";

	HTML += "</TR>\n";

	/* Blank Line */
	HTML += "<TR>\n";
	HTML += "<TD>&nbsp;</TD>";
	HTML += "</TR>\n";

	/* Top 10 Players */
	HTML += "<TR>\n";
	HTML.format("<TH CLASS=maintext ALIGN=\"CENTER\">%s</TH>",
				GETTEXT("Top 10 Players"));
	HTML += "</TR>\n";

	HTML += "<TR>\n";

	HTML += "<TD ALIGN=\"CENTER\">\n";
	HTML += "<TABLE WIDTH=570 BORDER=1 BORDERCOLOR=#2A2A2A CELLSPACING=0 CELLPADDING=0>\n";

	HTML += "<TR>\n";
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("Rank"));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("Name"));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("Race"));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("# of Planets Under Control"));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("# of Fleets Under Control"));
	HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
				GETTEXT("Score"));
	HTML += "</TR>\n";

	for (int i=0 ; i<10 & i<mPlayerScoreList.length() ; i++)
	{
		CRank *
			PlayerScore = (CRank *)mPlayerScoreList.get(i);
		CPlayer *
			Player = PLAYER_TABLE->get_by_game_id(PlayerScore->get_id());
		CPlanetList *
			PlanetList = Player->get_planet_list();
		CFleetList *
			FleetList = Player->get_fleet_list();

		HTML += "<TR>\n";
		HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
					dec2unit(i+1));
		HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
					Player->get_nick());
		HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
					Player->get_race_name());
		HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
					dec2unit(PlanetList->length()));
		HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
					dec2unit(FleetList->length()));
		HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
					dec2unit(PlayerScore->get_power()));
		HTML += "</TR>\n";
	}

	HTML += "</TABLE>\n";
	HTML += "</TD>\n";

	HTML += "</TR>\n";

	/* Blank Line */
	HTML += "<TR>\n";
	HTML += "<TD>&nbsp;</TD>";
	HTML += "</TR>\n";

	if (aPlayer != NULL)
	{
		if (aPlayer->is_dead() == false)
		{
			/* Player's Council Score */
			HTML += "<TR>\n";
			HTML.format("<TH CLASS=maintext ALIGN=\"CENTER\">%s</TH>",
						GETTEXT("Your Council's Score"));
			HTML += "</TR>\n";

			HTML += "<TR>\n";

			HTML += "<TD ALIGN=\"CENTER\">\n";
			HTML += "<TABLE WIDTH=570 BORDER=1 BORDERCOLOR=#2A2A2A CELLSPACING=0 CELLPADDING=0>\n";

			HTML += "<TR>\n";
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
						GETTEXT("Rank"));
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
						GETTEXT("Name"));
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
						GETTEXT("# of Members"));
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
						GETTEXT("# of Planets of Members"));
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
						GETTEXT("Total Score of Members"));
			HTML += "</TR>\n";

			CCouncil *
				YourCouncil = aPlayer->get_council();
			CRank *
				YourCouncilScore = (CRank *)mCouncilScoreList.get_by_id(YourCouncil->get_id());
			int
				YourCouncilRank = mCouncilScoreList.get_rank_by_id(YourCouncil->get_id());
			CPlayerList *
				YourCouncilMemberList = YourCouncil->get_members();

			int
				YourCouncilNumberOfPlanets = 0;
			for (int j=0 ; j<YourCouncilMemberList->length() ; j++)
			{
				CPlayer *
					Member = (CPlayer *)YourCouncilMemberList->get(j);
				CPlanetList *
					PlanetList = Member->get_planet_list();
				YourCouncilNumberOfPlanets += PlanetList->length();
			}

			HTML += "<TR>\n";
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
						dec2unit(YourCouncilRank));
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
						YourCouncil->get_nick());
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
						dec2unit(YourCouncilMemberList->length()));
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
						dec2unit(YourCouncilNumberOfPlanets));
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
						dec2unit(YourCouncilScore->get_power()));
			HTML += "</TR>\n";

			HTML += "</TABLE>\n";
			HTML += "</TD>\n";

			HTML += "</TR>\n";

			/* Blank Line */
			HTML += "<TR>\n";
			HTML += "<TD>&nbsp;</TD>";
			HTML += "</TR>\n";

			CRank *
				YourScore = mPlayerScoreList.get_by_id(aPlayer->get_game_id());
			if (YourScore == NULL)
			  	return (char *)HTML;
			/* Player's Score */
			HTML += "<TR>\n";
			HTML.format("<TH CLASS=maintext ALIGN=\"CENTER\">%s</TH>",
						GETTEXT("Your Score"));
			HTML += "</TR>\n";

			HTML += "<TR>\n";

			HTML += "<TD ALIGN=\"CENTER\">\n";
			HTML += "<TABLE WIDTH=570 BORDER=1 BORDERCOLOR=#2A2A2A CELLSPACING=0 CELLPADDING=0>\n";

			HTML += "<TR>\n";
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
						GETTEXT("Rank"));
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
						GETTEXT("Name"));
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
						GETTEXT("Race"));
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
						GETTEXT("# of Planets Under Control"));
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
						GETTEXT("# of Fleets Under Control"));
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\" BGCOLOR=\"#171717\">%s</TD>\n",
						GETTEXT("Score"));
			HTML += "</TR>\n";

			int
				YourRank = mPlayerScoreList.get_rank_by_id(aPlayer->get_game_id());
			CPlanetList *
				PlanetList = aPlayer->get_planet_list();
			CFleetList *
				FleetList = aPlayer->get_fleet_list();

			HTML += "<TR>\n";
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
						dec2unit(YourRank));
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
						aPlayer->get_nick());
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
						aPlayer->get_race_name());
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
						dec2unit(PlanetList->length()));
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
						dec2unit(FleetList->length()));
			HTML.format("<TD CLASS=tabletxt ALIGN=\"CENTER\">%s</TD>\n",
						dec2unit(YourScore->get_power()));
			HTML += "</TR>\n";

			HTML += "</TABLE>\n";
			HTML += "</TD>\n";

			HTML += "</TR>\n";
		}
	}

	return (char *)HTML;
}

