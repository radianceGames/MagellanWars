#include "../triggers.h"
#include "../archspace.h"

void
CCronTabNewPlayer::handler()
{
	SLOG("SYSTEM : Start CCronTabNewPlayer::handler()");

	if (PLAYER_TABLE->length() > 16000) return;

	for (int i=0 ; i<500 ; i++)
	{
		int
			PortalID = PLAYER_TABLE->length() + 1000;
		CString
			Name;
		Name.format("NPC(%d)", PortalID);

		CPlayer *
			Player = GAME->create_new_player(PortalID, (char *)Name, number(10));

		CPlanetList *
			PlanetList = Player->get_planet_list();
		CAdmiralList
			*AdmiralList = Player->get_admiral_list(),
			*AdmiralPool = Player->get_admiral_pool();
		CFleetList *
			FleetList = Player->get_fleet_list();
		CShipDesignList *
			ShipDesignList = Player->get_ship_design_list();

		while (PlanetList->length() < 25)
		{
			int
				ClusterID = Player->find_new_planet(true);
			if (ClusterID == -1)
			{
				for (int i=0 ; i<UNIVERSE->length() ; i++)
				{
					CCluster *
						Cluster = (CCluster *)UNIVERSE->get(i);
					if (Cluster->get_id() == EMPIRE_CLUSTER_ID) continue;
					if (Cluster->get_player_count()*20 < Cluster->get_planet_count()) continue;
					if (PlanetList->count_planet_from_cluster(Cluster->get_id()) > 20) continue;

					ClusterID = Cluster->get_id();
					break;
				}
			}
			if (ClusterID == -1)
			{
				CCluster *
					Cluster = UNIVERSE->new_cluster();
				ClusterID = Cluster->get_id();

				CMagistrate *
					Magistrate = new CMagistrate();

				CMagistrateList *
					MagistrateList = EMPIRE->get_magistrate_list();
				MagistrateList->add_magistrate(Magistrate);

				Magistrate->initialize(Cluster->get_id());
			}

			CCluster *
				Cluster = UNIVERSE->get_by_id(ClusterID);

			CPlanet *
				Planet = new CPlanet();
			Planet->initialize();
			Planet->set_cluster(Cluster);
			Planet->set_name(Cluster->get_new_planet_name());
			Player->add_planet(Planet);
			PLANET_TABLE->add_planet(Planet);
			Planet->start_terraforming();
			Player->new_planet_news(Planet);

			Planet->type(QUERY_INSERT);
			STORE_CENTER->store(*Planet);
		}

		while (AdmiralList->length() + AdmiralPool->length() < 50)
		{
			CAdmiral *
				Admiral = new CAdmiral(Player);
			AdmiralPool->add_admiral(Admiral);
			Player->new_admiral_news(Admiral);

			Admiral->type(QUERY_INSERT);
			STORE_CENTER->store(*Admiral);
		}

		while (FleetList->length() < 25)
		{
			CAdmiral *
				Admiral = (CAdmiral *)AdmiralPool->get(0);
			CShipDesign *
				ShipDesign = (CShipDesign *)ShipDesignList->get(ShipDesignList->length() - 1);

			CFleet *
				Fleet = new CFleet();
			Fleet->set_id(FleetList->get_new_fleet_id());
			Fleet->set_owner(Player->get_game_id());
			Fleet->set_name((char *)format("NPC Fleet(%d)", Fleet->get_id()));
			Fleet->set_admiral(Admiral->get_id());
			Fleet->set_ship_class(ShipDesign);
			Fleet->set_max_ship(Admiral->get_fleet_commanding());
			Fleet->set_current_ship(Admiral->get_fleet_commanding());
			Fleet->set_exp(25 + Player->get_control_model()->get_military()*3);

			FleetList->add_fleet(Fleet);

			Admiral->set_fleet_number(Fleet->get_id());
			AdmiralPool->remove_without_free_admiral(Admiral->get_id());
			AdmiralList->add_admiral(Admiral);

			Fleet->type(QUERY_INSERT);
			STORE_CENTER->store(*Fleet);

			Admiral->type(QUERY_UPDATE);
			STORE_CENTER->store(*Admiral);
		}
		SLOG("SYSTEM : Player %s has been created. Player->get_game_id() = %d",
				Player->get_nick(), Player->get_game_id());
		SLOG("SYSTEM : AdmiralList->length() = %d", AdmiralList->length());
		SLOG("SYSTEM : FleetList->length() = %d", FleetList->length());
	}

	int
		TotalAdmiral = 0,
		TotalFleet = 0;
	for (int i=0 ; i<PLAYER_TABLE->length() ; i++)
	{
		CPlayer *
			Player = (CPlayer *)PLAYER_TABLE->get(i);
		if (Player->get_game_id() == EMPIRE_GAME_ID) continue;

		Player->set_last_login(time(0));

		Player->type(QUERY_UPDATE);
		STORE_CENTER->store(*Player);

		TotalAdmiral += Player->get_admiral_list()->length();
		TotalFleet += Player->get_fleet_list()->length();
	}

	SLOG("SYSTEM : TotalAdmiral = %d, TotalFleet = %d", TotalAdmiral, TotalFleet);
	SLOG("SYSTEM : Now the server has %d players.", PLAYER_TABLE->length() - 1);

	SLOG("SYSTEM : End CCronTabNewPlayer::handler()");
};

