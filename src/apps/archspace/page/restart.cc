#include <libintl.h>
#include "../pages.h"
#include "../archspace.h"
#include "../game.h"
#include "../race.h"

bool
CPageRestart::handle(CConnection &aConnection)
{
	system_log("start page handler %s", get_name());
	if( !CPageCommon::handle(aConnection)) return false;
	int PortalID = CONNECTION->get_portal_id();
	CPlayer
		*Player = PLAYER_TABLE->get_by_portal_id(PortalID);

	if (Player == NULL)
	{
		ITEM("ERROR_MESSAGE",
				"<A HREF=/archspace/create.as>Please create a character first</A>");
		return output("error.html");
	}

	if (!Player->is_dead())
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You can restart only if your domain is demolished."));
		return output("error.html");
	}

	// Reset player honor if lower than starting
	Player->set_honor((Player->get_honor() < 50) ? 50:Player->get_honor());

	// Reset player empire relation if lower than starting
	CRace *aRace = Player->get_race_data();
	if (Player->get_empire_relation() < aRace->get_empire_relation())
        	Player->set_empire_relation(aRace->get_empire_relation());

	// join new council
	CPlanet* Planet;
	CCluster* Cluster;
	CCouncil* Council;
	int Weight;
	int Range = 0;
	int Available = 0;
	int Dice;

	// check available room
	Available = 0;
	for(int i=0; i<UNIVERSE->length(); i++)
	{
		Cluster = (CCluster*)UNIVERSE->get(i);
		if (Cluster->get_id() == EMPIRE_CLUSTER_ID) continue;

		if (Cluster->get_player_count() > MAX_PLAYER_PER_CLUSTER)
		{
			Cluster->set_accept_new_player(false);
			continue;
		}

		if (Cluster->get_planet_count() > (int)(MAX_PLANET_PER_CLUSTER*0.8))
		{
			Cluster->set_accept_new_player(false);
			continue;
		}

		Cluster->set_accept_new_player(true);
		Available++;
	}

	//
	if (Available < CGame::mMinAvailableCluster)
	{
		CCluster *
			Cluster = UNIVERSE->new_cluster();

		CMagistrate *
			Magistrate = new CMagistrate();
		Magistrate->initialize(Cluster->get_id());

		CMagistrateList *
			MagistrateList = EMPIRE->get_magistrate_list();
		MagistrateList->add_magistrate(Magistrate);
	}

	// calculate weight
	Range = 0;
	for(int i=0; i<UNIVERSE->length(); i++)
	{
		Cluster = (CCluster*)UNIVERSE->get(i);
		if (Cluster->get_id() == EMPIRE_CLUSTER_ID) continue;

		int PlayerCount = Cluster->get_player_count()/10;
		int PlanetCount = Cluster->get_planet_count()/100;
		if (PlayerCount && PlanetCount)
		{
			Weight = (int)(MAX_PLAYER_PER_CLUSTER*MAX_PLANET_PER_CLUSTER/1000
							/ PlayerCount / PlanetCount);
			Cluster->set_weight(Weight);
			Range += Weight;
		} else
		{
			Weight = (int)(MAX_PLAYER_PER_CLUSTER*
											MAX_PLANET_PER_CLUSTER/1000);
			Cluster->set_weight(Weight);
			Range += Weight;
		}
		/*SLOG("Cluster Name:%s, Weight:%d, Range:%d",
						Cluster->get_name(), Weight, Range);*/
	}

	// throw dice
	Dice = number(Range)-1;
	//SLOG("Throw dice %d:%d", Dice, Range);


	// select cluster
	Range = 0;
	for(int i=0; i<UNIVERSE->length(); i++)
	{
		Cluster = (CCluster*)UNIVERSE->get(i);
		if (Cluster->get_id() == EMPIRE_CLUSTER_ID) continue;

		if (Dice >= Range && Dice < Range+Cluster->get_weight())
			break;

		Range += Cluster->get_weight();
	}

	//SLOG("Select Cluster %s", Cluster->get_name());

// for debug and make council
	if (COUNCIL_TABLE->length() < CGame::mMinCouncilCount)
		Council = ((CArchspace*)gApplication)->game()->create_new_council(Cluster);
	else
	{
		for(int i=0; i<COUNCIL_TABLE->length(); i++)
		{
			Council = (CCouncil*)COUNCIL_TABLE->get(i);

			if (!Council->get_auto_assign()) continue;
			if (Council->get_number_of_members() > (int)(MAX_COUNCIL_MEMBER*0.8))
				continue;

			Available++;
			Weight = MAX_COUNCIL_MEMBER*2-Council->get_number_of_members();
			Range += Weight;
		}

		if (Available < CGame::mMinCouncilCount)
			Council = ((CArchspace*)gApplication)->game()->create_new_council(Cluster);
		else
		{
			Dice = number(Range)-1;

			Range = 0;
			for(int i=0; i<COUNCIL_TABLE->length(); i++)
			{
				Council = (CCouncil*)COUNCIL_TABLE->get(i);

				if (!Council->get_auto_assign()) continue;
				if (Council->get_number_of_members() > (int)(MAX_COUNCIL_MEMBER*0.8))
					continue;

				Weight = MAX_COUNCIL_MEMBER*2-Council->get_number_of_members();
				Range += Weight;

				if (Dice >= Range && Dice < Range+Weight)
					break;
			}
		}
	}

	Player->set_council(Council);

	// gain new planet
	Planet = new CPlanet();
	Planet->initialize(Player->get_race_data());
	Planet->set_owner(Player);
	Planet->set_cluster(Cluster);
	Planet->set_name(Cluster->get_new_planet_name());
	Planet->change_population( 50000 );
	Planet->normalize();

	PLANET_TABLE->add_planet(Planet);
	Player->add_planet(Planet);
	Player->new_planet_news(Planet);
	Player->set_home_cluster_id(Cluster->get_id());
	CKnownTechList *
		KnownTechList = Player->get_tech_list();

	// forget techs
	// forget 5 <4level tech
	for (int i=0 ; i<5 ; i++)
	{
		int
			HighestLevel = 0;
		CKnownTech
			*HighestTech;
		// find highest level tech
		for( int j = 0; j < KnownTechList->length(); j++ )
		{
			CKnownTech *
				Tech = (CKnownTech *)KnownTechList->get(j);
			if (Tech->get_level() > HighestLevel)
			{
				HighestLevel = Tech->get_level();
				HighestTech = Tech;
			}
		}
		// if highest level tech level is > 4 delete it
		if (HighestLevel > 4)
		{
			KnownTechList->remove_known_tech(HighestTech->get_id());
		// else break;
		} else
		{
			break;
		}
	}
	Player->init_race_innate();

	Player->set_available_tech_list();
	KnownTechList->type(QUERY_UPDATE);
	STORE_CENTER->store(*KnownTechList);

	// lose all project
	Player->lose_all_project();

	Player->build_control_model();

	// clear dead
	Player->resurrect();

	// store
	Player->type(QUERY_UPDATE);
	*STORE_CENTER << *Player;

	Planet->type(QUERY_INSERT);
	STORE_CENTER->store(*Planet);

	PLAYER_TABLE->add_player_rank(Player);
	COUNCIL_TABLE->refresh_rank_table();

	static CString
		RestartMessage;
	RestartMessage.clear();

	RestartMessage.format(GETTEXT("You started your new domain at %1$s again."),
							Planet->get_name());
	Player->time_news((char *)RestartMessage);

	ITEM("RESULT_MESSAGE", (char *)RestartMessage);

	ADMIN_TOOL->add_restarting_player_log(
			(char *)format("Player %s has restarted.", Player->get_nick()));

//	system_log("end page handler %s", get_name());

	return output("restart.html");
}
