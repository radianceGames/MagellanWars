#include <libintl.h>
#include "spy.h"
#include "relation.h"
#include "player.h"
#include "archspace.h"
#include "game.h"
#include "council.h"
#include "prerequisite.h"

int
CSpy::mHonorLossTable[][5] =
{
	{1,   0,  0,  0,  0},
	{2,  1,  0,  0,  0},
	{4, 3, 2,  1,  0},
	{5, 4, 3, 2,  1}
};

char *
CSpy::mRelationChangeTable[][5] =
{
	{"NONE",  "NONE", "NONE",      "NONE", "NONE"},
	{"PACT", "TRUCE",  "WAR",      "NONE", "NONE"},
	{"PACT", "TRUCE",  "WAR",      "NONE", "NONE"},
	{ "WAR",   "WAR",  "WAR", "TOTAL_WAR", "NONE"}
};

CSpy::CSpy()
{
	mID = 0;
	mDifficulty = 0;
	mCost = 0;
	mPrerequisite = 0;
	mType = -1;
}

CSpy::~CSpy()
{
}

int
CSpy::get_honor_loss(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer, int aSpyType)
{
	if (!aSpyPlayer || !aTargetPlayer) return -1;
	if (aSpyType < TYPE_ACCEPTABLE || aSpyType > TYPE_ATROCIOUS) return -1;

	CRelation *
		Relation = aSpyPlayer->get_relation(aTargetPlayer);
	if (!Relation) return -1;

	int
		BaseLoss;
	CControlModel *
		TargetCM = aTargetPlayer->get_control_model();

	switch (Relation->get_relation())
	{
		case CRelation::RELATION_ALLY :
			BaseLoss = mHonorLossTable[aSpyType][0];
			break;

		case CRelation::RELATION_PEACE :
			BaseLoss = mHonorLossTable[aSpyType][1];
			break;

		case CRelation::RELATION_TRUCE :
			BaseLoss = mHonorLossTable[aSpyType][2];
			break;

		case CRelation::RELATION_WAR :
			BaseLoss = mHonorLossTable[aSpyType][3];
			break;

		case CRelation::RELATION_TOTAL_WAR :
			BaseLoss = mHonorLossTable[aSpyType][4];
			break;

		default :
			return -1;
	}

	if (BaseLoss == 0) return 0;

	int
		HonorLoss = BaseLoss - TargetCM->get_diplomacy();
	if (HonorLoss < 0) HonorLoss = 0;

	return HonorLoss;
}

int
CSpy::get_relation_change(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer, int aSpyType)
{
	if (!aSpyPlayer || !aTargetPlayer) return -1;
	if (aSpyType < TYPE_ACCEPTABLE || aSpyType > TYPE_ATROCIOUS) return -1;

	CRelation *
		Relation = aSpyPlayer->get_relation(aTargetPlayer);
	if (!Relation) return -1;

	CString
		RelationName;
	RelationName.clear();

	switch (Relation->get_relation())
	{
		case CRelation::RELATION_ALLY :
			RelationName = mRelationChangeTable[aSpyType][0];
			break;

		case CRelation::RELATION_PEACE :
			RelationName = mRelationChangeTable[aSpyType][1];
			break;

		case CRelation::RELATION_TRUCE :
			RelationName = mRelationChangeTable[aSpyType][2];
			break;

		case CRelation::RELATION_WAR :
			RelationName = mRelationChangeTable[aSpyType][3];
			break;

		case CRelation::RELATION_TOTAL_WAR :
			RelationName = mRelationChangeTable[aSpyType][4];
			break;

		default :
			return -1;
	}

	if (!strcmp((char *)RelationName, "PACT")) return CRelation::RELATION_PEACE;
	if (!strcmp((char *)RelationName, "TRUCE")) return CRelation::RELATION_TRUCE;
	if (!strcmp((char *)RelationName, "WAR")) return CRelation::RELATION_WAR;
	if (!strcmp((char *)RelationName, "TOTAL_WAR")) return CRelation::RELATION_TOTAL_WAR;
	if (!strcmp((char *)RelationName, "NONE")) return CRelation::RELATION_NONE;

	return -1;
}

int
CSpy::get_security_status(int aSecurityLevel)
{
	if (aSecurityLevel < 1 || aSecurityLevel > 5) return -1;

	switch (aSecurityLevel)
	{
		case 1 :
			return 0;

		case 2 :
			return 50;

		case 3 :
			return 100;

		case 4 :
			return 200;

		case 5 :
			return 400;

		default :
			return -1;
	}
}

int
CSpy::get_security_upkeep(int aSecurityLevel, int aIncome)
{
	if (aSecurityLevel < 1 || aSecurityLevel > 5) return 0;
	if (aIncome < 0) return 0;

	switch (aSecurityLevel)
	{
		case 1 :
			return 0;

		case 2 :
			return (int)((long long int)aIncome * 2.5 / 100);

		case 3 :
			return (int)((long long int)aIncome * 5 / 100);

		case 4 :
			return (int)((long long int)aIncome * 10 / 100);

		case 5 :
			return (int)((long long int)aIncome * 20 / 100);

		default :
			return 0;
	}
}

char *
CSpy::get_security_level_description(int aSecurityLevel)
{
	switch (aSecurityLevel)
	{
		case 1 :
			return GETTEXT("Defenseless");

		case 2 :
			return GETTEXT("Loose");

		case 3 :
			return GETTEXT("Wary");

		case 4 :
			return GETTEXT("Alerted");

		case 5 :
			return GETTEXT("Impenetrable");

		default :
			return NULL;
	}
}

char *
CSpy::get_type_name()
{
	switch (mType)
	{
		case TYPE_ACCEPTABLE :
			return GETTEXT("Acceptable");

		case TYPE_ORDINARY :
			return GETTEXT("Ordinary");

		case TYPE_HOSTILE :
			return GETTEXT("Hostile");

		case TYPE_ATROCIOUS :
			return GETTEXT("Atrocious");

		default :
			return NULL;
	}
}

char *
CSpy::perform(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer, bool *aSuccess)
{
	*aSuccess = false;

	static CString
		Result;
	Result.clear();

	if (!aSpyPlayer || !aTargetPlayer)
	{
		Result = GETTEXT("The player trying to spy or the target player is wrong.");
		return (char *)Result;
	}

	if (aTargetPlayer->is_dead())
	{
		Result = GETTEXT("The target player is dead now.");
		return (char *)Result;
	}

	if (aSpyPlayer->get_production() < get_cost())
	{
		Result = GETTEXT("You don't have enough PP to perform this special operation.");
		return (char *)Result;
	}

	if (mPrerequisite)
	{
		CKnownTechList *
			KnownTechList = aSpyPlayer->get_tech_list();
		CKnownTech *
			KnownTech = KnownTechList->get_by_id(mPrerequisite);

		if (!KnownTech)
		{
			Result = GETTEXT("You don't know the prerequisite technology"
							" to perform this special operation.");
			return (char *)Result;
		}
	}

#define METEOR_STRIKE			8012
#define STELLAR_BOMBARDMENT		8014

	if (aSpyPlayer->has_ability(ABILITY_NO_SPY))
	{
		if (mID != METEOR_STRIKE && mID != STELLAR_BOMBARDMENT)
		{
			Result.format(GETTEXT("You can't perform %1$s for some reason."),
									(char *)mName);
			return (char *)Result;
		}
	}

	if (aTargetPlayer->has_ability(ABILITY_NO_SPY))
	{
		if (mID != METEOR_STRIKE && mID != STELLAR_BOMBARDMENT)
		{
			Result.format(GETTEXT("You can't perform %1$s for some reason."),
									(char *)mName);
			return (char *)Result;
		}
	}

#undef METEOR_STRIKE
#undef STELLAR_BOMBARDMENT

	if (aSpyPlayer->has_ability(ABILITY_PACIFIST) && mType == TYPE_ATROCIOUS)
	{
		Result.format(GETTEXT("You can't perform %1$s because you are a pacifist."), (char *)mName);
		return (char *)Result;
	}

	*aSuccess = true;

	Result = GETTEXT("Your spy was successful. The report is below:");
	Result += "<BR><BR>\n";

	switch (mID)
	{
		case 8001 :
			Result += general_information_gathering(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		case 8002 :
			Result += detailed_information_gathering(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		case 8003 :
			Result += steal_secret_info(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		case 8004 :
			Result += computer_virus_infiltration(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		case 8005 :
			Result += devastating_network_worm(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		case 8006 :
			Result += sabotage(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		case 8007 :
			Result += incite_riot(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		case 8008 :
			Result += steal_common_technology(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		case 8009 :
			Result += artificial_disease(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		case 8010 :
			Result += red_death(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		case 8011 :
			Result += strike_base(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		case 8012 :
			Result += meteor_strike(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		case 8013 :
			Result += emp_storm(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		case 8014 :
			Result += stellar_bombardment(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		case 8015 :
			Result += assassination(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		case 8016 :
			Result += steal_important_technology(aSpyPlayer, aTargetPlayer);
			return (char *)Result;
		case 8017 :
			Result += steal_secret_technology(aSpyPlayer, aTargetPlayer);
			return (char *)Result;

		default :
			Result += GETTEXT("A wrong spy ID was entered.");
			return (char *)Result;
	}
}

// ID : 8001
char *
CSpy::general_information_gathering(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	int
		NumberOfInfo = 1 + dice(1, 3);
	CCommandSet
		InfoSet;

	for (int i=1 ; i<=NumberOfInfo ; i++)
	{
		while (1)
		{
			int
				Random = number(8);
			if (InfoSet.has(Random)) continue;

			InfoSet += Random;
			break;
		}
	}

	bool
		AnyResult = false;

	if (InfoSet.has(1))
	{
		if (AnyResult) Result += "<BR>\n";
		Result.format(GETTEXT("PP : %1$s"), dec2unit(aTargetPlayer->get_production()));

		AnyResult = true;
	}
	if (InfoSet.has(2))
	{
		if (AnyResult) Result += "<BR>\n";
		Result.format(GETTEXT("Population : %1$s"),
						dec2unit(aTargetPlayer->calc_population()));

		AnyResult = true;
	}
	if (InfoSet.has(3))
	{
		if (AnyResult) Result += "<BR>\n";
		Result.format("%s : %s",
						GETTEXT("Concentration Mode"),
						aTargetPlayer->get_mode_name());

		AnyResult = true;
	}
	if (InfoSet.has(4))
	{
		CFleetList *
			FleetList = aTargetPlayer->get_fleet_list();

		if (AnyResult) Result += "<BR>\n";
		Result.format("%s : %s",
						GETTEXT("Fleet"),
						dec2unit(FleetList->length()));

		AnyResult = true;
	}
	if (InfoSet.has(5))
	{
		CDock *
			Dock = aTargetPlayer->get_dock();

		if (AnyResult) Result += "<BR>\n";
		Result.format(GETTEXT("Total Number of Ships in the Dock: %1$s"),
						dec2unit(Dock->get_ship_number()));

		AnyResult = true;
	}
	if (InfoSet.has(6))
	{
		int
			ProductPointPerTurn = 0;
		CPlanetList *
			PlanetList = aTargetPlayer->get_planet_list();

		for (int i=0 ; i<PlanetList->length() ; i++)
		{
			CPlanet *
				Planet = (CPlanet *)PlanetList->get(i);

			ProductPointPerTurn += Planet->get_pp_per_turn();
		}

		if (AnyResult) Result += "<BR>\n";
		Result.format(GETTEXT("PP Income Per Turn: %1$s"),
						dec2unit(ProductPointPerTurn));

		AnyResult = true;
	}
	if (InfoSet.has(7))
	{
		CKnownTechList *
			KnownTechList = aTargetPlayer->get_tech_list();

		if (AnyResult) Result += "<BR>\n";
		Result.format(GETTEXT("Total Number of Researched Techs : %1$s"),
					dec2unit(KnownTechList->length()));

		AnyResult = true;
	}
	if (InfoSet.has(8))
	{
		CAdmiralList *
			AdmiralPool = aTargetPlayer->get_admiral_pool();

		if (AnyResult) Result += "<BR>\n";
		Result.format(GETTEXT("Commanders in the Pool : %1$s"),
					dec2unit(AdmiralPool->length()));

		AnyResult = true;
	}

	aTargetPlayer->time_news(
			GETTEXT("Some of your general information has been drained by someone!"));

	return (char *)Result;
}

// ID : 8002
char *
CSpy::detailed_information_gathering(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	Result = general_information_gathering(aSpyPlayer, aTargetPlayer);
	Result += "<BR>\n";

	int
		DetailedInfo = number(9);
	if (DetailedInfo == 1)
	{
		Result.format("%s :<BR>\n", GETTEXT("A Random Planet's Status"));

		CPlanetList *
			PlanetList = aTargetPlayer->get_planet_list();
		int
			Random = number(PlanetList->length()) - 1;
		CPlanet *
			RandomPlanet = (CPlanet *)PlanetList->get(Random);

		Result += RandomPlanet->html_management_record();
	}
	if (DetailedInfo == 2)
	{
		Result.format("%s : ", GETTEXT("List of Projects Bought"));

		CPurchasedProjectList *
			PurchasedProjectList = aTargetPlayer->get_purchased_project_list();
		if (PurchasedProjectList->length() > 0)
		{
			bool
				AnyProject = false;
			for (int i=0 ; i<PurchasedProjectList->length() ; i++)
			{
				CPurchasedProject *
					Project = (CPurchasedProject *)PurchasedProjectList->get(i);

				if (AnyProject) Result += ", ";
				Result += Project->get_name();

				AnyProject = true;
			}
		}
		else
		{
			Result.format(GETTEXT("%1$s doesn't own any projects."),
							aTargetPlayer->get_nick());
		}
	}
	if (DetailedInfo == 3)
	{
		CTech *
			Tech = TECH_TABLE->get_by_id(aTargetPlayer->get_target_tech());
		if (!Tech)
		{
			Result.format("%s : %s",
							GETTEXT("Current Tech Research Goal"),
							GETTEXT("None"));
		}
		else
		{
			Result.format("%s : %s",
							GETTEXT("Current Tech Research Goal"),
							Tech->get_name());
		}
	}
	if (DetailedInfo == 4)
	{
		Result.format("%s : ", GETTEXT("Researched Tech List of Information Science"));

		CKnownTechList *
			KnownTechList = aTargetPlayer->get_tech_list();

		bool
			AnyTech = false;
		for (int i=0 ; i<KnownTechList->length() ; i++)
		{
			CKnownTech *
				Tech = (CKnownTech *)KnownTechList->get(i);
			if (Tech->get_type() != CTech::TYPE_INFORMATION) continue;

			if (AnyTech) Result += ", ";
			Result += Tech->get_name();

			AnyTech = true;
		}

		if (!AnyTech) Result += GETTEXT("None");
	}
	if (DetailedInfo == 5)
	{
		Result.format("%s : ", GETTEXT("Researched Tech List of Life Science"));

		CKnownTechList *
			KnownTechList = aTargetPlayer->get_tech_list();

		bool
			AnyTech = false;
		for (int i=0 ; i<KnownTechList->length() ; i++)
		{
			CKnownTech *
				Tech = (CKnownTech *)KnownTechList->get(i);
			if (Tech->get_type() != CTech::TYPE_LIFE) continue;

			if (AnyTech) Result += ", ";
			Result += Tech->get_name();

			AnyTech = true;
		}

		if (!AnyTech) Result += GETTEXT("None");
	}
	if (DetailedInfo == 6)
	{
		Result.format("%s : ", GETTEXT("Researched Tech List of Matter-Energy Science"));

		CKnownTechList *
			KnownTechList = aTargetPlayer->get_tech_list();

		bool
			AnyTech = false;
		for (int i=0 ; i<KnownTechList->length() ; i++)
		{
			CKnownTech *
				Tech = (CKnownTech *)KnownTechList->get(i);
			if (Tech->get_type() != CTech::TYPE_MATTER_ENERGY) continue;

			if (AnyTech) Result += ", ";
			Result += Tech->get_name();

			AnyTech = true;
		}

		if (!AnyTech) Result += GETTEXT("None");
	}
	if (DetailedInfo == 7)
	{
		Result.format("%s : ", GETTEXT("Researched Tech List of Social Science"));

		CKnownTechList *
			KnownTechList = aTargetPlayer->get_tech_list();

		bool
			AnyTech = false;
		for (int i=0 ; i<KnownTechList->length() ; i++)
		{
			CKnownTech *
				Tech = (CKnownTech *)KnownTechList->get(i);
			if (Tech->get_type() != CTech::TYPE_SOCIAL) continue;

			if (AnyTech) Result += ", ";
			Result += Tech->get_name();

			AnyTech = true;
		}

		if (!AnyTech) Result += GETTEXT("None");
	}

	if (DetailedInfo == 8)
	{
		Result.format("%s :<BR>\n", GETTEXT("Diplomacy Statuys List in the Target Player's Council"));
		CCouncil *
			Council = aTargetPlayer->get_council();
		Result += Council->player_relation_table_html(aTargetPlayer);
	}

	if (DetailedInfo == 9)
	{
		Result.format("%s :<BR>\n", GETTEXT("Control Model"));

		CControlModel *
			ControlModel = aTargetPlayer->get_control_model();

		Result = "<TABLE WIDTH=\"500\" BORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
		Result += "<TR>\n";

		Result += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">";
		Result.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>", GETTEXT("Environment"));
		Result += "</TD>\n";

		Result.format("<TD CLASS=\"tabletxt\" WIDTH=\"100\">&nbsp;%d</TD>\n",
						ControlModel->get_real_environment());

		Result += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">";
		Result.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>", GETTEXT("Production"));
		Result += "</TD>\n";

		Result += "</TR>\n";
		Result += "<TR>\n";

		Result.format("<TD CLASS=\"tabletxt\" WIDTH=\"100\">&nbsp;%d</TD>\n",
						ControlModel->get_real_production());

		Result += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">";
		Result.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>", GETTEXT("Growth"));
		Result += "</TD>\n";

		Result.format("<TD CLASS=\"tabletxt\" WIDTH=\"100\">&nbsp;%d</TD>\n",
						ControlModel->get_real_growth());

		Result += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">";
		Result.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>", GETTEXT("Military"));
		Result += "</TD>\n";

		Result.format("<TD CLASS=\"tabletxt\" WIDTH=\"100\">&nbsp;%d</TD>\n",
						ControlModel->get_real_military());

		Result += "</TR>\n";
		Result += "<TR>\n";

		Result += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">";
		Result.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>", GETTEXT("Commerce"));
		Result += "</TD>\n";

		Result.format("<TD CLASS=\"tabletxt\" WIDTH=\"100\">&nbsp;%d</TD>\n",
						ControlModel->get_real_commerce());

		Result += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">";
		Result.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>", GETTEXT("Diplomacy"));
		Result += "</TD>\n";

		Result.format("<TD CLASS=\"tabletxt\" WIDTH=\"100\">&nbsp;%d</TD>\n",
						ControlModel->get_real_diplomacy());

		Result += "</TR>\n";
		Result += "<TR>\n";

		Result += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">";
		Result.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>", GETTEXT("Efficiency"));
		Result += "</TD>\n";

		Result.format("<TD CLASS=\"tabletxt\" WIDTH=\"100\">&nbsp;%d</TD>\n",
						ControlModel->get_real_efficiency());

		Result += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">";
		Result.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>", GETTEXT("Genius"));
		Result += "</TD>\n";

		Result.format("<TD CLASS=\"tabletxt\" WIDTH=\"100\">&nbsp;%d</TD>\n",
						ControlModel->get_real_genius());

		Result += "</TR>\n";
		Result += "<TR>\n";

		Result += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">";
		Result.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>",
						GETTEXT("Facility Cost"));
		Result += "</TD>\n";

		Result.format("<TD CLASS=\"tabletxt\" WIDTH=\"100\">&nbsp;%d</TD>\n",
						ControlModel->get_real_facility_cost());

		Result += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">";
		Result.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>", GETTEXT("Spy"));
		Result += "</TD>\n";

		Result.format("<TD CLASS=\"tabletxt\" WIDTH=\"100\">&nbsp;%d</TD>\n",
						ControlModel->get_real_spy());

		Result += "</TR>\n";
		Result += "<TR>\n";

		Result += "<TD CLASS=\"tabletxt\" WIDTH=\"150\" BGCOLOR=\"#171717\">";
		Result.format("<FONT COLOR=\"666666\">&nbsp;%s</FONT>", GETTEXT("Research"));
		Result += "</TD>\n";

		Result.format("<TD COLSPAN=\"3\" CLASS=\"tabletxt\" WIDTH=\"100\">&nbsp;%d</TD>\n",
						ControlModel->get_real_research());

		Result += "</TR>\n";
		Result += "</TABLE>\n";
	}

	aTargetPlayer->time_news(
			GETTEXT("Some of your general information and detailed information have been drained by someone!"));

	return (char *)Result;
}

// ID : 8003
char *
CSpy::steal_secret_info(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	int
		DetailedInfo = number(3);
	switch (DetailedInfo)
	{
		case 1 :
		{
			Result.format("%s :<BR>\n", GETTEXT("An used ship design which was selected randomly"));

			CFleetList *
				FleetList = aTargetPlayer->get_fleet_list();
			if (FleetList->length() == 0)
			{
				Result.format(GETTEXT("%1$s has no fleets now."), aTargetPlayer->get_nick());
			}
			else
			{
				CFleet *
					Fleet = (CFleet *)FleetList->get(number(FleetList->length()) - 1);

				Result += Fleet->CShipDesign::print_html(aTargetPlayer);
			}

			aTargetPlayer->time_news(
					GETTEXT("Your secret information has been drained by someone!"));

			return (char *)Result;
		}

		case 2 :
		{
			Result.format("%s :<BR>\n", GETTEXT("Ship Pool Status"));

			CDock *
				Dock = aTargetPlayer->get_dock();
			if (Dock->length() == 0)
			{
				Result.format(GETTEXT("%1$s has no ships in his/her dock now."),
								aTargetPlayer->get_nick());

				return (char *)Result;
			}
			else Result += Dock->print_html();

			aTargetPlayer->time_news(
					GETTEXT("Your secret information has been drained by someone!"));

			return (char *)Result;
		}

		case 3 :
		{
			Result.format("%s :<BR>\n", GETTEXT("Fleet List"));

			CFleetList *
				FleetList = aTargetPlayer->get_fleet_list();

			Result += FleetList->all_fleet_list_html(aTargetPlayer);

			aTargetPlayer->time_news(
					GETTEXT("Your secret information has been drained by someone!"));

			return (char *)Result;
		}

		default :
			return GETTEXT("Wrong random number returned in secret spy."
							" Please ask Archspace Development Team.");
	}
}

// ID : 8004
char *
CSpy::computer_virus_infiltration(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	static CString
			News;
	News.clear();

	int
		ResearchDiff = aTargetPlayer->get_research() * number(20) / 100;
	if (ResearchDiff == 0)
	{
		Result = GETTEXT("Your spy was successful, but it was of no effect.");

		return (char *)Result;
	}

	aTargetPlayer->change_research(-ResearchDiff);

	Result.format(GETTEXT("Your spy was successful, and %1$s lost %2$d accumulated RP."),
					aTargetPlayer->get_nick(), ResearchDiff);

	News.format(GETTEXT("%1$d accumulated RP"), ResearchDiff);

	if (News.length() == 0)
	{
		// time news
	}
	else
	{
		aTargetPlayer->time_news(
				(char *)format(GETTEXT("You lost %1$s by someone's spy!"),
								(char *)News));
	}

	return (char *)Result;
}

// ID : 8005
char *
CSpy::devastating_network_worm(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	static CString
		News;
	News.clear();
	bool
		Comma = false;

	CKnownTechList *
		KnownTechList = aTargetPlayer->get_tech_list();
	int
		NumberOfTech = KnownTechList->count_by_category(CTech::TYPE_INFORMATION);

	int
		ResearchDiff = aTargetPlayer->get_research() * (65 - NumberOfTech) / 100;
	if (ResearchDiff == 0)
	{
		Result = GETTEXT("It had no effect on accumulated RP.");
	}
	else
	{
		aTargetPlayer->change_research(-ResearchDiff);

		Result.format(GETTEXT("%1$s has lost %2$d accumulated RP."),
						aTargetPlayer->get_nick(), ResearchDiff);

		News.format(GETTEXT("%1$d accumulated RP"), ResearchDiff);
		Comma = true;
	}

	int
		Rate = number(65) - NumberOfTech;
	if (Rate < 0) Rate = 0;

	int
		InvestedShipProductionDiff =
				aTargetPlayer->get_invested_ship_production() * Rate / 100;
	if (InvestedShipProductionDiff == 0)
	{
		Result += "<BR>\n";
		Result += GETTEXT("It had no effect on invested PP on ship production.");
	}
	else
	{
		aTargetPlayer->change_invested_ship_production(-InvestedShipProductionDiff);

		Result += "<BR>\n";
		Result.format(GETTEXT("%1$s has lost %2$d accumulated invested PP on ship production."),
						aTargetPlayer->get_nick(), InvestedShipProductionDiff);

		if (Comma) News += ", ";
		News.format(GETTEXT("%1$d invested PP on ship production"), InvestedShipProductionDiff);
		Comma = true;
	}

	int
		ResearchInvestDiff =
				aTargetPlayer->get_research_invest() * Rate / 100;
	if (ResearchInvestDiff == 0)
	{
		Result += "<BR>\n";
		Result += GETTEXT("It had no effect on invested PP on research.");
	}
	else
	{
		aTargetPlayer->change_research_invest(-ResearchInvestDiff);

		Result += "<BR>\n";
		Result.format(GETTEXT("%1$s has lost %2$d accumulated invested PP on ship production."),
						aTargetPlayer->get_nick(), ResearchInvestDiff);

		if (Comma) News += ", ";
		News.format(GETTEXT("%1$d invested PP on research"), ResearchInvestDiff);
		Comma = true;
	}

	CPlanetList *
		PlanetList = aTargetPlayer->get_planet_list();

	for (int i=0 ; i<PlanetList->length() ; i++)
	{
		CPlanet *
			Planet = (CPlanet *)PlanetList->get(i);

		int
			InvestmentDiff = Planet->get_investment() * Rate / 100;
		if (InvestmentDiff == 0)
		{
			Result += "<BR>\n";
			Result.format(GETTEXT("It had no effect on planet %1$s."),
					Planet->get_name());
		}
		else
		{
			Planet->change_investment(-InvestmentDiff);

			Result += "<BR>\n";
			Result.format(GETTEXT("%1$s has lost %2$d accumulated invested PP on %3$s."),
						aTargetPlayer->get_nick(), InvestmentDiff, Planet->get_name());

			if (Comma) News += ", ";
			News.format(GETTEXT("%1$d invested PP on %2$s"), InvestmentDiff, Planet->get_name());
			Comma = true;
		}
	}

	if (News.length() == 0)
	{
		// time news
	}
	else
	{
		aTargetPlayer->time_news(
				(char *)format(GETTEXT("You lost %1$s by someone's spy!"),
								(char *)News));
	}

	return (char *)Result;
}

// ID : 8006
char *
CSpy::sabotage(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	static CString
		News;
	News.clear();
	bool
		Comma = false;

	CPlanetList *
		PlanetList = aTargetPlayer->get_planet_list();
	CPlanet *
		Planet = (CPlanet *)PlanetList->get(number(PlanetList->length() - 1));
	CResource &
		Building = Planet->get_building();

	int
		FactoryDiff = Building.get(BUILDING_FACTORY) * number(20) / 100;
	if (FactoryDiff == 0)
	{
		Result.format(GETTEXT("It had no effect on planet %1$s's factories."),
						Planet->get_name());
	}
	else
	{
		Building.change(BUILDING_FACTORY, -FactoryDiff);

		Result.format(GETTEXT("%1$s has lost %2$d factories on %3$s."),
						aTargetPlayer->get_nick(), FactoryDiff, Planet->get_name());

		News.format(GETTEXT("%1$d factories on %2$s"),
					FactoryDiff, Planet->get_name());
		Comma = true;
	}

	int
		InvestmentDiff = Planet->get_investment() / 2;
	if (InvestmentDiff == 0)
	{
		Result += "<BR>\n";
		Result.format(GETTEXT("It had no effect on planet %1$s's invested PP."),
				Planet->get_name());
	}
	else
	{
		Planet->change_investment(-InvestmentDiff);

		Result += "<BR>\n";
		Result.format(GETTEXT("%1$s has lost %2$d accumulated invested PP on %3$s."),
						aTargetPlayer->get_nick(), InvestmentDiff, Planet->get_name());

		if (Comma) News += ", ";
		News.format(GETTEXT("%1$d accumulated PP on %2$s"), InvestmentDiff, Planet->get_name());
		Comma = true;
	}

	if (News.length() == 0)
	{
		// time news
	}
	else
	{
		aTargetPlayer->time_news(
				(char *)format(GETTEXT("You lost %1$s by someone's spy!"),
								(char *)News));
	}

	return (char *)Result;
}

// ID : 8007
char *
CSpy::incite_riot(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	static CString
		News;
	News.clear();
	bool
		Comma = false;

	CPlanetList *
		PlanetList = aTargetPlayer->get_planet_list();
	CPlanet *
		Planet = (CPlanet *)PlanetList->get(number(PlanetList->length()) - 1);
	CResource &
		Building = Planet->get_building();

	int
		FactoryDiff = Building.get(BUILDING_FACTORY) * number(15) / 100;
	if (FactoryDiff == 0)
	{
		Result.format(GETTEXT("It had no effect on planet %1$s's factories."),
						Planet->get_name());
	}
	else
	{
		Building.change(BUILDING_FACTORY, -FactoryDiff);

		Result.format(GETTEXT("%1$s has lost %2$d factories on %3$s."),
						aTargetPlayer->get_nick(), FactoryDiff, Planet->get_name());

		News.format(GETTEXT("%1$d factories on %2$s"),
					FactoryDiff, Planet->get_name());
		Comma = true;
	}

	int
		ResearchLabDiff = Building.get(BUILDING_RESEARCH_LAB) * number(15) / 100;
	if (ResearchLabDiff == 0)
	{
		Result.format(GETTEXT("It had no effect on planet %1$s's research labs."),
						Planet->get_name());
	}
	else
	{
		Building.change(BUILDING_RESEARCH_LAB, -ResearchLabDiff);

		Result.format(GETTEXT("%1$s has lost some research labs. on %2$s."),
						aTargetPlayer->get_nick(), Planet->get_name());

		if (Comma) News += ", ";
		News.format(GETTEXT("%1$d research labs. on %2$s"),
					ResearchLabDiff, Planet->get_name());
		Comma = true;
	}

	int
		InvestmentDiff = Planet->get_investment();
	if (InvestmentDiff == 0)
	{
		Result += "<BR>\n";
		Result.format(GETTEXT("It had no effect on planet %1$s's invested PP."),
				Planet->get_name());
	}
	else
	{
		Planet->change_investment(-InvestmentDiff);

		Result += "<BR>\n";
		Result.format(GETTEXT("%1$s lost all accumulated invested PP on %2$s."),
						aTargetPlayer->get_nick(), Planet->get_name());

		if (Comma) News += ", ";
		News.format(GETTEXT("all accumulated invested PP on %1$s"), Planet->get_name());
		Comma = true;
	}

	if (News.length() == 0)
	{
		// time news
	}
	else
	{
		aTargetPlayer->time_news(
				(char *)format(GETTEXT("You lost %1$s by someone's spy!"),
								(char *)News));
	}

	return (char *)Result;
}

// ID : 8008
char *
CSpy::steal_common_technology(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	CKnownTechList *
		KnownTechList = aSpyPlayer->get_tech_list();
	CTechList *
		AvailableTechList = aSpyPlayer->get_available_tech_list();
	CKnownTechList *
		TargetKnownTechList = aTargetPlayer->get_tech_list();

	CKnownTechList
		CandidateTechList;

	for (int i=0 ; i<TargetKnownTechList->length() ; i++)
	{
		CKnownTech *
			KnownTech = (CKnownTech *)TargetKnownTechList->get(i);
		if (KnownTech->get_level() >= 5) continue;

		if (AvailableTechList->get_by_id(KnownTech->get_id()))
		{
			CKnownTech
				*Tech = new CKnownTech(KnownTech->get_owner(), KnownTech->get_id(), KnownTech->get_attribute());
			CandidateTechList.add_known_tech(Tech);
		}
	}

	if (CandidateTechList.length() == 0)
	{
		Result.format(GETTEXT("There is no tech that %1$s has and you can learn."),
						aTargetPlayer->get_nick());
	}
	else
	{
		int
			RandomIndex = number(CandidateTechList.length()) - 1;
		CKnownTech *
			CandidateTech = (CKnownTech *)CandidateTechList.get(RandomIndex);

		aSpyPlayer->discover_tech(CandidateTech->get_id());

		KnownTechList->type(QUERY_UPDATE);
		STORE_CENTER->store(*KnownTechList);

		if (aSpyPlayer->get_target_tech() == CandidateTech->get_id())
		{
			aTargetPlayer->set_target_tech(0);
		}

		Result.format(GETTEXT("You stole a tech %1$s from %2$s successfully."),
						CandidateTech->get_name_with_level(), aTargetPlayer->get_nick());

		aTargetPlayer->time_news(
				(char *)format(GETTEXT("Your tech %1$s has been stolen by someone's spy!"),
								CandidateTech->get_name_with_level()));
	}

	return (char *)Result;
}

char *
CSpy::steal_important_technology(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	CKnownTechList *
		KnownTechList = aSpyPlayer->get_tech_list();
	CTechList *
		AvailableTechList = aSpyPlayer->get_available_tech_list();
	CKnownTechList *
		TargetKnownTechList = aTargetPlayer->get_tech_list();

	CKnownTechList
		CandidateTechList;

	for (int i=0 ; i<TargetKnownTechList->length() ; i++)
	{
		CKnownTech *
			KnownTech = (CKnownTech *)TargetKnownTechList->get(i);
		if (KnownTech->get_level() >= 7) continue;

		if (AvailableTechList->get_by_id(KnownTech->get_id()))
		{
			CKnownTech
				*Tech = new CKnownTech(KnownTech->get_owner(), KnownTech->get_id(), KnownTech->get_attribute());
			CandidateTechList.add_known_tech(Tech);
		}
	}

	if (CandidateTechList.length() == 0)
	{
		Result.format(GETTEXT("There is no tech that %1$s has and you can learn."),
						aTargetPlayer->get_nick());
	}
	else
	{
		int
			RandomIndex = number(CandidateTechList.length()) - 1;
		CKnownTech *
			CandidateTech = (CKnownTech *)CandidateTechList.get(RandomIndex);

		aSpyPlayer->discover_tech(CandidateTech->get_id());

		KnownTechList->type(QUERY_UPDATE);
		STORE_CENTER->store(*KnownTechList);

		if (aSpyPlayer->get_target_tech() == CandidateTech->get_id())
		{
			aTargetPlayer->set_target_tech(0);
		}

		Result.format(GETTEXT("You stole a tech %1$s from %2$s successfully."),
						CandidateTech->get_name_with_level(), aTargetPlayer->get_nick());

		aTargetPlayer->time_news(
				(char *)format(GETTEXT("Your tech %1$s has been stolen by someone's spy!"),
								CandidateTech->get_name_with_level()));
	}

	return (char *)Result;
}

char *
CSpy::steal_secret_technology(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	CKnownTechList *
		KnownTechList = aSpyPlayer->get_tech_list();
	CTechList *
		AvailableTechList = aSpyPlayer->get_available_tech_list();
	CKnownTechList *
		TargetKnownTechList = aTargetPlayer->get_tech_list();

	CKnownTechList
		CandidateTechList;

	for (int i=0 ; i<TargetKnownTechList->length() ; i++)
	{
		CKnownTech *
			KnownTech = (CKnownTech *)TargetKnownTechList->get(i);
		if (KnownTech->get_level() > 9) continue;

		if (AvailableTechList->get_by_id(KnownTech->get_id()))
		{
			CKnownTech
				*Tech = new CKnownTech(KnownTech->get_owner(), KnownTech->get_id(), KnownTech->get_attribute());
			CandidateTechList.add_known_tech(Tech);
		}
	}

	if (CandidateTechList.length() == 0)
	{
		Result.format(GETTEXT("There is no tech that %1$s has and you can learn."),
						aTargetPlayer->get_nick());
	}
	else
	{
		int
			RandomIndex = number(CandidateTechList.length()) - 1;
		CKnownTech *
			CandidateTech = (CKnownTech *)CandidateTechList.get(RandomIndex);

		aSpyPlayer->discover_tech(CandidateTech->get_id());

		KnownTechList->type(QUERY_UPDATE);
		STORE_CENTER->store(*KnownTechList);

		if (aSpyPlayer->get_target_tech() == CandidateTech->get_id())
		{
			aTargetPlayer->set_target_tech(0);
		}

		Result.format(GETTEXT("You stole a tech %1$s from %2$s successfully."),
						CandidateTech->get_name_with_level(), aTargetPlayer->get_nick());

		aTargetPlayer->time_news(
				(char *)format(GETTEXT("Your tech %1$s has been stolen by someone's spy!"),
								CandidateTech->get_name_with_level()));
	}

	return (char *)Result;
}

// ID : 8009
char *
CSpy::artificial_disease(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	CPlanetList *
		PlanetList = aTargetPlayer->get_planet_list();
	CKnownTechList *
		KnownTechList = aTargetPlayer->get_tech_list();
	int
		NumberOfTech = KnownTechList->count_by_category(CTech::TYPE_LIFE);

	for (int i=0 ; i<PlanetList->length() ; i++)
	{
		CPlanet *
			Planet = (CPlanet *)PlanetList->get(i);

		int
			Rate = 10 + number(20) - NumberOfTech;
		if (Rate < 0) Rate = 0;

		int
			PopulationDiff = Planet->get_population() * Rate / 100;
		if (PopulationDiff == 0)
		{
			if (i != 0) Result += "<BR>\n";
			Result.format(GETTEXT("It had no effect on planet %1$s."), Planet->get_name());
		}
		else
		{
			Planet->change_population(-PopulationDiff);

			if (i != 0) Result += "<BR>\n";
			Result.format(GETTEXT("%1$s has lost %2$d people on planet %3$s."),
					aTargetPlayer->get_nick(), PopulationDiff, Planet->get_name());

			aTargetPlayer->time_news(
					(char *)format(GETTEXT("You has lost some people on planet %1$s by someone's spy."),
									Planet->get_name()));
		}
	}

	return (char *)Result;
}

// ID : 8010
char *
CSpy::red_death(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	CPlanetList *
		PlanetList = aTargetPlayer->get_planet_list();
	CKnownTechList *
		KnownTechList = aTargetPlayer->get_tech_list();
	int
		NumberOfTech = KnownTechList->count_by_category(CTech::TYPE_LIFE);

	for (int i=0 ; i<PlanetList->length() ; i++)
	{
		CPlanet *
			Planet = (CPlanet *)PlanetList->get(i);

		int
			Rate = 20 + number(20) - NumberOfTech;
		if (Rate < 0) Rate = 0;

		int
			PopulationDiff = Planet->get_population() * Rate / 100;
		Planet->change_population(-PopulationDiff);

		if (aSpyPlayer->has_ability(ABILITY_GENETIC_ENGINEERING_SPECIALIST))
		{
			Planet->set_attribute(CPlanet::PA_OBSTINATE_MICROBE);

			Planet->build_control_model(aTargetPlayer);
		}

		if (PopulationDiff == 0)
		{
			if (i != 0) Result += "<BR>\n";
			Result.format(GETTEXT("It had no effect on planet %1$s."), Planet->get_name());

			if (aSpyPlayer->has_ability(ABILITY_GENETIC_ENGINEERING_SPECIALIST))
			{
				Result += " ";
				Result += GETTEXT("But it has an obstinate microbe now.");
			}

			if (aSpyPlayer->has_ability(ABILITY_GENETIC_ENGINEERING_SPECIALIST))
			{
				aTargetPlayer->time_news(
						(char *)format(GETTEXT("You didn't lose people on planet %1$s, but the planet has an obstinate microbe now."),
										Planet->get_name()));
			}
		}
		else
		{
			if (i != 0) Result += "<BR>\n";
			Result.format(GETTEXT("%1$s lost %2$d people on planet %3$s."),
							aTargetPlayer->get_nick(), PopulationDiff, Planet->get_name());

			if (aSpyPlayer->has_ability(ABILITY_GENETIC_ENGINEERING_SPECIALIST))
			{
				Result += " ";
				Result += GETTEXT("And it has an obstinate microbe now.");
			}

			if (aSpyPlayer->has_ability(ABILITY_GENETIC_ENGINEERING_SPECIALIST))
			{
				aTargetPlayer->time_news(
						(char *)format(GETTEXT("You lost %1$d people on planet %2$s by someone's spy, and the planet has an obstinate microbe now."),
										PopulationDiff, Planet->get_name()));
			}
			else
			{
				aTargetPlayer->time_news(
						(char *)format(GETTEXT("You lost %1$d people on planet %2$s by someone's spy."),
										PopulationDiff, Planet->get_name()));
			}
		}
	}

	return (char *)Result;
}

// ID : 8011
char *
CSpy::strike_base(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	static CString
		News;
	News.clear();
	bool
		Comma = false;

	CPlanetList *
		PlanetList = aTargetPlayer->get_planet_list();
	CPlanet *
		Planet = (CPlanet *)PlanetList->get(number(PlanetList->length()) - 1);
	CResource &
		Building = Planet->get_building();

	int
		MilitaryBaseDiff = Building.get(BUILDING_MILITARY_BASE) * number(15) / 100;
	if (MilitaryBaseDiff == 0)
	{
		Result.format(GETTEXT("It had no effect on planet %1$s's military bases."),
						Planet->get_name());
	}
	else
	{
		Building.change(BUILDING_MILITARY_BASE, -MilitaryBaseDiff);

		Result.format(GETTEXT("%1$s has lost %2$d military bases on %3$s."),
						aTargetPlayer->get_nick(), MilitaryBaseDiff, Planet->get_name());

		News.format(GETTEXT("%1$d military bases on %2$s"),
						MilitaryBaseDiff, Planet->get_name());
		Comma = true;
	}

	if (number(2) == 1)
	{
		CDock *
			Dock = aTargetPlayer->get_dock();
		if (Dock->length() != 0)
		{
			CDockedShip *
				Ship = (CDockedShip *)Dock->get(number(Dock->length()) - 1);
			int
				ShipDiff = Ship->get_number() * number(15) / 100;
			if (ShipDiff < 1) ShipDiff = 1;

			Ship->change_number(-ShipDiff);

			Result += "<BR>\n";
			Result.format(GETTEXT("%1$d ships in %2$s's ship pool were destroyed."),
							ShipDiff, aTargetPlayer->get_nick());

			if (Comma) News = ", ";
			News.format(GETTEXT("%1$d ships in your ship pool"), ShipDiff);
		}
	}

	if (News.length() == 0)
	{
		aTargetPlayer->time_news(
				GETTEXT("Someone tried to destroy your military bases and ships in your dock, but it had no effect."));
	}
	else
	{
		aTargetPlayer->time_news(
				(char *)format(GETTEXT("You lost %1$s on planet %2$s by someone's spy."),
								(char *)News, Planet->get_name()));
	}

	return (char *)Result;
}

// ID : 8012
char *
CSpy::meteor_strike(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	static CString
		News;
	News.clear();
	bool
		Comma = false;

	CPlanetList *
		PlanetList = aTargetPlayer->get_planet_list();
	CPlanet *
		Planet = (CPlanet *)PlanetList->get(number(PlanetList->length()) - 1);
	CResource &
		Building = Planet->get_building();

	bool
		AnyPopulationDiff = false,
		AnyFactoryDiff = false,
		AnyMilitaryBaseDiff = false,
		AnyResearchLabDiff = false;

	int
		DefenseRate;
	if (Planet->has_attribute(CPlanet::PA_ASTEROID))
	{
		DefenseRate = Building.get(BUILDING_MILITARY_BASE) / 2;
		if (DefenseRate > 90) DefenseRate = 90;
	}
	else
	{
		DefenseRate = Building.get(BUILDING_MILITARY_BASE);
		if (DefenseRate > 90) DefenseRate = 90;
	}

	int
		NumberOfAsteroid;
	if (aSpyPlayer->has_ability(ABILITY_ASTEROID_MANAGEMENT))
	{
		NumberOfAsteroid = 5;
	}
	else
	{
		NumberOfAsteroid = 1;
	}

	int PopulationDiffTotal = 0, FactoryDiffTotal = 0, MilitaryBaseDiffTotal = 0, ResearchLabDiffTotal = 0;

	for (int i=0 ; i<NumberOfAsteroid ; i++)
	{
		if (number(100) > DefenseRate)
		{
			int
				PopulationDiff = Planet->get_population() * number(20) / 100;
			if (PopulationDiff > 0)
			{
				Planet->change_population(-PopulationDiff);
				AnyPopulationDiff = true;
				PopulationDiffTotal += PopulationDiff;
			}

			int
				DestroyRate = number(20);
			int
				FactoryDiff = Building.get(BUILDING_FACTORY) * DestroyRate / 100,
				MilitaryBaseDiff = Building.get(BUILDING_MILITARY_BASE) * DestroyRate / 100,
				ResearchLabDiff = Building.get(BUILDING_RESEARCH_LAB) * DestroyRate / 100;

			if (FactoryDiff > 0)
			{
				Building.change(BUILDING_FACTORY, -FactoryDiff);
				AnyFactoryDiff = true;
				FactoryDiffTotal += FactoryDiff;
			}

			if (MilitaryBaseDiff > 0)
			{
				Building.change(BUILDING_MILITARY_BASE, -MilitaryBaseDiff);
				AnyMilitaryBaseDiff = true;
				MilitaryBaseDiffTotal += MilitaryBaseDiff;
			}

			if (ResearchLabDiff > 0)
			{
				Building.change(BUILDING_RESEARCH_LAB, -ResearchLabDiff);
				AnyResearchLabDiff = true;
				ResearchLabDiffTotal += ResearchLabDiff;
			}
		}
	}

	if (AnyPopulationDiff)
	{
		Result.format(GETTEXT("%1$s has lost %2$d people on planet %3$s."),
						aTargetPlayer->get_nick(), PopulationDiffTotal, Planet->get_name());

		News.format(GETTEXT("%1$d people"), PopulationDiffTotal);
		Comma = true;
	}
	else
	{
		Result.format(GETTEXT("It had no effect on planet %1$s's population."),
						Planet->get_name());
	}

	if (AnyFactoryDiff)
	{
		Result += "<BR>\n";
		Result.format(GETTEXT("%1$s has lost %2$d factories on %3$s."),
						aTargetPlayer->get_nick(), FactoryDiffTotal, Planet->get_name());

		if (Comma) News += ", ";
		News.format(GETTEXT("%1$d factories on %2$s"),
					FactoryDiffTotal, Planet->get_name());
		Comma = true;
	}
	else
	{
		Result += "<BR>\n";
		Result.format(GETTEXT("No factories have been destroyed on planet %1$s."),
						Planet->get_name());
	}

	if (AnyMilitaryBaseDiff)
	{
		Result += "<BR>\n";
		Result.format(GETTEXT("%1$s has lost %2$d military bases on %3$s."),
						aTargetPlayer->get_nick(), MilitaryBaseDiffTotal, Planet->get_name());

		if (Comma) News += ", ";
		News.format(GETTEXT("%1$d military bases on %2$s"),
					MilitaryBaseDiffTotal, Planet->get_name());
		Comma = true;
	}
	else
	{
		Result += "<BR>\n";
		Result.format(GETTEXT("No military bases have been destroyed on planet %1$s."),
						Planet->get_name());
	}

	if (AnyResearchLabDiff)
	{
		Result += "<BR>\n";
		Result.format(GETTEXT("%1$s has lost %2$d research labs. on %3$s."),
						aTargetPlayer->get_nick(), ResearchLabDiffTotal, Planet->get_name());

		if (Comma) News += ", ";
		News.format(GETTEXT("%1$d research labs. on %2$s"),
					ResearchLabDiffTotal, Planet->get_name());
		Comma = true;
	}
	else
	{
		Result += "<BR>\n";
		Result.format(GETTEXT("No research labs. have been destroyed on planet %1$s."),
						Planet->get_name());
	}

	if (News.length() == 0)
	{
		// time news
	}
	else
	{
		aTargetPlayer->time_news(
				(char *)format(GETTEXT("You lost %1$s on planet %2$s by someone's spy."),
								(char *)News, Planet->get_name()));
	}

	return (char *)Result;
}

// ID : 8013
char *
CSpy::emp_storm(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	CPlayerEffectList *
		EffectList = aTargetPlayer->get_effect_list();
	CPlanetList *
		PlanetList = aTargetPlayer->get_planet_list();

	int
		ApplyTurn = number(144);
	int
		ApplyPlanetIndex = number(PlanetList->length()) - 1;
	CPlanet *
		ApplyPlanet = (CPlanet *)PlanetList->get(ApplyPlanetIndex);

	CPlayerEffect *
		Effect = new CPlayerEffect();

	Effect->set_id(EffectList->get_new_id());
	Effect->set_owner(aTargetPlayer->get_game_id());
	Effect->set_life(CGame::get_game_time() + ApplyTurn*CGame::mSecondPerTurn);
	Effect->set_type(CPlayerEffect::PA_PARALYZE_PLANET);
	Effect->set_target(ApplyPlanet->get_id());
	Effect->set_source(CPlayerEffect::ST_SPY, mID);
	Effect->set_savable();

	EffectList->add_player_effect(Effect);

	Effect->type(QUERY_INSERT);
	STORE_CENTER->store(*Effect);

	Result.format(GETTEXT("Now the target player %1$s's random planet is paralyzed for %2$d turns."),
					aTargetPlayer->get_nick(), ApplyTurn);

	aTargetPlayer->time_news(
			(char *)format(GETTEXT("Your planet %1$s is now paralyzed for %2$d turns by someone's spy."),
							ApplyPlanet->get_name(), ApplyTurn));

	return (char *)Result;
}

// ID : 8014
char *
CSpy::stellar_bombardment(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	static CString
		News;
	News.clear();
	bool
		Comma = false;

	CPlanetList *
		PlanetList = aTargetPlayer->get_planet_list();
	CPlanet *
		Planet = (CPlanet *)PlanetList->get(number(PlanetList->length()) - 1);
	CResource &
		Building = Planet->get_building();

	bool
		AnyPopulationDiff = false,
		AnyFactoryDiff = false,
		AnyMilitaryBaseDiff = false,
		AnyResearchLabDiff = false;

	int
		DefenseRate;
	if (Planet->has_attribute(CPlanet::PA_ASTEROID))
	{
		DefenseRate = Building.get(BUILDING_MILITARY_BASE) / 2;
		if (DefenseRate > 90) DefenseRate = 90;
	}
	else
	{
		DefenseRate = Building.get(BUILDING_MILITARY_BASE);
		if (DefenseRate > 90) DefenseRate = 90;
	}

	int
		NumberOfAsteroid;
	if (aSpyPlayer->has_ability(ABILITY_ASTEROID_MANAGEMENT))
	{
		NumberOfAsteroid = 40;
	}
	else
	{
		NumberOfAsteroid = 10;
	}

	int PopulationDiffTotal = 0, FactoryDiffTotal = 0, MilitaryBaseDiffTotal = 0, ResearchLabDiffTotal = 0;

	for (int i=0 ; i<NumberOfAsteroid ; i++)
	{
		if (number(100) > DefenseRate)
		{
			int
				PopulationDiff = Planet->get_population() * number(20) / 100;
			if (PopulationDiff > 0)
			{
				Planet->change_population(-PopulationDiff);
				AnyPopulationDiff = true;
				PopulationDiffTotal += PopulationDiff;
			}

			int
				DestroyRate = number(20);
			int
				FactoryDiff = Building.get(BUILDING_FACTORY) * DestroyRate / 100,
				MilitaryBaseDiff = Building.get(BUILDING_MILITARY_BASE) * DestroyRate / 100,
				ResearchLabDiff = Building.get(BUILDING_RESEARCH_LAB) * DestroyRate / 100;

			if (FactoryDiff > 0)
			{
				Building.change(BUILDING_FACTORY, -FactoryDiff);
				AnyFactoryDiff = true;
				FactoryDiffTotal += FactoryDiff;
			}

			if (MilitaryBaseDiff > 0)
			{
				Building.change(BUILDING_MILITARY_BASE, -MilitaryBaseDiff);
				AnyMilitaryBaseDiff = true;
				MilitaryBaseDiffTotal += MilitaryBaseDiff;
			}

			if (ResearchLabDiff > 0)
			{
				Building.change(BUILDING_RESEARCH_LAB, -ResearchLabDiff);
				AnyResearchLabDiff = true;
				ResearchLabDiffTotal += ResearchLabDiff;
			}
		}
		else continue;
	}

	if (AnyPopulationDiff)
	{
		Result.format(GETTEXT("%1$s has lost %2$d people on planet %3$s."),
						aTargetPlayer->get_nick(), PopulationDiffTotal, Planet->get_name());

		News.format(GETTEXT("%1$d people"), PopulationDiffTotal);
		Comma = true;
	}
	else
	{
		Result.format(GETTEXT("It had no effect on planet %1$s's population."),
						Planet->get_name());
	}

	if (AnyFactoryDiff)
	{
		Result += "<BR>\n";
		Result.format(GETTEXT("%1$s has lost %2$d factories on %3$s."),
						aTargetPlayer->get_nick(), FactoryDiffTotal, Planet->get_name());

		if (Comma) News += ", ";
		News.format(GETTEXT("%1$d factories on %2$s"),
					FactoryDiffTotal, Planet->get_name());
		Comma = true;
	}
	else
	{
		Result += "<BR>\n";
		Result.format(GETTEXT("No factories have been destroyed on planet %1$s."),
						Planet->get_name());
	}

	if (AnyMilitaryBaseDiff)
	{
		Result += "<BR>\n";
		Result.format(GETTEXT("%1$s has lost %2$d military bases on %3$s."),
						aTargetPlayer->get_nick(), MilitaryBaseDiffTotal, Planet->get_name());

		if (Comma) News += ", ";
		News.format(GETTEXT("%1$d military bases on %2$s"),
					MilitaryBaseDiffTotal, Planet->get_name());
		Comma = true;
	}
	else
	{
		Result += "<BR>\n";
		Result.format(GETTEXT("No military bases have been destroyed on planet %1$s."),
						Planet->get_name());
	}

	if (AnyResearchLabDiff)
	{
		Result += "<BR>\n";
		Result.format(GETTEXT("%1$s has lost %2$d research labs. on %3$s."),
						aTargetPlayer->get_nick(), ResearchLabDiffTotal, Planet->get_name());

		if (Comma) News += ", ";
		News.format(GETTEXT("%1$d research labs. on %2$s"),
					ResearchLabDiffTotal, Planet->get_name());
		Comma = true;
	}
	else
	{
		Result += "<BR>\n";
		Result.format(GETTEXT("No research labs. have been destroyed on planet %1$s."),
						Planet->get_name());
	}

	if (News.length() == 0)
	{
		// time news
	}
	else
	{
		aTargetPlayer->time_news(
				(char *)format(GETTEXT("You lost %1$s on planet %2$s by someone's spy."),
								(char *)News, Planet->get_name()));
	}

	return (char *)Result;
}

// ID : 8015
char *
CSpy::assassination(CPlayer *aSpyPlayer, CPlayer *aTargetPlayer)
{
	static CString
		Result;
	Result.clear();

	CAdmiralList *
		AdmiralPool = aTargetPlayer->get_admiral_pool();
	if (AdmiralPool->length() == 0)
	{
		Result += GETTEXT("Your spy couldn't find any commander.");

		return (char *)Result;
	}

	CAdmiral *
		Admiral = (CAdmiral *)AdmiralPool->get(number(AdmiralPool->length()) - 1);

	Admiral->type(QUERY_DELETE);
	STORE_CENTER->store(*Admiral);

	Result.format(GETTEXT("%1$s's commander %2$s has been killed by your spy."),
					aTargetPlayer->get_nick(), Admiral->get_nick());

	aTargetPlayer->time_news(
			(char *)format(GETTEXT("Your commander %1$s was killed by someone's spy."),
							Admiral->get_nick()));

	AdmiralPool->remove_admiral(Admiral->get_id());

	return (char *)Result;
}

char *
CSpy::security_level_select_html()
{
	static CString
		Select;
	Select.clear();

	Select = "<SELECT NAME=\"SECURITY_LEVEL\">\n";

	for (int i=1 ; i<=5 ; i++)
	{
		Select.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
						i, CSpy::get_security_level_description(i));
	}

	Select += "</SELECT>\n";

	return (char *)Select;
}

CSpyTable::CSpyTable()
{
}

CSpyTable::~CSpyTable()
{
	remove_all();
}

bool
CSpyTable::free_item(TSomething aItem)
{
	CSpy *
		Spy = (CSpy *)aItem;

	if (!Spy) return false;

	delete Spy;

	return true;
}

int
CSpyTable::compare(TSomething aItem1, TSomething aItem2) const
{
	CSpy
		*Spy1 = (CSpy *)aItem1,
		*Spy2 = (CSpy *)aItem2;

	if (Spy1->get_id() > Spy2->get_id()) return 1;
	if (Spy1->get_id() < Spy2->get_id()) return -1;
	return 0;
}

int
CSpyTable::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CSpy *
		Spy = (CSpy *)aItem;

	if (Spy->get_id() > (int)aKey) return 1;
	if (Spy->get_id() < (int)aKey) return -1;
	return 0;
}

int
CSpyTable::add_spy(CSpy* aSpy)
{
	if (!aSpy) return 0;

	if (find_sorted_key((TConstSomething)aSpy->get_id()) >= 0)
		return 0;
	insert_sorted(aSpy);

	return aSpy->get_id();
}

bool
CSpyTable::remove_spy(int aSpyID)
{
	int
		Index = find_sorted_key((void *)aSpyID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

CSpy *
CSpyTable::get_by_id(int aSpyID)
{
	if (!length()) return NULL;

	int
		Index = find_sorted_key((void *)aSpyID);

	if (Index < 0) return NULL;

	return (CSpy *)get(Index);
}

