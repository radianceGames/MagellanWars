#include "../triggers.h"
#include "../archspace.h"

bool
CTriggerOptimize::handler()
{
	SLOG("database optimize");

	STORE_CENTER->query("cluster", "OPTIMIZE TABLE cluster");
	STORE_CENTER->query("player", "OPTIMIZE TABLE player");
	STORE_CENTER->query("planet", "OPTIMIZE TABLE planet");
	STORE_CENTER->query("tech", "OPTIMIZE TABLE tech");
	STORE_CENTER->query("project", "OPTIMIZE TABLE project");
	STORE_CENTER->query("admiral", "OPTIMIZE TABLE admiral");
	STORE_CENTER->query("council", "OPTIMIZE TABLE council");
	STORE_CENTER->query("diplomatic_message", 
						"OPTIMIZE TABLE diplomatic_message");
	STORE_CENTER->query("player_action", 
						"OPTIMIZE TABLE player_action");
	STORE_CENTER->query("council_action", 
						"OPTIMIZE TABLE council_action");
	STORE_CENTER->query("admission", "OPTIMIZE TABLE admission");
	STORE_CENTER->query("class", "OPTIMIZE TABLE class");
	STORE_CENTER->query("ship_building_q", 
						"OPTIMIZE TABLE ship_building_q");
	STORE_CENTER->query("docked_ship", "OPTIMIZE TABLE docked_ship");
	STORE_CENTER->query("damaged_ship", "OPTIMIZE TABLE damaged_ship");
	STORE_CENTER->query("fleet", "OPTIMIZE TABLE fleet");
	STORE_CENTER->query("defense_fleet", 
						"OPTIMIZE TABLE defense_fleet");
	STORE_CENTER->query("plan", "OPTIMIZE TABLE plan");
	STORE_CENTER->query("council_relation", 
						"OPTIMIZE TABLE council_relation");
	STORE_CENTER->query("player_relation", 
						"OPTIMIZE TABLE player_relation");
	STORE_CENTER->query("diplomatic_message", 
						"OPTIMIZE TABLE diplomatic_message");



	return true;
}
