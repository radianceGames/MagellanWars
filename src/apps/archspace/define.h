#if !defined(__ARCHSPACE_DEFINE_H__)
#define __ARCHSPACE_DEFINE_H__

// Classic Mode (Without Final Set Changes)
#define CLASSIC_MODE 0

//#define RETIREMENT_COOLDOWN
#define COUNCIL_PLAYER_DISMISSAL
#define PROTECTION_BEGINNER
//#define PROTECTION_24HOURS

#define EMPIRE_FLEET_MAX_SHIP 100

#define GETTEXT(String)		gettext(String)

#define NONE 0

/* Game Information */
#define ARCHSPACE ((CArchspace*)gApplication)
#define GAME \
			(((CArchspace*)gApplication)->game())
#define GAME_VERSION GETTEXT("2.4.0")
//#define MONKEY_BUSINESS 0

/* Race Information */
/*
#define RACE_HUMAN 1
#define RACE_TARGOID 2
#define RACE_BUCKANEER 3
#define RACE_TECANOID 4
#define RACE_EVINTOS 5
#define RACE_AGERUS 6
#define RACE_BOSALIAN 7
#define RACE_XELOSS 8
#define RACE_XERUSIAN 9
#define RACE_XESPERADOS 10
*/

#define PLANET_TABLE_LIMIT 			1024*128
#define PLANET_TABLE_BLOCK 			65536
#define PLANET_TABLE_INDEX_LIMIT		1024*128
#define PLANET_TABLE_INDEX_BLOCK		65536

#define PLAYER_TABLE_LIMIT			1024*128
#define PLAYER_TABLE_BLOCK			65536
#define PLAYER_TABLE_HASH_LIMIT			512
#define PLAYER_TABLE_HASH_BLOCK			256

#define COUNCIL_TABLE_LIMIT			1024
#define COUNCIL_TABLE_BLOCK			256

#define MAX_PLAYER_PER_CLUSTER			200
#define MAX_PLANET_PER_CLUSTER			2000

#define MAX_COUNCIL_MEMBER			20

#define TURNS_PER_24HOURS			96
#define TURNS_PER_6HOURS			24

#define PERIOD_24HOURS				4*60*24 // second
#define PERIOD_48HOURS				4*60*48 // second
#define PERIOD_72HOURS				4*60*72 // second

// Time in seconds
#define ONE_WEEK (60*60*24*7)
#define ONE_DAY (60*60*24)
#define ENDGAME_START_DAY 35

#define MAX_PLAYER_NAME				30
#define INIT_ADMIRAL_NUMBER			10

#define MAX_COMMERCE_PLANET			3

#define PROTECTION_MODE_TURNS			300
#define PROTECTION_MODE_PLANET_LIMIT		4

#define MAX_PLAYER_PP				2000000000
#define MAX_PLAYER_MP				2000000000
#define MAX_COUNCIL_PP				2000000000
#define MAX_INVESTED_SHIP_PP			2000000000
#define MAX_RESEARCH_INVESTMENT_PP		2000000000
#define MAX_PLANET_INVESTMENT_PP		2000000000
#define MAX_POWER				2000000000
#define MAX_PLANET_POPULATION			2000000000
#define MAX_SHIP_PRODUCTION			2000000000
#define MAX_RESEARCH				2000000000
#define MAX_VOTE_POWER				2000000000
#define MAX_GENERAL_INT				2000000000
#define MAX_RESOURCE				2000000000
#define MAX_BID_PRICE				2000000000

/* Control Model */
#define MAX_EFFICIENCY 100

/* Planets */
#define MAX_WASTE_RATE 90
#define MAX_COMMERCE_WASTE 90

#define MAX_PURSUED_BOUNTIES			4
#define MAX_PLAYER_BOUNTIES			2
#define BOUNTY_DURATION				24*60*60
#define MAX_BOUNTY_EMPIRE_CHANGE		12

#define UNIVERSE \
			(((CArchspace*)gApplication)->game()->universe())
#define PLAYER_TABLE \
			(((CArchspace*)gApplication)->game()->player_table())
#define PORTAL_TABLE \
			(((CArchspace*)gApplication)->game()->portal_table())
#define PLANET_TABLE \
			(((CArchspace*)gApplication)->game()->planet_table())
#define BANNER \
			(((CArchspace*)gApplication)->banner_center())
#define TECH_TABLE \
			(((CArchspace*)gApplication)->game()->tech_table())
#define PROJECT_TABLE \
			(((CArchspace*)gApplication)->game()->project_table())
#define ENDING_PROJECT_LIST \
			(((CArchspace*)gApplication)->game()->ending_project_list())
#define COMPONENT_TABLE \
			(((CArchspace*)gApplication)->game()->component_table())
#define RACE_TABLE \
			(((CArchspace*)gApplication)->game()->race_table())
#define COUNCIL_TABLE \
			(((CArchspace*)gApplication)->game()->council_table())
#define STORE_CENTER \
			(((CArchspace*)gApplication)->store_center())
#define ACTION_TABLE \
			(((CArchspace*)gApplication)->game()->action_table())
#define SHIP_SIZE_TABLE \
			(((CArchspace*)gApplication)->game()->ship_table())
#define ADMIRAL_NAME_TABLE \
			(((CArchspace*)gApplication)->game()->admiral_name_table())
#define PLAYER_ACTION_TABLE \
			(((CArchspace*)gApplication)->game()->player_action_table())
#define PLAYER_RELATION_TABLE \
		(((CArchspace*)gApplication)->game()->player_relation_table())
#define COUNCIL_ACTION_TABLE \
			(((CArchspace*)gApplication)->game()->council_action_table())
#define COUNCIL_RELATION_TABLE \
		(((CArchspace*)gApplication)->game()->council_relation_table())
#define BOUNTY_TABLE \
		(((CArchspace*)gApplication)->game()->bounty_table())
#define OFFERED_BOUNTY_TABLE \
		(((CArchspace*)gApplication)->game()->offered_bounty_table())
#define BATTLE_RECORD_TABLE \
			(((CArchspace*)gApplication)->game()->battle_record_table())
#define BLACK_MARKET \
			(((CArchspace*)gApplication)->game()->player_table()->get_by_game_id(0)->get_black_market())
#define SPY_TABLE \
			(((CArchspace*)gApplication)->game()->spy_table())
#define EVENT_TABLE \
			(((CArchspace*)gApplication)->game()->event_table())
#define GALACTIC_EVENT_LIST \
			(((CArchspace*)gApplication)->game()->galactic_event_list())
#define NPC_FLEET_DESIGN_LIST \
			(((CArchspace*)gApplication)->game()->NPC_fleet_design_list())

#define DETACHMENT_PLAYER_PLAYER_TABLE \
			(((CArchspace *)gApplication)->game()->get_admin_tool()->get_detachment_player_player_table())
#define DETACHMENT_PLAYER_COUNCIL_TABLE \
			(((CArchspace *)gApplication)->game()->get_admin_tool()->get_detachment_player_council_table())
#define DETACHMENT_COUNCIL_COUNCIL_TABLE \
			(((CArchspace *)gApplication)->game()->get_admin_tool()->get_detachment_council_council_table())

#define EMPIRE_GAME_ID		0
#define EMPIRE_CLUSTER_ID	0

#define EMPIRE \
			(((CArchspace*)gApplication)->game()->player_table()->get_by_game_id(0))
#define MAGISTRATE_LIST \
			(((CArchspace *)gApplication)->game()->player_table()->get_by_game_id(0))->get_magistrate_list()

#define FORTRESS_LIST \
			(((CArchspace *)gApplication)->game()->player_table()->get_by_game_id(0))->get_fortress_list()
#define EMPIRE_CAPITAL_PLANET \
			(((CArchspace *)gApplication)->game()->player_table()->get_by_game_id(0))->get_empire_capital_planet()

#define EMPIRE_SHIP_DESIGN_TABLE \
			(((CArchspace*)gApplication)->game()->get_empire_ship_design_table())

#define EMPIRE_PLANET_INFO_LIST \
			(((CArchspace *)gApplication)->game()->player_table()->get_by_game_id(0))->get_empire_planet_info_list()

#define PLAYER_ATTACK_RESTRICTION_POWER_MAX	200
#define PLAYER_ATTACK_RESTRICTION_POWER_MIN			50

#define PLAYER_ATTACK_RESTRICTION_POWER_MAX_HI	200
#define PLAYER_ATTACK_RESTRICTION_POWER_MIN_HI			50

#define COUNCIL_ATTACK_RESTRICTION_POWER_MAX	800
#define COUNCIL_ATTACK_RESTRICTION_POWER_MIN	50

#define ADMIN_TOOL \
			((CArchspace *)gApplication)->game()->get_admin_tool()

#define CONFIG \
			(((CArchspace*)gApplication)->game()->get_config())

#define MYSQL_POOL \
            (((CArchspace*)gApplication)->game()->get_mysql_pool())

#define SLOG system_log
#define SLOG_FOR_PSTAT system_log_for_pstat

/* For Ship Design Classes */
// ---Change--- //
#define DEVICE_MAX_NUMBER 5
#define WEAPON_MAX_NUMBER 7
#define MAX_SHIP_CLASS 10
// ---End Change--- //

/* Black Market */
#define BLACK_MARKET_PLANET_COUNT_OFFSET 10

/* URLs */
#define SERVER_URL "http://archspace.org"

#endif
