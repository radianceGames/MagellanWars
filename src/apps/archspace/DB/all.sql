DROP TABLE IF EXISTS cluster;

CREATE TABLE cluster (
	id smallint(6) DEFAULT '0' NOT NULL,
	name char(50) NOT NULL,
	name_number int(8) DEFAULT '0' NOT NULL,

	PRIMARY KEY (id)
);

DROP TABLE IF EXISTS player_pref;

CREATE TABLE player_pref (
  player_id int(11) NOT NULL DEFAULT '0',
  java_choice int(11) NOT NULL DEFAULT '0',
  accept_ally int(11) NOT NULL DEFAULT '-1',
  accept_truce int(11) NOT NULL DEFAULT '-1',
  accept_pact int(11) NOT NULL DEFAULT '-1',
  commander_view int(11) UNSIGNED NOT NULL DEFAULT '15',
  PRIMARY KEY  (player_id)
);

DROP TABLE IF EXISTS player;

CREATE TABLE player (
	game_id int(10) NOT NULL,
	portal_id int(10) NOT NULL,
	name char(30) NOT NULL,
	home_cluster_id int(10) DEFAULT '0' NOT NULL,
	last_login int(10) DEFAULT '0' NOT NULL,
	last_login_ip varchar(15) DEFAULT '000.000.000.000' NOT NULL,

	mode tinyint(1) DEFAULT '0' NOT NULL,
	race tinyint(2) DEFAULT '0' NOT NULL,

	honor int(3) DEFAULT '50' NOT NULL,

	research_invest int(10) DEFAULT '0' NOT NULL,

	tick int(10) DEFAULT '0' NOT NULL,
	turn int(10) DEFAULT '0' NOT NULL,

	production int(10) DEFAULT '0' NOT NULL,
	ship_production int(10) DEFAULT '0' NOT NULL,
	invested_ship_production int(10) DEFAULT '0' NOT NULL,
	research int(10) DEFAULT '0' NOT NULL,
	ability char(65) DEFAULT '',
	research_tech int(10) DEFAULT '0' NOT NULL,

	admiral_timer int(10) DEFAULT '0' NOT NULL,

	last_turn_production int(10) DEFAULT '0' NOT NULL,
	last_turn_research int(10) DEFAULT '0' NOT NULL,
	last_turn_military int(10) DEFAULT '0' NOT NULL,

	council_id smallint(6) DEFAULT '0' NOT NULL,
	council_vote smallint(6) DEFAULT '0' NOT NULL,
	council_production int(10) DEFAULT '0' NOT NULL,
	council_donation int(10) DEFAULT '0' NOT NULL,

	security_level tinyint(5) DEFAULT '1' NOT NULL,
	alertness int(11) DEFAULT '0' NOT NULL,
	empire_relation int(6) DEFAULT '50' NOT NULL,

	protected_mode tinyint(4) DEFAULT '0' NOT NULL,
	protected_terminate_time int(10) DEFAULT '0' NOT NULL,
	news_turn int(10) DEFAULT '0' NOT NULL,
	news_production int(10) DEFAULT '0' NOT NULL,
	news_research int(10) DEFAULT '0' NOT NULL,
	news_population int(10) DEFAULT '0' NOT NULL,
	news_ability char(65) DEFAULT '',
	news_tech text DEFAULT '',
	news_planet text DEFAULT '',
	news_project text DEFAULT '',
	news_admiral text DEFAULT '',
	news_time_news longtext DEFAULT NULL,

	planet_invest_pool int(10) DEFAULT '0' NOT NULL,
	admission_time_limit int(11) DEFAULT '-1' NOT NULL,	
	honor_timer int(10) DEFAULT '0' NOT NULL,
	rating int(10) DEFAULT '2000' NOT NULL,
	
	PRIMARY KEY (game_id),
	KEY idx0 (name),
	UNIQUE idx1 (portal_id)
);

DROP TABLE IF EXISTS planet;

CREATE TABLE planet (
	id int(8) NOT NULL,
	cluster int(8) NOT NULL,
	owner int(8) NOT NULL,
	order_ int(8) NOT NULL,
	name char(50) DEFAULT '' NOT NULL,
	attribute char(8) DEFAULT '',
	population int(10) DEFAULT '0' NOT NULL,

	building_factory smallint(5) DEFAULT '0' NOT NULL,
	building_military_base smallint(5) DEFAULT '0' NOT NULL,
	building_research_lab smallint(5) DEFAULT '0' NOT NULL,

	progress_factory smallint(5) DEFAULT '0' NOT NULL,
	progress_military_base smallint(5) DEFAULT '0' NOT NULL,
	progress_research_lab smallint(5) DEFAULT '0' NOT NULL,

	ratio_factory smallint(5) DEFAULT '40' NOT NULL,
	ratio_military_base smallint(5) DEFAULT '30' NOT NULL,
	ratio_research_lab smallint(5) DEFAULT '30' NOT NULL,

	atmosphere char(8) DEFAULT '' NOT NULL,
	temperature smallint(3) DEFAULT '300' NOT NULL,
	size tinyint(1) DEFAULT '2' NOT NULL,
	resource tinyint(1) DEFAULT '2' NOT NULL,
	gravity double DEFAULT '1.0' NOT NULL,

	investment int(10) DEFAULT '0' NOT NULL,

	terraforming smallint(1) DEFAULT '0' NOT NULL,
	terraforming_timer int(10) DEFAULT '0' NOT NULL,

	commerce_with_1 int(10) DEFAULT '0' NOT NULL,
	commerce_with_2 int(10) DEFAULT '0' NOT NULL,
	commerce_with_3 int(10) DEFAULT '0' NOT NULL,

	privateer_timer int(10) DEFAULT '0' NOT NULL,
	blockade_timer int(10) DEFAULT '0' NOT NULL,

	news_population int(10) DEFAULT '0' NOT NULL,
	news_factory smallint(5) DEFAULT '0' NOT NULL,
	news_military_base smallint(5) DEFAULT '0' NOT NULL,
	news_research_lab smallint(5) DEFAULT '0' NOT NULL,

	turns_till_destruction int(11) DEFAULT '0' NOT NULL,
	planet_invest_pool tinyint(1) DEFAULT '0' NOT NULL,

	PRIMARY KEY (id),
	KEY idx0 (owner)
);

DROP TABLE IF EXISTS tech;

CREATE TABLE tech (
	owner smallint(6) DEFAULT '0' NOT NULL,
	info char(10) NOT NULL,
	life char(10) NOT NULL,
	matter char(10) NOT NULL,
	social char(10) NOT NULL,
	upgrade char(10) NOT NULL,
	schematics char(10) NOT NULL,
	amatter char(10) NOT NULL,

	PRIMARY KEY (owner)
);

DROP TABLE IF EXISTS project;

CREATE TABLE project (
	owner smallint(6) DEFAULT '0' NOT NULL,
	project_id smallint(5) DEFAULT '0' NOT NULL,
	type smallint(6) DEFAULT '0' NOT NULL,

	PRIMARY KEY (owner, project_id)
);

DROP TABLE IF EXISTS admiral;

CREATE TABLE admiral (
	id bigint(10) DEFAULT '0' NOT NULL,
	owner int(10) DEFAULT '-1' NOT NULL,
	race smallint(3) DEFAULT '0' NOT NULL,
	type smallint(3) DEFAULT '0' NOT NULL,
	name char(40) NOT NULL,
	exp int(10) DEFAULT '0' NOT NULL,
	level smallint(3) DEFAULT '0' NOT NULL,
	fleet_number int(10) DEFAULT '0' NOT NULL,

	armada_commanding tinyint NOT NULL,
	fleet_commanding smallint(3) DEFAULT '-10' NOT NULL,
	efficiency smallint(3) DEFAULT '-10' NOT NULL,

	offense smallint(3) DEFAULT '-10' NOT NULL,
	offense_up_level smallint(3) DEFAULT '-10' NOT NULL,

	defense smallint(3) DEFAULT '-10' NOT NULL,
	defense_up_level smallint(3) DEFAULT '-10' NOT NULL,

	maneuver smallint(3) DEFAULT '-10' NOT NULL,
	maneuver_up_level smallint(3) DEFAULT '-10' NOT NULL,

	detection smallint(3) DEFAULT '-10' NOT NULL,
	detection_up_level smallint(3) DEFAULT '-10' NOT NULL,

	commonability smallint(2) DEFAULT '-10' NOT NULL,
	raceability smallint(2) DEFAULT '-10' NOT NULL,

	PRIMARY KEY (id)
);

DROP TABLE IF EXISTS council;

CREATE TABLE council (
	id int(10) DEFAULT '0' NOT NULL,
	speaker int(10) DEFAULT '0' NOT NULL,
	name char(40) DEFAULT '' NOT NULL,
	slogan char(255) DEFAULT '' NOT NULL,
	production int(10) DEFAULT '0' NOT NULL,
	honor int(3) DEFAULT '50' NOT NULL,
	auto_assign smallint(1) DEFAULT '1' NOT NULL,
	home_cluster_id int(10) DEFAULT '0' NOT NULL,
    	merge_penalty_time int(11) DEFAULT '-1' NOT NULL,
    	secondary_speaker int(10) DEFAULT '0' NOT NULL,
	PRIMARY KEY (id)
);

DROP TABLE IF EXISTS diplomatic_message;

CREATE TABLE diplomatic_message (
	id INT UNSIGNED DEFAULT '0' NOT NULL,
	type SMALLINT DEFAULT '0' NOT NULL,
	sender INT UNSIGNED DEFAULT '0' NOT NULL,
	receiver INT UNSIGNED DEFAULT '0' NOT NULL,
	time INT UNSIGNED DEFAULT '0' NOT NULL,
	status SMALLINT DEFAULT '0' NOT NULL,

	PRIMARY KEY(id),
	KEY idx0 (receiver)
);

DROP TABLE IF EXISTS council_message;

CREATE TABLE council_message (
	id INT UNSIGNED DEFAULT '0' NOT NULL,
	type SMALLINT DEFAULT '0' NOT NULL,
	sender INT UNSIGNED DEFAULT '0' NOT NULL,
	receiver INT UNSIGNED DEFAULT '0' NOT NULL,
	time INT UNSIGNED DEFAULT '0' NOT NULL,
	status SMALLINT DEFAULT '0' NOT NULL,

	PRIMARY KEY(id),
	KEY idx0 (receiver)
);



DROP TABLE IF EXISTS player_action;

CREATE TABLE player_action (
	id int NOT NULL,
	start_time int NOT NULL,
	action smallint NOT NULL,
	owner int NOT NULL,
	argument int UNSIGNED DEFAULT '0' NOT NULL,

	PRIMARY KEY(id)
);

DROP TABLE IF EXISTS council_action;

CREATE TABLE council_action (
	id int NOT NULL,
	start_time int NOT NULL,
	action smallint NOT NULL,
	owner int NOT NULL,
	argument int DEFAULT '0' NOT NULL,

	PRIMARY KEY(id)
);




DROP TABLE IF EXISTS admission;

CREATE TABLE admission (
	player int NOT NULL,
	council int NOT NULL,
	status smallint NOT NULL,
	time int NOT NULL,
	content text,

	PRIMARY KEY(player,council)
);

DROP TABLE IF EXISTS class;

CREATE TABLE class (
	owner int NOT NULL,
	design_id int NOT NULL,
	name char(40) NOT NULL,
	body int NOT NULL,
	armor int NOT NULL,
	engine int NOT NULL,
	computer int NOT NULL,
	shield int NOT NULL,
	weapon1 int NOT NULL,
	weapon2 int NOT NULL,
	weapon3 int NOT NULL,
	weapon4 int NOT NULL,
	weapon5 int NOT NULL,
	weapon6 int NOT NULL,
	weapon7 int NOT NULL,
	weapon8 int NOT NULL,
	weapon9 int NOT NULL,
	weapon10 int NOT NULL,
	weapon_number1 int NOT NULL,
	weapon_number2 int NOT NULL,
	weapon_number3 int NOT NULL,
	weapon_number4 int NOT NULL,
	weapon_number5 int NOT NULL,
	weapon_number6 int NOT NULL,
	weapon_number7 int NOT NULL,
	weapon_number8 int NOT NULL,
	weapon_number9 int NOT NULL,
	weapon_number10 int NOT NULL,
	device1 int NOT NULL,
	device2 int NOT NULL,
	device3 int NOT NULL,
	device4 int NOT NULL,
	device5 int NOT NULL,
	device6 int NOT NULL,
	device7 int NOT NULL,
	device8 int NOT NULL,
	time int NOT NULL,
	cost int NOT NULL,
    black_market_design int(3) NOT NULL DEFAULT 0, 
	empire_design int(3) NOT NULL DEFAULT 0,     
	PRIMARY KEY(owner, design_id)
);

DROP TABLE IF EXISTS ship_building_q;

CREATE TABLE ship_building_q (
	owner int NOT NULL,
	design_id int NOT NULL,
	number int NOT NULL,
	time_order int NOT NULL,

	PRIMARY KEY(owner, time_order)
);

DROP TABLE IF EXISTS docked_ship;

CREATE TABLE docked_ship (
	owner int NOT NULL,
	design_id int NOT NULL,
	number int NOT NULL,

	PRIMARY KEY(owner, design_id)
);

DROP TABLE IF EXISTS damaged_ship;

CREATE TABLE damaged_ship (
	owner int NOT NULL,
	id int NOT NULL,
	design_id int NOT NULL,
	hp int NOT NULL,

	PRIMARY KEY(owner, id)
);

DROP TABLE IF EXISTS fleet;

CREATE TABLE fleet (
	owner int NOT NULL,
	id int(10) NOT NULL,
	name char(40) NOT NULL,
	admiral bigint(10) NOT NULL,
	exp int NOT NULL,
	status int NOT NULL,
	maxship int NOT NULL,
	currentship int NOT NULL,
	shipclass int NOT NULL,
	mission int NOT NULL,
	mission_target int NOT NULL,
	mission_terminate_time int NOT NULL,
	killed_ship int NOT NULL,
	killed_fleet int NOT NULL,

	PRIMARY KEY(owner, id)
);

DROP TABLE IF EXISTS defense_fleet;

CREATE TABLE defense_fleet (
	owner int NOT NULL,
	plan_id int NOT NULL,
	fleet_id int NOT NULL,
	command int NOT NULL,
	x int NOT NULL,
	y int NOT NULL,
	
	PRIMARY KEY(owner, plan_id, fleet_id)
);

DROP TABLE IF EXISTS plan;

CREATE TABLE plan (
	owner int NOT NULL,
	id int NOT NULL,
	type tinyint NOT NULL,
	name char(40) NOT NULL,
	capital int NOT NULL,
	enemy int NOT NULL,
	min int NOT NULL,
	max int NOT NULL,
	attack_type int NOT NULL,

	PRIMARY KEY(owner, id)
);

DROP TABLE IF EXISTS council_relation;

CREATE TABLE council_relation
(
	id int NOT NULL,
	council1 int NOT NULL,
	council2 int NOT NULL,
	relation smallint NOT NULL,
	time int NOT NULL,

	PRIMARY KEY(id)
);

DROP TABLE IF EXISTS player_relation;

CREATE TABLE player_relation
(
	id int NOT NULL,
	player1 int NOT NULL,
	player2 int NOT NULL,
	relation smallint NOT NULL,
	time int NOT NULL,

	PRIMARY KEY(id)
);

DROP TABLE IF EXISTS battle_record;

CREATE TABLE battle_record
(
	id int NOT NULL,
	attacker_id int NOT NULL,
	defender_id int NOT NULL,
	attacker_name char(30) NOT NULL,
	defender_name char(30) NOT NULL,
	attacker_race int NOT NULL,
	defender_race int NOT NULL,
	attacker_council int NOT NULL,
	defender_council int NOT NULL,
	time int NOT NULL,
	war_type int NOT NULL,
	is_draw char(3) NOT NULL DEFAULT 'NO',
	winner int NOT NULL,
	planet_id int NOT NULL,
	battle_field_name char(50) NOT NULL,
	attacker_gain text,
	attacker_lose_fleet text,
	attacker_lose_admiral text,
	defender_lose_fleet text,
	defender_lose_admiral text,
	record_file char(50) NOT NULL,
	there_was_battle tinyint NOT NULL,
	PRIMARY KEY(id)
);

DROP TABLE IF EXISTS damage;

CREATE TABLE damage (
	id int NOT NULL,
	owner int NOT NULL,
	attacker int NOT NULL,
	base int NOT NULL,
	amount int NOT NULL,
	time int NOT NULL,

	KEY idx0 (owner, time),
	PRIMARY KEY(id)
);

DROP TABLE IF EXISTS blackmarket;

CREATE TABLE blackmarket (
	id int NOT NULL,
	type int NOT NULL,
	item int NOT NULL,
	winner int NOT NULL,
	price int NOT NULL,
	opend int NOT NULL,
	expire int NOT NULL,
	closed int NOT NULL,
	number_of_planet int NOT NULL,

	PRIMARY KEY(id)
);

DROP TABLE IF EXISTS empire_action;

CREATE TABLE empire_action (
	id int NOT NULL,
	owner int NOT NULL,
	action int NOT NULL,
	target int NOT NULL,
	amount int NOT NULL,
	answer int NOT NULL,
	time int NOT NULL,

	PRIMARY KEY(owner, id)
);

DROP TABLE IF EXISTS player_effect;

CREATE TABLE player_effect (
	id int NOT NULL,
	owner int NOT NULL,
	life int NOT NULL,
	type int NOT NULL,
	target int NOT NULL,
	apply int NOT NULL,
	arg1 int(10) NOT NULL,
	arg2 int(10) NOT NULL,
	source_type int NOT NULL,
	source int NOT NULL,

	PRIMARY KEY(owner, id)
);

DROP TABLE IF EXISTS player_event;

CREATE TABLE player_event (
	id int NOT NULL,
	owner int NOT NULL,
	event int NOT NULL,
	life int NOT NULL,
	time int NOT NULL,
	answered tinyint(1) NOT NULL,

	PRIMARY KEY(owner, id)
);

DROP TABLE IF EXISTS detachment_player_player;

CREATE TABLE detachment_player_player (
	id int NOT NULL,
	player1 int NOT NULL,
	player2 int NOT NULL,

	PRIMARY KEY(id)
);

DROP TABLE IF EXISTS detachment_player_council;

CREATE TABLE detachment_player_council (
	id int NOT NULL,
	player int NOT NULL,
	council int NOT NULL,

	PRIMARY KEY(id)
);

DROP TABLE IF EXISTS detachment_council_council;

CREATE TABLE detachment_council_council (
	id int NOT NULL,
	type int NOT NULL,
	council1 int NOT NULL,
	council2 int NOT NULL,

	PRIMARY KEY(id)
);

DROP TABLE IF EXISTS empire;

CREATE TABLE empire (
	current_outer_planets int DEFAULT '0' NOT NULL,
	current_inner_planets int DEFAULT '0' NOT NULL
);

DROP TABLE IF EXISTS fortress;

CREATE TABLE fortress (
	layer int DEFAULT '0' NOT NULL,
	sector int DEFAULT '0' NOT NULL,
	fortress_order int DEFAULT '0' NOT NULL,
	owner int DEFAULT '0' NOT NULL,

	PRIMARY KEY(layer, sector, fortress_order)
);

DROP TABLE IF EXISTS empire_admiral_info;

CREATE TABLE empire_admiral_info (
	admiral_id int DEFAULT '0' NOT NULL,
	admiral_type int DEFAULT '0' NOT NULL,
	position_arg1 int DEFAULT '0' NOT NULL,
	position_arg2 int DEFAULT '0' NOT NULL,
	position_arg3 int DEFAULT '0' NOT NULL,

	PRIMARY KEY(admiral_id)
);

DROP TABLE IF EXISTS empire_fleet_info;

CREATE TABLE empire_fleet_info (
	fleet_id int DEFAULT '0' NOT NULL,
	fleet_type int DEFAULT '0' NOT NULL,
	position_arg1 int DEFAULT '0' NOT NULL,
	position_arg2 int DEFAULT '0' NOT NULL,
	position_arg3 int DEFAULT '0' NOT NULL,

	PRIMARY KEY(fleet_id)
);

DROP TABLE IF EXISTS empire_planet_info;

CREATE TABLE empire_planet_info (
	planet_id int DEFAULT '0' NOT NULL,
	owner_id int DEFAULT '0' NOT NULL,
	planet_type int DEFAULT '0' NOT NULL,
	position_arg int DEFAULT '0' NOT NULL,

	PRIMARY KEY(planet_id)
);

DROP TABLE IF EXISTS empire_capital_planet;

CREATE TABLE empire_capital_planet (
	owner_id smallint(6) DEFAULT '0' NOT NULL
);

DROP TABLE IF EXISTS bounty;

CREATE TABLE bounty (
	id int UNSIGNED DEFAULT '0' NOT NULL,
	source_player int UNSIGNED DEFAULT '0' NOT NULL,
	target_player int UNSIGNED DEFAULT '0' NOT NULL,
	empire_points int DEFAULT '0' NOT NULL,
	expire_time int UNSIGNED DEFAULT '0' NOT NULL,

	PRIMARY KEY(id)
);

DROP TABLE IF EXISTS game_status;

CREATE TABLE game_status (
	last_game_time int (12) unsigned DEFAULT '0' NOT NULL
);
